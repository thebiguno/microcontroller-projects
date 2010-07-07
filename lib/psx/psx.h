#ifndef PSX_lib_h
#define PSX_lib_h


#include <avr/io.h>
#include <util/delay.h>

#define CTRL_CLK        20
#define CTRL_BYTE_DELAY 20

//These are our button constants
#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_GREEN       0x1000
#define PSB_RED         0x2000
#define PSB_BLUE        0x4000
#define PSB_PINK        0x8000

//These are stick values
#define PSS_RX 5
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

/* Initialize the controller, specifying which port, output, and pins to use.
 * Currently we force you to use the same port for all pins (i.e., PORTB / DDRB),
 * although if you want to change that it is easy enough.  Although we do not check
 * this, the output port DDRX should be the same as PORTX.
 */
void psx_init(volatile uint8_t *psx_port, volatile uint8_t *psx_ddr, 
				uint8_t clk_pin, 	
				uint8_t cmd_pin, 
				uint8_t att_pin, 
				uint8_t dat_pin);

/* Returns non-zero if the given button is pressed. */
uint8_t psx_button(uint16_t);

/* Returns the value of the given analog stick (0-255) */
uint8_t psx_stick(unsigned int);

/* Updates the current value of the game pad.  This should be called 
 * in the main loop, as psx_button() and psx_stick depend on values
 * set here.
 */
void psx_read_gamepad();

/* Private functions for internal use */
uint8_t _psx_gamepad_shiftinout (uint8_t byte);
void _psx_gamepad_shiftout (uint8_t byte);
uint8_t _psx_gamepad_shiftin();
uint8_t _psx_get_gamepad_mode();
uint8_t get_gamepad_mode();

#endif
