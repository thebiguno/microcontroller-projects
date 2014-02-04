/****************************************************************************************************
*
* NOTE: For Windows and Linux Ctrl and Meta need to be swapped.
*
* Keymap: Default Layer in Dvorak
*
* ,--------------------------------------------------.           ,--------------------------------------------------.
* | =+     |  1!  |  2@  |  3#  |  4$  |  5%  |      |           |      |  6^  |  7&  |  8*  |  9(  |  0)  | -_     |
* |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
* | Tab    |  '"  |  ,<  |  .>  |   P  |   Y  |      |           |      |   J  |   F  |   U  |   P  |   $  | /?     |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* | Esc    |   A  |   O  |   E  |   U  |   I  |------|           |------|   Y  |   N  |   E  |   O  |   I  | \|     |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* | Shift  |  ;:  |   Q  |   J  |   K  |   X  |      |           |      |   B  |   M  |   W  |   V  |   Z  | Shift  |
* `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
*   |      |  `~  |  §±  |  Lft |  Rgt |                                       |  Up  | Down |  [{  |  ]}  |      |
*   `----------------------------------'                                       `----------------------------------'
*                                        ,-------------.       ,-------------.
*                                        | Fn   | Alt  |       | Alt  | Fn   |
*                                 ,------|------|------|       |------+------+------.
*                                 |      |      | Ctrl |       | Ctrl |      |      |
*                                 | BkSp | Del  |------|       |------|Return| Space|
*                                 |      |      | Meta |       | Meta |      |      |
*                                 `--------------------'       `--------------------'
*
* Keymap: Fn/Keypad Layer
*
* ,--------------------------------------------------.           ,--------------------------------------------------.
* | Pause  |  F1  |  F2  |  F3  |  F4  | Pscr |      |           |      | Slck | Nlck |   =  |   /  |   *  | BkSp   |
* |--------+------+------+------+------+-------------|           |------+------+------+------+------+------+--------|
* | TRNS   |  F5  |  F6  |  F7  |  F8  | Vol+ |      |           |      | Next |   7  |   8  |   9  |   -  | Tab    |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* | Caps   |  F9  |  F10 |  F11 |  F12 | Mute |------|           |------| Play |   4  |   5  |   6  |   +  | TRNS   |
* |--------+------+------+------+------+------|      |           |      |------+------+------+------+------+--------|
* | TRNS   |  F13 |  F14 |  F15 |  F16 | Vol- |      |           |      | Prev |   1  |   2  |   3  | KEnt | TRNS   |
* `--------+------+------+------+------+-------------'           `-------------+------+------+------+------+--------'
*   |      |  App |  Ins | Home | End  |                                       | PgUp | PgDn |   .  | KEnt |      |
*   `----------------------------------'                                       `----------------------------------'
*                                        ,-------------.       ,-------------.
*                                        | TRNS | TRNS |       | TRNS | TRNS |
*                                 ,------|------|------|       |------+------+------.
*                                 |      |      | TRNS |       | TRNS |      |      |
*                                 | TRNS | Ins  |------|       |------| KEnt |  K0  |
*                                 |      |      | TRNS |       | TRNS |      |      |
*                                 `--------------------'       `--------------------'
*
*/

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Default */
    KEYMAP(
           EQL ,1   ,2   ,3   ,4   ,5   ,NO  ,
           TAB ,QUOT,COMM,DOT ,P   ,Y   ,NO  ,
           ESC ,A   ,O   ,E   ,U   ,I   ,     
           LSFT,SCLN,Q   ,J   ,K   ,X   ,LSFT,
           NO  ,GRV ,NUBS,LEFT,RGHT,          
                                    FN1 ,LALT,
                                         LCTL,
                               BSPC,DEL ,LGUI,
           NO  ,6   ,7   ,8   ,9   ,0   ,MINS,
           NO  ,F   ,G   ,C   ,R   ,L   ,SLSH,
                D   ,H   ,T   ,N   ,S   ,BSLS,
           RSFT,B   ,M   ,W   ,V   ,Z   ,RSFT,
                     UP  ,DOWN,LBRC,RBRC,NO  ,
           RALT,FN1 ,
           RCTL,
           RGUI,ENT ,SPC 
    ),
    /* Layer 1: Keypad */
    KEYMAP(
           PAUS,F1  ,F2  ,F3  ,F4  ,PSCR ,NO  ,
           TRNS,F5  ,F6  ,F7  ,F8  ,VOLU,NO  ,
           CAPS,F9  ,F10 ,F11 ,F12 ,MUTE,     
           TRNS,F13 ,F14 ,F15 ,F16 ,VOLD,TRNS,
           NO  ,APP ,INS ,HOME,END ,          
                                    TRNS,TRNS,
                                         TRNS,
                               TRNS,INS ,TRNS,
           NO  ,SLCK,NLCK,PEQL,PSLS,PAST,BSPC,
           NO  ,MNXT,P7  ,P8  ,P9  ,PMNS,TAB ,
                MPLY,P4  ,P5  ,P6  ,PPLS,TRNS,
           TRNS,MPRV,P1  ,P2  ,P3  ,PENT,TRNS,
                     PGUP,PGDN,PDOT,PENT,NO  ,
           TRNS,TRNS,
           TRNS,
           TRNS,PENT,P0
    ),};
enum function_id {
    TEENSY_KEY,
};
static const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_FUNCTION(TEENSY_KEY),
    [1] = ACTION_LAYER_TAP_TOGGLE(1)
};
void action_function(keyrecord_t *event, uint8_t id, uint8_t opt) {
    if (id == TEENSY_KEY) {
        clear_keyboard();
        _delay_ms(250);
        bootloader_jump();
    }
}
