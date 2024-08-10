/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2022 Rupert Carmichael
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, specifically version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <sstream>

#include <jg/jg.h>
#include <jg/jg_snes.h>

#include "bsnes.hpp"
#include "settings.hpp"

#include "gamedb.hpp"
#include "version.h"

#define SAMPLERATE 48000
#define FRAMERATE 60
#define FRAMERATE_PAL 50
#define CHANNELS 2
#define NUMINPUTS 5

#define ASPECT_NTSC 8.0 / 7.0
#define ASPECT_PAL 7375000.0 / 5320342.5

#define TIMING_NTSC 60.098812
#define TIMING_PAL 50.006979

#define VIDEO_HEIGHT 240
#define VIDEO_WIDTH 256

static jg_cb_audio_t jg_cb_audio;
static jg_cb_frametime_t jg_cb_frametime;
static jg_cb_log_t jg_cb_log;
static jg_cb_rumble_t jg_cb_rumble;

static jg_coreinfo_t coreinfo = {
    "bsnes", "bsnes-jg", JG_VERSION, "snes", NUMINPUTS, 0
};

static jg_videoinfo_t vidinfo = {
    JG_PIXFMT_RGBX5551, // pixfmt
    VIDEO_WIDTH << 1,   // wmax
    VIDEO_HEIGHT << 1,  // hmax
    VIDEO_WIDTH,        // w
    VIDEO_HEIGHT,       // h
    0,                  // x
    0,                  // y
    1024,               // p
    ASPECT_NTSC,        // aspect
    NULL
};

static jg_audioinfo_t audinfo = {
    JG_SAMPFMT_FLT32,
    SAMPLERATE,
    CHANNELS,
    (SAMPLERATE / FRAMERATE) * CHANNELS,
    NULL
};

static jg_pathinfo_t pathinfo;
static jg_fileinfo_t addoninfo;
static jg_fileinfo_t gameinfo;
static jg_fileinfo_t sufamiinfo;
static jg_inputinfo_t inputinfo[NUMINPUTS];
static jg_inputstate_t *input_device[NUMINPUTS];

// Emulator settings
static jg_setting_t settings_bsnes[] = {
    { "port1", "Controller Port 1",
      "0 = Auto, 1 = Controller, 2 = Mouse",
      "Select the device plugged into controller port 1",
      0, 0, 2, JG_SETTING_INPUT
    },
    { "port2", "Controller Port 2",
      "0 = Auto, 1 = Controller, 2 = Mouse, 3 = Multitap, 4 = Super Scope, "
      "5 = Justifier",
      "Select the device plugged into controller port 2",
      0, 0, 5, JG_SETTING_INPUT
    },
    { "aspect_ratio", "Aspect Ratio",
      "0 = Auto Region, 1 = 1:1 PAR, 2 = NTSC, 3 = PAL",
      "Set the aspect ratio",
      0, 0, 3, 0
    },
    { "overscan_t", "Overscan Mask (Top)",
      "N = Hide N pixels of Overscan (Top)",
      "Hide N pixels of Overscan (Top)",
      8, 0, 16, 0
    },
    { "overscan_b", "Overscan Mask (Bottom)",
      "N = Hide N pixels of Overscan (Bottom)",
      "Hide N pixels of Overscan (Bottom)",
      8, 0, 16, 0
    },
    { "overscan_l", "Overscan Mask (Left)",
      "N = Hide N pixels of Overscan (Left)",
      "Hide N pixels of Overscan (Left)",
      0, 0, 16, 0
    },
    { "overscan_r", "Overscan Mask (Right)",
      "N = Hide N pixels of Overscan (Right)",
      "Hide N pixels of Overscan (Right)",
      0, 0, 16, 0
    },
    { "coproc_delaysync", "Delay LLE Coprocessor Sync",
      "0 = Off, 1 = On",
      "Delay syncing Low Level Emulated coprocessors for a performance "
      "increase at the cost of a slight reduction in accuracy",
      0, 0, 1, JG_SETTING_RESTART
    },
    { "coproc_preferhle", "Prefer HLE Coprocessors",
      "0 = Off, 1 = On",
      "Prefer High Level Emulation of coprocessors when available",
      1, 0, 1, JG_SETTING_RESTART
    },
    { "rsqual", "Resampler Quality",
      "0 = Fastest, 1 = Medium, 2 = Best",
      "Quality level for the internal resampler",
      0, 0, 2, 1
    },
    { "region", "Region",
      "0 = Auto, 1 = NTSC, 2 = PAL",
      "Set the region to use",
      0, 0, 2, JG_SETTING_RESTART
    }
};

enum {
    PORT1,
    PORT2,
    ASPECT,
    OVERSCAN_T,
    OVERSCAN_B,
    OVERSCAN_L,
    OVERSCAN_R,
    COPROC_DELAYSYNC,
    COPROC_PREFERHLE,
    RSQUAL,
    REGION
};

static int hmult = 2;
static int vmult = 1;
static int ss_offset_x = 0;
static int ss_offset_y = 0;

static uint8_t addon = 0;

static SuperFamicom::Interface *interface;

static struct Location {
    std::string location;
} bsMemory, superFamicom, sufamiTurboA, sufamiTurboB;

// Set the aspect ratio
static void aspectRatio(void) {
    // First, figure out what the ideal height/width are
    unsigned aspect_w = VIDEO_WIDTH - (settings_bsnes[OVERSCAN_L].val +
        settings_bsnes[OVERSCAN_R].val);
    vidinfo.w = VIDEO_WIDTH - (settings_bsnes[OVERSCAN_L].val +
        settings_bsnes[OVERSCAN_R].val);
    vidinfo.h = VIDEO_HEIGHT - (settings_bsnes[OVERSCAN_T].val +
        settings_bsnes[OVERSCAN_B].val);
    vidinfo.x = settings_bsnes[OVERSCAN_L].val;
    vidinfo.y = settings_bsnes[OVERSCAN_T].val;

    double aspect_ratio = ASPECT_NTSC;

    switch (settings_bsnes[ASPECT].val) {
        default: case 0: { // Auto Region
            aspect_ratio = interface->getRegion() == "PAL" ?
                ASPECT_PAL : ASPECT_NTSC;
            break;
        }
        case 1: { // 1:1 PAR
            aspect_ratio = 1.0;
            break;
        }
        case 2: { // NTSC
            aspect_ratio = ASPECT_NTSC;
            break;
        }
        case 3: { // PAL
            aspect_ratio = ASPECT_PAL;
            break;
        }
    }

    vidinfo.aspect = (aspect_w * aspect_ratio) / (double)vidinfo.h;
}

static void inputSetup(void) {
    // Set up inputs
    int multitap = 0;
    int port[] = { settings_bsnes[PORT1].val, settings_bsnes[PORT2].val };
    unsigned portid[] = {
        SuperFamicom::ID::Port::Controller1,
        SuperFamicom::ID::Port::Controller2
    };

    // Autodetect Mouse
    for (size_t i = 0; i < db_mouse_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_mouse_games[i]) {
            port[0] = port[0] ? port[0] : 2;
            break;
        }
    }

    // Autodetect Multitap
    for (size_t i = 0; i < db_multitap_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_multitap_games[i].md5) {
            multitap = db_multitap_games[i].players;
            port[1] = port[1] ? port[1] : 3;
            break;
        }
    }

    // Autodetect Super Scope
    for (size_t i = 0; i < db_superscope_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_superscope_games[i].md5) {
            port[1] = port[1] ? port[1] : 4;
            ss_offset_x = db_superscope_games[i].x;
            ss_offset_y = db_superscope_games[i].y;
            break;
        }
    }

    // Autodetect Justifier
    for (size_t i = 0; i < db_justifier_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_justifier_games[i]) {
            port[1] = port[1] ? port[1] : 5;
            break;
        }
    }

    for (int i = 0; i < NUMINPUTS; ++i)
        inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_UNCONNECTED);

    for (int i = 0; i < 2; ++i) {
        switch (port[i]) {
            default: case 0: case 1: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_PAD);
                interface->connect(portid[i],
                    SuperFamicom::ID::Device::Gamepad);
                break;
            }
            case 2: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_MOUSE);
                interface->connect(portid[i],
                    SuperFamicom::ID::Device::Mouse);
                break;
            }
            case 3: {
                for (int j = 1; j < (multitap ? multitap : NUMINPUTS); ++j)
                    inputinfo[j] = jg_snes_inputinfo(j, JG_SNES_PAD);

                interface->connect(1, SuperFamicom::ID::Device::SuperMultitap);
                break;
            }
            case 4: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_SUPERSCOPE);
                interface->connect(portid[i],
                    SuperFamicom::ID::Device::SuperScope);
                break;
            }
            case 5: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_JUSTIFIER);
                interface->connect(portid[i],
                    SuperFamicom::ID::Device::Justifier);
                break;
            }
        }
    }
}

static bool fileOpenV(unsigned id, std::string name, std::vector<uint8_t>& v) {
    std::string path;
    bool required = false;

    if (id == 1) { // SFC
        if (name == "save.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        }
        else if (name == "dsp1.program.rom") {
            path = std::string(pathinfo.bios) + "/dsp1.program.rom";
        }
        else if (name == "dsp1.data.rom") {
            path = std::string(pathinfo.bios) + "/dsp1.data.rom";
        }
        else if (name == "dsp1b.program.rom") {
            path = std::string(pathinfo.bios) + "/dsp1b.program.rom";
        }
        else if (name == "dsp1b.data.rom") {
            path = std::string(pathinfo.bios) + "/dsp1b.data.rom";
        }
        else if (name == "dsp2.program.rom") {
            path = std::string(pathinfo.bios) + "/dsp2.program.rom";
        }
        else if (name == "dsp2.data.rom") {
            path = std::string(pathinfo.bios) + "/dsp2.data.rom";
        }
        else if (name == "dsp3.program.rom") {
            path = std::string(pathinfo.bios) + "/dsp3.program.rom";
            required = true;
        }
        else if (name == "dsp3.data.rom") {
            path = std::string(pathinfo.bios) + "/dsp3.data.rom";
            required = true;
        }
        else if (name == "dsp4.program.rom") {
            path = std::string(pathinfo.bios) + "/dsp4.program.rom";
        }
        else if (name == "dsp4.data.rom") {
            path = std::string(pathinfo.bios) + "/dsp4.data.rom";
        }
        else if (name == "st010.program.rom") {
            path = std::string(pathinfo.bios) + "/st010.program.rom";
        }
        else if (name == "st010.data.rom") {
            path = std::string(pathinfo.bios) + "/st010.data.rom";
        }
        else if (name == "st011.program.rom") {
            path = std::string(pathinfo.bios) + "/st011.program.rom";
            required = true;
        }
        else if (name == "st011.data.rom") {
            path = std::string(pathinfo.bios) + "/st011.data.rom";
            required = true;
        }
        else if (name == "arm6.program.rom") {
            path = std::string(pathinfo.bios) + "/st018.program.rom";
            required = true;
        }
        else if (name == "arm6.data.rom") {
            path = std::string(pathinfo.bios) + "/st018.data.rom";
            required = true;
        }
        else if (name == "msu1/data.rom") {
            path = superFamicom.location;
            path = path.substr(0, path.find_last_of(".")) + ".msu";
        }
        else if (name.find("msu1/track") != std::string::npos) {
            path = superFamicom.location;
            path = path.substr(0, path.find_last_of(".")) +
                name.substr(name.find_first_of("track") + 5);
        }
        else if (name == "download.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".psr";
        }
        else if (name == "time.rtc") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".rtc";
        }
        else {
            jg_cb_log(JG_LOG_WRN, "File load attempt: %s\n", name.c_str());
        }
    }
    else if (id == 2) { // GB
        if (name == "save.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        }
        else {
            jg_cb_log(JG_LOG_WRN, "File load attempt: %s\n", name.c_str());
        }
    }
    else if (id == 4) { // Sufami Turbo Slot A
        if (name == "save.ram") {
            if (sufamiinfo.size) {
                path = std::string(pathinfo.save) + "/" +
                    std::string(sufamiinfo.name) + ".srm";
            }
            else {
                path = std::string(pathinfo.save) + "/" +
                    std::string(gameinfo.name) + ".srm";
            }
        }
    }
    else if (id == 5) { // Sufami Turbo Slot B
        if (name == "save.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        }
    }

    std::ifstream stream(path, std::ios::in | std::ios::binary);
    if (!stream.is_open()) {
        if (required) {
            jg_cb_log(JG_LOG_ERR, "Failed to load file: %s\n", path.c_str());
        }
        else {
            return false;
        }
    }

    v = std::vector<uint8_t>((std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>());
    stream.close();

    jg_cb_log(JG_LOG_DBG, "Loaded %s (%ld bytes)\n", path.c_str(), v.size());

    return true;
}

static bool fileOpenS(std::string name, std::stringstream& ss) {
    std::string path = std::string(pathinfo.core) + "/" + name;
    std::ifstream stream(path, std::ios::in | std::ios::binary);

    if (!stream.is_open()) {
        jg_cb_log(JG_LOG_ERR, "Failed to load file: %s\n", path.c_str());
    }

    ss << stream.rdbuf();
    stream.close();

    return true;
}

static void fileWrite(unsigned id, std::string name, const uint8_t *data,
    unsigned size) {

    std::string path;

    if (id == 1) { // SFC
        if (name == "save.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        }
        else if (name == "time.rtc") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".rtc";
        }
        else if (name == "download.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".psr";
        }
    }
    else if (id == 2) { // GB
        if (name == "save.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        }
        else if (name == "time.rtc") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".rtc";
        }
    }
    else if (id == 3) { // BS-X
        if (name == "program.flash") { //writes are not flushed to disk in bsnes
            return;
        }
    }
    else if (id == 4) { // Sufami Turbo Slot A
        if (name == "save.ram") {
            if (sufamiinfo.size)
                path = std::string(pathinfo.save) + "/" +
                    std::string(sufamiinfo.name) + ".srm";
            else
                path = std::string(pathinfo.save) + "/" +
                    std::string(gameinfo.name) + ".srm";
        }
    }
    else if (id == 5) { // Sufami Turbo Slot B
        if (name == "save.ram") {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        }
    }

    if (path.empty()) {
        jg_cb_log(JG_LOG_WRN, "No file routines for: %s\n", name.c_str());
        return;
    }

    std::ofstream stream(path, std::ios::out | std::ios::binary);
    if (stream.is_open()) {
        stream.write((const char*)data, size);
        stream.close();
        jg_cb_log(JG_LOG_DBG, "File saved %s\n", path.c_str());
    }
    else {
        jg_cb_log(JG_LOG_WRN, "Failed to save file: %s\n", path.c_str());
    }
}

static void videoFrame(const uint16_t *data, unsigned pitch, unsigned w,
    unsigned h) {

    hmult = w / VIDEO_WIDTH;
    vmult = h / VIDEO_HEIGHT;

    vidinfo.x = settings_bsnes[OVERSCAN_L].val * hmult;
    w -= vidinfo.x + (settings_bsnes[OVERSCAN_R].val * hmult);

    vidinfo.y = settings_bsnes[OVERSCAN_T].val * vmult;
    h -= vidinfo.y + (settings_bsnes[OVERSCAN_B].val * vmult);

    vidinfo.w = w;
    vidinfo.h = h;
    vidinfo.p = pitch / 2; // Divide by pixel size - 16-bit pixels == 2
    vidinfo.buf = const_cast<void*>(reinterpret_cast<const void*>(data));
}

static uint8_t imap[12] = { 0, 1, 2, 3, 7, 6, 9, 8, 10, 11, 4, 5 };

static unsigned pollInputGamepad(unsigned port) {
    unsigned b = 0;

    if (input_device[port]->button[0]) b |= Input::Gamepad::Up;
    if (input_device[port]->button[1]) b |= Input::Gamepad::Down;
    if (input_device[port]->button[2]) b |= Input::Gamepad::Left;
    if (input_device[port]->button[3]) b |= Input::Gamepad::Right;
    if (input_device[port]->button[4]) b |= Input::Gamepad::Select;
    if (input_device[port]->button[5]) b |= Input::Gamepad::Start;
    if (input_device[port]->button[6]) b |= Input::Gamepad::A;
    if (input_device[port]->button[7]) b |= Input::Gamepad::B;
    if (input_device[port]->button[8]) b |= Input::Gamepad::X;
    if (input_device[port]->button[9]) b |= Input::Gamepad::Y;
    if (input_device[port]->button[10]) b |= Input::Gamepad::L;
    if (input_device[port]->button[11]) b |= Input::Gamepad::R;

    return b;
}

static int16_t pollInput(unsigned port, unsigned device, unsigned input) {
    if (device == SuperFamicom::ID::Device::SuperScope) {
        switch (input) {
            case 0: { // X
                return (input_device[port]->coord[0] / hmult) + ss_offset_x;
            }
            case 1: { // Y
                return (input_device[port]->coord[1] / vmult) + ss_offset_y;
            }
            case 2: { // Trigger
                int ret = 0;
                if (input_device[port]->button[1]) { // Offscreen
                    input_device[port]->coord[0] = 1026;
                    ret = 1;
                }
                else
                    ret = input_device[port]->button[0];
                return ret;
            }
            case 3: { // Cursor
                return input_device[port]->button[2];
            }
            case 4: { // Turbo
                return input_device[port]->button[3];
            }
            case 5: { // Pause
                return input_device[port]->button[4];
            }
            default: {
                return 0;
            }
        }
    }
    else if (device == SuperFamicom::ID::Device::Mouse) {
        switch (input) {
            case 0: { // X
                int ret = input_device[port]->rel[0] / 2;
                input_device[port]->rel[0] -= ret;
                return ret;
            }
            case 1: { // Y
                int ret = input_device[port]->rel[1] / 2;
                input_device[port]->rel[1] -= ret;
                return ret;
            }
            case 2: { // Left Click
                return input_device[port]->button[0];
            }
            case 3: { // Right Click
                return input_device[port]->button[1];
            }
        }
    }
    else if (device == SuperFamicom::ID::Device::Justifier) {
        switch (input) {
            case 0: { // X
                return input_device[port]->coord[0] / hmult;
            }
            case 1: { // Y
                return input_device[port]->coord[1] / vmult;
            }
            case 2: { // Trigger
                int ret = 0;
                if (input_device[port]->button[1]) { // Offscreen
                    input_device[port]->coord[0] = 1026;
                    ret = 1;
                }
                else
                    ret = input_device[port]->button[0];
                return ret;
            }
            case 3: { // Start
                return input_device[port]->button[2];
            }
            default: {
                return 0;
            }
        }
    }
    else if (device == SuperFamicom::ID::Device::SuperMultitap) {
        return input_device[(input / 12) + 1]->button[imap[input % 12]];
    }

    return port > 1 ? 0 : input_device[port]->button[imap[input]];
}

static inline std::vector<uint8_t> bufToVec(void *data, size_t size) {
    uint8_t *buf = (uint8_t*)data;
    std::vector<uint8_t> ret(buf, buf + size);
    return ret;
}

static bool loadRom(unsigned id) {
    if (id == 1) {
        std::vector<uint8_t> rom = addon ?
        bufToVec(addoninfo.data, addoninfo.size) :
        bufToVec(gameinfo.data, gameinfo.size);

        if (rom.size() < 0x8000) return false;

        //assume ROM and IPS agree on whether a copier header is present
        //superFamicom.patched = applyPatchIPS(rom, location);
        if ((rom.size() & 0x7fff) == 512) {
            //remove copier header
            rom.erase(rom.begin(), rom.begin() + 512);
        }

        interface->setRomSuperFamicom(rom, superFamicom.location);
        return true;
    }
    else if (id == 3) {
        std::vector<uint8_t> rom = bufToVec(gameinfo.data, gameinfo.size);
        if (rom.size() < 0x8000) return false;
        interface->setRomBSMemory(rom, bsMemory.location);
        return true;
    }
    else if (id == 4) {
        std::vector<uint8_t> rom;

        if (sufamiinfo.size)
            rom = bufToVec(sufamiinfo.data, sufamiinfo.size);
        else
            rom = bufToVec(gameinfo.data, gameinfo.size);

        if (rom.size() < 0x20000) return false;
        interface->setRomSufamiTurboA(rom, sufamiTurboA.location);
        return true;
    }
    else if (id == 5) {
        if (!sufamiinfo.size)
            return false;

        std::vector<uint8_t> rom = bufToVec(gameinfo.data, gameinfo.size);
        if (rom.size() < 0x20000) return false;
        interface->setRomSufamiTurboB(rom, sufamiTurboB.location);
        return true;
    }
    return false;
}

// Jolly Good API Calls
void jg_set_cb_audio(jg_cb_audio_t func) {
    jg_cb_audio = func;
}

void jg_set_cb_frametime(jg_cb_frametime_t func) {
    jg_cb_frametime = func;
}

void jg_set_cb_log(jg_cb_log_t func) {
    jg_cb_log = func;
}

void jg_set_cb_rumble(jg_cb_rumble_t func) {
    jg_cb_rumble = func;
}

int jg_init(void) {
    // Create interface and set callbacks
    interface = new SuperFamicom::Interface;
    interface->setInputCallback(&pollInput);
    interface->setInputGamepadCallback(&pollInputGamepad);
    interface->setOpenFileCallback(&fileOpenV);
    interface->setOpenStreamCallback(&fileOpenS);
    interface->setLogCallback(jg_cb_log);
    interface->setRomCallback(&loadRom);
    interface->setWriteCallback(&fileWrite);

    // Configuration
    interface->setCoprocDelayedSync(settings_bsnes[COPROC_DELAYSYNC].val);
    interface->setCoprocPreferHLE(settings_bsnes[COPROC_PREFERHLE].val);

    return 1;
}

void jg_deinit(void) {
    delete interface;
}

void jg_reset(int hard) {
    if (hard)
        interface->power();
    else
        interface->reset();
}

void jg_exec_frame(void) {
    interface->run();
}

int jg_game_load(void) {
    // Load the game
    std::string fname = std::string(gameinfo.fname);
    std::string ext = fname.substr(fname.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "gb" ||ext == "gbc") {
        if (!addoninfo.size) {
            jg_cb_log(JG_LOG_ERR,
                "Super Game Boy ROM required for GB/GBC content, exiting...\n");
            return 0;
        }

        superFamicom.location = std::string(addoninfo.path);
        interface->setRomGB((const uint8_t*)gameinfo.data, gameinfo.size);
        addon = 1;
    }
    else if (ext == "bs") {
        if (!addoninfo.size) {
            jg_cb_log(JG_LOG_ERR,
                "BS-X BIOS ROM required for BS-X content, exiting...\n");
            return 0;
        }

        superFamicom.location = std::string(addoninfo.path);
        bsMemory.location = std::string(gameinfo.path);
        addon = 2;
    }
    else if (ext == "st") {
        if (!addoninfo.size) {
            jg_cb_log(JG_LOG_ERR,
                "Sufami Turbo ROM required for Sufami Turbo content, "
                "exiting...\n");
            return 0;
        }

        superFamicom.location = std::string(addoninfo.path);

        if (sufamiinfo.size) {
            sufamiTurboA.location = std::string(sufamiinfo.path);
            sufamiTurboB.location = std::string(gameinfo.path);
        }
        else {
            sufamiTurboA.location = std::string(gameinfo.path);
        }
        addon = 3;
    }
    else {
        superFamicom.location = std::string(gameinfo.path);
    }

    interface->unload();

    if (settings_bsnes[REGION].val == 1)
        interface->setRegion("NTSC");
    else if (settings_bsnes[REGION].val == 2)
        interface->setRegion("PAL");

    if (!interface->load())
      jg_cb_log(JG_LOG_ERR, "Failed to load ROM\n");

    inputSetup();

    aspectRatio();

    interface->setAudioQuality(settings_bsnes[RSQUAL].val);

    // Audio and timing adjustments
    if (interface->getRegion() == "PAL") {
        audinfo.spf = (SAMPLERATE / FRAMERATE_PAL) * CHANNELS;
        interface->setAudioSpf(audinfo.spf);
        jg_cb_frametime(TIMING_PAL);
    }
    else {
        interface->setAudioSpf(audinfo.spf);
        jg_cb_frametime(TIMING_NTSC);
    }

    interface->power(); // Power up!

    return 1;
}

int jg_game_unload(void) {
    // Save happens on interface->unload
    interface->unload();
    return 1;
}

int jg_state_load(const char *filename) {
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    std::vector<uint8_t> state((std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>());
    stream.close();
    return interface->unserialize(state);
}

void jg_state_load_raw(const void *data) {
    if (data) { }
}

int jg_state_save(const char *filename) {
    std::vector<uint8_t> state;
    interface->serialize(state);
    std::ofstream stream(filename, std::ios::out | std::ios::binary);
    if (stream.is_open()) {
        stream.write((const char*)state.data(), state.size());
        stream.close();
        return 1;
    }
    return 0;
}

const void* jg_state_save_raw(void) {
    return nullptr;
}

size_t jg_state_size(void) {
    return 0;
}

void jg_media_select(void) {
}

void jg_media_insert(void) {
}

void jg_cheat_clear(void) {
    interface->cheatsClear();
}

void jg_cheat_set(const char *code) {
    if (!interface->cheatsDecode(addon, code)) {
        jg_cb_log(JG_LOG_WRN, "Failed to decode cheat: %s\n", code);
    }
}

void jg_rehash(void) {
    aspectRatio();
    inputSetup();
}

void jg_data_push(uint32_t type, int port, const void *ptr, size_t size) {
    if (type || port || ptr || size) { }
}

// JG Functions that return values to the frontend
jg_coreinfo_t* jg_get_coreinfo(const char *sys) {
    if (sys) { }
    return &coreinfo;
}

jg_videoinfo_t* jg_get_videoinfo(void) {
    return &vidinfo;
}

jg_audioinfo_t* jg_get_audioinfo(void) {
    return &audinfo;
}

jg_inputinfo_t* jg_get_inputinfo(int port) {
    return &inputinfo[port];
}

jg_setting_t* jg_get_settings(size_t *numsettings) {
    *numsettings = sizeof(settings_bsnes) / sizeof(jg_setting_t);
    return settings_bsnes;
}

void jg_setup_video(void) {
    interface->setVideoBuffer((uint16_t*)vidinfo.buf);
    interface->setVideoCallback(&videoFrame);
}

void jg_setup_audio(void) {
    interface->setAudioBuffer((float*)audinfo.buf);
    interface->setAudioCallback(jg_cb_audio);
    interface->setAudioFrequency((double)SAMPLERATE);
}

void jg_set_inputstate(jg_inputstate_t *ptr, int port) {
    input_device[port] = ptr;
}

void jg_set_gameinfo(jg_fileinfo_t info) {
    gameinfo = info;
}

void jg_set_auxinfo(jg_fileinfo_t info, int index) {
    if (index == 0)
        addoninfo = info;
    else if (index == 1)
        sufamiinfo = info;
    return;
}

void jg_set_paths(jg_pathinfo_t paths) {
    pathinfo = paths;
}
