/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2024 Rupert Carmichael
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
    JG_PIXFMT_XRGB8888, // pixfmt
    VIDEO_WIDTH << 1,   // wmax
    VIDEO_HEIGHT << 1,  // hmax
    VIDEO_WIDTH,        // w
    VIDEO_HEIGHT,       // h
    0,                  // x
    0,                  // y
    VIDEO_WIDTH << 2,   // p
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
    { "region", "Region",
      "0 = Auto, 1 = NTSC, 2 = PAL",
      "Set the region to use",
      0, 0, 2, JG_SETTING_RESTART
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
    { "luminance", "Colour: Luminance",
      "0 = 0%, 1 = 10%, 2 = 20%, 3 = 30%, 4 = 40%, 5 = 50%, "
      "6 = 60%, 7 = 70%, 8 = 80%, 9 = 90%, 10 = 100%",
      "Adjust luminance level",
      10, 0, 10, 0
    },
    { "saturation", "Colour: Saturation",
      "0 = 0%, 1 = 10%, 2 = 20%, 3 = 30%, 4 = 40%, 5 = 50%, "
      "6 = 60%, 7 = 70%, 8 = 80%, 9 = 90%, 10 = 100%, "
      "11 = 110%, 12 = 120%, 13 = 130%, 14 = 140%, 15 = 150%, "
      "16 = 160%, 17 = 170%, 18 = 180%, 19 = 190%, 20 = 200%",
      "Adjust saturation level",
      10, 0, 20, 0
    },
    { "gamma", "Colour: Gamma",
      "0 = 100%, 1 = 110%, 2 = 120%, 3 = 130%, 4 = 140%, 5 = 150%, "
      "6 = 160%, 7 = 170%, 8 = 180%, 9 = 190%, 10 = 200%",
      "Adjust gamma level",
      2, 0, 10, 0
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
      0, 0, 1, JG_SETTING_RESTART
    },
    { "rsqual", "Resampler Quality",
      "0 = Fastest, 1 = Medium, 2 = Best",
      "Quality level for the internal resampler",
      0, 0, 2, 1
    },
    { "spc_interp", "SPC Interpolation Algorithm",
      "0 = Gaussian, 1 = Sinc",
      "Set the SPC700 sample interpolation algorithm",
      0, 0, 1, 0
    },
    { "hotfixes", "Hotfixes",
      "0 = Off, 1 = On",
      "Enable hotfixes for games that were released with fundamental bugs",
      0, 0, 1, JG_SETTING_RESTART
    },
    { "runahead", "Run-Ahead (Input Latency Reduction)",
      "N = Number of frames to run ahead",
      "Run N frames ahead to decrease input latency (heavy CPU load)",
      0, 0, 4, 0
    },
    { "cmptn_timer", "Competition Timer",
      "N = Minutes of game time (competition boards)",
      "Set the DIP Switches for Competition/Event boards to control the number "
      "of minutes of game time",
      6, 3, 18, 0
    }
};

enum {
    PORT1,
    PORT2,
    REGION,
    ASPECT,
    OVERSCAN_T,
    OVERSCAN_B,
    OVERSCAN_L,
    OVERSCAN_R,
    LUMINANCE,
    SATURATION,
    GAMMA,
    COPROC_DELAYSYNC,
    COPROC_PREFERHLE,
    RSQUAL,
    SPC_INTERP,
    HOTFIXES,
    RUNAHEAD,
    CMPTN_TIMER
};

// State data
static std::vector<uint8_t> state;

// MSU-1 file management
static std::ifstream msu_file_audio;
static std::ifstream msu_file_data;

static int hmult = 2;
static int vmult = 1;
static int ss_offset_x = 0;
static int ss_offset_y = 0;
static unsigned numplugged = 2;

static uint8_t addon = 0;

static struct Location {
    std::string location;
} bsMemory, superFamicom, sufamiTurboA, sufamiTurboB;

static void logCallback(void*, int level, std::string& text) {
    jg_cb_log(level, text.c_str());
}

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
            aspect_ratio = Bsnes::getRegion() == Bsnes::Region::PAL ?
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

static bool fileOpenV(void*, std::string name, std::vector<uint8_t>& v) {
    std::string path;
    bool required = false;

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
    else if (name == "download.ram") {
        path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".psr";
    }
    else if (name == "time.rtc") {
        path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".rtc";
    }
    else if (name == "sufamiA.ram") {
        if (sufamiinfo.size) { // There are two Sufami Turbo carts inserted
            path = std::string(pathinfo.save) + "/" +
                std::string(sufamiinfo.name) + ".srm";
        }
        else {
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        }
    }
    else if (name == "sufamiB.ram") {
        path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".srm";
    }
    else {
        jg_cb_log(JG_LOG_WRN, "File load attempt: %s\n", name.c_str());
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

static bool fileOpenS(void*, std::string name, std::stringstream& ss) {
    std::string path = std::string(pathinfo.core) + "/" + name;
    std::ifstream stream(path, std::ios::in | std::ios::binary);

    if (!stream.is_open()) {
        jg_cb_log(JG_LOG_ERR, "Failed to load file: %s\n", path.c_str());
    }

    ss << stream.rdbuf();
    stream.close();

    return true;
}

static bool fileOpenMsu(void*, std::string name, std::istream **is) {
    std::string path = superFamicom.location;

    if (name == "msu1/data.rom") {
        path = path.substr(0, path.find_last_of(".")) + ".msu";
        msu_file_data.close();
        msu_file_data = std::ifstream(path, std::ios::in | std::ios::binary);
        *is = msu_file_data.is_open() ? &msu_file_data : nullptr;
        return msu_file_data.is_open();
    }
    else if (name.find("msu1/track") != std::string::npos) {
        path = path.substr(0, path.find_last_of(".")) +
            name.substr(name.find_first_of("track") + 5);
        msu_file_audio.close();
        msu_file_audio = std::ifstream(path, std::ios::in | std::ios::binary);
        *is = msu_file_audio.is_open() ? &msu_file_audio : nullptr;
        return msu_file_audio.is_open();
    }

    return false;
}

static void fileWrite(void*, std::string name, const uint8_t *data,
    unsigned size) {

    std::string path;

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
    else if (name == "program.flash") {
        return; // writes are not flushed to disk in bsnes
    }
    else if (name == "sufamiA.ram") {
        if (sufamiinfo.size) // There are two Sufami Turbo carts inserted
            path = std::string(pathinfo.save) + "/" +
                std::string(sufamiinfo.name) + ".srm";
        else
            path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
    }
    else if (name == "sufamiB.ram") {
        path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".srm";
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

static void videoFrame(const void*, unsigned w, unsigned h, unsigned pitch) {
    hmult = w / VIDEO_WIDTH;
    vmult = h / VIDEO_HEIGHT;

    vidinfo.x = settings_bsnes[OVERSCAN_L].val * hmult;
    w -= vidinfo.x + (settings_bsnes[OVERSCAN_R].val * hmult);

    vidinfo.y = settings_bsnes[OVERSCAN_T].val * vmult;
    h -= vidinfo.y + (settings_bsnes[OVERSCAN_B].val * vmult);

    vidinfo.w = w;
    vidinfo.h = h;
    vidinfo.p = pitch;
}

static void audioFrame(const void*, size_t numsamps) {
    jg_cb_audio(numsamps);
}

static int pollGamepad(const void *, unsigned port, unsigned) {
    if (port >= numplugged) {
      return 0;
    }

    int b = 0;

    if (input_device[port]->button[0]) b |= Bsnes::Input::Gamepad::Up;
    if (input_device[port]->button[1]) b |= Bsnes::Input::Gamepad::Down;
    if (input_device[port]->button[2]) b |= Bsnes::Input::Gamepad::Left;
    if (input_device[port]->button[3]) b |= Bsnes::Input::Gamepad::Right;
    if (input_device[port]->button[4]) b |= Bsnes::Input::Gamepad::Select;
    if (input_device[port]->button[5]) b |= Bsnes::Input::Gamepad::Start;
    if (input_device[port]->button[6]) b |= Bsnes::Input::Gamepad::A;
    if (input_device[port]->button[7]) b |= Bsnes::Input::Gamepad::B;
    if (input_device[port]->button[8]) b |= Bsnes::Input::Gamepad::X;
    if (input_device[port]->button[9]) b |= Bsnes::Input::Gamepad::Y;
    if (input_device[port]->button[10]) b |= Bsnes::Input::Gamepad::L;
    if (input_device[port]->button[11]) b |= Bsnes::Input::Gamepad::R;

    return b;
}

static int pollMouse(const void*, unsigned port, unsigned phase) {
    int b = 0;

    switch(phase) {
        case 0: { // X
            input_device[port]->rel[0] /= 2;
            b = input_device[port]->rel[0];
            break;
        }
        case 1: { // Y
            input_device[port]->rel[1] /= 2;
            b = input_device[port]->rel[1];
            break;
        }
        case 2: { // Buttons (pre-shifted)
            if (input_device[port]->button[0])
                b |= Bsnes::Input::Mouse::ButtonL;
            if (input_device[port]->button[1])
                b |= Bsnes::Input::Mouse::ButtonR;
            break;
        }
    }

    return b;
}

static int pollLightgun(const void*, unsigned port, unsigned phase) {
    switch (phase) {
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

static void inputSetup(void) {
    // Set up inputs
    numplugged = 2;
    unsigned multitap = 0;
    unsigned port[] = {
        unsigned(settings_bsnes[PORT1].val),
        unsigned(settings_bsnes[PORT2].val)
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
            numplugged = multitap;
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

    for (unsigned i = 0; i < 2; ++i) {
        switch (port[i]) {
            default:
            case Bsnes::Input::Device::Unconnected:
            case Bsnes::Input::Device::Gamepad: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_PAD);
                Bsnes::setInputSpec({i, 1, nullptr, pollGamepad});
                break;
            }
            case Bsnes::Input::Device::Mouse: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_MOUSE);
                Bsnes::setInputSpec({i, port[i], nullptr, pollMouse});
                break;
            }
            case Bsnes::Input::Device::Multitap: {
                for (unsigned j = 1; j < (multitap ? multitap : NUMINPUTS); ++j)
                    inputinfo[j] = jg_snes_inputinfo(j, JG_SNES_PAD);
                Bsnes::setInputSpec({1, port[i], nullptr, pollGamepad});
                break;
            }
            case Bsnes::Input::Device::SuperScope: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_SUPERSCOPE);
                Bsnes::setInputSpec({i, port[i], nullptr, pollLightgun});
                break;
            }
            case Bsnes::Input::Device::Justifier: {
                inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_JUSTIFIER);
                Bsnes::setInputSpec({i, port[i], nullptr, pollLightgun});
                ss_offset_x = ss_offset_y = 0;
                break;
            }
        }
    }
}

static inline std::vector<uint8_t> bufToVec(void *data, size_t size) {
    uint8_t *buf = (uint8_t*)data;
    return std::vector<uint8_t>(buf, buf + size);
}

static bool loadRom(void*, unsigned id) {
    if (id == Bsnes::GameType::SuperFamicom) {
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

        Bsnes::setRomSuperFamicom(rom, superFamicom.location);
        return true;
    }
    else if (id == Bsnes::GameType::BSX) {
        std::vector<uint8_t> rom = bufToVec(gameinfo.data, gameinfo.size);
        if (rom.size() < 0x8000) return false;
        Bsnes::setRomBSMemory(rom, bsMemory.location);
        return true;
    }
    else if (id == Bsnes::GameType::SufamiTurboA) {
        std::vector<uint8_t> rom;

        if (sufamiinfo.size)
            rom = bufToVec(sufamiinfo.data, sufamiinfo.size);
        else
            rom = bufToVec(gameinfo.data, gameinfo.size);

        if (rom.size() < 0x20000) return false;
        Bsnes::setRomSufamiTurboA(rom, sufamiTurboA.location);
        return true;
    }
    else if (id == Bsnes::GameType::SufamiTurboB) {
        if (!sufamiinfo.size)
            return false;

        std::vector<uint8_t> rom = bufToVec(gameinfo.data, gameinfo.size);
        if (rom.size() < 0x20000) return false;
        Bsnes::setRomSufamiTurboB(rom, sufamiTurboB.location);
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
    // Set callbacks
    Bsnes::setOpenFileCallback(nullptr, fileOpenV);
    Bsnes::setOpenStreamCallback(nullptr, fileOpenS);
    Bsnes::setOpenMsuCallback(nullptr, fileOpenMsu);
    Bsnes::setLogCallback(nullptr, logCallback);
    Bsnes::setRomLoadCallback(nullptr, loadRom);
    Bsnes::setWriteCallback(nullptr, fileWrite);

    // Configuration
    Bsnes::setCoprocDelayedSync(settings_bsnes[COPROC_DELAYSYNC].val);
    Bsnes::setCoprocPreferHLE(settings_bsnes[COPROC_PREFERHLE].val);
    Bsnes::setHotfixes(settings_bsnes[HOTFIXES].val);
    Bsnes::setVideoColourParams(settings_bsnes[LUMINANCE].val * 10,
        settings_bsnes[SATURATION].val * 10,
        settings_bsnes[GAMMA].val * 10 + 100);
    Bsnes::setSpcInterpolation(settings_bsnes[SPC_INTERP].val);

    /* DIP Switches only apply to competition boards for now, but if NSS is
       ever supported, the values will need to be set more intelligently.
       In this case we remove 3 from the value because the value of 0 is
       actually 3 minutes, and the setting value is set in a human-readable
       form with the 3 minutes already added.
    */
    Bsnes::setDIPSwitches((settings_bsnes[CMPTN_TIMER].val & 0xff) - 3);

    return 1;
}

void jg_deinit(void) {
}

void jg_reset(int hard) {
    if (hard)
        Bsnes::power();
    else
        Bsnes::reset();
}

void jg_exec_frame(void) {
    if (settings_bsnes[RUNAHEAD].val)
        Bsnes::runAhead(settings_bsnes[RUNAHEAD].val);
    else
        Bsnes::run();
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
        Bsnes::setRomGB((const uint8_t*)gameinfo.data, gameinfo.size);
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

    if (settings_bsnes[REGION].val == 1)
        Bsnes::setRegion(Bsnes::Region::NTSC);
    else if (settings_bsnes[REGION].val == 2)
        Bsnes::setRegion(Bsnes::Region::PAL);

    if (!Bsnes::load())
      jg_cb_log(JG_LOG_ERR, "Failed to load ROM\n");

    aspectRatio();

    // Audio and timing adjustments
    if (Bsnes::getRegion() == Bsnes::Region::PAL) {
        audinfo.spf = (SAMPLERATE / FRAMERATE_PAL) * CHANNELS;
        jg_cb_frametime(TIMING_PAL);
    }
    else {
        jg_cb_frametime(TIMING_NTSC);
    }

    // Power up!
    Bsnes::power();

    // Reserve data for states
    state.reserve(Bsnes::serializeSize());

    // Set up input devices
    inputSetup();

    return 1;
}

int jg_game_unload(void) {
    Bsnes::save();
    Bsnes::unload();

    state.clear();

    if (msu_file_audio.is_open())
        msu_file_audio.close();

    if (msu_file_data.is_open())
        msu_file_data.close();

    return 1;
}

int jg_state_load(const char *filename) {
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    if (stream.is_open()) {
        state = std::vector<uint8_t>((std::istreambuf_iterator<char>(stream)),
            std::istreambuf_iterator<char>());
        stream.close();
        return Bsnes::unserialize(state.data(), state.size());
    }
    return 0;
}

void jg_state_load_raw(const void*) {
}

int jg_state_save(const char *filename) {
    unsigned size = Bsnes::serialize(state.data());
    if (!size)
        return 0;

    std::ofstream stream(filename, std::ios::out | std::ios::binary);
    if (stream.is_open()) {
        stream.write((const char*)state.data(), size);
        stream.close();
        return 1;
    }
    return 0;
}

const void* jg_state_save_raw(void) {
    return nullptr;
}

size_t jg_state_size(void) {
    return Bsnes::serializeSize();
}

void jg_media_select(void) {
}

void jg_media_insert(void) {
}

void jg_cheat_clear(void) {
    Bsnes::cheatsClear();
}

void jg_cheat_set(const char *code) {
    Bsnes::cheatsSetCheat(code);
}

void jg_rehash(void) {
    aspectRatio();
    inputSetup();
    Bsnes::setVideoColourParams(settings_bsnes[LUMINANCE].val * 10,
        settings_bsnes[SATURATION].val * 10,
        settings_bsnes[GAMMA].val * 10 + 100);
    Bsnes::setSpcInterpolation(settings_bsnes[SPC_INTERP].val);
}

void jg_data_push(uint32_t, int, const void*, size_t) {
}

jg_coreinfo_t* jg_get_coreinfo(const char*) {
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
    Bsnes::setVideoSpec({(uint32_t*)vidinfo.buf, nullptr, &videoFrame});
}

void jg_setup_audio(void) {
    Bsnes::setAudioSpec({double(SAMPLERATE), audinfo.spf,
        unsigned(settings_bsnes[RSQUAL].val),
        (float*)audinfo.buf, nullptr, &audioFrame});
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
