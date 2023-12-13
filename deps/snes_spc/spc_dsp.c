/* snes_spc 1.0.0 (forked from 0.9.0, http://www.slack.net/~ant/)
 *
 * Copyright (C) 2007 Shay Green
 * Copyright (C) 2023 Rupert Carmichael
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "spc_dsp.h"

// DSP register addresses

#define SPC_BRR_BLOCK_SIZE 9
#define SPC_BRR_BUF_SIZE 12
#define SPC_ECHO_HIST_SIZE 8
#define SPC_EXTRA_SIZE 16
#define SPC_REGISTER_COUNT 128
#define SPC_VOICE_COUNT 8

// Global registers
enum {
    r_mvoll = 0x0C, r_mvolr = 0x1C,
    r_evoll = 0x2C, r_evolr = 0x3C,
    r_kon   = 0x4C, r_koff  = 0x5C,
    r_flg   = 0x6C, r_endx  = 0x7C,
    r_efb   = 0x0D, r_pmon  = 0x2D,
    r_non   = 0x3D, r_eon   = 0x4D,
    r_dir   = 0x5D, r_esa   = 0x6D,
    r_edl   = 0x7D,
    r_fir   = 0x0F // 8 coefficients at 0x0F, 0x1F ... 0x7F
};

// Voice registers
enum {
    v_voll   = 0x00, v_volr   = 0x01,
    v_pitchl = 0x02, v_pitchh = 0x03,
    v_srcn   = 0x04, v_adsr0  = 0x05,
    v_adsr1  = 0x06, v_gain   = 0x07,
    v_envx   = 0x08, v_outx   = 0x09
};

typedef enum _env_mode_t {
    env_release,
    env_attack,
    env_decay,
    env_sustain
} env_mode_t;

typedef struct _voice_t {
    int buf[SPC_BRR_BUF_SIZE*2];// decoded samples (twice the size to simplify wrap handling)
    int buf_pos;            // place in buffer where next samples will be decoded
    int interp_pos;         // relative fractional position in sample (0x1000 = 1.0)
    int brr_addr;           // address of current BRR block
    int brr_offset;         // current decoding offset in BRR block
    uint8_t* regs;          // pointer to voice's DSP registers
    int vbit;               // bitmask for voice: 0x01 for voice 0, 0x02 for voice 1, etc.
    int kon_delay;          // KON delay/current setup phase
    env_mode_t env_mode;
    int env;                // current envelope level
    int hidden_env;         // used by GAIN mode 7, very obscure quirk
    uint8_t t_envx_out;
} voice_t;

typedef struct _state_t {
    uint8_t regs[SPC_REGISTER_COUNT];

    // Echo history keeps most recent 8 samples (twice the size to simplify wrap handling)
    int echo_hist[SPC_ECHO_HIST_SIZE * 2][2];
    int (*echo_hist_pos)[2]; // &echo_hist [0 to 7]

    int every_other_sample; // toggles every sample
    int kon;                // KON value when last checked
    int noise;
    int counter;
    int echo_offset;        // offset from ESA in echo buffer
    int echo_length;        // number of bytes that echo_offset will stop at
    int phase;              // next clock cycle to run (0-31)
    bool kon_check;         // set when a new KON occurs

    // Hidden registers also written to when main register is written to
    int new_kon;
    uint8_t endx_buf;
    uint8_t envx_buf;
    uint8_t outx_buf;

    // Temporary state between clocks

    // read once per sample
    int t_pmon;
    int t_non;
    int t_eon;
    int t_dir;
    int t_koff;

    // read a few clocks ahead then used
    int t_brr_next_addr;
    int t_adsr0;
    int t_brr_header;
    int t_brr_byte;
    int t_srcn;
    int t_esa;
    int t_echo_enabled;

    // internal state that is recalculated every sample
    int t_dir_addr;
    int t_pitch;
    int t_output;
    int t_looped;
    int t_echo_ptr;

    // left/right sums
    int t_main_out[2];
    int t_echo_out[2];
    int t_echo_in[2];

    voice_t voices[SPC_VOICE_COUNT];

    // non-emulation state
    uint8_t* ram; // 64K shared RAM between DSP and SMP
    int mute_mask;
    int16_t* out;
    int16_t* out_end;
    int16_t* out_begin;
    int16_t extra[SPC_EXTRA_SIZE];
} state_t;

static state_t m;

int spc_dsp_sample_count(void) {
    return m.out - m.out_begin;
}

int spc_dsp_read(int addr) {
    assert((unsigned)addr < SPC_REGISTER_COUNT);
    return m.regs[addr];
}

void spc_dsp_write(int addr, int data) {
    assert((unsigned)addr < SPC_REGISTER_COUNT);

    m.regs[addr] = (uint8_t)data;
    switch (addr & 0x0F) {
        case v_envx:
            m.envx_buf = (uint8_t)data;
            break;

        case v_outx:
            m.outx_buf = (uint8_t)data;
            break;

        case 0x0C:
            if (addr == r_kon)
                m.new_kon = (uint8_t)data;

            if (addr == r_endx) { // always cleared, regardless of data written
                m.endx_buf = 0;
                m.regs[r_endx] = 0;
            }
            break;
    }
}

void spc_dsp_mute_voices(int mask) {
    m.mute_mask = mask;
}

bool spc_dsp_mute(void) {
    return m.regs[r_flg] & 0x40;
}

// CPU Byte Order Utilities

static inline unsigned get_le16(void const* p) {
    return  (unsigned)((unsigned char const*)p)[1] << 8 |
            (unsigned)((unsigned char const*)p)[0];
}

static inline void set_le16(void* p, unsigned n) {
    ((unsigned char*)p)[1] = (unsigned char)(n >> 8);
    ((unsigned char*)p)[0] = (unsigned char)n;
}

static uint8_t const initial_regs[SPC_REGISTER_COUNT] =
{
    // register values from byuu's snes_spc fork
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*
    0x45,0x8B,0x5A,0x9A,0xE4,0x82,0x1B,0x78,0x00,0x00,0xAA,0x96,0x89,0x0E,0xE0,0x80,
    0x2A,0x49,0x3D,0xBA,0x14,0xA0,0xAC,0xC5,0x00,0x00,0x51,0xBB,0x9C,0x4E,0x7B,0xFF,
    0xF4,0xFD,0x57,0x32,0x37,0xD9,0x42,0x22,0x00,0x00,0x5B,0x3C,0x9F,0x1B,0x87,0x9A,
    0x6F,0x27,0xAF,0x7B,0xE5,0x68,0x0A,0xD9,0x00,0x00,0x9A,0xC5,0x9C,0x4E,0x7B,0xFF,
    0xEA,0x21,0x78,0x4F,0xDD,0xED,0x24,0x14,0x00,0x00,0x77,0xB1,0xD1,0x36,0xC1,0x67,
    0x52,0x57,0x46,0x3D,0x59,0xF4,0x87,0xA4,0x00,0x00,0x7E,0x44,0x9C,0x4E,0x7B,0xFF,
    0x75,0xF5,0x06,0x97,0x10,0xC3,0x24,0xBB,0x00,0x00,0x7B,0x7A,0xE0,0x60,0x12,0x0F,
    0xF7,0x74,0x1C,0xE5,0x39,0x3D,0x73,0xC1,0x00,0x00,0x7A,0xB3,0xFF,0x4E,0x7B,0xFF
*/
};

// if ( io < -32768 ) io = -32768;
// if ( io >  32767 ) io =  32767;
#define CLAMP16(io)\
{\
    if ((int16_t)io != io)\
        io = (io >> 31) ^ 0x7FFF;\
}

void spc_dsp_set_output(int16_t* out, int size) {
    assert((size & 1) == 0); // must be even
    if (!out) {
        out  = m.extra;
        size = SPC_EXTRA_SIZE;
    }
    m.out_begin = out;
    m.out       = out;
    m.out_end   = out + size;
}

// Volume registers and efb are signed! Easy to forget int8_t cast.
// Prefixes are to avoid accidental use of locals with same names.

// Gaussian interpolation

static short const gauss[512] =
{
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
   2,   2,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   5,   5,   5,   5,
   6,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  10,
  11,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,  15,  16,  16,  17,  17,
  18,  19,  19,  20,  20,  21,  21,  22,  23,  23,  24,  24,  25,  26,  27,  27,
  28,  29,  29,  30,  31,  32,  32,  33,  34,  35,  36,  36,  37,  38,  39,  40,
  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,
  58,  59,  60,  61,  62,  64,  65,  66,  67,  69,  70,  71,  73,  74,  76,  77,
  78,  80,  81,  83,  84,  86,  87,  89,  90,  92,  94,  95,  97,  99, 100, 102,
 104, 106, 107, 109, 111, 113, 115, 117, 118, 120, 122, 124, 126, 128, 130, 132,
 134, 137, 139, 141, 143, 145, 147, 150, 152, 154, 156, 159, 161, 163, 166, 168,
 171, 173, 175, 178, 180, 183, 186, 188, 191, 193, 196, 199, 201, 204, 207, 210,
 212, 215, 218, 221, 224, 227, 230, 233, 236, 239, 242, 245, 248, 251, 254, 257,
 260, 263, 267, 270, 273, 276, 280, 283, 286, 290, 293, 297, 300, 304, 307, 311,
 314, 318, 321, 325, 328, 332, 336, 339, 343, 347, 351, 354, 358, 362, 366, 370,
 374, 378, 381, 385, 389, 393, 397, 401, 405, 410, 414, 418, 422, 426, 430, 434,
 439, 443, 447, 451, 456, 460, 464, 469, 473, 477, 482, 486, 491, 495, 499, 504,
 508, 513, 517, 522, 527, 531, 536, 540, 545, 550, 554, 559, 563, 568, 573, 577,
 582, 587, 592, 596, 601, 606, 611, 615, 620, 625, 630, 635, 640, 644, 649, 654,
 659, 664, 669, 674, 678, 683, 688, 693, 698, 703, 708, 713, 718, 723, 728, 732,
 737, 742, 747, 752, 757, 762, 767, 772, 777, 782, 787, 792, 797, 802, 806, 811,
 816, 821, 826, 831, 836, 841, 846, 851, 855, 860, 865, 870, 875, 880, 884, 889,
 894, 899, 904, 908, 913, 918, 923, 927, 932, 937, 941, 946, 951, 955, 960, 965,
 969, 974, 978, 983, 988, 992, 997,1001,1005,1010,1014,1019,1023,1027,1032,1036,
1040,1045,1049,1053,1057,1061,1066,1070,1074,1078,1082,1086,1090,1094,1098,1102,
1106,1109,1113,1117,1121,1125,1128,1132,1136,1139,1143,1146,1150,1153,1157,1160,
1164,1167,1170,1174,1177,1180,1183,1186,1190,1193,1196,1199,1202,1205,1207,1210,
1213,1216,1219,1221,1224,1227,1229,1232,1234,1237,1239,1241,1244,1246,1248,1251,
1253,1255,1257,1259,1261,1263,1265,1267,1269,1270,1272,1274,1275,1277,1279,1280,
1282,1283,1284,1286,1287,1288,1290,1291,1292,1293,1294,1295,1296,1297,1297,1298,
1299,1300,1300,1301,1302,1302,1303,1303,1303,1304,1304,1304,1304,1304,1305,1305,
};

static inline int interpolate(voice_t const* v) {
    // Make pointers into gaussian based on fractional position between samples
    int offset = v->interp_pos >> 4 & 0xFF;
    short const* fwd = gauss + 255 - offset;
    short const* rev = gauss       + offset; // mirror left half of gaussian

    int const* in = &v->buf[(v->interp_pos >> 12) + v->buf_pos];
    int out;
    out  = (fwd[0]   * in[0]) >> 11;
    out += (fwd[256] * in[1]) >> 11;
    out += (rev[256] * in[2]) >> 11;
    out = (int16_t)out;
    out += (rev[0] * in[3]) >> 11;

    CLAMP16(out);
    out &= ~1;
    return out;
}

//// Counters

int const simple_counter_range = 2048 * 5 * 3; // 30720

static unsigned const counter_rates[32] =
{
   simple_counter_range + 1, // never fires
          2048, 1536,
    1280, 1024,  768,
     640,  512,  384,
     320,  256,  192,
     160,  128,   96,
      80,   64,   48,
      40,   32,   24,
      20,   16,   12,
      10,    8,    6,
       5,    4,    3,
             2,
             1
};

static unsigned const counter_offsets[32] =
{
      1, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
    536, 0, 1040,
         0,
         0
};

static inline void init_counter(void) {
    m.counter = 0;
}

static inline void run_counters(void) {
    if (--m.counter < 0)
        m.counter = simple_counter_range - 1;
}

static inline unsigned read_counter(int rate) {
    return ((unsigned)m.counter + counter_offsets[rate]) % counter_rates[rate];
}

//// Envelope

static inline void run_envelope(voice_t* const v) {
    int env = v->env;
    if (v->env_mode == env_release) { // 60%
        if ((env -= 0x8) < 0)
            env = 0;
        v->env = env;
    }
    else {
        int rate;
        int env_data = v->regs[v_adsr1];
        if (m.t_adsr0 & 0x80) { // 99% ADSR
            if (v->env_mode >= env_decay) { // 99%
                env--;
                env -= env >> 8;
                rate = env_data & 0x1F;
                if (v->env_mode == env_decay) // 1%
                    rate = (m.t_adsr0 >> 3 & 0x0E) + 0x10;
            }
            else { // env_attack
                rate = (m.t_adsr0 & 0x0F) * 2 + 1;
                env += rate < 31 ? 0x20 : 0x400;
            }
        }
        else { // GAIN
            int mode;
            env_data = v->regs[v_gain];
            mode = env_data >> 5;
            if (mode < 4) { // direct
                env = env_data * 0x10;
                rate = 31;
            }
            else {
                rate = env_data & 0x1F;
                if (mode == 4) { // 4: linear decrease
                    env -= 0x20;
                }
                else if (mode < 6) { // 5: exponential decrease
                    env--;
                    env -= env >> 8;
                }
                else { // 6,7: linear increase
                    env += 0x20;
                    if (mode > 6 && (unsigned)v->hidden_env >= 0x600)
                        env += 0x8 - 0x20; // 7: two-slope linear increase
                }
            }
        }

        // Sustain level
        if ((env >> 8) == (env_data >> 5) && v->env_mode == env_decay)
            v->env_mode = env_sustain;

        v->hidden_env = env;

        // unsigned cast because linear decrease going negative also triggers this
        if ((unsigned)env > 0x7FF) {
            env = (env < 0 ? 0 : 0x7FF);
            if (v->env_mode == env_attack)
                v->env_mode = env_decay;
        }

        if (!read_counter(rate))
            v->env = env; // nothing else is controlled by the counter
    }
}

//// BRR Decoding

static inline void decode_brr(voice_t* v) {
    // Arrange the four input nybbles in 0xABCD order for easy decoding
    int nybbles = m.t_brr_byte * 0x100 + m.ram[(v->brr_addr + v->brr_offset + 1) & 0xFFFF];

    int const header = m.t_brr_header;

    // Write to next four samples in circular buffer
    int* pos = &v->buf[v->buf_pos];
    int* end;
    if ((v->buf_pos += 4) >= SPC_BRR_BUF_SIZE)
        v->buf_pos = 0;

    // Decode four samples
    for (end = pos + 4; (end - pos) > 0; ++pos, nybbles <<= 4) {
        // Extract nybble and sign-extend
        int s = (int16_t)nybbles >> 12;

        // Shift sample based on header
        int const shift = header >> 4;
        s = (s << shift) >> 1;
        if (shift >= 0xD) // handle invalid range
            s = (s >> 25) << 11; // same as: s = (s < 0 ? -0x800 : 0)

        // Apply IIR filter (8 is the most commonly used)
        int const filter = header & 0x0C;
        int const p1 = pos[SPC_BRR_BUF_SIZE - 1];
        int const p2 = pos[SPC_BRR_BUF_SIZE - 2] >> 1;
        if (filter >= 8) {
            s += p1;
            s -= p2;
            if (filter == 8) { // s += p1 * 0.953125 - p2 * 0.46875
                s += p2 >> 4;
                s += (p1 * -3) >> 6;
            }
            else { // s += p1 * 0.8984375 - p2 * 0.40625
                s += (p1 * -13) >> 7;
                s += (p2 * 3) >> 4;
            }
        }
        else if (filter) { // s += p1 * 0.46875
            s += p1 >> 1;
            s += (-p1) >> 5;
        }

        // Adjust and write sample
        CLAMP16(s);
        s = (int16_t)(s * 2);
        pos[SPC_BRR_BUF_SIZE] = pos[0] = s; // second copy simplifies wrap-around
    }
}

//// Misc

static inline void misc_27(void) {
    m.t_pmon = m.regs[r_pmon] & 0xFE; // voice 0 doesn't support PMON
}

static inline void misc_28(void) {
    m.t_non = m.regs[r_non];
    m.t_eon = m.regs[r_eon];
    m.t_dir = m.regs[r_dir];
}

static inline void misc_29(void) {
    if ((m.every_other_sample ^= 1) != 0)
        m.new_kon &= ~m.kon; // clears KON 63 clocks after it was last read
}

static inline void misc_30(void) {
    if (m.every_other_sample) {
        m.kon    = m.new_kon;
        m.t_koff = m.regs[r_koff] | m.mute_mask;
    }

    run_counters();

    // Noise
    if (!read_counter(m.regs[r_flg] & 0x1F)) {
        int feedback = (m.noise << 13) ^ (m.noise << 14);
        m.noise = (feedback & 0x4000) ^ (m.noise >> 1);
    }
}

//// Voices

static inline void voice_V1(voice_t* const v) {
    m.t_dir_addr = m.t_dir * 0x100 + m.t_srcn * 4;
    m.t_srcn = v->regs[v_srcn];
}

static inline void voice_V2(voice_t* const v) {
    // Read sample pointer (ignored if not needed)
    uint8_t const* entry = &m.ram[m.t_dir_addr];
    if (!v->kon_delay)
        entry += 2;
    m.t_brr_next_addr = get_le16(entry);

    m.t_adsr0 = v->regs[v_adsr0];

    // Read pitch, spread over two clocks
    m.t_pitch = v->regs[v_pitchl];
}

static inline void voice_V3a(voice_t* const v) {
    m.t_pitch += (v->regs[v_pitchh] & 0x3F) << 8;
}

static inline void voice_V3b(voice_t* const v) {
    // Read BRR header and byte
    m.t_brr_byte   = m.ram[(v->brr_addr + v->brr_offset) & 0xFFFF];
    m.t_brr_header = m.ram[v->brr_addr]; // brr_addr doesn't need masking
}

static inline void voice_V3c(voice_t* const v) {
    // Pitch modulation using previous voice's output
    if (m.t_pmon & v->vbit)
        m.t_pitch += ((m.t_output >> 5) * m.t_pitch) >> 10;

    if (v->kon_delay) {
        // Get ready to start BRR decoding on next sample
        if (v->kon_delay == 5) {
            v->brr_addr    = m.t_brr_next_addr;
            v->brr_offset  = 1;
            v->buf_pos     = 0;
            m.t_brr_header = 0; // header is ignored on this sample
            m.kon_check    = true;
        }

        // Envelope is never run during KON
        v->env        = 0;
        v->hidden_env = 0;

        // Disable BRR decoding until last three samples
        v->interp_pos = 0;
        if (--v->kon_delay & 3)
            v->interp_pos = 0x4000;

        // Pitch is never added during KON
        m.t_pitch = 0;
    }

    // Gaussian interpolation
    {
        int output = interpolate(v);

        // Noise
        if (m.t_non & v->vbit)
            output = (int16_t)(m.noise * 2);

        // Apply envelope
        m.t_output = (output * v->env) >> 11 & ~1;
        v->t_envx_out = (uint8_t)(v->env >> 4);
    }

    // Immediate silence due to end of sample or soft reset
    if (m.regs[r_flg] & 0x80 || (m.t_brr_header & 3) == 1) {
        v->env_mode = env_release;
        v->env      = 0;
    }

    if (m.every_other_sample) {
        // KOFF
        if (m.t_koff & v->vbit)
            v->env_mode = env_release;

        // KON
        if (m.kon & v->vbit) {
            v->kon_delay = 5;
            v->env_mode  = env_attack;
        }
    }

    // Run envelope for next sample
    if (!v->kon_delay)
        run_envelope(v);
}

static inline void voice_output(voice_t const* v, int ch) {
    // Apply left/right volume
    int amp = (m.t_output * (int8_t)v->regs[v_voll + ch]) >> 7;

    // Add to output total
    m.t_main_out[ch] += amp;
    CLAMP16(m.t_main_out[ch]);

    // Optionally add to echo total
    if (m.t_eon & v->vbit) {
        m.t_echo_out[ch] += amp;
        CLAMP16(m.t_echo_out[ch]);
    }
}

static inline void voice_V4(voice_t* const v) {
    // Decode BRR
    m.t_looped = 0;
    if (v->interp_pos >= 0x4000) {
        decode_brr(v);

        if ((v->brr_offset += 2) >= SPC_BRR_BLOCK_SIZE) {
            // Start decoding next BRR block
            assert(v->brr_offset == SPC_BRR_BLOCK_SIZE);
            v->brr_addr = (v->brr_addr + SPC_BRR_BLOCK_SIZE) & 0xFFFF;
            if (m.t_brr_header & 1)
            {
                v->brr_addr = m.t_brr_next_addr;
                m.t_looped = v->vbit;
            }
            v->brr_offset = 1;
        }
    }

    // Apply pitch
    v->interp_pos = (v->interp_pos & 0x3FFF) + m.t_pitch;

    // Keep from getting too far ahead (when using pitch modulation)
    if (v->interp_pos > 0x7FFF)
        v->interp_pos = 0x7FFF;

    // Output left
    voice_output(v, 0);
}

static inline void voice_V5(voice_t* const v) {
    // Output right
    voice_output(v, 1);

    // ENDX, OUTX, and ENVX won't update if you wrote to them 1-2 clocks earlier
    int endx_buf = m.regs[r_endx] | m.t_looped;

    // Clear bit in ENDX if KON just began
    if (v->kon_delay == 5)
        endx_buf &= ~v->vbit;
    m.endx_buf = (uint8_t)endx_buf;
}

static inline void voice_V6(voice_t* const v) {
    (void)v; // avoid compiler warning about unused v
    m.outx_buf = (uint8_t)(m.t_output >> 8);
}

static inline void voice_V7(voice_t* const v) {
    // Update ENDX
    m.regs[r_endx] = m.endx_buf;

    m.envx_buf = v->t_envx_out;
}

static inline void voice_V8(voice_t* const v) {
    // Update OUTX
    v->regs[v_outx] = m.outx_buf;
}

static inline void voice_V9(voice_t* const v) {
    // Update ENVX
    v->regs[v_envx] = m.envx_buf;
}

// Most voices do all these in one clock, so make a handy composite
static inline void voice_V3(voice_t* const v) {
    voice_V3a(v);
    voice_V3b(v);
    voice_V3c(v);
}

// Common combinations of voice steps on different voices. This greatly reduces
// code size and allows everything to be inlined in these functions.
static inline void voice_V7_V4_V1(voice_t* const v) {
    voice_V7(v);
    voice_V1(v+3);
    voice_V4(v+1);
}

static inline void voice_V8_V5_V2(voice_t* const v) {
    voice_V8(v);
    voice_V5(v+1);
    voice_V2(v+2);
}

static inline void voice_V9_V6_V3(voice_t* const v) {
    voice_V9(v);
    voice_V6(v+1);
    voice_V3(v+2);
}

//// Echo

// Current echo buffer pointer for left/right channel
#define ECHO_PTR(ch)      (&m.ram[m.t_echo_ptr + ch * 2])

// Sample in echo history buffer, where 0 is the oldest
#define ECHO_FIR(i)       (m.echo_hist_pos[i])

// Calculate FIR point for left/right channel
#define CALC_FIR(i, ch)   ((ECHO_FIR(i + 1)[ch] * (int8_t)m.regs[r_fir + i * 0x10]) >> 6)

static inline void echo_read(int ch) {
    int s = ((int16_t)get_le16(ECHO_PTR(ch)));
    // second copy simplifies wrap-around handling
    ECHO_FIR(0)[ch] = ECHO_FIR(8)[ch] = s >> 1;
}

static inline void echo_22(void) {
    // History
    if (++m.echo_hist_pos >= &m.echo_hist[SPC_ECHO_HIST_SIZE])
        m.echo_hist_pos = m.echo_hist;

    m.t_echo_ptr = (m.t_esa * 0x100 + m.echo_offset) & 0xFFFF;
    echo_read(0);

    // FIR (using l and r temporaries below helps compiler optimize)
    int l = CALC_FIR(0, 0);
    int r = CALC_FIR(0, 1);

    m.t_echo_in[0] = l;
    m.t_echo_in[1] = r;
}

static inline void echo_23(void) {
    int l = CALC_FIR(1, 0) + CALC_FIR(2, 0);
    int r = CALC_FIR(1, 1) + CALC_FIR(2, 1);

    m.t_echo_in[0] += l;
    m.t_echo_in[1] += r;

    echo_read(1);
}

static inline void echo_24(void) {
    int l = CALC_FIR(3, 0) + CALC_FIR(4, 0) + CALC_FIR(5, 0);
    int r = CALC_FIR(3, 1) + CALC_FIR(4, 1) + CALC_FIR(5, 1);

    m.t_echo_in[0] += l;
    m.t_echo_in[1] += r;
}

static inline void echo_25(void) {
    int l = m.t_echo_in[0] + CALC_FIR(6, 0);
    int r = m.t_echo_in[1] + CALC_FIR(6, 1);

    l = (int16_t)l;
    r = (int16_t)r;

    l += (int16_t)CALC_FIR(7, 0);
    r += (int16_t)CALC_FIR(7, 1);

    CLAMP16(l);
    CLAMP16(r);

    m.t_echo_in[0] = l & ~1;
    m.t_echo_in[1] = r & ~1;
}

static inline int echo_output(int ch) {
    int out = (int16_t)((m.t_main_out[ch] * (int8_t)m.regs[r_mvoll + ch * 0x10]) >> 7) +
            (int16_t)((m.t_echo_in[ch] * (int8_t)m.regs[r_evoll + ch * 0x10]) >> 7);
    CLAMP16(out);
    return out;
}

static inline void echo_26(void) {
    // Left output volumes
    // (save sample for next clock so we can output both together)
    m.t_main_out[0] = echo_output(0);

    // Echo feedback
    int l = m.t_echo_out[0] + (int16_t)((m.t_echo_in[0] * (int8_t)m.regs[r_efb]) >> 7);
    int r = m.t_echo_out[1] + (int16_t)((m.t_echo_in[1] * (int8_t)m.regs[r_efb]) >> 7);

    CLAMP16(l);
    CLAMP16(r);

    m.t_echo_out[0] = l & ~1;
    m.t_echo_out[1] = r & ~1;
}

static inline void echo_27(void) {
    // Output
    int l = m.t_main_out[0];
    int r = echo_output(1);
    m.t_main_out[0] = 0;
    m.t_main_out[1] = 0;

    // TODO: global muting isn't this simple (turns DAC on and off
    // or something, causing small ~37-sample pulse when first muted)
    if (m.regs[r_flg] & 0x40) {
        l = 0;
        r = 0;
    }

    // Output sample to DAC
    int16_t* out = m.out;
    out[0] = l;
    out[1] = r;
    out += 2;
    if (out >= m.out_end) {
        out       = m.extra;
        m.out_end = &m.extra[SPC_EXTRA_SIZE];
    }
    m.out = out;
}

static inline void echo_28(void) {
    m.t_echo_enabled = m.regs[r_flg];
}

static inline void echo_write(int ch) {
    if (!(m.t_echo_enabled & 0x20))
        set_le16(ECHO_PTR(ch), m.t_echo_out[ch]);
    m.t_echo_out[ch] = 0;
}

static inline void echo_29(void) {
    m.t_esa = m.regs[r_esa];

    if (!m.echo_offset)
        m.echo_length = (m.regs[r_edl] & 0x0F) * 0x800;

    m.echo_offset += 4;
    if (m.echo_offset >= m.echo_length)
        m.echo_offset = 0;

    // Write left echo
    echo_write(0);

    m.t_echo_enabled = m.regs[r_flg];
}

static inline void echo_30(void) {
    // Write right echo
    echo_write(1);
}

//// Timing

// Execute clock for a particular voice
#define V(clock, voice)   voice_##clock(&m.voices[voice]);

/* The most common sequence of clocks uses composite operations
for efficiency. For example, the following are equivalent to the
individual steps on the right:

V(V7_V4_V1,2) -> V(V7,2) V(V4,3) V(V1,5)
V(V8_V5_V2,2) -> V(V8,2) V(V5,3) V(V2,4)
V(V9_V6_V3,2) -> V(V9,2) V(V6,3) V(V3,4) */

// Voice      0      1      2      3      4      5      6      7
#define GEN_DSP_TIMING \
PHASE(0)  V(V5,0)V(V2,1)\
PHASE(1)  V(V6,0)V(V3,1)\
PHASE(2)  V(V7_V4_V1,0)\
PHASE(3)  V(V8_V5_V2,0)\
PHASE(4)  V(V9_V6_V3,0)\
PHASE(5)         V(V7_V4_V1,1)\
PHASE(6)         V(V8_V5_V2,1)\
PHASE(7)         V(V9_V6_V3,1)\
PHASE(8)                V(V7_V4_V1,2)\
PHASE(9)                V(V8_V5_V2,2)\
PHASE(10)                V(V9_V6_V3,2)\
PHASE(11)                       V(V7_V4_V1,3)\
PHASE(12)                       V(V8_V5_V2,3)\
PHASE(13)                       V(V9_V6_V3,3)\
PHASE(14)                              V(V7_V4_V1,4)\
PHASE(15)                              V(V8_V5_V2,4)\
PHASE(16)                              V(V9_V6_V3,4)\
PHASE(17)  V(V1,0)                            V(V7,5)V(V4,6)\
PHASE(18)                                     V(V8_V5_V2,5)\
PHASE(19)                                     V(V9_V6_V3,5)\
PHASE(20)         V(V1,1)                            V(V7,6)V(V4,7)\
PHASE(21)                                            V(V8,6)V(V5,7)  V(V2,0)  /* t_brr_next_addr order dependency */\
PHASE(22)  V(V3a,0)                                  V(V9,6)V(V6,7)  echo_22();\
PHASE(23)                                                   V(V7,7)  echo_23();\
PHASE(24)                                                   V(V8,7)  echo_24();\
PHASE(25)  V(V3b,0)                                         V(V9,7)  echo_25();\
PHASE(26)                                                            echo_26();\
PHASE(27) misc_27();                                                 echo_27();\
PHASE(28) misc_28();                                                 echo_28();\
PHASE(29) misc_29();                                                 echo_29();\
PHASE(30) misc_30();V(V3c,0)                                         echo_30();\
PHASE(31)  V(V4,0)       V(V1,2)\

void spc_dsp_run(int clocks_remain) {
    assert(clocks_remain > 0);

    int const phase = m.phase;
    m.phase = (phase + clocks_remain) & 31;
    switch (phase) {
    loop:

        #define PHASE(n) if (n && !--clocks_remain) break;\
            /* fallthrough */\
            case n:
        GEN_DSP_TIMING
        #undef PHASE

        if (--clocks_remain)
            goto loop;
    }
}

//// Setup

void spc_dsp_init(void* ram_64k) {
    m.ram = (uint8_t*)ram_64k;
    spc_dsp_mute_voices(0);
    spc_dsp_set_output(0, 0);
    spc_dsp_reset();
}

static void soft_reset_common(void) {
    assert(m.ram); // init() must have been called already

    m.noise              = 0x4000;
    m.echo_hist_pos      = m.echo_hist;
    m.every_other_sample = 1;
    m.echo_offset        = 0;
    m.phase              = 0;

    init_counter();
}

void spc_dsp_soft_reset(void) {
    m.regs[r_flg] = 0xE0;
    soft_reset_common();
}

void spc_dsp_reset(void) {
    memcpy(m.regs, initial_regs, sizeof m.regs);
    memset(&m.regs[SPC_REGISTER_COUNT], 0, offsetof (state_t,ram) - SPC_REGISTER_COUNT);

    // Internal state
    for (int i = SPC_VOICE_COUNT; --i >= 0;) {
        voice_t* v = &m.voices[i];
        v->brr_offset = 1;
        v->vbit       = 1 << i;
        v->regs       = &m.regs[i * 0x10];
    }
    m.new_kon = m.regs[r_kon];
    m.t_dir   = m.regs[r_dir];
    m.t_esa   = m.regs[r_esa];

    soft_reset_common();
}

//// State save/load
static int spc_state_copy_int(unsigned char** buf, dsp_copy_func_t func, int state, int size) {
    uint8_t s[2];
    set_le16(s, state);
    func(buf, &s, size);
    return get_le16(s);
}

#define SPC_COPY(type, state)\
{\
    state = (type)spc_state_copy_int(io, copy, state, sizeof (type));\
    assert((type)state == state);\
}

void spc_dsp_copy_state(unsigned char** io, dsp_copy_func_t copy) {
    int extra = 0;

    // DSP registers
    copy(io, m.regs, SPC_REGISTER_COUNT);

    // Internal state

    // Voices
    for (int i = 0; i < SPC_VOICE_COUNT; i++) {
        voice_t* v = &m.voices[i];

        // BRR buffer
        for (int n = 0; n < SPC_BRR_BUF_SIZE; n++) {
            int s = v->buf[n];
            SPC_COPY(int16_t, s);
            v->buf[n] = v->buf[n + SPC_BRR_BUF_SIZE] = s;
        }

        SPC_COPY(uint16_t, v->interp_pos);
        SPC_COPY(uint16_t, v->brr_addr);
        SPC_COPY(uint16_t, v->env);
        SPC_COPY(int16_t, v->hidden_env);
        SPC_COPY(uint8_t, v->buf_pos);
        SPC_COPY(uint8_t, v->brr_offset);
        SPC_COPY(uint8_t, v->kon_delay);
        {
            int mode = v->env_mode;
            SPC_COPY(uint8_t, mode);
            v->env_mode = mode;
        }
        SPC_COPY(uint8_t, v->t_envx_out);

        SPC_COPY(uint8_t, extra);
    }

    // Echo history
    for (int i = 0; i < SPC_ECHO_HIST_SIZE; i++) {
        for (int j = 0; j < 2; j++) {
            int s = m.echo_hist_pos[i][j];
            SPC_COPY(int16_t, s);
            m.echo_hist[i][j] = s; // write back at offset 0
        }
    }
    m.echo_hist_pos = m.echo_hist;
    memcpy(&m.echo_hist[SPC_ECHO_HIST_SIZE], m.echo_hist, SPC_ECHO_HIST_SIZE * sizeof m.echo_hist[0]);

    // Misc
    SPC_COPY(uint8_t, m.every_other_sample);
    SPC_COPY(uint8_t, m.kon);

    SPC_COPY(uint16_t, m.noise);
    SPC_COPY(uint16_t, m.counter);
    SPC_COPY(uint16_t, m.echo_offset);
    SPC_COPY(uint16_t, m.echo_length);
    SPC_COPY(uint8_t, m.phase);

    SPC_COPY(uint8_t, m.new_kon);
    SPC_COPY(uint8_t, m.endx_buf);
    SPC_COPY(uint8_t, m.envx_buf);
    SPC_COPY(uint8_t, m.outx_buf);

    SPC_COPY(uint8_t, m.t_pmon);
    SPC_COPY(uint8_t, m.t_non);
    SPC_COPY(uint8_t, m.t_eon);
    SPC_COPY(uint8_t, m.t_dir);
    SPC_COPY(uint8_t, m.t_koff);

    SPC_COPY(uint16_t, m.t_brr_next_addr);
    SPC_COPY(uint8_t, m.t_adsr0);
    SPC_COPY(uint8_t, m.t_brr_header);
    SPC_COPY(uint8_t, m.t_brr_byte);
    SPC_COPY(uint8_t, m.t_srcn);
    SPC_COPY(uint8_t, m.t_esa);
    SPC_COPY(uint8_t, m.t_echo_enabled);

    SPC_COPY(int16_t, m.t_main_out[0]);
    SPC_COPY(int16_t, m.t_main_out[1]);
    SPC_COPY(int16_t, m.t_echo_out[0]);
    SPC_COPY(int16_t, m.t_echo_out[1]);
    SPC_COPY(int16_t, m.t_echo_in[0]);
    SPC_COPY(int16_t, m.t_echo_in[1]);

    SPC_COPY(uint16_t, m.t_dir_addr);
    SPC_COPY(uint16_t, m.t_pitch);
    SPC_COPY(int16_t, m.t_output);
    SPC_COPY(uint16_t, m.t_echo_ptr);
    SPC_COPY(uint8_t, m.t_looped);

    SPC_COPY(uint8_t, extra);
}
