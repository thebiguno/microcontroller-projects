other projects:
https://github.com/chrisandreae/keyboard-firmware


		4y0	4y1	4y2	4y3	4y4	4y5	4y6	4y7	5y0	5y1	5y2	5y3	5y4	5y5	5y6	5y7	
		r1	r2	r3	r4	r5	r6	r7	r8	r9	r10	r11	r12	r13	r14	r15	r16
		A	B	C	D	E	F	G	H	I	J	K	L	M	N	O	P	
21	c1	f6	f8	f7	5	4	3	2	1	=+								
22	c2	f3	f5	f4	t	r	e	w	q	TAB								
23	c3	ESC	f2	f1	g	f	d	s	a	CL								
24	c4	f9	f11	f10	b	v	c	x	z	LS	UP		DN		[{	]}		
25	c5  f12	SL	PS	RT		LT	§±	`~		6	7	8		9	0	-_ 	
26	c6	PB	*PG	*KP							y	u	i		o	p	\	
27	c7  			LC	DL	BS	RC	EN	SP	h	j	k		l	;:	'"	
28	c8					RA		PU		PD	n	m	,<		.>	/?	RS	

   DL2	1	40	VCC
   DR1	2	39	A
   DR2	3	38	B
   DL1	4	37	C
   KPD	5	36	G
   PGM	6	35
   SCL	7	34
   SDA	8	33
   RST	9	32	BUZZ
    WP	10	31	EA
   FS1	11	30	ALE
 CLOCK	12	29	PSEN
  DATA	13	28	r8
		14	27	r7
   FS3	15	26	r6
		16	25	r5
   FS2	17	24	r4
 XTAL1	18	23	r3
 XTAL2	19	22	r2
   GND	20	21	r1



J3 left fn
1	c3		esc,f1,f2				2	3	4
2	rA		esc,f3,f6			1	ESC	F1	F2
3	rC		f1,f4,f7			8	F3	F4	F5
4	rB		f2,f5,f8			5	F6	F7	F8
5	c1		f6,f7,f8
6	rC		f1,f4,f7
7	rB		f2,f5,f8
8	c2		f3,f4,f5
9	N/C
10	r3		f1,f4,f7
11	r2		f2,f5,f8
12	N/C
13	N/C

J4 right fn
1	PM								5	11	7
2	KY							6	F9	F10	F11
3	GND		PM, KY					F12	PS	SL
4	c6		PB					4	PB
5	rA		f9,f12,PB					2	1
6	c4		f9,f10,f11			3		KY	PM
7	rB		f11,SL
8	c6		PB
9	
10	N/C
11	rC		f10,PS
12	GND		PM, KY
13	N/C