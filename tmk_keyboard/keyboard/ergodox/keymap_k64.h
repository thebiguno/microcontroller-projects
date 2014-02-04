static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer 0: Standard */
    KEYMAP(
           ESC ,1   ,2   ,3   ,4   ,5   ,NO  ,
           LCTL,SLSH,COMM,DOT ,P   ,Y   ,CAPS,
           LGUI,A   ,O   ,E   ,U   ,I   ,     
           LSFT,SCLN,Q   ,J   ,K   ,X   ,TAB ,
           NO  ,NO  ,NO  ,LEFT,RGHT,          
                                    MPRV,MNXT,
                                         MPLY,
                               BSPC,LALT,NO  ,
           NO  ,6   ,7   ,8   ,9   ,0   ,ENT ,
           FN2 ,F   ,G   ,C   ,R   ,L   ,RCTL,
                D   ,H   ,T   ,N   ,S   ,RGUI,
           ENT ,B   ,M   ,W   ,V   ,Z   ,RSFT,
                     UP  ,DOWN,NO  ,NO  ,NO  ,
           VOLD,VOLU,
           MUTE,
           NO  ,FN1 ,SPC 
    ),
    /* Layer 1: Symbol */
    KEYMAP(
           TRNS,F1  ,F2  ,F3  ,F4  ,F5  ,NO  ,
           TRNS,FN3 ,GRV ,FN4 ,FN5 ,BSLS,APP ,
           TRNS,FN9 ,LBRC,FN10,QUOT,MINS,     
           TRNS,FN15,FN16,FN17,EQL ,FN18,FN23,
           NO  ,NO  ,NO  ,HOME,END ,          
                                    TRNS,TRNS,
                                         TRNS,
                               DEL ,LALT,NO  ,
           NO  ,F6  ,F7  ,F8  ,F9  ,F10 ,TRNS,
           FN22,FN6 ,FN7 ,FN8 ,F11 ,F12 ,TRNS,
                FN11,FN12,FN13,RBRC,FN14,TRNS,
           FN24,FN19,FN20,FN21,SLSH,FN25,TRNS,
                     PGUP,PGDN,NO  ,NO  ,NO  ,
           TRNS,TRNS,
           TRNS,
           NO  ,TRNS,SPC 
    ),
    /* Layer 2: Keypad */
    KEYMAP(
           TRNS,NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
           TRNS,NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
           TRNS,NO  ,NO  ,NO  ,NO  ,NO  ,     
           TRNS,NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
           NO  ,NO  ,NO  ,NO  ,NO  ,          
                                    NO  ,NO  ,
                                         NO  ,
                               BSPC,NO  ,NO  ,
           NO  ,INS ,NLCK,PEQL,PSLS,PAST,ESC ,
           FN2 ,NO  ,P7  ,P8  ,P9  ,PMNS,TAB ,
                NO  ,P4  ,P5  ,P6  ,PPLS,BSPC,
           SPC ,COMM,P1  ,P2  ,P3  ,PENT,DEL ,
                     P0  ,P0  ,PDOT,PENT,NO  ,
           NO  ,NO  ,
           NO  ,
           NO  ,NO  ,PENT
    ),
    /* Layer 3: Mouse */
    KEYMAP(
           TRNS,NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
           TRNS,NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
           TRNS,NO  ,ACL0,ACL1,ACL2,BTN4,     
           TRNS,NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,
           NO  ,NO  ,NO  ,NO  ,NO  ,          
                                    NO  ,NO  ,
                                         NO  ,
                               BTN2,BTN3,NO  ,
           NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,TRNS,
           FN22,NO  ,WH_L,WH_D,WH_U,WH_R,TRNS,
                BTN5,MS_L,MS_D,MS_U,MS_R,TRNS,
           NO  ,NO  ,NO  ,NO  ,NO  ,NO  ,TRNS,
                     NO  ,NO  ,NO  ,NO  ,NO  ,
           NO  ,NO  ,
           NO  ,
           NO  ,NO  ,BTN1
    ),};
enum function_id {
    TEENSY_KEY,
};
static const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_FUNCTION(TEENSY_KEY),
    [1] = ACTION_LAYER_MOMENTARY(1),
    [2] = ACTION_LAYER_TOGGLE(2),
    [3] = ACTION_MODS_KEY(MOD_RSFT, KC_GRV ),
    [4] = ACTION_MODS_KEY(MOD_RSFT, KC_4   ),
    [5] = ACTION_MODS_KEY(MOD_RSFT, KC_2   ),
    [6] = ACTION_MODS_KEY(MOD_RSFT, KC_6   ),
    [7] = ACTION_MODS_KEY(MOD_RSFT, KC_5   ),
    [8] = ACTION_MODS_KEY(MOD_RSFT, KC_8   ),
    [9] = ACTION_MODS_KEY(MOD_RSFT, KC_LBRC),
    [10] = ACTION_MODS_KEY(MOD_RSFT, KC_9   ),
    [11] = ACTION_MODS_KEY(MOD_RSFT, KC_MINS),
    [12] = ACTION_MODS_KEY(MOD_RSFT, KC_QUOT),
    [13] = ACTION_MODS_KEY(MOD_RSFT, KC_0   ),
    [14] = ACTION_MODS_KEY(MOD_RSFT, KC_RBRC),
    [15] = ACTION_MODS_KEY(MOD_RSFT, KC_1   ),
    [16] = ACTION_MODS_KEY(MOD_RSFT, KC_3   ),
    [17] = ACTION_MODS_KEY(MOD_RSFT, KC_COMM),
    [18] = ACTION_MODS_KEY(MOD_RSFT, KC_BSLS),
    [19] = ACTION_MODS_KEY(MOD_RSFT, KC_7   ),
    [20] = ACTION_MODS_KEY(MOD_RSFT, KC_EQL ),
    [21] = ACTION_MODS_KEY(MOD_RSFT, KC_DOT ),
    [22] = ACTION_LAYER_TOGGLE(3),
    [23] = ACTION_MODS_KEY(MOD_LGUI, KC_TAB ),
    [24] = ACTION_MODS_KEY(MOD_LGUI, KC_GRV ),
    [25] = ACTION_MODS_KEY(MOD_LSFT, KC_SLSH),
};
void action_function(keyrecord_t *event, uint8_t id, uint8_t opt) {
    if (id == TEENSY_KEY) {
        clear_keyboard();
        _delay_ms(250);
        bootloader_jump();
    }
}
