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

#ifndef SPC_DSP_H
#define SPC_DSP_H

#ifdef __cplusplus
extern "C" {
#endif

#define SPC_STATE_SIZE 640

typedef void (*dsp_copy_func_t)(unsigned char**, void*, size_t);

// Setup

// Initializes DSP and has it use the 64K RAM provided
void spc_dsp_init(void*);

// Sets destination for output samples. If out is NULL or out_size is 0,
// doesn't generate any.
void spc_dsp_set_output(int16_t*, int);

// Sets the interpolation algorithm
void spc_dsp_set_interpolation(int);

// Emulation

// Resets DSP to power-on state
void spc_dsp_reset(void);

// Emulates pressing reset switch on SNES
void spc_dsp_soft_reset(void);

// Sound control

// This is from byuu's snes_spc fork
bool spc_dsp_mute(void);

// Number of samples written to output since it was last set, always
// a multiple of 2. Undefined if more samples were generated than
// output buffer could hold.
int spc_dsp_sample_count(void);

// Reads/writes DSP registers. For accuracy, you must first call run()
// to catch the DSP up to present.
int  spc_dsp_read(int);
void spc_dsp_write(int, int);

// Runs DSP for specified number of clocks (~1024000 per second). Every 32 clocks
// a pair of samples is be generated.
void spc_dsp_run(int);

// State

// Saves/loads exact emulator state
void spc_dsp_copy_state(unsigned char**, dsp_copy_func_t);

#ifdef __cplusplus
}
#endif

#endif
