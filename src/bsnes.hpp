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

#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Bsnes {
  namespace Audio {
    typedef struct _Spec {
      double freq;                      /**< Output frequency (Hz) */
      unsigned spf;                     /**< Samples per frame (approx) */
      unsigned rsqual;                  /**< Resampler quality: 0-2 for fast, medium, or best */
      float *buf;                       /**< Buffer for internally resampled and mixed audio samples */
      void *ptr;                        /**< User data passed to callback */
      void (*cb)(const void*, size_t);  /**< Callback for audio output */
    } Spec;

    namespace Interpolation {
      constexpr unsigned Gaussian   = 0;    /**< Gaussian */
      constexpr unsigned Sinc       = 1;    /**< Sinc */
    }
  }

  namespace Video {
    typedef struct _Spec {
      uint32_t *buf;                                            /**< Buffer for raw pixel data */
      void *ptr;                                                /**< User data passed to callback */
      void (*cb)(const void*, unsigned, unsigned, unsigned);    /**< Callback for video output */
    } Spec;
  }

  namespace Input {
    namespace Device {
      constexpr unsigned Unconnected    = 0;    /**< No input device connected */
      constexpr unsigned Gamepad        = 1;    /**< SFC/SNES Controller */
      constexpr unsigned Mouse          = 2;    /**< SFC/SNES Mouse */
      constexpr unsigned Multitap       = 3;    /**< Multitap */
      constexpr unsigned SuperScope     = 4;    /**< Super Scope (light gun) */
      constexpr unsigned Justifier      = 5;    /**< Konami Justifier (light gun) */
    }

    namespace Gamepad {
      constexpr unsigned Up =       (1 << 11);  /**< D-Pad Up */
      constexpr unsigned Down =     (1 << 10);  /**< D-Pad Down */
      constexpr unsigned Left =     (1 << 9);   /**< D-Pad Left */
      constexpr unsigned Right =    (1 << 8);   /**< D-Pad Right */
      constexpr unsigned Select =   (1 << 13);  /**< Button Select */
      constexpr unsigned Start =    (1 << 12);  /**< Button Start */
      constexpr unsigned A =        (1 << 7);   /**< Button A */
      constexpr unsigned B =        (1 << 15);  /**< Button B */
      constexpr unsigned X =        (1 << 6);   /**< Button X */
      constexpr unsigned Y =        (1 << 14);  /**< Button Y */
      constexpr unsigned L =        (1 << 5);   /**< Button L */
      constexpr unsigned R =        (1 << 4);   /**< Button R */
    }

    namespace Mouse {
      constexpr unsigned ButtonL =  (1 << 22);  /**< Mouse: Left Click */
      constexpr unsigned ButtonR =  (1 << 23);  /**< Mouse: Right Click */
    }

    typedef struct _Spec {
      unsigned port;                                /**< Control Port */
      unsigned device;                              /**< Input::Device type */
      void *ptr;                                    /**< User data passed to callback */
      int (*cb)(const void*, unsigned, unsigned);   /**< Input poll callback */
    } Spec;
  }

  namespace GameType {
    constexpr unsigned SuperFamicom =   1;  /**< Super Famicom, SNES */
    constexpr unsigned GameBoy =        2;  /**< Game Boy (Super Game Boy) */
    constexpr unsigned BSX =            3;  /**< BS-X Satellaview */
    constexpr unsigned SufamiTurboA =   4;  /**< Sufami Turbo Slot A */
    constexpr unsigned SufamiTurboB =   5;  /**< Sufami Turbo Slot B */
  }

  namespace Memory {
    constexpr unsigned CartRAM =        0;  /**< SRAM on Cartridge */
    constexpr unsigned RealTimeClock =  1;  /**< Real Time Clock */
    constexpr unsigned SGBCartRAM =     2;  /**< SRAM on Game Boy Cartridge */
    constexpr unsigned BSXDownloadRAM = 3;  /**< BS-X Satellaview Download RAM */
    constexpr unsigned SufamiARAM =     4;  /**< Sufami Turbo Cartridge A SRAM */
    constexpr unsigned SufamiBRAM =     5;  /**< Sufami Turbo Cartridge B SRAM */
    constexpr unsigned MainRAM =        6;  /**< Main system RAM */
    constexpr unsigned VideoRAM =       7;  /**< Video RAM (PPU) */
  }

  namespace Region {
    constexpr unsigned NTSC =   0;  /**< NTSC: Japan, North America */
    constexpr unsigned PAL =    1;  /**< PAL: UK, Europe, Australia */
  }

  /**
   * Determine if content is loaded
   * @return Content is loaded
   */
  bool loaded();

  /**
   * Load content into the emulator (insert cartridge)
   * @return Success/fail
   */
  bool load();

  /**
   * Save any non-volatile data associated with the cartridge
   */
  void save();

  /**
   * Unload content from the emulator (remove cartridge)
   */
  void unload();

  /**
   * Start emulation of the system (turn the power on)
   */
  void power();

  /**
   * Soft reset the emulator (press the reset button)
   */
  void reset();

  /**
   * Run one frame of emulation
   */
  void run();

  /**
   * Run multiple frames of emulation in advance to reduce input latency
   * @param frames Number of frames to run ahead
   */
  void runAhead(unsigned frames);

  /**
   * Determine the size of the state in bytes
   * @return Size of state in bytes
   */
  unsigned serializeSize();

  /**
   * Serialize emulated system state (save state)
   * @param data Empty buffer to store state data
   * @return Size of state in bytes
   */
  unsigned serialize(uint8_t *data);

  /**
   * Unserialize emulated system state (load state)
   * @param data Buffer containing state data
   * @param size Size of buffer containing state data
   * @return Success/fail
   */
  bool unserialize(const uint8_t *data, unsigned size);

  /**
   * Deactivate all cheats and clear the cheat list
   */
  void cheatsClear();

  /**
   * Activate a cheat
   * @param code Cheat code to apply
   */
  void cheatsSetCheat(std::string code);

  /**
   * Determine the emulated system's region
   * @return Emulated system's region
   */
  unsigned getRegion();

  /**
   * Set emulated system's region
   * @param region Emulated system's region
   */
  void setRegion(unsigned region);

  /**
   * Determine whether the loaded content contains a Real-time Clock
   * @return Loaded content contains a Real-time Clock
   */
  bool getRtcPresent();

  /**
   * Retrieve pointer to and size of raw data
   * @param type Type of raw data to retrieve
   * @return Pointer to and size of raw data
   */
  std::pair<void*, unsigned> getMemoryRaw(unsigned type);

  /**
   * Set audio specifications
   * @param spec Audio specifications
   */
  void setAudioSpec(Audio::Spec spec);

  /**
   * Set video specifications
   * @param spec Video specifications
   */
  void setVideoSpec(Video::Spec spec);

  /**
   * Set input specifications
   * @param spec Input specifications
   */
  void setInputSpec(Input::Spec spec);

  /**
   * Delay coprocessor sync for a speed boost at the expense of accuracy
   * @param value on/off
   */
  void setCoprocDelayedSync(bool value);

  /**
   * Prefer high level emulation of coprocessors when available
   * @param value on/off
   */
  void setCoprocPreferHLE(bool value);

  /**
   * Apply hotfixes for games released with fundamental bugs
   * @param value on/off
   */
  void setHotfixes(bool value);

  /**
   * Set video colour parameters
   * @param luminance Luminance, also known as brightness, 0-100
   * @param saturation Saturation, 0-200
   * @param gamma Gamma, 100-200
   */
  void setVideoColourParams(unsigned luminance, unsigned saturation,
    unsigned gamma);

  /**
   * Set the SPC700 (audio processing unit) sample interpolation algorithm
   * @param algo Algorithm: 0-1 for Gaussian, Sinc
   */
  void setSpcInterpolation(unsigned algo);

  /**
   * Set the callback for opening files to be loaded into a buffer
   * @param ptr User data passed to the callback
   * @param cb Callback for opening files to be loaded into a buffer
   */
  void setOpenFileCallback(void *ptr, bool (*cb)(void*, std::string,
    std::vector<uint8_t>&));

  /**
   * Set the callback for opening files as a std::stringstream
   * @param ptr User data passed to the callback
   * @param cb Callback for opening files as a std::stringstream
   */
  void setOpenStreamCallback(void *ptr, bool (*cb)(void*, std::string,
    std::stringstream&));

  /**
   * Set the callback for opening MSU-1 related files as a std::ifstream
   * @param ptr User data passed to the callback
   * @param cb Callback for opening MSU-1 related files as a std::ifstream
   */
  void setOpenMsuCallback(void *ptr, bool (*cb)(void*, std::string,
    std::istream**));

  /**
   * Set the callback for log output
   * @param ptr User data passed to the callback
   * @param cb Callback log output
   */
  void setLogCallback(void *ptr, void (*cb)(void*, int, std::string&));

  /**
   * Set the callback for writing files (non-volatile save data)
   * @param ptr User data passed to the callback
   * @param cb Callback for writing files (non-volatile save data)
   */
  void setWriteCallback(void *ptr, void (*cb)(void*, std::string,
    const uint8_t*, unsigned));

  /**
   * Set the callback for requesting ROM data to be loaded into the emulated system
   * @param ptr User data passed to the callback
   * @param cb Callback for requesting ROM data to be loaded into the emulated system
   */
  void setRomLoadCallback(void *ptr, bool (*cb)(void*, unsigned));

  /**
   * Load BS-X Satellaview content into the emulated system
   * @param data Buffer containing BS-X memory cartridge data
   * @param loc Location of the data (filesystem path)
   */
  void setRomBSMemory(std::vector<uint8_t>& data, std::string& loc);

  /**
   * Load Game Boy content into the emulated system
   * @param data Buffer containing Game Boy cartridge data
   * @param size Size of Game Boy cartridge data buffer
   */
  void setRomGB(const uint8_t *data, size_t size);

  /**
   * Load Sufami Turbo (Slot A) content into the emulated system
   * @param data Buffer containing Sufami Turbo (Slot A) cartridge data
   * @param loc Location of the data (filesystem path)
   */
  void setRomSufamiTurboA(std::vector<uint8_t>& data, std::string& loc);

  /**
   * Load Sufami Turbo (Slot B) content into the emulated system
   * @param data Buffer containing Sufami Turbo (Slot B) cartridge data
   * @param loc Location of the data (filesystem path)
   */
  void setRomSufamiTurboB(std::vector<uint8_t>& data, std::string& loc);

  /**
   * Load Super Famicom/SNES content into the emulated system
   * @param data Buffer containing Super Famicom/SNES cartridge data
   * @param loc Location of the data (filesystem path)
   */
  void setRomSuperFamicom(std::vector<uint8_t>& data, std::string& loc);
}
