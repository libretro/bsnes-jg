// Highly accurate SNES SPC-700 DSP emulator

// snes_spc 1.0.0
#ifndef SPC_DSP_H
#define SPC_DSP_H

#define SPC_REGISTER_COUNT 128
#define SPC_STATE_SIZE 640

typedef void (*dsp_copy_func_t)( unsigned char**, void*, size_t );

// State

// Saves/loads exact emulator state
void spc_dsp_copy_state( unsigned char**, dsp_copy_func_t );

// Sound control

// This is from byuu's snes_spc fork
bool spc_dsp_mute(void);

// Mutes voices corresponding to non-zero bits in mask (issues repeated KOFF events).
// Reduces emulation accuracy.
void spc_dsp_mute_voices( int );

// Number of samples written to output since it was last set, always
// a multiple of 2. Undefined if more samples were generated than
// output buffer could hold.
int spc_dsp_sample_count(void);

// Reads/writes DSP registers. For accuracy, you must first call run()
// to catch the DSP up to present.
int  spc_dsp_read ( int );
void spc_dsp_write( int, int );

// Runs DSP for specified number of clocks (~1024000 per second). Every 32 clocks
// a pair of samples is be generated.
void spc_dsp_run( int );

// Setup

// Initializes DSP and has it use the 64K RAM provided
void spc_dsp_init( void* );

// Sets destination for output samples. If out is NULL or out_size is 0,
// doesn't generate any.
void spc_dsp_set_output( int16_t*, int );

// Emulation

// Resets DSP to power-on state
void spc_dsp_reset(void);

// Emulates pressing reset switch on SNES
void spc_dsp_soft_reset(void);

#endif
