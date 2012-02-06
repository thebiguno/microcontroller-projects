/*
 * The mapping for scan codes to characters for QWERTY.
 */
//uint8_t map[128] = {
///*	 0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F */
///* 0 */		NP.	UP	LALT		NP0		F6	F5	'		H	G	F4		ESC
///* 1 */		NP6	NP+		LSFT	NP5	NP4	]	BS	[	F7	Y	T	F3	CAP	TAB
///* 2 */	LCTL	PU	HOME			INS	DEL	=	F9	-	F8	6	5	F2	F1	~
///* 3 */		PD	END	PRT		F12	F11	8	F10	0	9	7	4	3	2	1
///* 4 */		NP9		SL		NP8	NP7	I		P	O	U	R	E	W	Q
///* 5 */		NP3	NPEN			NP2	NP1	K	\	;	L	J	F	D	S	A
///* 6 */	RCTL	NP*	BRK		RSFT	NP/	NL	,	EN		.	M	V	C	X	Z
///* 7 */		NP-	LEFT	LALT		RGHT	DWN		SP	/		N	B
//}

PROGMEM uint8_t map[128] = {
/*	 0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F */
/* 0 */		PAD_PERIOD,	KEY_ARROWUP,	LALT		PAD_0		KEY_F6	KEY_F5	KEY_QUOTE		KEY_H	KEY_G	KEY_F4		KEY_ESC
/* 1 */		PAD_6,	PAD_PLUS,		LSFT	PAD_5	PAD_4	]	KEY_DELETE	[	F7	KEY_Y	KEY_T	F3	KEY_CAP	KEY_TAB
/* 2 */	LCTL	PU,	HOME,			INS	DEL	=	F9	-	F8	KEY_6	KEY_5	F2	F1	KEY_TILDE
/* 3 */		PD,	END,	PRT		F12	F11	8	F10	KEY_0	KEY_9	KEY_7	KEY_4	KEY_3	KEY_2	KEY_1
/* 4 */		PAD_9,	0x00,	SL		PAD_8	PAD_7	I		KEY_P	KEY_O	KEY_U	KEY_R	KEY_E	KEY_W	KEY_Q
/* 5 */		PAD_3,	PAD_ENTER,			PAD_2	PAD_1	K	\	;	KEY_L	KEY_J	KEY_F	KEY_D	KEY_S	KEY_A
/* 6 */	RCTL	PAD_STAR,	BRK,		RSFT	PAD_SLASH	KEY_NUMLCK	KEY_COMMA,	EN		.	KEY_M	KEY_V	KEY_C	KEY_X	KEY_Z
/* 7 */		PAD_MINUS,	KEY_ARROWLEFT,	LALT		KEY_ARROWRIGHT	KEY_ARROWDOWN		SP	/		KEY_N	KEY_B
}

void lookup(uint8_t row, uint8_t col, uchar *key, uchar *modifier_flags){
	
}
