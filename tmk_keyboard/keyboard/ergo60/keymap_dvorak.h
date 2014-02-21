/****************************************************************************************************
*
* Keymap: Default Layer in Dvorak
*
* ,----------------------------------.                                   ,----------------------------------.
* |  1!  |  2@  |  3#  |  4$  |  5%  |                                   |  6^  |  7&  |  8*  |  9(  |  0)  |
* |------+------+------+------+------|                                   |------+------+------+------+------+
* |  '"  |  ,<  |  .>  |   P  |   Y  |                                   |   F  |   G  |   C  |   R  |   L  |
* |------+------+------+------+------|                                   |------+------+------+------+------+
* |   A  |   O  |   E  |   U  |   I  |                                   |   D  |   H  |   T  |   N  |   S  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |  ;:  |   Q  |   J  |   K  |   X  |                                   |   B  |   M  |   W  |   V  |   Z  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |  =+  |  -_  | Left | Right|      |                                   |      | Up   | Down |  [{  |  ]}  |
* | Ctrl | Alt  | Cmd  | Shift|      |                                   |      | Shift| Cmd  | Alt  | Ctrl |
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |  \|  | /?   |       | '~   | §±   |
*                             ,------|------|------|       |------+------+------.
*                             | Bspc | Del  | Esc  |       | Tab  | Ent  | Space|
*                             | Shift| Fn   | Cmd  |       | Cmd  | Fn   | Shift|
*                             `--------------------'       `--------------------'
*
* Keymap: Fn Layer
*
* ,----------------------------------.                                   ,----------------------------------.
* |  F1  |  F2  |  F3  |  F4  |  F5  |                                   |  F6  |  F7  |  F8  |  F9  |  F10 |
* |------+------+------+------+------|                                   |------+------+------+------+------
* |  F11 |  F12 |  F13 |  F14 | Vol+ |                                   |  >>  |      |      |      |      |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* | Caps |      |      |      | Mute |                                   |  >|| |      |      |      |      |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |      |      | Home | End  | Vol- |                                   |  <<  | PgUp | PgDn |      |      |
* |------+------+------+------+------'                                   |------+------+------+------+------|
* | TRNS | TRNS | TRNS | TRNS |      |                                   |      | TRNS | TRNS | TRNS | TRNS |
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |      |      |       |      |      |
*                             ,------|------|------|       |------+------+------.
*                             | TRNS | TRNS | TRNS |       | TRNS | TRNS | TRNS |
*                             `--------------------'       `--------------------'
*
*/

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default */
    KEYMAP(
           1   ,2   ,3   ,4   ,5   ,
           QUOT,COMM,DOT ,P   ,Y   ,
           A   ,O   ,E   ,U   ,I   ,     
           SCLN,Q   ,J   ,K   ,X   ,
           FN4 ,FN5 ,FN6 ,FN7 ,NO  ,          
                     NO  ,NO ,
                     FN1 ,FN2 ,FN3 ,
           6   ,7   ,8   ,9   ,0   ,
           F   ,G   ,C   ,R   ,L   ,
           D   ,H   ,T   ,N   ,S   ,
           B   ,M   ,W   ,V   ,Z   ,
           NO  ,FN8 ,FN9 ,FN10,FN11,
           NO  ,NO ,
           FN12,FN13,FN14 
    ),
    /* Layer 1: Symbols */
    KEYMAP(
           F1  ,F2  ,F3  ,F4  ,F5  ,
           F11 ,F12 ,F13 ,F14 ,VOLU,
           NO  ,NO  ,NO  ,NO  ,MUTE,     
           NO  ,NO  ,HOME,END ,VOLD,
           TRNS,TRNS,TRNS,TRNS,TRNS,         
                          TRNS,TRNS,
                     TRNS,TRNS,TRNS,
           F6  ,F7  ,F8  ,F9  ,F10 ,
           MNXT,FN15,FN16,FN17,FN18,
	       MPLY,BSLS,EQL ,MINS,NUBS,
           MPRV,PGUP,PGDN,NO  ,NO  ,
           TRNS,TRNS,TRNS,TRNS,TRNS,
           TRNS,TRNS,
           TRNS,TRNS,TRNS
    ),};
enum function_id {
    TEENSY_KEY,
};
static const uint16_t PROGMEM fn_actions[] = {
		[0] = ACTION_FUNCTION(TEENSY_KEY),
		[1] = ACTION_MODS_TAP_KEY(MOD_LSFT, KC_BSPC),
		[2] = ACTION_LAYER_TAP_KEY(1, KC_DEL),
		[3] = ACTION_MODS_TAP_KEY(MOD_LGUI, KC_ESC),
		[4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_SLSH),
		[5] = ACTION_MODS_TAP_KEY(MOD_LALT, KC_NUBS),
		[6] = ACTION_MODS_TAP_KEY(MOD_LGUI, KC_LEFT),
		[7] = ACTION_MODS_TAP_KEY(MOD_LSFT, KC_RGHT),
		[8] = ACTION_MODS_TAP_KEY(MOD_RSFT, KC_UP),
		[9] = ACTION_MODS_TAP_KEY(MOD_RGUI, KC_DOWN),
		[10] = ACTION_MODS_TAP_KEY(MOD_RALT, KC_LBRC),
		[11] = ACTION_MODS_TAP_KEY(MOD_RCTL, KC_RBRC),
		[12] = ACTION_MODS_TAP_KEY(MOD_RGUI, KC_TAB),
		[13] = ACTION_LAYER_TAP_KEY(1, KC_ENT),
		[14] = ACTION_MODS_TAP_KEY(MOD_RSFT, KC_SPC),
		[15] = ACTION_MODS_KEY(MOD_LSFT, KC_BSLS),
		[16] = ACTION_MODS_KEY(MOD_LSFT, KC_EQL),
		[17] = ACTION_MODS_KEY(MOD_LSFT, KC_MINS),
		[18] = ACTION_MODS_KEY(MOD_LSFT, KC_NUBS)
};
void action_function(keyrecord_t *event, uint8_t id, uint8_t opt) {
    if (id == TEENSY_KEY) {
        clear_keyboard();
        _delay_ms(250);
        bootloader_jump();
    }
}
