/****************************************************************************************************
*
* Keymap: Default Layer in Dvorak
*
* ,--------------------------------------------------.           ,--------------------------------------------------.
* |        |  1!  |  2@  |  3#  |  4$  |  5%  |      |           |      |  6^  |  7&  |  8*  |  9(  |  0)  |        |
* |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
* |        |  '"  |  ,<  |  .>  |   P  |   Y  |      |           |      |   F  |   G  |   C  |   R  |   L  |        |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* |        |   A  |   O  |   E  |   U  |   I  |------|           |------|   D  |   H  |   T  |   N  |   S  |        |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* |        |  ;:  |   Q  |   J  |   K  |   X  |      |           |      |   B  |   M  |   W  |   V  |   Z  |        |
* `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
*   |      | Ctrl | Alt  | Cmd  | Fn   |                                       | Fn   | Cmd  | Alt  | Ctrl |      |
*   `----------------------------------'                                       `----------------------------------'
*                                        ,-------------.       ,-------------.
*                                        |      | Tab  |       | Esc  |      |
*                                 ,------|------|------|       |------+------+------.
*                                 |      |      |      |       |      |      |      |
*                                 | Shift| Bksp |------|       |------| Space| Shift|
*                                 |      |      | Del  |       | Enter|      |      |
*                                 `--------------------'       `--------------------'
*
* Keymap: Fn Layer
*
* ,--------------------------------------------------.           ,--------------------------------------------------.
* |        |  F1  |  F2  |  F3  |  F4  |  F5  |      |           |      |  F6  |  F7  |  F8  |  F9  |  F10 |        |
* |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
* |        |  ~   | Home | Up   | PgUp | Vol+ |      |           |      |  >>  |   [  |   ]  |   {  |   }  |        |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* |        |  `   | Left | Down | Rght | Mute |------|           |------|  >|| |   \  |   /  |   |  |   ?  |        |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* |        |  F11 | End  | F12  | PgDn | Vol- |      |           |      |  <<  |   =  |   -  |   +  |   _  |        |
* `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
*   |      | TRNS | TRNS | TRNS | TRNS |                                       | TRNS | TRNS | TRNS | TRNS |      |
*   `----------------------------------'                                       `----------------------------------'
*                                        ,-------------.       ,-------------.
*                                        | TRNS | TRNS |       | TRNS | TRNS |
*                                 ,------|------|------|       |------+------+------.
*                                 |      |      | TRNS |       | TRNS |      |      |
*                                 | TRNS | TRNS |------|       |------| TRNS | TRNS |
*                                 |      |      | TRNS |       | TRNS |      |      |
*                                 `--------------------'       `--------------------'
*
*/

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default */
    KEYMAP(
           NO  ,1   ,2   ,3   ,4   ,5   ,NO  ,
           NO  ,QUOT,COMM,DOT ,P   ,Y   ,NO  ,
           NO  ,A   ,O   ,E   ,U   ,I   ,     
           NO  ,SCLN,Q   ,J   ,K   ,X   ,NO ,
           NO  ,LCTL,LALT,LGUI,FN1 ,          
                                    TAB ,NO  ,
                                         NO  ,
                               LSFT,BSPC,DEL ,
           NO  ,6   ,7   ,8   ,9   ,0   ,NO  ,
           NO  ,F   ,G   ,C   ,R   ,L   ,NO  ,
                D   ,H   ,T   ,N   ,S   ,NO  ,
           NO  ,B   ,M   ,W   ,V   ,Z   ,NO  ,
                     FN1, RGUI,RALT,LCTL,NO  ,
           NO  ,ESC ,
           NO  ,
           ENT ,SPC ,RSFT
    ),
    /* Layer 1: Keypad */
    KEYMAP(
           NO  ,F1  ,F2  ,F3  ,F4  ,F5  ,NO  ,
           NO  ,GRV ,HOME,UP  ,PGUP,VOLU,NO  ,
           NO  ,FN8 ,LEFT,DOWN,RGHT,MUTE,     
           NO  ,F11 ,END ,F12 ,PGDN,VOLD,NO  ,
           NO  ,TRNS ,TRNS,TRNS,TRNS,          
                                    TRNS,TRNS,
                                         TRNS,
                               TRNS,TRNS,TRNS,
           NO  ,F6  ,F7  ,F8  ,F9  ,F10 ,NO  ,
           NO  ,MNXT,LBRC,RBRC,FN2 ,FN3 ,NO  ,
                MPLY,BSLS,SLSH,FN4 ,FN5 ,NO  ,
           TRNS,MPRV,EQL ,MINS,FN6 ,FN7 ,NO  ,
                     TRNS,TRNS,TRNS,TRNS,NO  ,
           TRNS,CAPS,
           TRNS,
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
