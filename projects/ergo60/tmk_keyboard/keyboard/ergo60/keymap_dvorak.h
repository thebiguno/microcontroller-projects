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
* |  ;: *|   Q  |   J  |   K  |   X  |                                   |   B  |   M  |   W  |   V  |   Z *|
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |  -_  |  =+  | Left | Right|      |                                   |      | Up   | Down |  [{  |  ]}  |
* | Shift| Ctrl | Alt  | Gui  |  Fn  |                                   |  Fn  | Gui  | Alt  | Ctrl | Shift|
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |  /?  |  \|  |       |  `~  |  §±  |
*                             ,------|------|------|       |------+------+------.
*                             | Bspc | Del  | Esc  |       | Tab  | Ent  | Space|
*                             `--------------------'       `--------------------'
*
* Keymap: Fn Layer
*
* ,----------------------------------.                                   ,----------------------------------.
* |  F1  |  F2  |  F3  |  F4  |  F5  |                                   |  F6  |  F7  |  F8  |  F9  |  F10 |
* |------+------+------+------+------|                                   |------+------+------+------+------
* |  F11 |  F12 |  F13 |  F14 | Vol+ |                                   |  >>  |   7  |   8  |   9  |   -  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* | PgUp | Home |  Up  | End  | Mute |                                   |  >|| |   4  |   5  |   6  |   +  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* | PgDn | Left | Down | Right| Vol- |                                   |  <<  |   1  |   2  |   3  |   *  |
* |------+------+------+------+------'                                   |------+------+------+------+------|
* | Shift| Ctrl | Home | End  | TRNS |                                   | TRNS | PgUp | PgDn |   .  |   /  |
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |      |Teensy|       |      |   =  |
*                             ,------|------|------|       |------+------+------.
*                             | Caps | Ins  | Caps |       |NL/Clr| KEnt |   0  |
*                             `--------------------'       `--------------------'
*
*/

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default */
    KEYMAP(
           1   ,2   ,3   ,4   ,5   ,
           QUOT,COMM,DOT ,P   ,Y   ,
           A   ,O   ,E   ,U   ,I   ,
           FN11,Q   ,J   ,K   ,X   ,
           FN1 ,FN2 ,FN3 ,FN4 ,FN5  ,
                          SLSH,BSLS,
                     BSPC,DEL ,ESC ,
           6   ,7   ,8   ,9   ,0   ,
           F   ,G   ,C   ,R   ,L   ,
           D   ,H   ,T   ,N   ,S   ,
           B   ,M   ,W   ,V   ,FN12,
           FN6 ,FN7 ,FN8 ,FN9 ,FN10,
           GRV ,NUBS,
           TAB ,ENT ,SPC
    ),
    /* Layer 1: Symbols */
    KEYMAP(
           F1  ,F2  ,F3  ,F4  ,F5  ,
           F11 ,F12 ,F13 ,F14 ,VOLU,
           PGUP,HOME,UP  ,END ,MUTE,
           PGDN,LEFT,DOWN,RGHT,VOLD,
           LSFT,LCTL,HOME,END ,TRNS,
                          NO  ,FN0 ,
                     CAPS,INS ,CAPS,
           F6  ,F7  ,F8  ,F9  ,F10 ,
           MNXT,P7  ,P8  ,P9  ,PMNS,
           MPLY,P4  ,P5  ,P6  ,PPLS,
           MPRV,P1  ,P2  ,P3  ,PAST,
           TRNS,PGUP,PGDN,PDOT,PSLS,
           NO  ,PEQL,
           NLCK,PENT,P0  
    ),};
enum function_id {
    TEENSY_KEY,
};
static const uint16_t PROGMEM fn_actions[] = {
		[0] = ACTION_FUNCTION(TEENSY_KEY),
		[1] = ACTION_MODS_TAP_KEY(MOD_LSFT, KC_MINS),
		[2] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_EQL),
		[3] = ACTION_MODS_TAP_KEY(MOD_LALT, KC_LEFT),
		[4] = ACTION_MODS_TAP_KEY(MOD_LGUI, KC_RGHT),
		[5] = ACTION_LAYER_TAP_TOGGLE(1),
		[6] = ACTION_LAYER_TAP_TOGGLE(1),
		[7] = ACTION_MODS_TAP_KEY(MOD_RGUI, KC_UP),
		[8] = ACTION_MODS_TAP_KEY(MOD_RALT, KC_DOWN),
		[9] = ACTION_MODS_TAP_KEY(MOD_RCTL, KC_LBRC),
		[10] = ACTION_MODS_TAP_KEY(MOD_RSFT, KC_RBRC),
		[11] = ACTION_MODS_TAP_KEY(MOD_LSFT, KC_SCLN),
		[12] = ACTION_MODS_TAP_KEY(MOD_RSFT, KC_Z)
};
void action_function(keyrecord_t *event, uint8_t id, uint8_t opt) {
    if (id == TEENSY_KEY) {
        clear_keyboard();
        _delay_ms(250);
        bootloader_jump();
    }
}