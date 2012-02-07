#ifndef HID_H
#define HID_H

// report descriptor constants
#define REPORT_SIZE     0x75
#define REPORT_COUNT    0x95

#define USAGE_PAGE      0x05
#define USAGE           0x09
#define USAGE_MINIMUM   0x19
#define USAGE_MAXIMUM   0x29

#define LOGICAL_MINIMUM	0x15
#define LOGICAL_MAXIMUM	0x25

#define COLLECTION      0xa1
#define END_COLLECTION	0xc0

#define INPUT           0x81
#define OUTPUT          0x91

#define REPORT_ID		0x85

#define FLAG_DATA       0x00
#define FLAG_CNST	    0x01
#define FLAG_ARR        0x00
#define FLAG_VAR        0x02
#define FLAG_ABS        0x00
#define FLAG_REL        0x04

// hid keyboard constants
#define MOD_CONTROL_LEFT    0x01
#define MOD_SHIFT_LEFT      0x02
#define MOD_ALT_LEFT        0x04
#define MOD_GUI_LEFT        0x08
#define MOD_CONTROL_RIGHT   0x10
#define MOD_SHIFT_RIGHT     0x20
#define MOD_ALT_RIGHT       0x40
#define MOD_GUI_RIGHT       0x80

#define KEY_A       0x04
#define KEY_B       0x05
#define KEY_C       0x06
#define KEY_D       0x07
#define KEY_E       0x08
#define KEY_F       0x09
#define KEY_G       0x0A
#define KEY_H       0x0B
#define KEY_I       0x0C
#define KEY_J       0x0D
#define KEY_K       0x0E
#define KEY_L       0x0F
#define KEY_M       0x10
#define KEY_N       0x11
#define KEY_O       0x12
#define KEY_P       0x13
#define KEY_Q       0x14
#define KEY_R       0x15
#define KEY_S       0x16
#define KEY_T       0x17
#define KEY_U       0x18
#define KEY_V       0x19
#define KEY_W       0x1A
#define KEY_X       0x1B
#define KEY_Y       0x1C
#define KEY_Z       0x1D

#define KEY_1       0x1E
#define KEY_2       0x1F
#define KEY_3       0x20
#define KEY_4       0x21
#define KEY_5       0x22
#define KEY_6       0x23
#define KEY_7       0x24
#define KEY_8       0x25
#define KEY_9       0x26
#define KEY_0       0x27

#define KEY_RET		0x28
#define KEY_ESC		0x29
#define KEY_DEL		0x2A
#define KEY_TAB		0x2B
#define KEY_SPACE	0x2C
#define KEY_MINUS	0x2D
#define KEY_EQUAL	0x2E
#define KEY_LSQBRK	0x2F
#define KEY_RSQBRK	0x30
#define KEY_BKSLASH	0x31
//#define KEY_TILDE	//Non-US #, not included
#define KEY_SEMICOLON	0x33
#define KEY_QUOTE	0x34
#define KEY_TILDE	0x35
#define KEY_COMMA	0x36
#define KEY_PERIOD	0x37
#define KEY_SLASH	0x38
#define KEY_CAPSLOCK	0x39

#define KEY_F1      0x3A
#define KEY_F2      0x3B
#define KEY_F3      0x3C
#define KEY_F4      0x3D
#define KEY_F5      0x3E
#define KEY_F6      0x3F
#define KEY_F7      0x40
#define KEY_F8      0x41
#define KEY_F9      0x42
#define KEY_F10     0x43
#define KEY_F11     0x44
#define KEY_F12     0x45

#define KEY_PRTSCR	0x46
#define KEY_SCRLCK	0x47
#define KEY_PAUSE	0x48
#define KEY_INS		0x49
#define KEY_HOME	0x4A
#define KEY_PGUP	0x4B
#define KEY_DELFWD	0x4C
#define KEY_END		0x4D
#define KEY_PGDN	0x4E
#define KEY_ARROWRIGHT	0x4F
#define KEY_ARROWLEFT	0x50
#define KEY_ARROWDOWN	0x51
#define KEY_ARROWUP	0x52
#define KEY_NUMLCK	0x53

#define PAD_SLASH	0x54
#define PAD_STAR	0x55
#define PAD_MINUS	0x56
#define PAD_PLUS	0x57
#define PAD_ENTER	0x58
#define PAD_1		0x59
#define PAD_2		0x5A
#define PAD_3		0x5B
#define PAD_4		0x5C
#define PAD_5		0x5D
#define PAD_6		0x5E
#define PAD_7		0x5F
#define PAD_8		0x60
#define PAD_9		0x61
#define PAD_0		0x62
#define PAD_PERIOD	0x63
//#define PAD_ //Non-US BackSlash
#define KEY_APP		0x65
#define KEY_POWER	0x66
#define PAD_EQUAL	0x67

//Don't send these, just use them for lookups
#define KEY_LCTL	0xE0
#define KEY_LSHFT	0xE1
#define KEY_LALT	0xE2
//Win, Apple, Meta
#define KEY_LGUI	0xE3
#define KEY_RCTL	0xE4
#define KEY_RSHFT	0xE5
#define KEY_RALT	0xE6
#define KEY_RGUI	0xE7

#define KEY_MUTE	0x7F
#define KEY_VOLUP	0x80
#define KEY_VOLDOWN	0x81

/* The LED states */
#define LED_NUM     0x01
#define LED_CAPS    0x02
#define LED_SCROLL  0x04
#define LED_COMPOSE 0x08
#define LED_KANA    0x10

#endif
