/*
Copyright 2014 Warren Janssens <warren.janssens@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "keycode.h"
#include "action.h"
#include "action_code.h"
#include "action_macro.h"
#include "bootloader.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"
#include "ergo60.h"


/* ErgoDox keymap definition macro */
#define KEYMAP(                                         \
                                                        \
    /* left hand, spatial positions */                  \
    k00,k01,k02,k03,k04,                                \
    k10,k11,k12,k13,k14,                                \
    k20,k21,k22,k23,k24,                                \
    k30,k31,k32,k33,k34,                                \
    k40,k41,k42,k43,k44,                                \
                        k53,k54,                        \
                    k62,k63,k64,                        \
                                                        \
    /* right hand, spatial positions */                 \
            k05,k06,k07,k08,k09,                        \
            k15,k16,k17,k18,k19,                        \
            k25,k26,k27,k28,k29,                        \
            k35,k36,k37,k38,k39,                        \
            k45,k46,k47,k48,k49,                        \
    k55,k56,                                            \
    k65,k66,k67 )                                       \
                                                        \
   /* matrix positions */                                                \
   {                                                                     \
    { KC_##k00,KC_##k10,KC_##k20,KC_##k30,KC_##k40,KC_NO,   KC_NO   },   \
    { KC_##k01,KC_##k11,KC_##k21,KC_##k31,KC_##k41,KC_NO,   KC_NO   },   \
    { KC_##k02,KC_##k12,KC_##k22,KC_##k32,KC_##k42,KC_NO,   KC_##k62},   \
    { KC_##k03,KC_##k13,KC_##k23,KC_##k33,KC_##k43,KC_##k53,KC_##k63},   \
    { KC_##k04,KC_##k14,KC_##k24,KC_##k34,KC_##k44,KC_##k54,KC_##k64},   \

    { KC_##k05,KC_##k15,KC_##k25,KC_##k35,KC_##k45,KC_##k55,KC_##k65},   \
    { KC_##k06,KC_##k16,KC_##k26,KC_##k36,KC_##k46,KC_##k56,KC_##k66},   \
    { KC_##k07,KC_##k17,KC_##k27,KC_##k37,KC_##k47,KC_NO   ,KC_##k67},   \
    { KC_##k08,KC_##k18,KC_##k28,KC_##k38,KC_##k48,KC_NO   ,KC_NO   },   \
    { KC_##k09,KC_##k19,KC_##k29,KC_##k39,KC_##k49,KC_NO   ,KC_NO   },   \
   }

#if defined(KEYMAP_DVORAK)
#include "keymap_dvorak.h"
#elif defined(KEYMAP_COLEMAK)
#include "keymap_colemak.h"
#elif defined(KEYMAP_WORKMAN)
#include "keymap_workman.h"
#else
#include "keymap_qwerty.h"
#endif


#define KEYMAPS_SIZE    (sizeof(keymaps) / sizeof(keymaps[0]))
#define FN_ACTIONS_SIZE (sizeof(fn_actions) / sizeof(fn_actions[0]))

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, key_t key)
{
    if (layer < KEYMAPS_SIZE) {
        return pgm_read_byte(&keymaps[(layer)][(key.row)][(key.col)]);
    } else {
        // fall back to layer 0
        return pgm_read_byte(&keymaps[0][(key.row)][(key.col)]);
    }
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    action_t action;
    if (FN_INDEX(keycode) < FN_ACTIONS_SIZE) {
        action.code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]);
    } else {
        action.code = ACTION_NO;
    }
    return action;
}

