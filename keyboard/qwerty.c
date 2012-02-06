/*
 * The mapping for scan codes to characters for QWERTY.
 */
//uint8_t map[128] = {
///*	 	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F	*/
///* 0 */		NP.	UP	LALT		NP0		F6	F5	'		H	G	F4		ESC
///* 1 */		NP6	NP+		LSFT	NP5	NP4	]	BS	[	F7	Y	T	F3	CAP	TAB
///* 2 */	LCTL	PU	HOME			INS	DEL	=	F9	-	F8	6	5	F2	F1	~
///* 3 */		PD	END	PRT		F12	F11	8	F10	0	9	7	4	3	2	1
///* 4 */		NP9		SL		NP8	NP7	I		P	O	U	R	E	W	Q
///* 5 */		NP3	NPEN			NP2	NP1	K	\	;	L	J	F	D	S	A
///* 6 */	RCTL	NP*	BRK		RSFT	NP/	NL	,	EN		.	M	V	C	X	Z
///* 7 */		NP-	LEFT	LALT		RGHT	DWN		SP	/		N	B
//}

uint8_t map[128] = {
/*	 0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F	*/
/* 0 */		NP.	UP	LALT		NP0		F6	F5	'		H	G	F4		ESC
/* 1 */		NP6	NP+		LSFT	NP5	NP4	]	BS	[	F7	Y	T	F3	CAP	TAB
/* 2 */	LCTL	PU	HOME			INS	DEL	=	F9	-	F8	6	5	F2	F1	~
/* 3 */		PD	END	PRT		F12	F11	8	F10	0	9	7	4	3	2	1
/* 4 */		NP9		SL		NP8	NP7	I		P	O	U	R	E	W	Q
/* 5 */		NP3	NPEN			NP2	NP1	K	\	;	L	J	F	D	S	A
/* 6 */	RCTL	NP*	BRK		RSFT	NP/	NL	,	EN		.	M	V	C	X	Z
/* 7 */		NP-	LEFT	LALT		RGHT	DWN		SP	/		N	B
}

void send_key(uint8_t row, uint8_t column){

}