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
* |  =+  |  -_  | Left | Right|      |                                   |      | Up   | Down |  [{  |  ]}  |
* | Ctrl | Alt  | Cmd  | Shift|  Fn  |                                   |  Fn  | Shift| Cmd  | Alt  | Ctrl |
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
* | Caps |      |      |      | Mute |                                   |  >|| |   4  |   5  |   6  |   +  |
* |------+------+------+------+------|                                   |------+------+------+------+------|
* |      |      |      |      | Vol- |                                   |  <<  |   1  |   2  |   3  |   -  |
* |------+------+------+------+------'                                   |------+------+------+------+------|
* | TRNS | TRNS | Home | End  | TRNS |                                   | TRNS | PgUp | PgDn | TRNS | TRNS |
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |      |      |       |   *  |   /  |
*                             ,------|------|------|       |------+------+------.
*                             | Del  | TRNS | TRNS |       | TRNS | KEnt |   0  |
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
           LCTL,LALT,LGUI,TAB ,NO  ,          
                     NO  ,DEL ,
                     FN1 ,LSFT,BSPC ,
           6   ,7   ,8   ,9   ,0   ,
           Y   ,U   ,I   ,O   ,P   ,
           H   ,J   ,K   ,L   ,SCLN,
           N   ,M   ,COMM,DOT ,SLSH,
           NO  ,ENT ,RGUI,RALT,LCTL,
           ESC ,NO ,
           SPC ,RSFT,FN1 
    ),
    /* Layer 1: Symbols */
    KEYMAP(
           F1  ,F2  ,F3  ,F4  ,F5  ,
           GRV ,HOME,UP  ,PGUP,VOLU,
           FN8 ,LEFT,DOWN,RGHT,MUTE,     
           F11 ,END ,F12 ,PGDN,VOLD,
           TRNS,TRNS,TRNS,TRNS,TRNS,         
                          TRNS,TRNS,
                     TRNS,TRNS,TRNS,
           F6  ,F7  ,F8  ,F9  ,F10 ,
           MNXT,LBRC,RBRC,FN2 ,FN3 ,
           MPLY,BSLS,SLSH,FN4 ,FN5 ,
           MPRV,EQL ,MINS,FN6 ,FN7 ,
           TRNS,TRNS,TRNS,TRNS,TRNS,
           TRNS,CAPS,
           TRNS,TRNS,TRNS
    ),};
enum function_id {
    TEENSY_KEY,
};
static const uint16_t PROGMEM fn_actions[] = {
		[0] = ACTION_FUNCTION(TEENSY_KEY),
		[1] = ACTION_LAYER_MOMENTARY(1),
		[2] = ACTION_MODS_KEY(MOD_LSFT, KC_LBRC),
		[3] = ACTION_MODS_KEY(MOD_LSFT, KC_RBRC),
		[4] = ACTION_MODS_KEY(MOD_LSFT, KC_BSLS),
		[5] = ACTION_MODS_KEY(MOD_LSFT, KC_SLSH),
		[6] = ACTION_MODS_KEY(MOD_LSFT, KC_EQL ),
		[7] = ACTION_MODS_KEY(MOD_LSFT, KC_MINS),
		[8] = ACTION_MODS_KEY(MOD_LSFT, KC_GRV )
};
void action_function(keyrecord_t *event, uint8_t id, uint8_t opt) {
    if (id == TEENSY_KEY) {
        clear_keyboard();
        _delay_ms(250);
        bootloader_jump();
    }
}
