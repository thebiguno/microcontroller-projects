/****************************************************************************************************
*
* Keymap: Default Layer in Dvorak
*
* ,----------------------------------.                                   ,----------------------------------.
* |  1!  |  2@  |  3#  |  4$  |  5%  |                                   |  6^  |  7&  |  8*  |  9(  |  0)  |
* |------+------+------+------+------|                                   |------+------+------+------+------+
* |   Q  |   W  |   E  |   R  |   T  |                                   |   Y  |   U  |   I  |   O  |   P  |
* |------+------+------+------+------|                                   |------+------+------+------+------+
* |   A  |   S  |   D  |   F  |   G  |                                   |   H  |   J  |   K  |   L  |  ;:  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |   Z  |   X  |   C  |   V  |   B  |                                   |   N  |   M  |  .<  |  ,>  |  /?  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* | Shift| Ctrl | Alt  | Gui  |  Fn  |                                   |  Fn  | Gui  | Alt  | Ctrl | Shift|
* |  =+  |  -_  | Left | Right|      |                                   |      | Up   | Down |  [{  |  ]}  |
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |      | \|   |       | `~   | §±   |
*                             ,------|------|------|       |------+------+------.
*                             | Bspc | Esc  | '"   |       | Tab  | Ent  | Space|
*                             `--------------------'       `--------------------'
*
* Keymap: Fn Layer
*
* ,----------------------------------.                                   ,----------------------------------.
* |  F1  |  F2  |  F3  |  F4  |  F5  |                                   |  F6  |  F7  |  F8  |  F9  |  F10 |
* |------+------+------+------+------|                                   |------+------+------+------+------
* |  F11 |  F12 |  F13 |  F14 | Vol+ |                                   |  >>  |   7  |   8  |   9  |   =  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |      |      |      |      | Mute |                                   |  >|| |   4  |   5  |   6  |   +  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |      |      |      |      | Vol- |                                   |  <<  |   1  |   2  |   3  |   -  |
* |------+------+------+------+------'                                   |------+------+------+------+------|
* | TRNS | TRNS | Home | End  | TRNS |                                   | TRNS | PgUp | PgDn |   .  | Clr  |
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |      |      |       |   *  |   /  |
*                             ,------|------|------|       |------+------+------.
*                             | Del  | Caps | TRNS |       | TRNS | KEnt |   0  |
*                             `--------------------'       `--------------------'
*
*/

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default */
    KEYMAP(
           1   ,2   ,3   ,4   ,5   ,
           Q   ,W   ,E   ,R   ,T   ,
           A   ,S   ,D   ,F   ,G   ,
           Z   ,X   ,C   ,V   ,B   ,
           FN1 ,FN2 ,FN3 ,FN4 ,FN5  ,
                          CAPS,BSLS,
                     BSPC,ESC ,SLSH ,
           6   ,7   ,8   ,9   ,0   ,
           Y   ,U   ,I   ,O   ,P   ,
           H   ,J   ,K   ,L   ,SCLN,
           N   ,M   ,COMM,DOT ,SLSH,
           FN6 ,FN7 ,FN8 ,FN9,FN10,
           GRV ,NUBS,
           TAB ,ENT ,SPC
    ),
    /* Layer 1: Symbols */
    KEYMAP(
           F1  ,F2  ,F3  ,F4  ,F5  ,
           F11 ,F12 ,F13 ,F14 ,VOLU,
           NO  ,NO  ,NO  ,NO  ,MUTE,
           NO  ,NO  ,NO  ,NO  ,VOLD,
           TRNS,TRNS,HOME,END ,TRNS,
                          NO  ,NO  ,
                     DEL ,CAPS,NO  ,
           F6  ,F7  ,F8  ,F9  ,F10 ,
           MNXT,P7  ,P8  ,P9  ,PEQL,
           MPLY,P4  ,P5  ,P6  ,PPLS,
           MPRV,P1  ,P2  ,P3  ,PMNS,
           TRNS,PGUP,PGDN,PDOT,NLCK,
           PAST,PSLS,
           NO  ,PENT,P0  
    ),};
enum function_id {
    TEENSY_KEY,
};
static const uint16_t PROGMEM fn_actions[] = {
		[0] = ACTION_FUNCTION(TEENSY_KEY),
		[1] = ACTION_MODS_TAP_KEY(MOD_LSFT, KC_EQL),
		[2] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_MINS),
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
    if (id == TEENSY_KEY) {
        clear_keyboard();
        _delay_ms(250);
        bootloader_jump();
    }
}
