/* Copyright 2023 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_TRIPLE_TAP,
} td_action_t;

enum {
  TD_BTN1,
};

static td_action_t btn1_td_action = TD_NONE;

static keyrecord_t fake_record;

bool process_record_user(uint16_t keycode, keyrecord_t *record);

static void btn1_td_finished(tap_dance_state_t *state, void *user_data) ;
static void btn1_td_reset(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {
    [TD_BTN1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, btn1_td_finished, btn1_td_reset),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( KC_BTN1, KC_BTN4, KC_BTN5, KC_BTN2, TD(TD_BTN1), KC_BTN3 )
};

static void setup_fake_record(uint8_t col, uint8_t row, bool pressed);
static void register_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row);
static void unregister_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row);

static td_action_t get_tap_dance_action(tap_dance_state_t *state) {
    if (state->count == 1) return (state->pressed) ? TD_SINGLE_HOLD : TD_SINGLE_TAP;
    else if (state->count == 2) return (state->pressed) ? TD_DOUBLE_HOLD : TD_DOUBLE_TAP;
    else if (state->count == 3 && !state->pressed) return TD_TRIPLE_TAP;
    else return TD_UNKNOWN;
}

static void btn1_td_finished(tap_dance_state_t *state, void *user_data) {
    btn1_td_action = get_tap_dance_action(state);
    switch (btn1_td_action) {
        case TD_TRIPLE_TAP:
            tap_code16(KC_BTN1);
        case TD_DOUBLE_TAP:
            tap_code16(KC_BTN1);
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN1);
            break;
        case TD_SINGLE_HOLD:
            register_custom_keycode(DRAG_SCROLL, 0, 0);
            break;
        case TD_DOUBLE_HOLD:
        register_code16(KC_BTN1);
            break;
        default:
            break;
    }
}

static void btn1_td_reset(tap_dance_state_t *state, void *user_data) {
    switch (btn1_td_action) {
        case TD_SINGLE_HOLD:
            unregister_custom_keycode(DRAG_SCROLL, 0, 0);
            break;
        case TD_DOUBLE_HOLD:
            unregister_code(KC_BTN1);
            break;
        default:
            break;
    }
    btn1_td_action = TD_NONE;
}

// Setup fake_record for process_record_kb()
static void setup_fake_record(uint8_t col, uint8_t row, bool pressed) {
    fake_record.event.key.col = col;
    fake_record.event.key.row = row;
    fake_record.event.pressed = pressed;
    fake_record.event.time = timer_read();
}

// Register a custom keycode with process_record_kb()
static void register_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
    setup_fake_record(col, row, true);
    process_record_kb(keycode, &fake_record);
}

// Unregister a custom keycode with process_record_kb()
static void unregister_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
    setup_fake_record(col, row, false);
    process_record_kb(keycode, &fake_record);
}
