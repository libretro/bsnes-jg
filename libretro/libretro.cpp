/*
 * bsnes-jg libretro shim
 *
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

#include <fstream>

#include <samplerate.h>

#include "bsnes.hpp"

#include "libretro.h"
#include "libretro_core_options.h"

// BML raw strings
#include "boards_bml.hpp"
#include "BSMemory_bml.hpp"
#include "SuperFamicom_bml.hpp"
#include "SufamiTurbo_bml.hpp"

#if defined(_WIN32)
   static const char pss = '\\';
#else
   static const char pss = '/';
#endif

#define TIMING_NTSC 60.098812
#define TIMING_PAL  50.006979

#define SAMPLERATE 48000

#define ASPECT_NTSC 8.0 / 7.0
#define ASPECT_PAL  7375000.0 / 5320342.5

#define VIDEO_WIDTH     256
#define VIDEO_HEIGHT    240

#define RETRO_DEVICE_MULTITAP   RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)
#define RETRO_DEVICE_SUPERSCOPE RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 0)
#define RETRO_DEVICE_JUSTIFIER  RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 1)

#define RETRO_GAME_TYPE_SGB     0x101 | 0x1000
#define RETRO_GAME_TYPE_BSX     0x102 | 0x1000
#define RETRO_GAME_TYPE_ST      0x103 | 0x1000

#define RETRO_MEMORY_SGB_SRAM   ((1 << 8) | RETRO_MEMORY_SAVE_RAM)
#define RETRO_MEMORY_BSX_PRAM   ((2 << 8) | RETRO_MEMORY_SAVE_RAM)
#define RETRO_MEMORY_ST_A_SRAM  ((3 << 8) | RETRO_MEMORY_SAVE_RAM)
#define RETRO_MEMORY_ST_B_SRAM  ((4 << 8) | RETRO_MEMORY_SAVE_RAM)

// Audio/Video buffers
static uint32_t vbuf[(VIDEO_WIDTH * VIDEO_HEIGHT) << 3];
static float abuf[(SAMPLERATE / 50) << 1];
static int16_t abuf_out[(SAMPLERATE / 50) << 1];

// Core Options
static int run_ahead_frames = 0;
static int rsqual = 0;
static int aspect_ratio_mode = 0;
static int overscan_t = 8;
static int overscan_b = 8;
static int overscan_l = 0;
static int overscan_r = 0;
static int c_luminance = 100;
static int c_saturation = 100;
static int c_gamma = 120;

// Variables used at runtime
static std::string sysdir;
static std::string savedir;
static std::string gamename;
static std::ifstream msu_file_audio;
static std::ifstream msu_file_data;
static bool statecontexts = false;
static bool bitmasks = false;
static bool addon = false;
static int hmult = 2;
static int vmult = 1;

// libretro callbacks
static retro_log_printf_t log_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_audio_sample_t audio_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;
static retro_input_poll_t input_poll_cb = NULL;
static retro_input_state_t input_state_cb = NULL;

// Game info structs for content
static struct retro_game_info cartinfo[2]; // Game Cartridge (SFC, GB, BS-X, ST)
static struct retro_game_info addoninfo; // SGB ROM, BS-X ROM, Sufami Turbo ROM

// Input related
static unsigned bindmap_gamepad[] = {
    Bsnes::Input::Gamepad::B,       Bsnes::Input::Gamepad::Y,
    Bsnes::Input::Gamepad::Select,  Bsnes::Input::Gamepad::Start,
    Bsnes::Input::Gamepad::Up,      Bsnes::Input::Gamepad::Down,
    Bsnes::Input::Gamepad::Left,    Bsnes::Input::Gamepad::Right,
    Bsnes::Input::Gamepad::A,       Bsnes::Input::Gamepad::X,
    Bsnes::Input::Gamepad::L,       Bsnes::Input::Gamepad::R
};

static struct retro_input_descriptor input_desc_gamepad[] = {
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "D-Pad Left" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "D-Pad Up" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "D-Pad Down" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D-Pad Right" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "X" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Y" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "L" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "R" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },

{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "D-Pad Left" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "D-Pad Up" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "D-Pad Down" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D-Pad Right" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "X" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Y" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "L" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "R" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },

{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "D-Pad Left" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "D-Pad Up" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "D-Pad Down" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D-Pad Right" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "X" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Y" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "L" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "R" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
{ 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },

{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "D-Pad Left" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "D-Pad Up" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "D-Pad Down" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D-Pad Right" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "X" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Y" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "L" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "R" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
{ 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },

{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "D-Pad Left" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "D-Pad Up" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "D-Pad Down" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D-Pad Right" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "B" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "A" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "X" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Y" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "L" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "R" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Select" },
{ 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },

{ 0 },
};

static void logCallback(void*, int level, std::string& text) {
    log_cb(retro_log_level(level), text.c_str());
}

static float aspectRatio(void) {
    float aspect_ratio = ASPECT_NTSC;

    switch (aspect_ratio_mode) {
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
        case 4: { // 4:3 DAR
            return 4.0 / 3.0;
        }
    }

    return ((VIDEO_WIDTH - (overscan_l + overscan_r)) * aspect_ratio) /
        (double(VIDEO_HEIGHT) - (overscan_t + overscan_b));
}

static void check_variables(bool first_run) {
    struct retro_variable var = {0};

    if (first_run) {
        var.key   = "bsnes_jg_coproc_delaysync";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
            Bsnes::setCoprocDelayedSync(!strcmp(var.value, "on"));

        var.key   = "bsnes_jg_coproc_preferhle";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
            Bsnes::setCoprocPreferHLE(!strcmp(var.value, "on"));

        var.key   = "bsnes_jg_hotfixes";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
            Bsnes::setHotfixes(!strcmp(var.value, "on"));

        var.key   = "bsnes_jg_rsqual";
        var.value = NULL;
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
            if (!strcmp(var.value, "fast"))
                rsqual = 0;
            else if (!strcmp(var.value, "medium"))
                rsqual = 1;
            else if (!strcmp(var.value, "best"))
                rsqual = 2;
        }

        var.key = "bsnes_jg_competition_timer";
        var.value = NULL;

        // DIP Switches at 0 is 3 minutes, 15 is 18 minutes
        if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
            Bsnes::setDIPSwitches((atoi(var.value) & 0xff) - 3);
    }

    // Run-Ahead
    var.key   = "bsnes_jg_runahead";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
        run_ahead_frames = atoi(var.value);

    // SPC Interpolation Algorithm
    var.key   = "bsnes_jg_spc_interp";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
        if (!strcmp(var.value, "gaussian"))
            Bsnes::setSpcInterpolation(Bsnes::Audio::Interpolation::Gaussian);
        else if (!strcmp(var.value, "sinc"))
            Bsnes::setSpcInterpolation(Bsnes::Audio::Interpolation::Sinc);
    }

    // Aspect Ratio
    var.key   = "bsnes_jg_aspect";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
        if (!strcmp(var.value, "Auto"))
            aspect_ratio_mode = 0;
        else if (!strcmp(var.value, "1:1"))
            aspect_ratio_mode = 1;
        else if (!strcmp(var.value, "8:7"))
            aspect_ratio_mode = 2;
        else if (!strcmp(var.value, "11:8"))
            aspect_ratio_mode = 3;
        else if (!strcmp(var.value, "4:3"))
            aspect_ratio_mode = 4;
    }

    // Overscan
    var.key   = "bsnes_jg_overscan_t";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
        if (!strcmp(var.value, "0"))
            overscan_t = 0;
        else if (!strcmp(var.value, "4"))
            overscan_t = 4;
        else if (!strcmp(var.value, "8"))
            overscan_t = 8;
        else if (!strcmp(var.value, "12"))
            overscan_t = 12;
        else if (!strcmp(var.value, "16"))
            overscan_t = 16;
        else if (!strcmp(var.value, "20"))
            overscan_t = 20;
    }

    var.key   = "bsnes_jg_overscan_b";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
        if (!strcmp(var.value, "0"))
            overscan_b = 0;
        else if (!strcmp(var.value, "4"))
            overscan_b = 4;
        else if (!strcmp(var.value, "8"))
            overscan_b = 8;
        else if (!strcmp(var.value, "12"))
            overscan_b = 12;
        else if (!strcmp(var.value, "16"))
            overscan_b = 16;
        else if (!strcmp(var.value, "20"))
            overscan_b = 20;
        else if (!strcmp(var.value, "21"))
            overscan_b = 21;
    }

    var.key   = "bsnes_jg_overscan_l";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
        if (!strcmp(var.value, "0"))
            overscan_l = 0;
        else if (!strcmp(var.value, "4"))
            overscan_l = 4;
        else if (!strcmp(var.value, "8"))
            overscan_l = 8;
    }

    var.key   = "bsnes_jg_overscan_r";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
        if (!strcmp(var.value, "0"))
            overscan_r = 0;
        else if (!strcmp(var.value, "4"))
            overscan_r = 4;
        else if (!strcmp(var.value, "8"))
            overscan_r = 8;
    }

    // Colour Adjustments
    bool adjust_colours = false;

    var.key = "bsnes_jg_luminance";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var)) {
        int val = atoi(var.value);
        if (c_luminance != val) {
            c_luminance = val;
            adjust_colours = true;
        }
    }

    var.key = "bsnes_jg_saturation";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var)) {
        int val = atoi(var.value);
        if (c_saturation != val) {
            c_saturation = val;
            adjust_colours = true;
        }
    }

    var.key = "bsnes_jg_gamma";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var)) {
        int val = atoi(var.value);
        if (c_gamma != val) {
            c_gamma = val;
            adjust_colours = true;
        }
    }

    if (adjust_colours || first_run)
        Bsnes::setVideoColourParams(c_luminance, c_saturation, c_gamma);

    // Refresh geometry
    struct retro_system_av_info avinfo;
    retro_get_system_av_info(&avinfo);
    avinfo.geometry.base_width = unsigned(VIDEO_WIDTH - (overscan_l + overscan_r)),
    avinfo.geometry.base_height = unsigned(VIDEO_HEIGHT - (overscan_t + overscan_b)),
    avinfo.geometry.aspect_ratio = aspectRatio();
    environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &avinfo);
}

static bool fileOpenS(void*, std::string name, std::stringstream& ss) {
    if (name == "boards.bml")
        ss << boards_bml;
    else if (name == "SuperFamicom.bml")
        ss << SuperFamicom_bml;
    else if (name == "SufamiTurbo.bml")
        ss << SufamiTurbo_bml;
    else if (name == "BSMemory.bml")
        ss << BSMemory_bml;
    return true;
}

static bool fileOpenV(void*, std::string name, std::vector<uint8_t>& v) {
    std::string path;
    bool required = false;

    if (name == "save.ram") {
        return false; // libretro handles this via raw memory
    }
    else if (name == "dsp1.program.rom") {
        path = sysdir + pss + "dsp1.program.rom";
    }
    else if (name == "dsp1.data.rom") {
        path = sysdir + pss + "dsp1.data.rom";
    }
    else if (name == "dsp1b.program.rom") {
        path = sysdir + pss + "dsp1b.program.rom";
    }
    else if (name == "dsp1b.data.rom") {
        path = sysdir + pss + "dsp1b.data.rom";
    }
    else if (name == "dsp2.program.rom") {
        path = sysdir + pss + "dsp2.program.rom";
    }
    else if (name == "dsp2.data.rom") {
        path = sysdir + pss + "dsp2.data.rom";
    }
    else if (name == "dsp3.program.rom") {
        path = sysdir + pss + "dsp3.program.rom";
        required = true;
    }
    else if (name == "dsp3.data.rom") {
        path = sysdir + pss + "dsp3.data.rom";
        required = true;
    }
    else if (name == "dsp4.program.rom") {
        path = sysdir + pss + "dsp4.program.rom";
    }
    else if (name == "dsp4.data.rom") {
        path = sysdir + pss + "dsp4.data.rom";
    }
    else if (name == "st010.program.rom") {
        path = sysdir + pss + "st010.program.rom";
    }
    else if (name == "st010.data.rom") {
        path = sysdir + pss + "st010.data.rom";
    }
    else if (name == "st011.program.rom") {
        path = sysdir + pss + "st011.program.rom";
        required = true;
    }
    else if (name == "st011.data.rom") {
        path = sysdir + pss + "st011.data.rom";
        required = true;
    }
    else if (name == "arm6.program.rom") {
        path = sysdir + pss + "st018.program.rom";
        required = true;
    }
    else if (name == "arm6.data.rom") {
        path = sysdir + pss + "st018.data.rom";
        required = true;
    }
    else if (name == "msu1/data.rom") {
        path = std::string(cartinfo[0].path);
        path = path.substr(0, path.find_last_of(".")) + ".msu";
    }
    else if (name == "time.rtc") {
        path = savedir + pss + gamename + ".rtc";
    }

    std::ifstream stream(path, std::ios::in | std::ios::binary);
    if (!stream.is_open()) {
        if (required)
            log_cb(RETRO_LOG_ERROR, "Failed to load file: %s\n", path.c_str());
        else
            return false;
    }
    v = std::vector<uint8_t>((std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>());
    stream.close();

    log_cb(RETRO_LOG_INFO, "Loaded %s (%ld bytes)\n", path.c_str(), v.size());
    return true;
}

static bool fileOpenMsu(void*, std::string name, std::istream **is) {
    std::string path = cartinfo[0].path;

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

static void fileWrite(void*, std::string name, const uint8_t *data, unsigned size) {
    /* libretro only writes files directly for RTC data. It would be ideal
       if this could be removed in favour of using retro_get_memory*, but
       bsnes just wasn't written in a way which is conducive to this.
    */
    std::string path;
    if (name == "time.rtc")
        path = savedir + pss + gamename + ".rtc";

    if (path.empty())
        return;

    std::ofstream stream(path, std::ios::out | std::ios::binary);
    if (stream.is_open()) {
        stream.write((const char*)data, size);
        stream.close();
        log_cb(RETRO_LOG_INFO, "File saved %s\n", path.c_str());
    }
    else {
        log_cb(RETRO_LOG_WARN, "Failed to save file: %s\n", path.c_str());
    }
}

static void audioFrame(const void*, size_t numsamps) {
    src_float_to_short_array(abuf, abuf_out, numsamps);
    audio_batch_cb(abuf_out, numsamps >> 1);
}

static void videoFrame(const void*, unsigned w, unsigned h, unsigned pitch) {
    uint32_t *vptr = vbuf;
    hmult = w / VIDEO_WIDTH;
    vmult = h / VIDEO_HEIGHT;

    vptr += overscan_t * pitch * vmult;
    h -= (overscan_t + overscan_b) * vmult;

    vptr += (hmult * overscan_l);
    w -= (overscan_l + overscan_r) * hmult;

    video_cb(vptr, w, h, pitch << 2);
}

static int pollNull(const void*, unsigned, unsigned) {
    return 0;
}

static int pollGamepad(const void*, unsigned port, unsigned) {
    input_poll_cb();
    int b = 0;

    if (bitmasks) {
        int16_t ret = input_state_cb(port, RETRO_DEVICE_JOYPAD, 0,
            RETRO_DEVICE_ID_JOYPAD_MASK);
        for (unsigned i = 0; i <= RETRO_DEVICE_ID_JOYPAD_R; ++i) {
            if (ret & (1 << i)) b |= bindmap_gamepad[i];
        }
        return b;
    }

    for (unsigned i = 0; i <= RETRO_DEVICE_ID_JOYPAD_R; ++i) {
        if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, i))
            b |= bindmap_gamepad[i];
    }

    return b;
}

static int pollMouse(const void*, unsigned port, unsigned phase) {
    input_poll_cb();
    int b = 0;

    switch (phase) {
        case 0: { // X
            b = input_state_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X) / 2;
            break;
        }
        case 1: { // Y
            b = input_state_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y) / 2;
            break;
        }
        case 2: { // Buttons (pre-shifted)
            if (input_state_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT))
                b |= Bsnes::Input::Mouse::ButtonL;
            if (input_state_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT))
                b |= Bsnes::Input::Mouse::ButtonR;
            break;
        }
    }

    return b;
}

static int pollLightgun(const void*, unsigned port, unsigned phase) {
    input_poll_cb();
    int b = 0;

    switch (phase) {
        case 0: { // X
            int vwidth = ((VIDEO_WIDTH << 1) / hmult) - (overscan_l + overscan_r);
            b = input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_X);
            b = (b + 0x7fff) * vwidth / (0x7fff * 2);
            b += overscan_l;
            if (input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_RELOAD))
                b = 1026; // Pretend to be aiming offscreen
            break;
        }
        case 1: { // Y
            int vheight = ((VIDEO_HEIGHT << 1) / vmult) - (overscan_t + overscan_b);
            b = input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_SCREEN_Y);
            b = (b + 0x7fff) * vheight / (0x7fff * 2);
            b += overscan_t;
            break;
        }
        case 2: { // Trigger (SuperScope/Justifier), Offscreen (Justifier)
            b = input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_TRIGGER) ||
                input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_RELOAD);
            break;
        }
        case 3: { // Cursor (SuperScope)
            b = input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_AUX_A);
            break;
        }
        case 4: { // Turbo (SuperScope)
            b = input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_AUX_B);
            break;
        }
        case 5: { // Pause (SuperScope), Start (Justifier)
            b = input_state_cb(port, RETRO_DEVICE_LIGHTGUN, 0, RETRO_DEVICE_ID_LIGHTGUN_START);
            break;
        }
    }

    return b;
}

static inline std::vector<uint8_t> bufToVec(const void *data, size_t size) {
    uint8_t *buf = (uint8_t*)data;
    return std::vector<uint8_t>(buf, buf + size);
}

static bool loadRom(void*, unsigned id) {
    if (id == Bsnes::GameType::SuperFamicom) {
        std::vector<uint8_t> rom = addon ?
            bufToVec(addoninfo.data, addoninfo.size) :
            bufToVec(cartinfo[0].data, cartinfo[0].size);

        if (rom.size() < 0x8000)
            return false;

        if ((rom.size() & 0x7fff) == 512) // remove copier header
            rom.erase(rom.begin(), rom.begin() + 512);

        std::string loc(cartinfo[0].path);
        Bsnes::setRomSuperFamicom(rom, loc);
        return true;
    }
    else if (id == Bsnes::GameType::BSX) {
        std::vector<uint8_t> rom = bufToVec(cartinfo[0].data, cartinfo[0].size);
        if (rom.size() < 0x8000)
            return false;

        std::string loc(cartinfo[0].path);
        Bsnes::setRomBSMemory(rom, loc);
        return true;
    }
    else if (id == Bsnes::GameType::SufamiTurboA) {
        std::vector<uint8_t> rom = bufToVec(cartinfo[0].data, cartinfo[0].size);
        if (rom.size() < 0x20000)
            return false;

        std::string loc(cartinfo[0].path);
        Bsnes::setRomSufamiTurboA(rom, loc);
        return true;
    }
    else if (id == Bsnes::GameType::SufamiTurboB) {
        std::vector<uint8_t> rom = bufToVec(cartinfo[1].data, cartinfo[1].size);
        if (rom.size() < 0x20000)
            return false;

        std::string loc(cartinfo[1].path);
        Bsnes::setRomSufamiTurboB(rom, loc);
        return true;
    }
    return false;
}

static bool prepareLoadGame() {
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt)) {
        log_cb(RETRO_LOG_INFO, "XRGB8888 unsupported\n");
        return false;
    }

    // Get the system directory path
    const char *sdbuf;
    if (!environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &sdbuf) || !sdbuf)
        return false;
    sysdir = std::string(sdbuf);

    // Get the save directory path
    if (!environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &sdbuf) || !sdbuf)
        return false;
    savedir = std::string(sdbuf);

    return true;
}

static bool finishLoadGame() {
    if (!Bsnes::load()) {
        log_cb(RETRO_LOG_ERROR, "Failed to load ROM\n");
        return false;
    }

    // Set up audio/video
    unsigned spf(SAMPLERATE / (Bsnes::getRegion() ? TIMING_PAL : TIMING_NTSC));
    Bsnes::setAudioSpec({double(SAMPLERATE), spf << 1, 0, abuf, nullptr, &audioFrame});
    Bsnes::setVideoSpec({vbuf, nullptr, &videoFrame});

    // Power up!
    Bsnes::power();

    // Set input descriptors
    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, input_desc_gamepad);

    return true;
}

void retro_init(void) {
    // Set up frontend log callback
    struct retro_log_callback log;
    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
        log_cb = log.log;

    // Check if frontend supports input bitmasks
    if (environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
        bitmasks = true;

    if (environ_cb(RETRO_ENVIRONMENT_GET_SAVESTATE_CONTEXT, NULL))
        statecontexts = true;
    else
        log_cb(RETRO_LOG_WARN, "Savestate context support is required for runahead\n");

    // Set initial core options
    check_variables(true);

    // Set callbacks
    Bsnes::setOpenFileCallback(nullptr, fileOpenV);
    Bsnes::setOpenStreamCallback(nullptr, fileOpenS);
    Bsnes::setOpenMsuCallback(nullptr, fileOpenMsu);
    Bsnes::setLogCallback(nullptr, logCallback);
    Bsnes::setRomLoadCallback(nullptr, loadRom);
    Bsnes::setWriteCallback(nullptr, fileWrite);
}

void retro_deinit(void) {
}

unsigned retro_api_version(void) {
    return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device) {
    log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
    switch (device) {
        case RETRO_DEVICE_JOYPAD: {
            Bsnes::setInputSpec({port, Bsnes::Input::Device::Gamepad,
                    nullptr, pollGamepad});
            break;
        }
        case RETRO_DEVICE_MOUSE: {
            Bsnes::setInputSpec({port, Bsnes::Input::Device::Mouse,
                    nullptr, pollMouse});
            break;
        }
        case RETRO_DEVICE_MULTITAP: {
            Bsnes::setInputSpec({port, Bsnes::Input::Device::Multitap,
                    nullptr, pollGamepad});
            break;
        }
        case RETRO_DEVICE_SUPERSCOPE: {
            Bsnes::setInputSpec({port, Bsnes::Input::Device::SuperScope,
                    nullptr, pollLightgun});
            break;
        }
        case RETRO_DEVICE_JUSTIFIER: {
            Bsnes::setInputSpec({port, Bsnes::Input::Device::Justifier,
                    nullptr, pollLightgun});
            break;
        }
        default: {
            Bsnes::setInputSpec({port, Bsnes::Input::Device::Unconnected,
                    nullptr, pollNull});
            break;
        }
    }
}

void retro_get_system_info(struct retro_system_info *info) {
    memset(info, 0, sizeof(*info));
    info->library_name     = "bsnes-jg";
    info->library_version  = "2.0.0";
    info->need_fullpath    = false;
    info->valid_extensions = "smc|sfc|gb|gbc|bs|st";
}

void retro_get_system_av_info(struct retro_system_av_info *info) {
    info->timing = (struct retro_system_timing) {
        .fps = Bsnes::getRegion() ? TIMING_PAL : TIMING_NTSC,
        .sample_rate = SAMPLERATE
    };

    info->geometry = (struct retro_game_geometry) {
        .base_width   = unsigned(VIDEO_WIDTH - (overscan_l + overscan_r)),
        .base_height  = unsigned(VIDEO_HEIGHT - (overscan_t + overscan_b)),
        .max_width    = VIDEO_WIDTH << 1,
        .max_height   = VIDEO_HEIGHT << 1,
        .aspect_ratio = aspectRatio()
    };
}

void retro_set_environment(retro_environment_t cb) {
    environ_cb = cb;
    libretro_set_core_options(environ_cb);

    // Subsystem - Super Game Boy
    static const retro_subsystem_memory_info sgb_mem[] = {
        { "srm", RETRO_MEMORY_SGB_SRAM }
    };

    static const struct retro_subsystem_rom_info sgb_roms[] = {
        { "Game Boy", "gb|gbc", false, false, true, sgb_mem, 1 },
        { "Super Game Boy ROM", "sfc|smc", false, false, true, NULL, 0 }
    };


    // Subsystem - BS-X Satellaview
    static const retro_subsystem_memory_info bsx_memory[] = {
        { "srm", RETRO_MEMORY_SAVE_RAM },
        { "psr", RETRO_MEMORY_BSX_PRAM },
    };

    static const struct retro_subsystem_rom_info bsx_roms[] = {
        { "BS-X Memory", "bs", false, false, true, bsx_memory, 2 },
        { "BS-X BIOS", "sfc|smc", false, false, true, NULL, 0 },
    };

    // Subsystem - Sufami Turbo
    static const retro_subsystem_memory_info sufami_a_memory[] = {
        { "srm", RETRO_MEMORY_ST_A_SRAM },
    };

    static const retro_subsystem_memory_info sufami_b_memory[] = {
        { "srm", RETRO_MEMORY_ST_B_SRAM },
    };

    static const struct retro_subsystem_rom_info sufami_roms[] = {
        { "Sufami Turbo Cartridge A", "sfc|smc|st",
            false, false, true, sufami_a_memory, 1 },
        { "Sufami Turbo Cartridge B", "sfc|smc|st",
            false, false, true, sufami_b_memory, 1 },
        { "Sufami Turbo BIOS", "sfc|smc", false, false, true, NULL, 0 },
    };

    // List of all subsystems
    static const struct retro_subsystem_info subsystems[] = {
        { "Super GameBoy", "sgb", sgb_roms, 2, RETRO_GAME_TYPE_SGB },
        { "BS-X Satellaview", "bsx", bsx_roms, 2, RETRO_GAME_TYPE_BSX },
        { "Sufami Turbo (One Cart)", "sufami", sufami_roms, 2, RETRO_GAME_TYPE_ST },
        { "Sufami Turbo (Two Carts)", "sufami2", sufami_roms, 3, RETRO_GAME_TYPE_ST },
        { NULL }
    };

    environ_cb(RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO, (void*)subsystems);

    // Input Devices
    static const struct retro_controller_description port1[] = {
        { "Gamepad", RETRO_DEVICE_JOYPAD },
        { "Mouse", RETRO_DEVICE_MOUSE },
        { NULL, 0 },
    };

    static const struct retro_controller_description port2[] = {
        { "Gamepad", RETRO_DEVICE_JOYPAD },
        { "Mouse", RETRO_DEVICE_MOUSE },
        { "Multitap", RETRO_DEVICE_MULTITAP },
        { "SuperScope", RETRO_DEVICE_SUPERSCOPE },
        { "Justifier", RETRO_DEVICE_JUSTIFIER },
        { NULL, 0 },
    };

   static const struct retro_controller_info ports[] = {
        { port1, 2 },
        { port2, 5 },
        { NULL, 0 },
   };

   environ_cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

void retro_set_audio_sample(retro_audio_sample_t cb) {
    audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
    audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb) {
    input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb) {
    input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb) {
    video_cb = cb;
}

void retro_reset(void) {
    Bsnes::reset();
}

void retro_run(void) {
    bool update = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &update) && update)
        check_variables(false);

    bool fastforwarding = false;
    environ_cb(RETRO_ENVIRONMENT_GET_FASTFORWARDING, &fastforwarding);
    if (fastforwarding || !run_ahead_frames)
        Bsnes::run();
    else
        Bsnes::runAhead(run_ahead_frames);
}

bool retro_load_game(const struct retro_game_info *info) {
    if (!prepareLoadGame())
        return false;

    // Deduce the game name from the filesystem path
    gamename = std::string(info->path);
    gamename = gamename.substr(gamename.find_last_of(pss) + 1);
    gamename = gamename.substr(0, gamename.find_last_of('.'));

    // Load the game
    cartinfo[0] = *info;
    return finishLoadGame();
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num) {
    if (!prepareLoadGame())
        return false;

    // An "addon" is present: SGB, BS-X, or Sufami Turbo
    addon = true;

    // Load the game
    switch (type) {
        case RETRO_GAME_TYPE_SGB: {
            cartinfo[0] = info[0];
            addoninfo = info[1];
            Bsnes::setRomGB((const uint8_t*)cartinfo[0].data, cartinfo[0].size);
            break;
        }
        case RETRO_GAME_TYPE_BSX: {
            cartinfo[0] = info[0];
            addoninfo = info[1];
            break;
        }
        case RETRO_GAME_TYPE_ST: {
            cartinfo[0] = info[0];
            if (num > 2) {
                cartinfo[1] = info[1];
                addoninfo = info[2];
            }
            else {
                addoninfo = info[1];
            }
            break;
        }
        default: {
            return false;
        }
    }

    return finishLoadGame();
}

void retro_unload_game(void) {
    if (Bsnes::getRtcPresent())
        Bsnes::save();
    Bsnes::unload();

    if (msu_file_audio.is_open())
        msu_file_audio.close();

    if (msu_file_data.is_open())
        msu_file_data.close();
}

unsigned retro_get_region(void) {
    return Bsnes::getRegion() == Bsnes::Region::PAL ? RETRO_REGION_PAL : RETRO_REGION_NTSC;
}

size_t retro_serialize_size(void) {
    return Bsnes::serializeSize();
}

bool retro_serialize(void *data, size_t size) {
    return size == Bsnes::serialize(static_cast<uint8_t*>(data));
}

bool retro_unserialize(const void *data, size_t size) {
    return Bsnes::unserialize(static_cast<const uint8_t*>(data), size);
}

void *retro_get_memory_data(unsigned id) {
    if (!Bsnes::loaded())
        return nullptr;

    switch (id) {
        case RETRO_MEMORY_SAVE_RAM: {
            return Bsnes::getMemoryRaw(Bsnes::Memory::CartRAM).first;
        }
        // RTC raw access does not work, but maybe one day
        /*case RETRO_MEMORY_RTC: {
            if (Bsnes::getRtcPresent()) {
                return Bsnes::getMemoryRaw(Bsnes::Memory::RealTimeClock).first;
            }
        }*/
        case RETRO_MEMORY_SYSTEM_RAM: {
            return Bsnes::getMemoryRaw(Bsnes::Memory::MainRAM).first;
        }
        case RETRO_MEMORY_VIDEO_RAM: {
            return Bsnes::getMemoryRaw(Bsnes::Memory::VideoRAM).first;
        }
        case RETRO_MEMORY_SGB_SRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::SGBCartRAM).first;
        }
        case RETRO_MEMORY_BSX_PRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::BSXDownloadRAM).first;
        }
        case RETRO_MEMORY_ST_A_SRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::SufamiARAM).first;
        }
        case RETRO_MEMORY_ST_B_SRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::SufamiBRAM).first;
        }
    }

    return nullptr;
}

size_t retro_get_memory_size(unsigned id) {
    if (!Bsnes::loaded())
        return 0;

    switch (id) {
        case RETRO_MEMORY_SAVE_RAM: {
            return Bsnes::getMemoryRaw(Bsnes::Memory::CartRAM).second;
        }
        /*case RETRO_MEMORY_RTC: {
            if (Bsnes::getRtcPresent()) {
                return Bsnes::getMemoryRaw(Bsnes::Memory::RealTimeClock).second;
            }
        }*/
        case RETRO_MEMORY_SYSTEM_RAM: {
            return Bsnes::getMemoryRaw(Bsnes::Memory::MainRAM).second;
        }
        case RETRO_MEMORY_VIDEO_RAM: {
            return Bsnes::getMemoryRaw(Bsnes::Memory::VideoRAM).second;
        }
        case RETRO_MEMORY_SGB_SRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::SGBCartRAM).second;
        }
        case RETRO_MEMORY_BSX_PRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::BSXDownloadRAM).second;
        }
        case RETRO_MEMORY_ST_A_SRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::SufamiARAM).second;
        }
        case RETRO_MEMORY_ST_B_SRAM : {
            return Bsnes::getMemoryRaw(Bsnes::Memory::SufamiBRAM).second;
        }
    }

    return 0;
}

void retro_cheat_reset(void) {
    Bsnes::cheatsClear();
}

void retro_cheat_set(unsigned, bool, const char *code) {
    Bsnes::cheatsSetCheat(code);
}
