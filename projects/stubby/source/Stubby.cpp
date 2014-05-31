#include "Stubby.h"

Leg legs[LEG_COUNT];

int main (void){
	//Set up the leg objects, including servo details and mounting angle
	legs[FRONT_LEFT] = new Leg(&PORTB, PORTB2, &PORTB, PORTB1, &PORTB, PORTB0, 2 * M_PI / 3);
	legs[FRONT_RIGHT] = new Leg(&PORTA, PORTA4, &PORTA, PORTA5, &PORTA, PORTA6, M_PI / 3);
	legs[MIDDLE_LEFT] = new Leg(&PORTB, PORTB3, &PORTA, PORTA3, &PORTB, PORTB4, 3 * M_PI / 3);
	legs[MIDDLE_RIGHT] = new Leg(&PORTC, PORTC7, &PORTC, PORTC6, &PORTC, PORTC5, 0);
	legs[REAR_LEFT] = new Leg(&PORTD, PORTD2, &PORTD, PORTD3, &PORTC, PORTC4, 4 * M_PI / 3);
	legs[REAR_RIGHT] = new Leg(&PORTC, PORTC3, &PORTC, PORTC2, &PORTD, PORTD7, 5 * M_PI / 3);

	//Set up the servos using the leg details + status LEDs
	volatile uint8_t *ports[PWM_COUNT];
	uint8_t pins[PWM_COUNT];

	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			ports[(l * JOINT_COUNT) + j] = legs[l].getPort(j);
			pins[(l * JOINT_COUNT) + j] = legs[l].getPin(j);
			
			legs[l].setOffset(j, eeprom_read_byte((uint8_t*) (l * JOINT_COUNT) + j));
		}
	}
	
	ports[LED_RED] = &PORTD;		//Red LED
	pins[LED_RED] = PORTD4;
	ports[LED_GREEN] = &PORTD;		//Green LED
	pins[LED_GREEN] = PORTD6;
	ports[LED_BLUE] = &PORTD;		//Blue LED
	pins[LED_BLUE] = PORTD5;
	
	pwm_init(ports, pins, PWM_COUNT, 20000);
	
	comm_init();
	status_init();
	
	calibration_main();
	
	while(1){

	}
}
