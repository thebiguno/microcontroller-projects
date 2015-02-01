/****************************************************************************************************
*
* Keymap: Default Layer in Dvorak
* 
* ,-------------------------------------------------------------------------------------------------------------------.
* | Caps   |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F8  |  F9  |  F10 |  F12 | PSCR | SLCK | PAUS |      | Boot   |
* |--------+------+------+------+------+------+---------------------------+------+------+------+------+------+--------|
* | =+     |  1!  |  2@  |  3#  |  4$  |  5%  |                           |  6^  |  7&  |  8*  |  9(  |  0)  | -_     |
* |--------+------+------+------+------+------|                           +------+------+------+------+------+--------|
* | Tab    |  '"  |  ,<  |  .>  |   P  |   Y  |                           |   J  |   F  |   U  |   P  |   $  | /?     |
* |--------+------+------+------+------+------|                           |------+------+------+------+------+--------|
* | Esc    |   A  |   O  |   E  |   U  |   I  |                           |   Y  |   N  |   E  |   O  |   I  | \|     |
* |--------+------+------+------+------+------|                           |------+------+------+------+------+--------|
* | Shift  |  ;:  |   Q  |   J  |   K  |   X  |                           |   B  |   M  |   W  |   V  |   Z  | Shift  |
* `--------+------+------+------+------+-------                           `------+------+------+------+------+--------'
*          | Shift| Ctrl | Alt  | Gui  |                                         | Gui  | Alt  | Ctrl | Shift|
*          |  -_  |  =+  | Left | Right|                                         | Up   | Left |  [{  |  ]}  |
*          `---------------------------'                                         `---------------------------'
*                                        ,-------------.         ,-------------.
*                                        |  /?  |  \|  |         |  `~  | §±   |
*                                 ,------|------|------|         |------+------+------.
*                                 |      |      | Fn   |         | Fn   |      |      |
*                                 | BkSp | Del  |------|         |------|Return| Space|
*                                 |      |      | Esc  |         | Tab  |      |      |
*                                 `--------------------'         `--------------------'
*
* Keymap: Fn/Keypad Layer
*
* ,-------------------------------------------------------------------------------------------------------------------.
* | Esc    |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F8  |  F9  |  F10 |  F12 | PSCR | SLCK | PAUS |      | Boot   |
* |--------+------+------+------+------+------+---------------------------+------+------+------+------+------+--------|
* | Pause  |  F1  |  F2  |  F3  |  F4  |  F5  |                           |  F6  |  F7  |  F8  |  F9  |  F10 | Clear  |
* |--------+------+------+------+------+------|                           +------+------+------+------+------+--------|
* | TRNS   |  F11 |  F12 |  F13 |  F14 | Vol+ |                           | Next |   7  |   8  |   9  |   -  |        |
* |--------+------+------+------+------+------|                           |------+------+------+------+------+--------|
* | Caps   | PgUp | Home |  Up  |  End | Mute |                           | Play |   4  |   5  |   6  |   +  |        |
* |--------+------+------+------+------+------|                           |------+------+------+------+------+--------|
* | TRNS   | PgDn | Left | Down | Right| Vol- |                           | Prev |   1  |   2  |   3  |   *  | TRNS   |
* `--------+------+------+------+------+------'                           `------+------+------+------+------+--------'
*          |  App |  Ins | Home | End  |                                         | PgUp | PgDn |   .  |   /  |
*          `---------------------------'                                         `---------------------------'
*                                        ,-------------.         ,-------------.
*                                        | TRNS | TRNS |         | TRNS | =    |
*                                 ,------|------|------|         |------+------+------.
*                                 |      |      | TRNS |         | TRNS |      |      |
*                                 | TRNS | Ins  |------|         |------| KEnt |  K0  |
*                                 |      |      | Caps |         | NumL |      |      |
*                                 `--------------------'         `--------------------'
*
*/


static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default */
    KEYMAP(
           CAPS,F1  ,F2  ,F3  ,F4  ,F5  ,F6  ,F7  ,F8,
           EQL, 1   ,2   ,3   ,4   ,5   ,
           TAB, QUOT,COMM,DOT ,P   ,Y   ,
           ESC, A   ,O   ,E   ,U   ,I   ,
           LSFT,SCLN,Q   ,J   ,K   ,X   ,
                FN1 ,FN2 ,FN3 ,FN4 ,
                               SLSH,BSLS,
                                    FN5 ,
                          BSPC,DEL ,ESC ,
           F9  ,F10 ,F11 ,F12 ,PSCR,SLCK,PAUS,NO  ,NO  ,
           6   ,7   ,8   ,9   ,0   ,MINS,
           F   ,G   ,C   ,R   ,L   ,SLSH,
           D   ,H   ,T   ,N   ,S   ,BSLS,
           B   ,M   ,W   ,V   ,Z   ,RSFT,
                FN7 ,FN8 ,FN9 ,FN10,
           GRV ,NUBS,
           FN6 ,
           TAB ,ENT ,SPC
    ),
    /* Layer 1: Symbols */
    KEYMAP(
           CAPS,F1  ,F2  ,F3  ,F4  ,F5  ,F6  ,F7  ,F8,
           PAUS,F1  ,F2  ,F3  ,F4  ,F5  ,
           TRNS,F11 ,F12 ,F13 ,F14 ,VOLU,
           CAPS,PGUP,HOME,UP  ,END ,MUTE,
           TRNS,PGDN,LEFT,DOWN,RGHT,VOLD,
                LCTL,HOME,END ,TRNS,
                          TRNS,TRNS,
                               TRNS,
                     CAPS,INS ,CAPS,
           F9  ,F10 ,F11 ,F12 ,PSCR,SLCK,PAUS,NO  ,NO  ,
           F6  ,F7  ,F8  ,F9  ,F10 ,NLCK,
           MNXT,P7  ,P8  ,P9  ,PMNS,NO  ,
           MPLY,P4  ,P5  ,P6  ,PPLS,NO  ,
           MPRV,P1  ,P2  ,P3  ,PAST,TRNS,
                PGUP,PGDN,PDOT,PSLS,
           TRNS,PEQL,
           TRNS,
           NLCK,PENT,P0  
    ),};
enum function_id {
    BOOTLOAD_KEY,
};
static const uint16_t PROGMEM fn_actions[] = {
		[0] = ACTION_FUNCTION(BOOTLOAD_KEY),
		[1] = ACTION_MODS_TAP_KEY(MOD_LSFT, KC_MINS),
		[2] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_EQL),
		[3] = ACTION_MODS_TAP_KEY(MOD_LALT, KC_LEFT),
		[4] = ACTION_MODS_TAP_KEY(MOD_LGUI, KC_RGHT),
		[5] = ACTION_LAYER_TAP_TOGGLE(1),
		[6] = ACTION_LAYER_TAP_TOGGLE(1),
		[7] = ACTION_MODS_TAP_KEY(MOD_RGUI, KC_UP),
		[8] = ACTION_MODS_TAP_KEY(MOD_RALT, KC_DOWN),
		[9] = ACTION_MODS_TAP_KEY(MOD_RCTL, KC_LBRC),
		[10] = ACTION_MODS_TAP_KEY(MOD_RSFT, KC_RBRC)
};
void action_function(keyrecord_t *event, uint8_t id, uint8_t opt) {
    if (id == BOOTLOAD_KEY) {
        clear_keyboard();
        _delay_ms(250);
        bootloader_jump();
    }
}