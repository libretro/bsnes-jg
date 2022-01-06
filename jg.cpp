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
#include <sstream>
#include <optional>

#include <jg/jg.h>
#include <jg/jg_snes.h>

#include "src/audio.hpp"
#include "src/cheat.hpp"
#include "src/heuristics.hpp"
#include "src/interface.hpp"
#include "src/markup.hpp"
#include "src/sha256.hpp"
#include "src/sfc.hpp"
#include "src/settings.hpp"

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
static jg_cb_settings_read_t jg_cb_settings_read;

static jg_coreinfo_t coreinfo = {
    "bsnes", "bsnes-jg", "1.0.0-pre1", "snes", NUMINPUTS, JG_HINT_VIDEO_INTERNAL
};

static jg_videoinfo_t vidinfo = {
    JG_PIXFMT_RGB1555,  // pixfmt
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
static jg_setting_t settings_bsnes[] = { // name, default, min, max
    // 0 = Auto Region, 1 = 8:7, 2 = NTSC, 3 = PAL
    { "aspect_ratio", "", 0, 0, 3 },
    { "coproc_delaysync", "", 0, 0, 1 }, // 0 = Off, 1 = On
    { "coproc_preferhle", "", 1, 0, 1 }, // 0 = Off, 1 = On
    { "rsqual", "", 0, 0, 2 }, // 0 = Fastest, 1 = Medium, 2 = Best
};

enum {
    ASPECT,
    COPROC_DELAYSYNC,
    COPROC_PREFERHLE,
    RSQUAL,
};

static std::vector<std::string> cheatList;

static int hmult = 2;
static int vmult = 1;
static int ss_offset_x = 0;
static int ss_offset_y = 0;

static bool addon = false;

static Emulator::Interface *emulator;

struct Program : Emulator::Platform {
    Program();
    ~Program() override;

    Emulator::Platform::Load load(unsigned id, std::string name,
        std::string type, std::vector<std::string> options = {}) override;
    std::ifstream fopen(unsigned id, std::string name) override;
    std::vector<uint8_t> mopen(unsigned id, std::string name) override;
    std::string stropen(unsigned id, std::string name) override;
    void write(unsigned id, std::string name, const uint8_t *data,
        unsigned size) override;
    void videoFrame(const uint16_t *data, unsigned pitch, unsigned width,
        unsigned height) override;
    void audioFrame(unsigned numsamps) override;
    int16_t inputPoll(unsigned port, unsigned device, unsigned input) override;

    void load();
    std::vector<uint8_t> loadFile(void *data, size_t size);
    bool loadSuperFamicom(std::string location);
    bool loadBSMemory(std::string location);
    bool loadSufamiTurboA(std::string location);
    bool loadSufamiTurboB(std::string location);

    void save();

    void hackPatchMemory(std::vector<uint8_t>& data);

public:
    struct Game {
        std::string option;
        std::string location;
        std::string manifest;
    };

    struct SuperFamicom : Game {
        std::string title;
        std::string region;
        std::vector<uint8_t> program;
        std::vector<uint8_t> data;
        std::vector<uint8_t> expansion;
        std::vector<uint8_t> firmware;
    } superFamicom;

    struct GameBoy : Game {
        std::vector<uint8_t> program;
    } gameBoy;

    struct BSMemory : Game {
        std::vector<uint8_t> program;
    } bsMemory;

    struct SufamiTurbo : Game {
        std::vector<uint8_t> program;
    } sufamiTurboA, sufamiTurboB;
};

static Program *program = nullptr;

Program::Program() {
    Emulator::platform = this;
}

Program::~Program() {
    delete emulator;
}

void Program::save() {
    if (!emulator->loaded()) return;
    emulator->save();
}

void Program::load() {
    emulator->unload();
    emulator->load();
}

Emulator::Platform::Load Program::load(unsigned id, std::string name,
    std::string type, std::vector<std::string> options) {

    if (id == 1) {
        if (loadSuperFamicom(superFamicom.location)) {
            return {id, superFamicom.region};
        }
    }
    else if (id == 2) {
    }
    else if (id == 3) {
        if (loadBSMemory(bsMemory.location)) {
            return { id, "" };
        }
    }
    else if (id == 4) {
        if (loadSufamiTurboA(sufamiTurboA.location)) {
            return { id, "" };
        }
    }
    else if (id == 5) {
        if (loadSufamiTurboB(sufamiTurboB.location)) {
            return { id, "" };
        }
    }
    return { id, "" };
}

std::ifstream Program::fopen(unsigned id, std::string name) {
    std::string path;
    bool required = false;

    if (id == 0) {
        if (name == "boards.bml") {
            path = std::string(pathinfo.core) + "/boards.bml";
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

std::vector<uint8_t> Program::mopen(unsigned id, std::string name) {
    if (id == 1) { // Super Famicom
        if (name == "program.rom") {
            return superFamicom.program;
        }
        else if (name == "data.rom") {
            return superFamicom.data;
        }
        else if (name == "expansion.rom") {
            return superFamicom.expansion;
        }
    }
    else if (id == 2) { // GB
        if (name == "program.rom") {
            std::vector<uint8_t> rom;
            rom = loadFile(gameinfo.data, gameinfo.size);

            if (rom.size() < 0x4000)
                jg_cb_log(JG_LOG_ERR, "Game Boy ROM size too small\n");

            auto heuristics = Heuristics::GameBoy(rom, gameinfo.path);

            gameBoy.manifest = heuristics.manifest();
            gameBoy.location = gameinfo.path;
            gameBoy.program = rom;
            return gameBoy.program;
        }
    }
    else if (id == 3) { // BS-X
        if (name == "program.rom" || name == "program.flash") {
            return bsMemory.program;
        }
    }
    else if (id == 4) { // Sufami Turbo Slot A
        if (name == "program.rom") {
            return sufamiTurboA.program;
        }
    }
    else if (id == 5) { // Sufami Turbo Slot B
        if (name == "program.rom") {
            return sufamiTurboB.program;
        }
    }
    return {};
}

std::string Program::stropen(unsigned id, std::string name) {
    if (id == 1) { // Super Famicom
        if (name == "manifest.bml") {
            return superFamicom.manifest;
        }
    }
    if (id == 3) { // BS-X
        if (name == "manifest.bml") {
            return bsMemory.manifest;
        }
    }
    if (id == 4) { // Sufami Turbo Slot A
        if (name == "manifest.bml") {
            return sufamiTurboA.manifest;
        }
    }
    if (id == 5) { // Sufami Turbo Slot B
        if (name == "manifest.bml") {
            return sufamiTurboB.manifest;
        }
    }
    return {};
}

void Program::write(unsigned id, std::string name, const uint8_t *data,
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

void Program::videoFrame(const uint16_t *data, unsigned pitch, unsigned width,
    unsigned height) {
    hmult = width / 256;
    vmult = height / 240;
    vidinfo.y = 8 * vmult;
    height -= 2 * vidinfo.y;

    vidinfo.w = width;
    vidinfo.h = height;
    vidinfo.p = pitch / 2; // Divide by pixel size - 16-bit pixels == 2
    vidinfo.buf = (void*)data;
}

void Program::audioFrame(unsigned numsamps) {
    jg_cb_audio(numsamps);
}

static uint8_t imap[12] = { 0, 1, 2, 3, 7, 6, 9, 8, 10, 11, 4, 5 };

static int16_t pollInputDevices(unsigned port, unsigned device, unsigned input) {
    //print("port: ", port, " device: ", device, " input: ", input, "\n");
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

int16_t Program::inputPoll(unsigned port, unsigned device, unsigned input) {
    return pollInputDevices(port, device, input);
}

std::vector<uint8_t> Program::loadFile(void *data, size_t size) {
    uint8_t *buf = (uint8_t*)data;
    std::vector<uint8_t> ret;

    for (size_t i = 0; i < size; ++i)
        ret.push_back(buf[i]);

    return ret;
}

bool Program::loadSuperFamicom(std::string location) {
    std::vector<uint8_t> rom = addon ?
        loadFile(addoninfo.data, addoninfo.size) :
        loadFile(gameinfo.data, gameinfo.size);

    if (rom.size() < 0x8000) return false;

    //assume ROM and IPS agree on whether a copier header is present
    //superFamicom.patched = applyPatchIPS(rom, location);
    if ((rom.size() & 0x7fff) == 512) {
        //remove copier header
        rom.erase(rom.begin(), rom.begin() + 512);
    }

    auto heuristics = Heuristics::SuperFamicom(rom, location);
    auto sha256 = sha256_digest(rom.data(), rom.size());

    superFamicom.title = heuristics.title();
    superFamicom.region = heuristics.videoRegion();

    std::string dbpath = std::string(pathinfo.core) + "/Super Famicom.bml";
    std::string manifest = BML::gendoc(dbpath, "game", "sha256", sha256);

    if (manifest.empty()) {
        superFamicom.manifest = heuristics.manifest();
    }
    else {
        //the internal ROM header title is not present in the database, but
        //is needed for internal core overrides
        manifest += "  title: " + superFamicom.title + "\n";
        superFamicom.manifest = manifest;
    }

    hackPatchMemory(rom);
    superFamicom.location = location;

    unsigned offset = 0;
    if (auto size = heuristics.programRomSize()) {
        superFamicom.program.resize(size);
        std::memcpy(&superFamicom.program[0], &rom[offset], size);
        offset += size;
    }
    if (auto size = heuristics.dataRomSize()) {
        superFamicom.data.resize(size);
        std::memcpy(&superFamicom.data[0], &rom[offset], size);
        offset += size;
    }
    if (auto size = heuristics.expansionRomSize()) {
        superFamicom.expansion.resize(size);
        std::memcpy(&superFamicom.expansion[0], &rom[offset], size);
        offset += size;
    }
    if (auto size = heuristics.firmwareRomSize()) {
        superFamicom.firmware.resize(size);
        std::memcpy(&superFamicom.firmware[0], &rom[offset], size);
        offset += size;
    }
    return true;
}

bool Program::loadBSMemory(std::string location) {
    std::vector<uint8_t> rom;
    rom = loadFile(gameinfo.data, gameinfo.size);

    if (rom.size() < 0x8000) return false;

    auto heuristics = Heuristics::BSMemory(rom, location);
    auto sha256 = sha256_digest(rom.data(), rom.size());

    std::string dbpath = std::string(pathinfo.core) + "/BS Memory.bml";
    std::string manifest = BML::gendoc(dbpath, "game", "sha256", sha256);

    bsMemory.manifest = manifest.empty() ? heuristics.manifest() : manifest;
    bsMemory.location = location;

    bsMemory.program = rom;
    return true;
}

bool Program::loadSufamiTurboA(std::string location) {
    std::vector<uint8_t> rom;

    if (sufamiinfo.size)
        rom = loadFile(sufamiinfo.data, sufamiinfo.size);
    else
        rom = loadFile(gameinfo.data, gameinfo.size);

    if (rom.size() < 0x20000) return false;

    auto heuristics = Heuristics::SufamiTurbo(rom, location);
    auto sha256 = sha256_digest(rom.data(), rom.size());

    std::string dbpath = std::string(pathinfo.core) + "/Sufami Turbo.bml";
    std::string manifest = BML::gendoc(dbpath, "game", "sha256", sha256);

    sufamiTurboA.manifest = manifest.empty() ? heuristics.manifest() : manifest;
    sufamiTurboA.location = std::string(gameinfo.path);

    sufamiTurboA.program = rom;
    return true;
}

bool Program::loadSufamiTurboB(std::string location) {
    if (!sufamiinfo.size)
        return false;

    std::vector<uint8_t> rom;
    rom = loadFile(gameinfo.data, gameinfo.size);

    if (rom.size() < 0x20000) return false;

    auto heuristics = Heuristics::SufamiTurbo(rom, location);
    auto sha256 = sha256_digest(rom.data(), rom.size());

    std::string dbpath = std::string(pathinfo.core) + "/Sufami Turbo.bml";
    std::string manifest = BML::gendoc(dbpath, "game", "sha256", sha256);

    sufamiTurboB.manifest = manifest.empty() ? heuristics.manifest() : manifest;
    sufamiTurboB.location = location;

    sufamiTurboB.program = rom;
    return true;
}

void Program::hackPatchMemory(std::vector<uint8_t>& data) {
    auto title = superFamicom.title;

    if (title == "Satellaview BS-X" && data.size() >= 0x100000) {
        //BS-X: Sore wa Namae o Nusumareta Machi no Monogatari (JPN) (1.1)
        //disable limited play check for BS Memory flash cartridges
        //benefit: allow locked out BS Memory flash games to play without manual
        //header patching.
        //detriment: BS Memory ROM cartridges will cause the game to hang in the
        //load menu
        if (data[0x4a9b] == 0x10) data[0x4a9b] = 0x80;
        if (data[0x4d6d] == 0x10) data[0x4d6d] = 0x80;
        if (data[0x4ded] == 0x10) data[0x4ded] = 0x80;
        if (data[0x4e9a] == 0x10) data[0x4e9a] = 0x80;
    }
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

void jg_set_cb_settings_read(jg_cb_settings_read_t func) {
    jg_cb_settings_read = func;
}

int jg_init() {
    jg_cb_settings_read(settings_bsnes,
        sizeof(settings_bsnes) / sizeof(jg_setting_t));
    emulator = new SuperFamicom::Interface;
    program = new Program;

    return 1;
}

void jg_deinit() {
    delete program;
}

void jg_reset(int hard) {
    emulator->reset();
}

void jg_exec_frame() {
    emulator->run();
}

int jg_game_load() {
    SuperFamicom::configuration.coprocessor.delayedSync =
        (bool)settings_bsnes[COPROC_DELAYSYNC].value;
    SuperFamicom::configuration.coprocessor.preferHLE =
        (bool)settings_bsnes[COPROC_PREFERHLE].value;

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

        program->superFamicom.location = std::string(addoninfo.path);
        program->gameBoy.location = std::string(gameinfo.path);
        addon = true;
    }
    else if (ext == "bs") {
        if (!addoninfo.size) {
            jg_cb_log(JG_LOG_ERR,
                "BS-X BIOS ROM required for BS-X content, exiting...\n");
            return 0;
        }

        program->superFamicom.location = std::string(addoninfo.path);
        program->bsMemory.location = std::string(gameinfo.path);
        addon = true;
    }
    else if (ext == "st") {
        if (!addoninfo.size) {
            jg_cb_log(JG_LOG_ERR,
                "Sufami Turbo ROM required for Sufami Turbo content, "
                "exiting...\n");
            return 0;
        }

        program->superFamicom.location = std::string(addoninfo.path);

        if (sufamiinfo.size) {
            program->sufamiTurboA.location = std::string(sufamiinfo.path);
            program->sufamiTurboB.location = std::string(gameinfo.path);
        }
        else {
            program->sufamiTurboA.location = std::string(gameinfo.path);
        }
        addon = true;
    }
    else {
        program->superFamicom.location = std::string(gameinfo.path);
    }

    program->load();

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
    emulator->connect(SuperFamicom::ID::Port::Controller1,
        SuperFamicom::ID::Device::Gamepad);

    inputinfo[1] = jg_snes_inputinfo(1, JG_SNES_PAD);
    emulator->connect(SuperFamicom::ID::Port::Controller2,
        SuperFamicom::ID::Device::Gamepad);

    if (multitap) {
        for (int i = 2; (i < multitap) && (i < 5); ++i)
            inputinfo[i] = jg_snes_inputinfo(i, JG_SNES_PAD);

        emulator->connect(SuperFamicom::ID::Port::Controller2,
            SuperFamicom::ID::Device::SuperMultitap);
    }
    else if (mouse) { // Plug in a mouse if the game supports it
        inputinfo[0] = jg_snes_inputinfo(0, JG_SNES_MOUSE);
        emulator->connect(SuperFamicom::ID::Port::Controller1,
            SuperFamicom::ID::Device::Mouse);
    }
    else if (superscope) { // Plug in a Super Scope if the game supports it
        inputinfo[1] = jg_snes_inputinfo(1, JG_SNES_SUPERSCOPE);
        emulator->connect(SuperFamicom::ID::Port::Controller2,
            SuperFamicom::ID::Device::SuperScope);
    }
    else if (justifier) { // Plug in a Justifier if the game supports it
        inputinfo[1] = jg_snes_inputinfo(1, JG_SNES_JUSTIFIER);
        emulator->connect(SuperFamicom::ID::Port::Controller2,
            SuperFamicom::ID::Device::Justifier);
    }

    // Set the aspect ratio
    switch (settings_bsnes[ASPECT].value) {
        default: case 0: { // Auto Region
            vidinfo.aspect = program->superFamicom.region == "PAL" ?
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

    Emulator::audio.setQuality(settings_bsnes[RSQUAL].value);

    // Audio and timing adjustments
    if (program->superFamicom.region == "PAL") {
        audinfo.spf = (SAMPLERATE / FRAMERATE_PAL) * CHANNELS;
        Emulator::audio.setSpf(audinfo.spf);
        jg_cb_frametime(TIMING_PAL);
    }
    else {
        Emulator::audio.setSpf(audinfo.spf);
        jg_cb_frametime(TIMING_NTSC);
    }

    emulator->power(); // Power up!

    return 1;
}

int jg_game_unload() {
    /* Save happens on emulator->unload, program->save() is useful for saving
       at a time interval to avoid losing SRAM data if something crashes.
    */
    //program->save();
    emulator->unload();
    return 1;
}

int jg_state_load(const char *filename) {
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    std::vector<uint8_t> state((std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>());
    stream.close();
    serializer s(state.data(), state.size());
    return emulator->unserialize(s);
}

int jg_state_save(const char *filename) {
    serializer s = emulator->serialize();
    std::ofstream stream(filename, std::ios::out | std::ios::binary);
    if (stream.is_open()) {
        stream.write((const char*)s.data(), s.size());
        stream.close();
        return 1;
    }
    return 0;
}

void jg_media_select() {
}

void jg_media_insert() {
}

void jg_cheat_clear() {
    cheatList.clear();
    emulator->cheats(cheatList);
}

void jg_cheat_set(const char *code) {
    std::string cht = std::string(code);
    bool decoded = false;

    if (!program->gameBoy.program.empty())
        decoded = CheatDecoder::gb(cht);
    else
        decoded = CheatDecoder::snes(cht);

    if (decoded) {
        cheatList.push_back(cht);
        emulator->cheats(cheatList);
    }
}

// JG Functions that return values to the frontend
jg_coreinfo_t* jg_get_coreinfo(const char *sys) {
    return &coreinfo;
}

jg_videoinfo_t* jg_get_videoinfo() {
    return &vidinfo;
}

jg_audioinfo_t* jg_get_audioinfo() {
    return &audinfo;
}

jg_inputinfo_t* jg_get_inputinfo(int port) {
    return &inputinfo[port];
}

void jg_setup_video() {
}

void jg_setup_audio() {
    Emulator::audio.setBuffer((float*)audinfo.buf);
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
