/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2021 Rupert Carmichael
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <stdint.h>

#include <jg/jg.h>
#include <jg/jg_snes.h>

#include "emulator/emulator.hpp"
#include "src/heuristics.hpp"
#include "src/interface.hpp"

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
    "bsnes", "bsnes-jg", "115.0.0", "snes", NUMINPUTS, JG_HINT_VIDEO_INTERNAL
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
};

enum {
    ASPECT,
};

static vector<string> cheatList;

static uint16_t audio_buffer_index = 0;
static uint16_t audio_buffer_max = audinfo.spf;

static int vidmult = 1;
static int ss_offset_x = 0;
static int ss_offset_y = 0;

static bool addon = false;

static const unsigned char iplrom[64] = {
    0xcd, 0xef, 0xbd, 0xe8, 0x00, 0xc6, 0x1d, 0xd0,
    0xfc, 0x8f, 0xaa, 0xf4, 0x8f, 0xbb, 0xf5, 0x78,
    0xcc, 0xf4, 0xd0, 0xfb, 0x2f, 0x19, 0xeb, 0xf4,
    0xd0, 0xfc, 0x7e, 0xf4, 0xd0, 0x0b, 0xe4, 0xf5,
    0xcb, 0xf4, 0xd7, 0x00, 0xfc, 0xd0, 0xf3, 0xab,
    0x01, 0x10, 0xef, 0x7e, 0xf4, 0x10, 0xeb, 0xba,
    0xf6, 0xda, 0x00, 0xba, 0xf4, 0xc4, 0xf4, 0xdd,
    0x5d, 0xd0, 0xdb, 0x1f, 0x00, 0x00, 0xc0, 0xff
};

static Emulator::Interface *emulator;

struct Program : Emulator::Platform {
    Program();
    ~Program();
    
    shared_pointer<vfs::file> open(unsigned id, std::string name,
        vfs::file::mode mode, bool required) override;
    Emulator::Platform::Load load(unsigned id, std::string name,
        std::string type, vector<std::string> options = {}) override;
    void videoFrame(const uint16_t *data, unsigned pitch, unsigned width,
        unsigned height, unsigned scale) override;
    void audioFrame(const double* samples, unsigned channels) override;
    int16_t inputPoll(unsigned port, unsigned device, unsigned input) override;
    void inputRumble(unsigned port, unsigned device, unsigned input,
        bool enable) override;
    
    void load();
    vector<uint8_t> loadFile(void *data, size_t size);
    bool loadSuperFamicom(std::string location);
    bool loadGameBoy(std::string location);
    bool loadBSMemory(std::string location);
    bool loadSufamiTurboA(std::string location);
    bool loadSufamiTurboB(std::string location);
    
    void save();
    
    shared_pointer<vfs::file>
        openRomSuperFamicom(std::string name, vfs::file::mode mode);
    shared_pointer<vfs::file>
        openRomGameBoy(std::string name, vfs::file::mode mode);
    shared_pointer<vfs::file>
        openRomBSMemory(std::string name, vfs::file::mode mode);
    shared_pointer<vfs::file>
        openRomSufamiTurboA(std::string name, vfs::file::mode mode);
    shared_pointer<vfs::file>
        openRomSufamiTurboB(std::string name, vfs::file::mode mode);
    
    void hackPatchMemory(vector<uint8_t>& data);

public: 
    struct Game {
        std::string option;
        std::string location;
        std::string manifest;
        Markup::Node document;
        Boolean patched;
        Boolean verified;
    };
    
    struct SuperFamicom : Game {
        std::string title;
        std::string region;
        vector<uint8_t> program;
        vector<uint8_t> data;
        vector<uint8_t> expansion;
        vector<uint8_t> firmware;
    } superFamicom;
    
    struct GameBoy : Game {
        vector<uint8_t> program;
    } gameBoy;
    
    struct BSMemory : Game {
        vector<uint8_t> program;
    } bsMemory;
    
    struct SufamiTurbo : Game {
        vector<uint8_t> program;
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

shared_pointer<vfs::file> Program::open(unsigned id, std::string name,
    vfs::file::mode mode, bool required) {
    
    shared_pointer<vfs::file> result;
    
    if (name == "ipl.rom" && mode == vfs::file::mode::read) {
        result = vfs::memory::file::open(iplrom, sizeof(iplrom));
    }
    if (name == "boards.bml" && mode == vfs::file::mode::read) {
        std::string boardspath = std::string(pathinfo.core) + "/boards.bml";
        return vfs::fs::file::open(boardspath.c_str(), mode);
    }
    
    if (id == 1) { // Super Famicom
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            std::vector<uint8_t> manifest(superFamicom.manifest.begin(),
                superFamicom.manifest.end());
            result = vfs::memory::file::open(manifest.data(), manifest.size());
        }
        else if (name == "program.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(superFamicom.program.data(),
                superFamicom.program.size());
        }
        else if (name == "data.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(superFamicom.data.data(),
                superFamicom.data.size());
        }
        else if (name == "expansion.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(superFamicom.expansion.data(),
                superFamicom.expansion.size());
        }
        else {
            result = openRomSuperFamicom(name, mode);
        }
    }
    else if (id == 2) { // Game Boy
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            std::vector<uint8_t> manifest(gameBoy.manifest.begin(),
                gameBoy.manifest.end());
            result = vfs::memory::file::open(manifest.data(), manifest.size());
        }
        else if (name == "program.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(gameBoy.program.data(),
                gameBoy.program.size());
        }
        else {
            result = openRomGameBoy(name, mode);
        }
    }
    else if (id == 3) { // BS Memory
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            std::vector<uint8_t> manifest(bsMemory.manifest.begin(),
                bsMemory.manifest.end());
            result = vfs::memory::file::open(manifest.data(), manifest.size());
        }
        else if (name == "program.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(bsMemory.program.data(),
                bsMemory.program.size());
        }
        else if (name == "program.flash") {
            //writes are not flushed to disk in bsnes
            result = vfs::memory::file::open(bsMemory.program.data(),
                bsMemory.program.size());
        }
        else {
            result = openRomBSMemory(name, mode);
        }
    }
    else if (id == 4) { // Sufami Turbo - Slot A
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            std::vector<uint8_t> manifest(sufamiTurboA.manifest.begin(),
                sufamiTurboA.manifest.end());
            result = vfs::memory::file::open(manifest.data(), manifest.size());
        }
        else if (name == "program.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(sufamiTurboA.program.data(),
                sufamiTurboA.program.size());
        }
        else {
            result = openRomSufamiTurboA(name, mode);
        }
    }
    else if (id == 5) { // Sufami Turbo - Slot B
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            std::vector<uint8_t> manifest(sufamiTurboB.manifest.begin(),
                sufamiTurboB.manifest.end());
            result = vfs::memory::file::open(manifest.data(), manifest.size());
        }
        else if (name == "program.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(sufamiTurboB.program.data(),
            sufamiTurboB.program.size());
        }
        else {
            result = openRomSufamiTurboB(name, mode);
        }
    }
    
    return result;
}

void Program::load() {
    emulator->unload();
    emulator->load();
}

Emulator::Platform::Load Program::load(unsigned id, std::string name,
    std::string type, vector<std::string> options) {
    
    if (id == 1) {
        if (loadSuperFamicom(superFamicom.location)) {
            return {id, superFamicom.region.c_str()};
        }
    }
    else if (id == 2) {
        if (loadGameBoy(gameBoy.location)) {
            return { id, "" };
        }
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

void Program::videoFrame(const uint16_t *data, unsigned pitch, unsigned width,
    unsigned height, unsigned scale) {
    
    //print("p: ", pitch, " w: ", width, " h: ", height, "\n");
    vidmult = height / 240;
    vidinfo.y = 8 * vidmult;
    height -= 2 * vidinfo.y;

    vidinfo.w = width;
    vidinfo.h = height;
    vidinfo.p = pitch / 2; // Divide by pixel size - 16-bit pixels == 2
    vidinfo.buf = (void*)data;
}

void Program::audioFrame(const double* samples, unsigned channels) {
    float *abuf = (float*)audinfo.buf;
    abuf[audio_buffer_index++] = (float)samples[0];
    abuf[audio_buffer_index++] = (float)samples[1];
}

static uint8_t imap[12] = { 0, 1, 2, 3, 7, 6, 9, 8, 10, 11, 4, 5 };

int16_t pollInputDevices(unsigned port, unsigned device, unsigned input) {
    //print("port: ", port, " device: ", device, " input: ", input, "\n");
    if (device == SuperFamicom::ID::Device::SuperScope) {
        switch (input) {
            case 0: { // X
                return (input_device[port]->coord[0] / vidmult) + ss_offset_x;
            }
            case 1: { // Y
                return (input_device[port]->coord[1] / vidmult) + ss_offset_y;
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
                return input_device[port]->coord[0] / vidmult;
            }
            case 1: { // Y
                return input_device[port]->coord[1] / vidmult;
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

void Program::inputRumble(unsigned port, unsigned device, unsigned input,
    bool enable) {
}

shared_pointer<vfs::file> Program::openRomSuperFamicom(std::string name,
    vfs::file::mode mode) {
    
    if (name == "program.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(superFamicom.program.data(),
            superFamicom.program.size());
    }
    
    if (name == "data.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(superFamicom.data.data(),
            superFamicom.data.size());
    }
    
    if (name == "expansion.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(superFamicom.expansion.data(),
            superFamicom.expansion.size());
    }
    
    if (name == "msu1/data.rom") {
        std::string location(superFamicom.location);
        return vfs::fs::file::open(
            {location.substr(0, location.find_last_of(".")).c_str(), ".msu"},
            mode);
    }
    
    if (name.find("msu1/track") != std::string::npos) {
        std::string location = superFamicom.location;
        location = location.substr(0, location.find_last_of(".")) +
            name.substr(name.find_first_of("track") + 5);
        return vfs::fs::file::open(location.c_str(), mode);
    }
    
    if (name == "save.ram") {
        std::string save_path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".srm";
        return vfs::fs::file::open(save_path.c_str(), mode);
    }
    
    if (name == "download.ram") {
        std::string ram_path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".psr";
        return vfs::fs::file::open(ram_path.c_str(), mode);
    }
    
    return {};
}

shared_pointer<vfs::file> Program::openRomGameBoy(std::string name,
    vfs::file::mode mode) {
    
    if (name == "program.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(gameBoy.program.data(),
            gameBoy.program.size());
    }

    if (name == "save.ram") {
        std::string save_path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".srm";
        return vfs::fs::file::open(save_path.c_str(), mode);
    }
    
    if (name == "time.rtc") {
        std::string save_path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".rtc";
        return vfs::fs::file::open(save_path.c_str(), mode);
    }

    return {};
}

shared_pointer<vfs::file> Program::openRomBSMemory(std::string name,
    vfs::file::mode mode) {
    
    if (name == "program.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(bsMemory.program.data(),
            bsMemory.program.size());
    }
    
    if (name == "program.flash") {
        //writes are not flushed to disk
        return vfs::memory::file::open(bsMemory.program.data(),
            bsMemory.program.size());
    }

    return {};
}

shared_pointer<vfs::file> Program::openRomSufamiTurboA(std::string name,
    vfs::file::mode mode) {
    if (name == "program.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(sufamiTurboA.program.data(),
            sufamiTurboA.program.size());
    }
    
    if (name == "save.ram") {
        std::string save_path;
        if (sufamiinfo.size)
            save_path = std::string(pathinfo.save) + "/" +
                std::string(sufamiinfo.name) + ".srm";
        else
            save_path = std::string(pathinfo.save) + "/" +
                std::string(gameinfo.name) + ".srm";
        
        return vfs::fs::file::open(save_path.c_str(), mode);
    }
    
    return {};
}

shared_pointer<vfs::file> Program::openRomSufamiTurboB(std::string name,
    vfs::file::mode mode) {
    if (name == "program.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(sufamiTurboB.program.data(),
            sufamiTurboB.program.size());
    }
    
    if (name == "save.ram") {
        std::string save_path = std::string(pathinfo.save) + "/" +
            std::string(gameinfo.name) + ".srm";
        return vfs::fs::file::open(save_path.c_str(), mode);
    }
    
    return {};
}

vector<uint8_t> Program::loadFile(void *data, size_t size) {
    uint8_t *buf = (uint8_t*)data;
    vector<uint8_t> ret;
    
    for (int i = 0; i < size; ++i)
        ret.append(buf[i]);
    
    return ret;
}

bool Program::loadSuperFamicom(std::string location) {
    std::string manifest;
    vector<uint8_t> rom;
    
    rom = addon ? loadFile(addoninfo.data, addoninfo.size) :
        loadFile(gameinfo.data, gameinfo.size);
    
    if (rom.size() < 0x8000) return false;
    
    //assume ROM and IPS agree on whether a copier header is present
    //superFamicom.patched = applyPatchIPS(rom, location);
    if ((rom.size() & 0x7fff) == 512) {
        //remove copier header
        memory::move(&rom[0], &rom[512], rom.size() - 512);
        rom.resize(rom.size() - 512);
    }
    
    auto heuristics = Heuristics::SuperFamicom(rom, location.c_str());
    auto sha256 = Hash::SHA256(rom).digest();
    heuristics.manifest();
    
    superFamicom.title = heuristics.title();
    superFamicom.region = heuristics.videoRegion();
    
    std::string dbpath = std::string(pathinfo.core) + "/Super Famicom.bml";
    
    if (auto document = BML::unserialize(string::read(dbpath.c_str()))) {
        if (auto game = document[{"game(sha256=", sha256, ")"}]) {
            manifest = BML::serialize(game);
            //the internal ROM header title is not present in the database, but
            //is needed for internal core overrides
            manifest += "  title: " + superFamicom.title + "\n";
            superFamicom.verified = true;
        }
    }
    
    superFamicom.manifest = manifest.empty() ? heuristics.manifest() : manifest;
    
    hackPatchMemory(rom);
    superFamicom.document = BML::unserialize(superFamicom.manifest.c_str());
    superFamicom.location = location;
    
    unsigned offset = 0;
    if (auto size = heuristics.programRomSize()) {
        superFamicom.program.resize(size);
        memory::copy(&superFamicom.program[0], &rom[offset], size);
        offset += size;
    }
    if (auto size = heuristics.dataRomSize()) {
        superFamicom.data.resize(size);
        memory::copy(&superFamicom.data[0], &rom[offset], size);
        offset += size;
    }
    if (auto size = heuristics.expansionRomSize()) {
        superFamicom.expansion.resize(size);
        memory::copy(&superFamicom.expansion[0], &rom[offset], size);
        offset += size;
    }
    if (auto size = heuristics.firmwareRomSize()) {
        superFamicom.firmware.resize(size);
        memory::copy(&superFamicom.firmware[0], &rom[offset], size);
        offset += size;
    }
    return true;
}

bool Program::loadGameBoy(std::string location) {
    vector<uint8_t> rom;
    rom = loadFile(gameinfo.data, gameinfo.size);
    
    if (rom.size() < 0x4000) return false;
    
    auto heuristics = Heuristics::GameBoy(rom, location.c_str());
    
    gameBoy.manifest = heuristics.manifest();
    gameBoy.document = BML::unserialize(gameBoy.manifest.c_str());
    gameBoy.location = location;
    gameBoy.program = rom;
    
    return true;
}

bool Program::loadBSMemory(std::string location) {
    string manifest;
    vector<uint8_t> rom;
    rom = loadFile(gameinfo.data, gameinfo.size);
    
    if (rom.size() < 0x8000) return false;
    
    auto heuristics = Heuristics::BSMemory(rom, location.c_str());
    auto sha256 = Hash::SHA256(rom).digest();
    
    std::string dbpath = std::string(pathinfo.core) + "/BS Memory.bml";
    
    if (auto document = BML::unserialize(string::read(dbpath.c_str()))) {
        if (auto game = document[{"game(sha256=", sha256, ")"}]) {
            manifest = BML::serialize(game);
            bsMemory.verified = true;
        }
    }
    
    bsMemory.manifest = manifest ? manifest : heuristics.manifest();
    bsMemory.document = BML::unserialize(bsMemory.manifest.c_str());
    bsMemory.location = location;
    
    bsMemory.program = rom;
    return true;
}

bool Program::loadSufamiTurboA(std::string location) {
    std::string manifest;
    vector<uint8_t> rom;
    
    if (sufamiinfo.size)
        rom = loadFile(sufamiinfo.data, sufamiinfo.size);
    else
        rom = loadFile(gameinfo.data, gameinfo.size);
    
    if (rom.size() < 0x20000) return false;
    
    auto heuristics = Heuristics::SufamiTurbo(rom, location.c_str());
    auto sha256 = Hash::SHA256(rom).digest();
    
    std::string dbpath = std::string(pathinfo.core) + "/Sufami Turbo.bml";
    
    if (auto document = BML::unserialize(string::read(dbpath.c_str()))) {
        if (auto game = document[{"game(sha256=", sha256, ")"}]) {
            manifest = BML::serialize(game);
            sufamiTurboA.verified = true;
        }
    }
    
    sufamiTurboA.manifest = manifest.empty() ? heuristics.manifest() : manifest;
    sufamiTurboA.document = BML::unserialize(sufamiTurboA.manifest.c_str());
    sufamiTurboA.location = string(gameinfo.path);//location;
    
    sufamiTurboA.program = rom;
    return true;
}

bool Program::loadSufamiTurboB(std::string location) {
    if (!sufamiinfo.size)
        return false;
    
    std::string manifest;
    vector<uint8_t> rom;
    rom = loadFile(gameinfo.data, gameinfo.size);
    
    if (rom.size() < 0x20000) return false;
    
    auto heuristics = Heuristics::SufamiTurbo(rom, location.c_str());
    auto sha256 = Hash::SHA256(rom).digest();
    
    std::string dbpath = std::string(pathinfo.core) + "/Sufami Turbo.bml";
    
    if (auto document = BML::unserialize(string::read(dbpath.c_str()))) {
        if (auto game = document[{"game(sha256=", sha256, ")"}]) {
            manifest = BML::serialize(game);
            sufamiTurboB.verified = true;
        }
    }
    
    sufamiTurboB.manifest = manifest.empty() ? heuristics.manifest() : manifest;
    sufamiTurboB.document = BML::unserialize(sufamiTurboB.manifest.c_str());
    sufamiTurboB.location = location;
    
    sufamiTurboB.program = rom;
    return true;
}

void Program::hackPatchMemory(vector<uint8_t>& data) {
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

bool decodeSNES(string& code) {
    //Game Genie
    if (code.size() == 9 && code[4u] == '-') {
        //strip '-'
        code = {code.slice(0, 4), code.slice(5, 4)};
        //validate
        for (unsigned n : code) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        
        //decode
        code.transform("df4709156bc8a23e", "0123456789abcdef");
        uint32_t r = toHex(code);
        //abcd efgh ijkl mnop qrst uvwx
        //ijkl qrst opab cduv wxef ghmn
        unsigned address =
          (!!(r & 0x002000) << 23) | (!!(r & 0x001000) << 22)
        | (!!(r & 0x000800) << 21) | (!!(r & 0x000400) << 20)
        | (!!(r & 0x000020) << 19) | (!!(r & 0x000010) << 18)
        | (!!(r & 0x000008) << 17) | (!!(r & 0x000004) << 16)
        | (!!(r & 0x800000) << 15) | (!!(r & 0x400000) << 14)
        | (!!(r & 0x200000) << 13) | (!!(r & 0x100000) << 12)
        | (!!(r & 0x000002) << 11) | (!!(r & 0x000001) << 10)
        | (!!(r & 0x008000) <<  9) | (!!(r & 0x004000) <<  8)
        | (!!(r & 0x080000) <<  7) | (!!(r & 0x040000) <<  6)
        | (!!(r & 0x020000) <<  5) | (!!(r & 0x010000) <<  4)
        | (!!(r & 0x000200) <<  3) | (!!(r & 0x000100) <<  2)
        | (!!(r & 0x000080) <<  1) | (!!(r & 0x000040) <<  0);
        unsigned data = r >> 24;
        code = {hex(address, 6L), "=", hex(data, 2L)};
        return true;
    }
    
    //Pro Action Replay
    if (code.size() == 8) {
        //validate
        for (unsigned n : code) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        
        //decode
        uint32_t r = toHex(code);
        unsigned address = r >> 8;
        unsigned data = r & 0xff;
        code = {hex(address, 6L), "=", hex(data, 2L)};
        return true;
    }
    
    //higan: address=data
    if (code.size() == 9 && code[6u] == '=') {
        string nibbles = {code.slice(0, 6), code.slice(7, 2)};
        //validate
        for (unsigned n : nibbles) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        //already in decoded form
        return true;
    }
    
    //higan: address=compare?data
    if (code.size() == 12 && code[6u] == '=' && code[9u] == '?') {
        string nibbles =
            {code.slice(0, 6), code.slice(7, 2), code.slice(10, 2)};
        //validate
        for (unsigned n : nibbles) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        //already in decoded form
        return true;
    }
    
    //unrecognized code format
    return false;
}

bool decodeGB(string& code) {
    auto nibble = [&](const string& s, unsigned index) -> unsigned {
        if (index >= s.size()) return 0;
        if (s[index] >= '0' && s[index] <= '9') return s[index] - '0';
        return s[index] - 'a' + 10;
    };
    
    //Game Genie
    if (code.size() == 7 && code[3u] == '-') {
        code = {code.slice(0, 3), code.slice(4, 3)};
        //validate
        for (unsigned n : code) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        
        unsigned data = nibble(code, 0) << 4 | nibble(code, 1) << 0;
        unsigned address = (nibble(code, 5) ^ 15) << 12 | nibble(code, 2) << 8 |
            nibble(code, 3) << 4 | nibble(code, 4) << 0;
        code = {hex(address, 4L), "=", hex(data, 2L)};
        return true;
    }
    
    //Game Genie
    if (code.size() == 11 && code[3u] == '-' && code[7u] == '-') {
        code = {code.slice(0, 3), code.slice(4, 3), code.slice(8, 3)};
        //validate
        for (unsigned n : code) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        
        unsigned data = nibble(code, 0) << 4 | nibble(code, 1) << 0;
        unsigned address = (nibble(code, 5) ^ 15) << 12 | nibble(code, 2) << 8 |
            nibble(code, 3) << 4 | nibble(code, 4) << 0;
        uint8_t t = nibble(code, 6) << 4 | nibble(code, 8) << 0;
        t = t >> 2 | t << 6;
        unsigned compare = t ^ 0xba;
        code = {hex(address, 4L), "=", hex(compare, 2L), "?", hex(data, 2L)};
        return true;
    }
    
    //GameShark
    if (code.size() == 8) {
        //validate
        for (unsigned n : code) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        
        //first two characters are the code type / VRAM bank, which is almost
        //always 01. other values are presumably supported, but I have no info
        //on them, so they're not supported.
        if (code[0u] != '0') return false;
        if (code[1u] != '1') return false;
        unsigned data = toHex(code.slice(2, 2));
        uint16_t address = toHex(code.slice(4, 4));
        address = address >> 8 | address << 8;
        code = {hex(address, 4L), "=", hex(data, 2L)};
        return true;
    }
    
    //higan: address=data
    if (code.size() == 7 && code[4u] == '=') {
        string nibbles = {code.slice(0, 4), code.slice(5, 2)};
        //validate
        for (unsigned n : nibbles) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        //already in decoded form
        return true;
    }
    
    //higan: address=compare?data
    if (code.size() == 10 && code[4u] == '=' && code[7u] == '?') {
        string nibbles = {code.slice(0, 4), code.slice(5, 2), code.slice(8, 2)};
        //validate
        for (unsigned n : nibbles) {
            if (n >= '0' && n <= '9') continue;
            if (n >= 'a' && n <= 'f') continue;
            return false;
        }
        //already in decoded form
        return true;
    }
    
    //unrecognized code format
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
    jg_cb_audio(audio_buffer_index);
    audio_buffer_index = 0;
}

int jg_game_load() {
    emulator->configure("Audio/Frequency", SAMPLERATE);
    emulator->configure("Video/BlurEmulation", false);
    
    emulator->configure("Hacks/Entropy", "Low");
    emulator->configure("Hacks/CPU/Overclock", 100);
    emulator->configure("Hacks/CPU/FastMath", false);
    emulator->configure("Hacks/SA1/Overclock", 100);
    emulator->configure("Hacks/SuperFX/Overclock", 100);
    emulator->configure("Hacks/Coprocessor/DelayedSync", false); // default true
    emulator->configure("Hacks/Coprocessor/PreferHLE", true);
    
    // Load the game
    if (string(gameinfo.path).endsWith(".gb") ||
        string(gameinfo.path).endsWith(".gbc")) {
        
        if (!addoninfo.size) {
            jg_cb_log(JG_LOG_ERR,
                "Super Game Boy ROM required for GB/GBC content, exiting...\n");
            return 0;
        }
        
        program->superFamicom.location = std::string(addoninfo.path);
        program->gameBoy.location = std::string(gameinfo.path);
        addon = true;
    }
    else if (string(gameinfo.path).endsWith(".bs")) {
        if (!addoninfo.size) {
            jg_cb_log(JG_LOG_ERR,
                "BS-X BIOS ROM required for BS-X content, exiting...\n");
            return 0;
        }
        
        program->superFamicom.location = std::string(addoninfo.path);
        program->bsMemory.location = std::string(gameinfo.path);
        addon = true;
    }
    else if (string(gameinfo.path).endsWith(".st")) {
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
    for (int i = 0; i < sizeof(db_multitap_games) / sizeof(mtentry_t); ++i) {
        if ((std::string)gameinfo.md5 == db_multitap_games[i].md5) {
            multitap = db_multitap_games[i].players;
            break;
        }
    }
    
    bool mouse = false;
    for (int i = 0; i < sizeof(db_mouse_games) / sizeof(string); ++i) {
        if ((std::string)gameinfo.md5 == db_mouse_games[i]) {
            mouse = true;
            break;
        }
    }
    
    bool superscope = false;
    for (int i = 0; i < sizeof(db_superscope_games) / sizeof(ssentry_t); ++i) {
        if ((std::string)gameinfo.md5 == db_superscope_games[i].md5) {
            superscope = true;
            ss_offset_x = db_superscope_games[i].x;
            ss_offset_y = db_superscope_games[i].y;
            break;
        }
    }
    
    bool justifier = false;
    for (int i = 0; i < sizeof(db_justifier_games) / sizeof(string); ++i) {
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
    
    // Audio and timing adjustments
    if (program->superFamicom.region == "PAL") {
        audinfo.spf = (SAMPLERATE / FRAMERATE_PAL) * CHANNELS;
        jg_cb_frametime(TIMING_PAL);
    }
    else {
        jg_cb_frametime(TIMING_NTSC);
    }
    
    emulator->power(); // Power up!
    
    return 1;
}

int jg_game_unload() {
    program->save();
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
    cheatList.reset();
    emulator->cheats(cheatList);
}

void jg_cheat_set(const char *code) {
    string cheat = string(code).downcase();
    bool decoded = false;

    if (program->gameBoy.program)
        decoded = decodeGB(cheat);
    else
        decoded = decodeSNES(cheat);

    if (decoded) {
        cheatList.append(cheat);
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
