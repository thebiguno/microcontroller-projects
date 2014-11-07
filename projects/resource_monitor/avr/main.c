#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/Hd44780/Hd44780_Direct.h"
#include "lib/pwm/pwm.h"
#include "lib/usb/rawhid/usb_rawhid.h"

using namespace digitalcave;

#define BRIGHTNESS_PORT					PORTB
#define BRIGHTNESS_PIN					PORTB0
#define CONTRAST_PORT					PORTC
#define CONTRAST_PIN					PORTC2

#define PWM_PHASE						0xFF
#define PWM_CONTRAST_PIN				0
#define PWM_BRIGHTNESS_PIN				1
#define EEPROM_CONTRAST_ADDRESS			((uint8_t*) 0x00)
#define EEPROM_BRIGHTNESS_ADDRESS		((uint8_t*) 0x01)
#define TIMEOUT_MILLIS					2000

#define STATE_SPLASH					0
#define STATE_DISPLAY					1
#define STATE_SLEEP						2
//The state machine goes as follows: After USB enumeration, start in splash screen.
//When you receive a message from the client software (in any state), go to display and show the message.
//When at display, if you do not receive a message for TIMEOUT seconds, go back to splash screen
//When at splash screen, if you do not receive a message for TIMEOUT seconds, turn display off.

static uint8_t rx_buffer[64];
static volatile uint16_t timeout_millis = 0;
static volatile uint8_t state;
static Hd44780_Direct display(display.FUNCTION_LINE_2 | display.FUNCTION_SIZE_5x8);

void initUsb(){
	// Initialize the USB, and then wait for the host to set configuration.
	// If the AVR is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured());

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);
}

void initDisplay(){
	uint8_t custom[64] = {
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,			// 0 (Use 0xFE)
		0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1f,			// 1
		0x0,0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,			// 2
		0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,			// 3
		0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,		// 4
		0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,		// 5
		0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,		// 6
		0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f		// 7
													// 8 (Use 0xFF)
	};
	display.setCgramAddress(0x00);
	_delay_ms(64);
	display.setBytes(custom, 64);
}

void initTimer(){
	TCCR0A = 0x0;
	TCCR0B |= _BV(CS02);
	OCR0A = F_CPU / 256 / 1000;		//Fire every millisecond
	TIMSK0 = _BV(OCIE0A);
	sei();
}

void initPwm(){
	volatile uint8_t *ports[2];
	uint8_t pins[2];
	ports[PWM_CONTRAST_PIN] = &CONTRAST_PORT;
	ports[PWM_BRIGHTNESS_PIN] = &BRIGHTNESS_PORT;
	pins[PWM_CONTRAST_PIN] = CONTRAST_PIN;
	pins[PWM_BRIGHTNESS_PIN] = BRIGHTNESS_PIN;
	pwm_init(ports, pins, 2, PWM_PHASE);
	pwm_set_phase(PWM_CONTRAST_PIN, eeprom_read_byte(EEPROM_CONTRAST_ADDRESS));
	pwm_set_phase(PWM_BRIGHTNESS_PIN, eeprom_read_byte(EEPROM_BRIGHTNESS_ADDRESS));
}

void doSplash(){
	state = STATE_SPLASH;
	timeout_millis = 0;
	
	display.setDdramAddress(0x00);
	display.setText((char*) "  Wyatt  Olson  ", 16);
	display.setDdramAddress(0x40);
	display.setText((char*) " digitalcave.ca ", 16);
}

void doDisplay(){
	pwm_start();
	
	state = STATE_DISPLAY;
	timeout_millis = 0;
	
	display.setDdramAddress(0x00);
	display.setText((char*) &rx_buffer[1], 16);
	display.setDdramAddress(0x40);
	display.setText((char*) &rx_buffer[17], 16);
}

void doSleep(){
	pwm_stop();
	
	display.setDdramAddress(0x00);
	display.setText((char*) "                ", 16);
	display.setDdramAddress(0x40);
	display.setText((char*) "                ", 16);
}

int main (void){
	initUsb();
	initDisplay();
	initTimer();
	initPwm();
	
	doSplash();
	
	while (1) {
		// if received data, do something with it
		uint8_t count = usb_rawhid_recv(rx_buffer, 0);
		if (count >= 33 && rx_buffer[0] == 0x01) {
			doDisplay();
		}
		else if (count >= 2 && rx_buffer[0] == 0x02){
			if (state == STATE_SLEEP) doSplash();
			pwm_set_phase(0, rx_buffer[1]);
			eeprom_update_byte(EEPROM_CONTRAST_ADDRESS, rx_buffer[1]);
		}
		else if (count >= 2 && rx_buffer[0] == 0x03){
			if (state == STATE_SLEEP) doSplash();
			pwm_set_phase(1, rx_buffer[1]);
			eeprom_update_byte(EEPROM_BRIGHTNESS_ADDRESS, rx_buffer[1]);
		}
		
		if (state != STATE_SLEEP && timeout_millis > TIMEOUT_MILLIS){
			if (state == STATE_DISPLAY){
				doSplash();
			}
			else if (state == STATE_SPLASH){
				doSleep();
			}
		}
	}
}

EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)
ISR(TIMER0_COMPA_vect){
	timeout_millis++;
}
