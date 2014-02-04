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
* | Ctrl | Alt  | Cmd  | Tab  |      |                                   |      | Ent  | Cmd  | Alt  | Ctrl |
* `----------------------------------'                                   `----------------------------------'
*                                    ,-------------.       ,-------------.
*                                    |      | Del  |       | Esc  |      |
*                             ,------|------|------|       |------+------+------.
*                             | Fn   | Sft  | Bspc |       | Spc  | Sft  | Fn   |
*                             `--------------------'       `--------------------'
*
* Keymap: Fn Layer
*
* ,----------------------------------.                                    ,----------------------------------.
* |  F1  |  F2  |  F3  |  F4  |  F5  |                                    |  F6  |  F7  |  F8  |  F9  |  F10 |
* |------+------+------+------+------|                                    |------+------+------+------+------
* |  ~   | Home | Up   | PgUp | Vol+ |                                    |  >>  |   [  |   ]  |   {  |   }  |
* |------+------+------+------+------|                                    |------+------+------+------+------|
* |  `   | Left | Down | Rght | Mute |                                    |  >|| |   \  |   '  |   |  |   "  |
* |------+------+------+------+------|                                    |------+------+------+------+------|
* |  F11 | End  | F12  | PgDn | Vol- |                                    |  <<  |   =  |   -  |   +  |   _  |
* |------+------+------+------+------'                                    |------+------+------+------+------|
* | TRNS | TRNS | TRNS | TRNS |      |                                    |      | TRNS | TRNS | TRNS | TRNS |
* `----------------------------------'                                    `----------------------------------'
*                                     ,-------------.       ,-------------.
*                                     | TRNS | TRNS |       | TRNS | TRNS |
*                              ,------|------|------|       |------+------+------.
*                              | TRNS | TRNS | TRNS |       | TRNS | TRNS | TRNS |
*                              `--------------------'       `--------------------'
*
*/

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default */
    KEYMAP(
           1   ,2   ,3   ,4   ,5   ,
           QUOT,COMM,DOT ,P   ,Y   ,
           A   ,O   ,E   ,U   ,I   ,     
           SCLN,Q   ,J   ,K   ,X   ,
           LCTL,LALT,LGUI,TAB ,NO  ,          
                     NO  ,DEL ,
                     FN1 ,LSFT,BSPC ,
           6   ,7   ,8   ,9   ,0   ,
           F   ,G   ,C   ,R   ,L   ,
           D   ,H   ,T   ,N   ,S   ,
           B   ,M   ,W   ,V   ,Z   ,
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
           MPLY,BSLS,QUOT,FN4 ,FN5 ,
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
		[5] = ACTION_MODS_KEY(MOD_LSFT, KC_QUOT),
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
