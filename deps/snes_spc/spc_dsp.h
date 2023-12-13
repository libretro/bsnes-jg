// Highly accurate SNES SPC-700 DSP emulator

// snes_spc 1.0.0
#ifndef SPC_DSP_H
#define SPC_DSP_H

typedef void (*dsp_copy_func_t)( unsigned char**, void*, size_t );

// DSP register addresses

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

enum { extra_size = 16 };
enum { echo_hist_size = 8 };
typedef enum _env_mode_t { env_release, env_attack, env_decay, env_sustain } env_mode_t;
enum { brr_buf_size = 12 };
enum { brr_block_size = 9 };
enum { voice_count = 8 };
enum { register_count = 128 };

typedef struct _voice_t {
    int buf [brr_buf_size*2];// decoded samples (twice the size to simplify wrap handling)
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

// State

// Resets DSP and uses supplied values to initialize registers
void load( uint8_t const regs [register_count] );

// Saves/loads exact emulator state
enum { state_size = 640 }; // maximum space needed when saving
void spc_dsp_copy_state( unsigned char**, dsp_copy_func_t );

typedef struct _state_t {
    uint8_t regs[register_count];

    // Echo history keeps most recent 8 samples (twice the size to simplify wrap handling)
    int echo_hist [echo_hist_size * 2] [2];
    int (*echo_hist_pos) [2]; // &echo_hist [0 to 7]

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
    int t_main_out [2];
    int t_echo_out [2];
    int t_echo_in  [2];

    voice_t voices [voice_count];

    // non-emulation state
    uint8_t* ram; // 64K shared RAM between DSP and SMP
    int mute_mask;
    int16_t* out;
    int16_t* out_end;
    int16_t* out_begin;
    int16_t extra [extra_size];
} state_t;

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
