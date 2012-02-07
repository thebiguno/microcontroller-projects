#include "qwerty.h"

/*
 * The mapping for scan codes to characters for QWERTY.
 */
//uint8_t map[128] = {
///*		 0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F */
///* 0 */		NP.	UP	LALT		NP0		F6	F5	'		H	G	F4		ESC
///* 1 */		NP6	NP+		LSFT	NP5	NP4	]	BS	[	F7	Y	T	F3	CAP	TAB
///* 2 */	LCTL	PU	HOME			INS	DEL	=	F9	-	F8	6	5	F2	F1	~
///* 3 */		PD	END	PRT		F12	F11	8	F10	0	9	7	4	3	2	1
///* 4 */		NP9		SL		NP8	NP7	I		P	O	U	R	E	W	Q
///* 5 */		NP3	NPEN			NP2	NP1	K	\	;	L	J	F	D	S	A
///* 6 */	RCTL	NP*	BRK		RSFT	NP/	NL	,	EN		.	M	V	C	X	Z
///* 7 */		NP-	LEFT	LALT		RGHT	DWN		SP	/		N	B
//}

PROGMEM uchar map_qwerty[128] = {
/*	 0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F */
/* 0 */	0x00,	PAD_PERIOD,	KEY_ARROWUP,	KEY_LALT,	0x00,	PAD_0,	0x00,	KEY_F6,	KEY_F5,	KEY_QUOTE,	0x00,	KEY_H,	KEY_G,	KEY_F4,	0x00,	KEY_ESC,
/* 1 */	0x00,	PAD_6,	PAD_PLUS,	0x00,	KEY_LSHFT,	PAD_5,	PAD_4,	KEY_RSQBRK,	KEY_DEL,	KEY_LSQBRK,	KEY_F7,	KEY_Y,	KEY_T,	KEY_F3,	KEY_CAPSLOCK,	KEY_TAB,
/* 2 */	KEY_LCTL,	KEY_PGUP,	KEY_HOME,	0x00,	0x00,	KEY_INS,	KEY_DELFWD,	KEY_EQUAL,	KEY_F9,	KEY_MINUS,	KEY_F8,	KEY_6,	KEY_5,	KEY_F2,	KEY_F1,	KEY_TILDE,
/* 3 */	0x00,	KEY_PGDN,	KEY_END,	KEY_PRTSCR,	0x00,	KEY_F12,	KEY_F11,	KEY_8,	KEY_F10,	KEY_0,	KEY_9,	KEY_7,	KEY_4,	KEY_3,	KEY_2,	KEY_1,
/* 4 */	0x00,	PAD_9,	0x00,	KEY_SCRLCK,	0x00,	PAD_8,	PAD_7,	KEY_I,	0x00,	KEY_P,	KEY_O,	KEY_U,	KEY_R,	KEY_E,	KEY_W,	KEY_Q,
/* 5 */	0x00,	PAD_3,	PAD_ENTER,	0x00,	0x00,	PAD_2,	PAD_1,	KEY_K,	KEY_BKSLASH,	KEY_SEMICOLON,	KEY_L,	KEY_J,	KEY_F,	KEY_D,	KEY_S,	KEY_A,
/* 6 */	KEY_RCTL,	PAD_STAR,	KEY_PAUSE,	0x00,	KEY_RSHFT,	PAD_SLASH,	KEY_NUMLCK,	KEY_COMMA,	KEY_RET,	0x00,	KEY_PERIOD,	KEY_M,	KEY_V,	KEY_C,	KEY_X,	KEY_Z,
/* 7 */	0x00,	PAD_MINUS,	KEY_ARROWLEFT,	KEY_LALT,	0x00,	KEY_ARROWRIGHT,	KEY_ARROWDOWN,	0x00,	KEY_SPACE,	KEY_SLASH,	0x00,	KEY_N,	KEY_B,	0x00,	0x00,	0x00
};

//TODO There are two LALT keys; figure out which is which!
void lookup_qwerty(uint8_t row, uint8_t col, uchar *key, uchar *modifier_flags){
	uchar keyCode = map_qwerty[row * 16 + col];

	switch(keyCode){
		case KEY_LCTL:
			*modifier_flags |= MOD_CONTROL_LEFT;
			break;
		default:
			*key = keyCode;
			break;
	}
}
