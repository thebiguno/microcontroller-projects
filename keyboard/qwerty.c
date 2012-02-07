#include "qwerty.h"

/*
 * The mapping for scan codes to characters for QWERTY.
 */
PROGMEM uchar map_qwerty[128] = {
/*	 0	1	2	3	4	5 (J)	6 (J)	7 (J)	8	9	A	B	C	D	E	F */
/* 0 */	0x00,	PAD_PERIOD,	KEY_ARROWUP,	KEY_LALT,	0x00,	PAD_0,	0x00,	KEY_F6,	KEY_SPACE,	KEY_SLASH,	0x00,	KEY_N,	KEY_B,	0x00,	0x00,	0x00,
/* 1 */	0x00,	PAD_6,	PAD_PLUS,	0x00,	KEY_LSHFT,	PAD_5,	PAD_4,	KEY_RSQBRK,	KEY_F5,	KEY_QUOTE,	0x00,	KEY_H,	KEY_G,	KEY_F4,	0x00,	KEY_ESC,
/* 2 */	KEY_LCTL,	KEY_PGUP,	KEY_HOME,	0x00,	0x00,	KEY_INS,	KEY_DELFWD,	KEY_EQUAL,	KEY_DEL,	KEY_LSQBRK,	KEY_F7,	KEY_Y,	KEY_T,	KEY_F3,	KEY_CAPSLOCK,	KEY_TAB,
/* 3 */	0x00,	KEY_PGDN,	KEY_END,	KEY_PRTSCR,	0x00,	KEY_F12,	KEY_F11,	KEY_8,	KEY_F9,	KEY_MINUS,	KEY_F8,	KEY_6,	KEY_5,	KEY_F2,	KEY_F1,	KEY_TILDE,
/* 4 */	0x00,	PAD_9,	0x00,	KEY_SCRLCK,	0x00,	PAD_8,	PAD_7,	KEY_I,	KEY_F10,	KEY_0,	KEY_9,	KEY_7,	KEY_4,	KEY_3,	KEY_2,	KEY_1,
/* 5 */	0x00,	PAD_3,	PAD_ENTER,	0x00,	0x00,	PAD_2,	PAD_1,	KEY_K,	0x00,	KEY_P,	KEY_O,	KEY_U,	KEY_R,	KEY_E,	KEY_W,	KEY_Q,
/* 6 */	KEY_RCTL,	PAD_STAR,	KEY_PAUSE,	0x00,	KEY_RSHFT,	PAD_SLASH,	KEY_NUMLCK,	KEY_COMMA,	KEY_BKSLASH,	KEY_SEMICOLON,	KEY_L,	KEY_J,	KEY_F,	KEY_D,	KEY_S,	KEY_A,
/* 7 */	0x00,	PAD_MINUS,	KEY_ARROWLEFT,	KEY_RALT,	0x00,	KEY_ARROWRIGHT,	KEY_ARROWDOWN,	0x00,	KEY_RET,	0x00,	KEY_PERIOD,	KEY_M,	KEY_V,	KEY_C,	KEY_X,	KEY_Z
};

void lookup_qwerty(uint8_t row, uint8_t col, uchar *key, uchar *modifier_flags){
	uchar keyCode = pgm_read_byte(map_qwerty + (row * 16) + col);

//	uchar keyCode = map_qwerty[(row * 16) + col];
/*
	switch(row){
		case 0: *key = KEY_0; break;
		case 1: *key = KEY_1; break;
		case 2: *key = KEY_2; break;
		case 3: *key = KEY_3; break;
		case 4: *key = KEY_4; break;
		case 5: *key = KEY_5; break;
		case 6: *key = KEY_6; break;
		case 7: *key = KEY_7; break;
		case 8: *key = KEY_8; break;
		case 9: *key = KEY_9; break;
		case 0xA: *key = KEY_A; break;
		case 0xB: *key = KEY_B; break;
		case 0xC: *key = KEY_C; break;
		case 0xD: *key = KEY_D; break;
		case 0xE: *key = KEY_E; break;
		case 0xF: *key = KEY_F; break;
	}

	return;
*/	
	switch(keyCode){
		case KEY_LCTL:
			*modifier_flags |= MOD_CONTROL_LEFT;
			break;
		case KEY_LSHFT:
			*modifier_flags |= MOD_SHIFT_LEFT;
			break;
		case KEY_LALT:
			*modifier_flags |= MOD_ALT_LEFT;
			break;
		case KEY_LGUI:
			*modifier_flags |= MOD_GUI_LEFT;
			break;
		case KEY_RCTL:
			*modifier_flags |= MOD_CONTROL_RIGHT;
			break;
		case KEY_RSHFT:
			*modifier_flags |= MOD_SHIFT_RIGHT;
			break;
		case KEY_RALT:
			*modifier_flags |= MOD_ALT_RIGHT;
			break;
		case KEY_RGUI:
			*modifier_flags |= MOD_GUI_RIGHT;
			break;
		default:
			*key = keyCode;
			break;
	}
}
