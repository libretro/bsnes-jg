/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020 Rupert Carmichael
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
#include <stdint.h>

#include <emulator/emulator.hpp>
#include <sfc/interface/interface.hpp>
#include <filter/filter.hpp>
#include <heuristics/heuristics.hpp>
#include <heuristics/heuristics.cpp>
#include <heuristics/super-famicom.cpp>
#include <heuristics/game-boy.cpp>
#include <heuristics/bs-memory.cpp>

#include <jg/jg.h>
#include <jg/jg_snes.h>

#define SAMPLERATE 48000
#define FRAMERATE 60
#define FRAMERATE_PAL 50
#define CHANNELS 2
#define NUMINPUTS 2

#define ASPECT_NTSC 1.306122
#define ASPECT_PAL 1.4257812

#define TIMING_NTSC 60.098812
#define TIMING_PAL 50.006979

static jg_cb_audio_t jg_cb_audio;
static jg_cb_frametime_t jg_cb_frametime;
static jg_cb_log_t jg_cb_log;
static jg_cb_rumble_t jg_cb_rumble;
static jg_cb_settings_read_t jg_cb_settings_read;

static jg_coreinfo_t coreinfo = {
    "bsnes", "bsnes-jg", "115", "snes", NUMINPUTS, JG_HINT_VIDEO_INTERNAL
};

static jg_videoinfo_t vidinfo = {
    JG_PIXFMT_RGB1555,  // pixfmt
    2304,               // wmax
    2160,               // hmax
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
static jg_gameinfo_t gameinfo;
static jg_inputinfo_t inputinfo[NUMINPUTS];
static jg_inputstate_t *input_device[NUMINPUTS];

// Emulator settings
static jg_setting_t settings_bsnes[] = { // name, default, min, max
    { "aspect_ratio", 0, 0, 1 }, // 0 = 8:7, 1 = Auto Region
    { "fast_ppu", 0, 0, 1 }, // 0 = Disabled, 1 = Enabled
    { "mode7_scale", 1, 1, 8 }, // 0 = Disabled, 1 = Enabled
};

enum {
    ASPECT,
    FASTPPU,
    M7SCALE,
};

static uint16_t audio_buffer_index = 0;
static uint16_t audio_buffer_max = audinfo.spf;

static int vidmult = 1;
static int ss_offset_x = 0;
static int ss_offset_y = 0;

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
    
    shared_pointer<vfs::file> open(uint id, string name, vfs::file::mode mode,
        bool required) override;
    Emulator::Platform::Load load(uint id, string name, string type,
        vector<string> options = {}) override;
    void videoFrame(const uint16* data, uint pitch, uint width, uint height,
        uint scale) override;
    void audioFrame(const double* samples, uint channels) override;
    int16 inputPoll(uint port, uint device, uint input) override;
    void inputRumble(uint port, uint device, uint input, bool enable) override;
    
    void load();
    vector<uint8_t> loadFile(string location);
    bool loadSuperFamicom(string location);
    bool loadGameBoy(string location);
    bool loadBSMemory(string location);
    
    void save();
    
    shared_pointer<vfs::file>
        openRomSuperFamicom(string name, vfs::file::mode mode);
    shared_pointer<vfs::file>
        openRomGameBoy(string name, vfs::file::mode mode);
    shared_pointer<vfs::file>
        openRomBSMemory(string name, vfs::file::mode mode);
    
    void hackPatchMemory(vector<uint8_t>& data);
    
    string base_name;
    
    bool overscan = true;

public: 
    struct Game {
        explicit operator bool() const { return (bool)location; }
        
        string option;
        string location;
        string manifest;
        Markup::Node document;
        boolean patched;
        boolean verified;
    };
    
    struct SuperFamicom : Game {
        string title;
        string region;
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
};

static Program *program = nullptr;

Program::Program() {
    Emulator::platform = this;
}

Program::~Program() {
    delete emulator;
}

void Program::save() {
    if(!emulator->loaded()) return;
    emulator->save();
}

shared_pointer<vfs::file> Program::open(uint id, string name,
    vfs::file::mode mode, bool required) {
    
    shared_pointer<vfs::file> result;
    
    if (name == "ipl.rom" && mode == vfs::file::mode::read) {
        result = vfs::memory::file::open(iplrom, sizeof(iplrom));
    }
    if (name == "boards.bml" && mode == vfs::file::mode::read) {
        string boardspath = {pathinfo.core, "/boards.bml"};
        return vfs::fs::file::open(boardspath, mode);
    }
    
    if (id == 1) { //Super Famicom
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(
                superFamicom.manifest.data<uint8_t>(),
                superFamicom.manifest.size());
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
    else if (id == 2) { //Game Boy
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(gameBoy.manifest.data<uint8_t>(),
                gameBoy.manifest.size());
        }
        else if (name == "program.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(gameBoy.program.data(),
                gameBoy.program.size());
        }
        else {
            result = openRomGameBoy(name, mode);
        }
    }
    else if (id == 3) {  //BS Memory
        if (name == "manifest.bml" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(bsMemory.manifest.data<uint8_t>(),
                bsMemory.manifest.size());
        }
        else if (name == "program.rom" && mode == vfs::file::mode::read) {
            result = vfs::memory::file::open(bsMemory.program.data(),
                bsMemory.program.size());
        }
        else if(name == "program.flash") {
            //writes are not flushed to disk in bsnes
            result = vfs::memory::file::open(bsMemory.program.data(),
                bsMemory.program.size());
        }
        else {
            result = openRomBSMemory(name, mode);
        }
    }
    
    return result;
}

void Program::load() {
    emulator->unload();
    emulator->load();
    
    // per-game hack overrides
    auto title = superFamicom.title;
    auto region = superFamicom.region;
    
    //sometimes menu options are skipped over in the main menu with cycle-based
    //joypad polling
    if (title == "Arcades Greatest Hits")
        emulator->configure("Hacks/CPU/FastJoypadPolling", true);
    
    //the start button doesn't work in this game with cycle-based joypad polling
    else if (title == "TAIKYOKU-IGO Goliath")
        emulator->configure("Hacks/CPU/FastJoypadPolling", true);
    
    //holding up or down on the menu quickly cycles through options instead of
    //stopping after each button press
    else if (title == "WORLD MASTERS GOLF")
        emulator->configure("Hacks/CPU/FastJoypadPolling", true);
    
    //relies on mid-scanline rendering techniques
    else if (title == "AIR STRIKE PATROL" || title == "DESERT FIGHTER")
        emulator->configure("Hacks/PPU/Fast", false);
    
    //the dialogue text is blurry due to an issue in the scanline-based
    //renderer's color math support
    else if (title == "マーヴェラス")
        emulator->configure("Hacks/PPU/Fast", false);
    
    //stage 2 uses pseudo-hires in a way that's not compatible with the
    //scanline-based renderer
    else if (title == "SFC クレヨンシンチャン")
        emulator->configure("Hacks/PPU/Fast", false);
    
    //title screen game select (after choosing a game) changes OAM tiledata
    //address mid-frame. This is only supported by the cycle-based PPU renderer
    else if (title == "Winter olympics")
        emulator->configure("Hacks/PPU/Fast", false);
    
    //title screen shows remnants of the flag after choosing a language with the
    //scanline-based renderer
    else if (title == "WORLD CUP STRIKER")
        emulator->configure("Hacks/PPU/Fast", false);
    
    //relies on cycle-accurate writes to the echo buffer
    else if (title == "KOUSHIEN_2")
        emulator->configure("Hacks/DSP/Fast", false);
    
    //will hang immediately
    else if (title == "RENDERING RANGER R2")
        emulator->configure("Hacks/DSP/Fast", false);
    
    //will hang sometimes in the "Bach in Time" stage
    else if (title == "BUBSY II" && region == "PAL")
        emulator->configure("Hacks/DSP/Fast", false);
    
    //fixes for an errant scanline on the title screen due to writing to PPU
    //registers too late
    else if (title == "ADVENTURES OF FRANKEN" && region == "PAL")
        emulator->configure("Hacks/PPU/RenderCycle", 32);
    
    else if (title == "FIREPOWER 2000" || title == "SUPER SWIV")
        emulator->configure("Hacks/PPU/RenderCycle", 32);
    
    else if (title == "NHL '94" || title == "NHL PROHOCKEY'94")
        emulator->configure("Hacks/PPU/RenderCycle", 32);
    
    else if (title == "Sugoro Quest++")
        emulator->configure("Hacks/PPU/RenderCycle", 128);
    
    else if (emulator->configuration("Hacks/Hotfixes")) {
        //this game transfers uninitialized memory into video RAM: this can
        //cause a row of invalid tiles to appear in the background of stage 12.
        //this one is a bug in the original game, so only enable it if the
        //hotfixes option has been enabled.
        if (title == "The Hurricanes")
            emulator->configure("Hacks/Entropy", "None");
        
        //Frisky Tom attract sequence sometimes hangs when WRAM is initialized
        //to pseudo-random patterns
        else if (title == "ニチブツ・アーケード・クラシックス")
            emulator->configure("Hacks/Entropy", "None");
    }
}

Emulator::Platform::Load Program::load(uint id, string name, string type,
    vector<string> options) {
    
    if (id == 1) {
        if (loadSuperFamicom(superFamicom.location)) {
            return {id, superFamicom.region};
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
    return { id, options(0) };
}

void Program::videoFrame(const uint16* data, uint pitch, uint width,
    uint height, uint scale) {
    
    //print("p: ", pitch, " w: ", width, " h: ", height, "\n");
    vidmult = height / 240;
    vidinfo.y = 8 * vidmult;
    height -= 2 * vidinfo.y;

    vidinfo.w = width;
    vidinfo.h = height;
    vidinfo.p = pitch / 2; // Divide by pixel size - 16-bit pixels == 2
    vidinfo.buf = (void*)data;
}

void Program::audioFrame(const double* samples, uint channels) {
    float *abuf = (float*)audinfo.buf;
    abuf[audio_buffer_index++] = (float)samples[0];
    abuf[audio_buffer_index++] = (float)samples[1];
}

static uint8_t imap[12] = { 0, 1, 2, 3, 7, 6, 9, 8, 10, 11, 4, 5 };

int16 pollInputDevices(uint port, uint device, uint input) {
    //print("port: ", port, " device: ", device, " input: ", input, "\n");
    if (device == SuperFamicom::ID::Device::SuperScope) {
        switch (input) {
            case 0: // X
                return (input_device[port]->coord[0] / vidmult) + ss_offset_x;
            case 1: // Y
                return (input_device[port]->coord[1] / vidmult) + ss_offset_y;
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
            case 3: // Cursor
                return input_device[port]->button[2];
            case 4: // Turbo
                return input_device[port]->button[3];
            case 5: // Pause
                return input_device[port]->button[4];
            default:
                return 0;
        }
    }
    /*else if (device == SuperFamicom::ID::Device::Justifier) {
        switch (input) {
            case 0: // X
                return (input_device[port]->coord[0] / vidmult) + ss_offset_x;
            case 1: // Y
                return (input_device[port]->coord[1] / vidmult) + ss_offset_y;
            case 2: {// Trigger
                int ret = 0;
                if (input_device[port]->button[1]) { // Offscreen
                    input_device[port]->coord[0] = 1026;
                    ret = 1;
                }
                else
                    ret = input_device[port]->button[0];
                return ret;
            }
            case 3: // Start
                return input_device[port]->button[2];
            default:
                return 0;
        }
    }*/
    
    return port > 1 ? 0 : input_device[port]->button[imap[input]];
}

int16 Program::inputPoll(uint port, uint device, uint input) {
    return pollInputDevices(port, device, input);
}

void Program::inputRumble(uint port, uint device, uint input, bool enable) {
}

shared_pointer<vfs::file> Program::openRomSuperFamicom(string name,
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
        return vfs::fs::file::open({Location::notsuffix(superFamicom.location),
            ".msu"}, mode);
    }
    
    if (name.match("msu1/track*.pcm")) {
        name.trimLeft("msu1/track", 1L);
        return vfs::fs::file::open({Location::notsuffix(superFamicom.location),
            name}, mode);
    }
    
    if (name == "save.ram") {
        string save_path = { pathinfo.save, "/", gameinfo.name, ".srm" };
        const char *save = nullptr;
        return vfs::fs::file::open(save_path, mode);    
    }
    
    if (name == "download.ram") {
        string ram_path = { pathinfo.save, "/", gameinfo.name, ".psr" };
        const char *save = nullptr;
        return vfs::fs::file::open(ram_path, mode);
    }
    
    return {};
}

shared_pointer<vfs::file> Program::openRomGameBoy(string name,
    vfs::file::mode mode) {
    
    if (name == "program.rom" && mode == vfs::file::mode::read) {
        return vfs::memory::file::open(gameBoy.program.data(),
            gameBoy.program.size());
    }

    if (name == "save.ram") {
        string save_path = { pathinfo.save, "/", gameinfo.name, ".srm" };
        const char *save = nullptr;
        return vfs::fs::file::open(save_path, mode);    
    }
    
    if (name == "time.rtc") {
        string save_path = { pathinfo.save, "/", gameinfo.name, ".rtc" };
        const char *save = nullptr;
        return vfs::fs::file::open(save_path, mode);    
    }

    return {};
}

shared_pointer<vfs::file> Program::openRomBSMemory(string name,
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

vector<uint8_t> Program::loadFile(string location) {
    uint8_t *game = (uint8_t*)gameinfo.data;
    vector<uint8_t> ret;
    
    for (int i = 0; i < gameinfo.size; i++)
        ret.append(game[i]);
    
    return ret;
}

bool Program::loadSuperFamicom(string location) {
    vector<uint8_t> rom;
    rom = loadFile(location);
    
    if (rom.size() < 0x8000) return false;
    
    //assume ROM and IPS agree on whether a copier header is present
    //superFamicom.patched = applyPatchIPS(rom, location);
    if ((rom.size() & 0x7fff) == 512) {
        //remove copier header
        memory::move(&rom[0], &rom[512], rom.size() - 512);
        rom.resize(rom.size() - 512);
    }
    
    auto heuristics = Heuristics::SuperFamicom(rom, location);
    //auto sha256 = Hash::SHA256(rom).digest();
    
    superFamicom.title = heuristics.title();
    superFamicom.region = heuristics.videoRegion();
    superFamicom.manifest = heuristics.manifest();
    
    hackPatchMemory(rom);
    superFamicom.document = BML::unserialize(superFamicom.manifest);
    superFamicom.location = location;
    
    uint offset = 0;
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

bool Program::loadGameBoy(string location) {
    vector<uint8_t> rom;
    rom = loadFile(location);
    
    if (rom.size() < 0x4000) return false;
    
    auto heuristics = Heuristics::GameBoy(rom, location);
    auto sha256 = Hash::SHA256(rom).digest();
    
    gameBoy.manifest = heuristics.manifest();
    gameBoy.document = BML::unserialize(gameBoy.manifest);
    gameBoy.location = location;
    gameBoy.program = rom;
    
    return true;
}

bool Program::loadBSMemory(string location) {
    string manifest;
    vector<uint8_t> rom;
    rom = loadFile(location);
    
    if (rom.size() < 0x8000) return false;
    
    auto heuristics = Heuristics::BSMemory(rom, location);
    auto sha256 = Hash::SHA256(rom).digest();
    
    bsMemory.manifest = manifest ? manifest : heuristics.manifest();
    bsMemory.document = BML::unserialize(bsMemory.manifest);
    bsMemory.location = location;
    
    bsMemory.program = rom;
    return true;
}

void Program::hackPatchMemory(vector<uint8_t>& data) {
    auto title = superFamicom.title;
    
    if(title == "Satellaview BS-X" && data.size() >= 0x100000) {
        //BS-X: Sore wa Namae o Nusumareta Machi no Monogatari (JPN) (1.1)
        //disable limited play check for BS Memory flash cartridges
        //benefit: allow locked out BS Memory flash games to play without manual
        //header patching.
        //detriment: BS Memory ROM cartridges will cause the game to hang in the
        //load menu
        if(data[0x4a9b] == 0x10) data[0x4a9b] = 0x80;
        if(data[0x4d6d] == 0x10) data[0x4d6d] = 0x80;
        if(data[0x4ded] == 0x10) data[0x4ded] = 0x80;
        if(data[0x4e9a] == 0x10) data[0x4e9a] = 0x80;
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
    jg_cb_audio(audio_buffer_index);
    audio_buffer_index = 0;
}

int jg_game_load() {
    emulator->configure("Audio/Frequency", SAMPLERATE);
    
    emulator->configure("Video/BlurEmulation", false);
    
    emulator->configure("Hacks/Entropy", "Low");
    emulator->configure("Hacks/Hotfixes", false);
    
    emulator->configure("Hacks/CPU/Overclock", 100);
    emulator->configure("Hacks/CPU/FastMath", false);
    emulator->configure("Hacks/SA1/Overclock", 100);
    emulator->configure("Hacks/SuperFX/Overclock", 100);
    
    emulator->configure("Hacks/PPU/Fast",
        settings_bsnes[FASTPPU].value ? true : false);
    emulator->configure("Hacks/PPU/Deinterlace", false);
    emulator->configure("Hacks/PPU/NoSpriteLimit", false);
    emulator->configure("Hacks/PPU/NoVRAMBlocking", false);
    emulator->configure("Hacks/PPU/Mode7/Scale", settings_bsnes[M7SCALE].value);
    emulator->configure("Hacks/PPU/Mode7/Perspective", false);
    emulator->configure("Hacks/PPU/Mode7/Supersample", false);
    emulator->configure("Hacks/PPU/Mode7/Mosaic", false);
    
    emulator->configure("Hacks/DSP/Fast", false); // default true
    emulator->configure("Hacks/DSP/Cubic", false);
    emulator->configure("Hacks/DSP/EchoShadow", false);
    emulator->configure("Hacks/Coprocessor/DelayedSync", false); // default true
    emulator->configure("Hacks/Coprocessor/PreferHLE", true);
    
    // Overscan should be drawn because the frontend does the clipping
    program->overscan = true;
    
    // Load the game
    if (string(gameinfo.path).endsWith(".gb") ||
        string(gameinfo.path).endsWith(".gbc")) {
        //string sgb_bios = string(pathinfo.bios, "/Super Game Boy (JU).sfc");
        //string sgb_bios = string(pathinfo.bios, "/Super Game Boy (UE).sfc");
        string sgb_bios = string(pathinfo.bios, "/Super Game Boy 2 (J).sfc");
        if (!file::exists(sgb_bios)) {
            jg_cb_log(JG_LOG_ERR,
                "Missing BIOS file \'Super Game Boy 2 (J).sfc\', exiting...\n");
            return 0;
        }
        
        program->superFamicom.location = sgb_bios;
        program->gameBoy.location = string(gameinfo.path);
    }
    else if (string(gameinfo.path).endsWith(".bs")) {
        string bsx_bios = string(pathinfo.bios, "/BsxBios.sfc");
        if (!file::exists(bsx_bios)) {
            jg_cb_log(JG_LOG_ERR,
                "Missing BIOS file \'BsxBios.sfc\', exiting...\n");
            return 0;
        }
        program->superFamicom.location = bsx_bios;
        program->bsMemory.location = string(gameinfo.path);
    }
    else {
        program->superFamicom.location = string(gameinfo.path);
    }
    
    program->base_name = string(gameinfo.path);
    program->load();
    
    // Set up inputs
    string title = program->superFamicom.title;
    bool superscope = false;
    
    // There must be a better way of doing this. But nall must be killed first.
    // The requirement for offsets suggests something broken at deeper levels.
    if (title == "BAZOOKA BLITZKRIEG" || title == "SFC DESTRUCTIVE") {
        superscope = true;
        ss_offset_x = 1;
        ss_offset_y = 19;
    }
    else if(title == "T2 ARCADE") {
        superscope = true;
        ss_offset_x = 36;
        ss_offset_y = -14;
    }
    else if(title == "X ZONE") {
        superscope = true;
        ss_offset_x = 40;
        ss_offset_y = -7;
    }
    // Games that do not require an offset
    else if (title == "BATTLE CLASH" || title == "Hunt for Red October" ||
        title == "LAMBORGHINI AMERICAN" || title == "METAL COMBAT" ||
        title == "OPERATION THUNDERBOLT" || title == "SUPER SCOPE 6" ||
        title == "TINSTAR" || title == "YOSHI'S SAFARI") {
        superscope = true;
    }
    
    // Default input devices are SNES Controllers
    inputinfo[0] = (jg_inputinfo_t){JG_INPUT_CONTROLLER, 0,
        "pad1", "Controller 1", defs_snespad, 0, NDEFS_SNESPAD};
    emulator->connect(SuperFamicom::ID::Port::Controller1,
        SuperFamicom::ID::Device::Gamepad);
    
    inputinfo[1] = (jg_inputinfo_t){JG_INPUT_CONTROLLER, 1,
        "pad2", "Controller 2", defs_snespad, 0, NDEFS_SNESPAD};
    emulator->connect(SuperFamicom::ID::Port::Controller2,
        SuperFamicom::ID::Device::Gamepad);//*/
    
    /*inputinfo[0] = (jg_inputinfo_t){JG_INPUT_POINTER, 0,
        "mouse", "SNES Mouse", defs_snesmouse, 0, NDEFS_SNESMOUSE};
    emulator->connect(SuperFamicom::ID::Port::Controller1,
        SuperFamicom::ID::Device::Mouse);//*/
    
    // Plug in a Super Scope if the game supports it
    if (superscope) {
        inputinfo[1] = (jg_inputinfo_t){JG_INPUT_GUN, 1,
            "superscope", "Super Scope", defs_superscope, 0, NDEFS_SUPERSCOPE};
        emulator->connect(SuperFamicom::ID::Port::Controller2,
            SuperFamicom::ID::Device::SuperScope);
    }
    
    // Justifier support appears fundamentally broken at the core level
    /*inputinfo[1] = (jg_inputinfo_t){JG_INPUT_GUN, 1,
        "justifier", "Justifier", defs_snesjustifier, 0, NDEFS_SNESJUSTIFIER};
    emulator->connect(SuperFamicom::ID::Port::Controller2,
        SuperFamicom::ID::Device::Justifier);*/
    
    // Set the aspect ratio
    if (settings_bsnes[ASPECT].value)
        vidinfo.aspect = program->superFamicom.region == "PAL" ?
            ASPECT_PAL : ASPECT_NTSC;
    
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
    vector<uint8_t> memory;
    memory = file::read(filename);
    serializer s(memory.data(), (uint)memory.size());
    return emulator->unserialize(s);
}

int jg_state_save(const char *filename) {
    serializer s = emulator->serialize();
    FILE *file;
    file = fopen(filename, "wb");
    if (!file)
        return 0;
    
    fwrite(s.data(), s.size(), sizeof(uint8_t), file);
    fclose(file);
    
    return 1;
}

void jg_media_select() {
}

void jg_media_insert() {
}

// JG Functions that return values to the frontend
jg_coreinfo_t* jg_get_coreinfo(const char *sys) { return &coreinfo; }
jg_videoinfo_t* jg_get_videoinfo() { return &vidinfo; }
jg_audioinfo_t* jg_get_audioinfo() { return &audinfo; }
jg_inputinfo_t* jg_get_inputinfo(int port) { return &inputinfo[port]; }

void jg_setup_video() { }
void jg_setup_audio() { }

void jg_set_inputstate(jg_inputstate_t *ptr, int port) {
    input_device[port] = ptr;
}

void jg_set_gameinfo(jg_gameinfo_t info) { gameinfo = info; }
void jg_set_paths(jg_pathinfo_t paths) { pathinfo = paths; }
