#ifndef joypad_h
#define joypad_h
#include "defs.h"
#include <stdbool.h>

typedef enum {
    GB_KEY_RIGHT,
    GB_KEY_LEFT,
    GB_KEY_UP,
    GB_KEY_DOWN,
    GB_KEY_A,
    GB_KEY_B,
    GB_KEY_SELECT,
    GB_KEY_START,
    GB_KEY_MAX
} GB_key_t;

typedef enum {
    GB_KEY_RIGHT_MASK = 1 << GB_KEY_RIGHT,
    GB_KEY_LEFT_MASK = 1 << GB_KEY_LEFT,
    GB_KEY_UP_MASK = 1 << GB_KEY_UP,
    GB_KEY_DOWN_MASK = 1 << GB_KEY_DOWN,
    GB_KEY_A_MASK = 1 << GB_KEY_A,
    GB_KEY_B_MASK = 1 << GB_KEY_B,
    GB_KEY_SELECT_MASK = 1 << GB_KEY_SELECT,
    GB_KEY_START_MASK = 1 << GB_KEY_START,
} GB_key_mask_t;

// For example, for player 2's (0-based; logical player 3) A button, use GB_MASK_FOR_PLAYER(GB_KEY_A_MASK, 2)
#define GB_MASK_FOR_PLAYER(mask, player) ((x) << (player * 8))

void GB_set_key_state(GB_gameboy_t *gb, GB_key_t index, bool pressed);
void GB_set_key_state_for_player(GB_gameboy_t *gb, GB_key_t index, unsigned player, bool pressed);
void GB_set_key_mask(GB_gameboy_t *gb, GB_key_mask_t mask);
void GB_set_key_mask_for_player(GB_gameboy_t *gb, GB_key_mask_t mask, unsigned player);
void GB_icd_set_joyp(GB_gameboy_t *gb, uint8_t value);
bool GB_get_joyp_accessed(GB_gameboy_t *gb);
void GB_clear_joyp_accessed(GB_gameboy_t *gb);
void GB_set_allow_illegal_inputs(GB_gameboy_t *gb, bool allow);
void GB_set_emulate_joypad_bouncing(GB_gameboy_t *gb, bool emulate);

#ifdef GB_INTERNAL
internal void GB_update_joyp(GB_gameboy_t *gb);
internal void GB_joypad_run(GB_gameboy_t *gb, unsigned cycles);
#endif
#endif /* joypad_h */
