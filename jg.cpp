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
#include <fstream>
#include <vector>

#include <jg/jg.h>
#include <jg/jg_snes.h>

#include "serializer.hpp"
#include "interface.hpp"
#include "settings.hpp"

#include "gamedb.hpp"

#define SAMPLERATE 48000
#define FRAMERATE 60
#define FRAMERATE_PAL 50
#define CHANNELS 2
#define NUMINPUTS 5

#define ASPECT_NTSC 1.3061224
#define ASPECT_PAL 1.4257812

#define TIMING_NTSC 60.098812
#define TIMING_PAL 50.006979

static jg_cb_audio_t jg_cb_audio;
static jg_cb_frametime_t jg_cb_frametime;
static jg_cb_log_t jg_cb_log;
static jg_cb_rumble_t jg_cb_rumble;

static jg_coreinfo_t coreinfo = {
    "bsnes", "bsnes-jg", "1.0.1", "snes", NUMINPUTS, 0
};

static jg_videoinfo_t vidinfo = {
    JG_PIXFMT_RGB5551,  // pixfmt
    512,                // wmax
    480,                // hmax
    256,                // w
    224,                // h
    0,                  // x
    8,                  // y
    1024,               // p
    8.0/7.0,            // aspect
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
    { "aspect_ratio", "Aspect Ratio",
      "0 = Auto Region, 1 = 8:7, 2 = NTSC, 3 = PAL",
      "Set the aspect ratio",
      0, 0, 3, 0
    },
    { "coproc_delaysync", "Delay LLE Coprocessor Sync",
      "0 = Off, 1 = On",
      "Delay syncing Low Level Emulated coprocessors for a performance "
      "increase at the cost of a slight reduction in accuracy",
      0, 0, 1, 1
    },
    { "coproc_preferhle", "Prefer HLE Coprocessors",
      "0 = Off, 1 = On",
      "Prefer High Level Emulation of coprocessors when available",
      1, 0, 1, 1
    },
    { "rsqual", "Resampler Quality",
      "0 = Fastest, 1 = Medium, 2 = Best",
      "Quality level for the internal resampler",
      0, 0, 2, 1
    }
};

enum {
    ASPECT,
    COPROC_DELAYSYNC,
    COPROC_PREFERHLE,
    RSQUAL,
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
    switch (settings_bsnes[ASPECT].val) {
        default: case 0: { // Auto Region
            vidinfo.aspect = interface->getRegion() == "PAL" ?
                ASPECT_PAL : ASPECT_NTSC;
            break;
        }
        case 1: { // 8:7
            vidinfo.aspect = 8.0/7.0;
            break;
        }
        case 2: { // NTSC
            vidinfo.aspect = ASPECT_NTSC;
            break;
        }
        case 3: { // PAL
            vidinfo.aspect = ASPECT_PAL;
            break;
        }
    }
}

static std::ifstream fileOpen(unsigned id, std::string name) {
    std::string path;
    bool required = false;

    if (id == 0) {
        if (name == "boards.bml") {
            path = std::string(pathinfo.core) + "/boards.bml";
            required = true;
        }
        else if (name == "BS Memory.bml") {
            path = std::string(pathinfo.core) + "/BS Memory.bml";
            required = true;
        }
        else if (name == "Sufami Turbo.bml") {
            path = std::string(pathinfo.core) + "/Sufami Turbo.bml";
            required = true;
        }
        else if (name == "Super Famicom.bml") {
            path = std::string(pathinfo.core) + "/Super Famicom.bml";
            required = true;
        }
    }
    else if (id == 1) { // SFC
        if (name == "program.rom") {
            path = std::string(gameinfo.fname);
            required = true;
        }
        else if (name == "save.ram") {
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
        else if (name == "msu1/data.rom") {
            path = superFamicom.location;
            path = path.substr(0, path.find_last_of(".")) + ".msu";
        }
        else if (name.find("msu1/track") != std::string::npos) {
            path = superFamicom.location;
            path = path.substr(0, path.find_last_of(".")) +
                name.substr(name.find_first_of("track") + 5);
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
        else {
            jg_cb_log(JG_LOG_WRN, "File load attempt: %s\n", name.c_str());
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
    if (required && !stream.is_open()) {
        jg_cb_log(JG_LOG_ERR, "Failed to load file: %s\n", path.c_str());
    }
    return stream;
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

static void videoFrame(const uint16_t *data, unsigned pitch, unsigned w, unsigned h) {
    hmult = w / 256;
    vmult = h / 240;
    vidinfo.y = 8 * vmult;
    h -= 2 * vidinfo.y;

    vidinfo.w = w;
    vidinfo.h = h;
    vidinfo.p = pitch / 2; // Divide by pixel size - 16-bit pixels == 2
    vidinfo.buf = const_cast<void*>(reinterpret_cast<const void*>(data));
}

static uint8_t imap[12] = { 0, 1, 2, 3, 7, 6, 9, 8, 10, 11, 4, 5 };

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
                int ret = input_device[port]->axis[0] / 2;
                input_device[port]->axis[0] -= ret;
                return ret;
            }
            case 1: { // Y
                int ret = input_device[port]->axis[1] / 2;
                input_device[port]->axis[1] -= ret;
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
    std::vector<uint8_t> ret;

    for (size_t i = 0; i < size; ++i)
        ret.push_back(buf[i]);

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
    interface->setOpenCallback(&fileOpen);
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

    if (!interface->load())
      jg_cb_log(JG_LOG_ERR, "Failed to load ROM\n");

    // Set up inputs
    int multitap = 0;
    for (size_t i = 0; i < db_multitap_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_multitap_games[i].md5) {
            multitap = db_multitap_games[i].players;
            break;
        }
    }

    bool mouse = false;
    for (size_t i = 0; i < db_mouse_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_mouse_games[i]) {
            mouse = true;
            break;
        }
    }

    bool superscope = false;
    for (size_t i = 0; i < db_superscope_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_superscope_games[i].md5) {
            superscope = true;
            ss_offset_x = db_superscope_games[i].x;
            ss_offset_y = db_superscope_games[i].y;
            break;
        }
    }

    bool justifier = false;
    for (size_t i = 0; i < db_justifier_games.size(); ++i) {
        if ((std::string)gameinfo.md5 == db_justifier_games[i]) {
            justifier = true;
            break;
        }
    }

    // Default input devices are SNES Controllers
    inputinfo[0] = jg_snes_inputinfo(0, JG_SNES_PAD);
    interface->connect(SuperFamicom::ID::Port::Controller1,
        SuperFamicom::ID::Device::Gamepad);

    inputinfo[1] = jg_snes_inputinfo(1, JG_SNES_PAD);
    interface->connect(SuperFamicom::ID::Port::Controller2,
        SuperFamicom::ID::Device::Gamepad);

    if (multitap) {
        for (int i = 2; (i < multitap) && (i < 5); ++i)
            inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_PAD);

        interface->connect(SuperFamicom::ID::Port::Controller2,
            SuperFamicom::ID::Device::SuperMultitap);
    }
    else if (mouse) { // Plug in a mouse if the game supports it
        inputinfo[0] = jg_snes_inputinfo(0, JG_SNES_MOUSE);
        interface->connect(SuperFamicom::ID::Port::Controller1,
            SuperFamicom::ID::Device::Mouse);
    }
    else if (superscope) { // Plug in a Super Scope if the game supports it
        inputinfo[1] = jg_snes_inputinfo(1, JG_SNES_SUPERSCOPE);
        interface->connect(SuperFamicom::ID::Port::Controller2,
            SuperFamicom::ID::Device::SuperScope);
    }
    else if (justifier) { // Plug in a Justifier if the game supports it
        inputinfo[1] = jg_snes_inputinfo(1, JG_SNES_JUSTIFIER);
        interface->connect(SuperFamicom::ID::Port::Controller2,
            SuperFamicom::ID::Device::Justifier);
    }

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
    serializer s(state.data(), state.size());
    return interface->unserialize(s);
}

void jg_state_load_raw(const void *data) {
    if (data) { }
}

int jg_state_save(const char *filename) {
    serializer s = interface->serialize();
    std::ofstream stream(filename, std::ios::out | std::ios::binary);
    if (stream.is_open()) {
        stream.write((const char*)s.data(), s.size());
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
}

void jg_input_audio(int port, const int16_t *buf, size_t numsamps) {
    if (port || buf || numsamps) { }
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
