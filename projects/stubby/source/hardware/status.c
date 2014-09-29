#include "status.h"

//The magic number 78 comes from X/20000 = color/255; X = color * 20000 / 255; X = color * 78.
#define PHASE_MULTIPLIER		(uint8_t) (20000/255)

void status_init(volatile uint8_t **ports, uint8_t *pins){
	ports[LED_RED] = &PORTD;		//Red LED
	pins[LED_RED] = PORTD4;
	ports[LED_GREEN] = &PORTD;		//Green LED
	pins[LED_GREEN] = PORTD6;
	ports[LED_BLUE] = &PORTD;		//Blue LED
	pins[LED_BLUE] = PORTD5;
}

void status_flash(uint8_t red, uint8_t green, uint8_t blue, uint8_t count){
	for (uint8_t i = 0; i < count; i++){
		wdt_reset();
		status_set_color(red, green, blue);
		delay_ms(100);
		status_set_color(0x00, 0x00, 0x00);
		delay_ms(100);
	}	
}

void status_set_color(uint8_t red, uint8_t green, uint8_t blue){
	pwm_set_phase_batch(LED_RED, ((uint16_t) red) * PHASE_MULTIPLIER);
	pwm_set_phase_batch(LED_GREEN, ((uint16_t) green) * PHASE_MULTIPLIER);
	pwm_set_phase_batch(LED_BLUE, ((uint16_t) blue) * PHASE_MULTIPLIER);
	
	pwm_apply_batch();
}

