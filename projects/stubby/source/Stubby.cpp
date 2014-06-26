#include "Stubby.h"

using namespace digitalcave;

//Set up the leg objects, including servo details and mounting angle.  Leg indices follows
// DIP numbering format, starting at front left and proceeding counter clockwise around.
Leg legs[LEG_COUNT] = {
	Leg(FRONT_LEFT,		&PORTB, PORTB0, &PORTB, PORTB1, &PORTB, PORTB2, 2 * LEG_MOUNTING_ANGLE, Point(-65, 112, 0)),
	Leg(MIDDLE_LEFT,	&PORTB, PORTB3, &PORTA, PORTA3, &PORTB, PORTB4, 3 * LEG_MOUNTING_ANGLE, Point(-130, 0, 0)),
	Leg(REAR_LEFT,		&PORTD, PORTD2, &PORTD, PORTD3, &PORTC, PORTC4, 4 * LEG_MOUNTING_ANGLE, Point(-65, -112, 0)),
	Leg(REAR_RIGHT,		&PORTC, PORTC3, &PORTC, PORTC2, &PORTD, PORTD7, 5 * LEG_MOUNTING_ANGLE, Point(65, -112, 0)),
	Leg(MIDDLE_RIGHT,	&PORTC, PORTC7, &PORTC, PORTC6, &PORTC, PORTC5, 0 * LEG_MOUNTING_ANGLE, Point(130, 0, 0)),
	Leg(FRONT_RIGHT,	&PORTA, PORTA4, &PORTA, PORTA5, &PORTA, PORTA6, 1 * LEG_MOUNTING_ANGLE, Point(65, 112, 0))
};

int main (void){
	
	servo_init();
	
	comm_init();
	
	//TODO Wait for start button before initializing servos.  If Select is pressed
	// before start, we enter calibration mode.
	uint16_t buttons = 0x00;
	while ((buttons & _BV(CONTROLLER_BUTTON_VALUE_START)) == 0){
		buttons = comm_read_buttons();
		
		if (buttons & _BV(CONTROLLER_BUTTON_VALUE_SELECT)){
			calibration_init();
			buttons = 0x00;
		}
	}
	
	
	pwm_start();
	
	status_set_color(0x00, 0xFF, 0x00);
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].resetPosition();
	}
	pwm_apply_batch();
	
	//status_init();
	
	int8_t step_index = 0;
	while(1){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			legs[l].setOffset(gait_step(l, step_index));
		}
		step_index++;
		if (step_index > gait_step_count()){
			step_index = 0;
		}
		pwm_apply_batch();
		
		_delay_ms(20);
	}
}

void servo_init(){
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

	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].resetPosition();
	}
	pwm_apply_batch();

	_delay_ms(500);
	
	pwm_stop();	
}

/*
 * Calibration mode.  Use the following buttons to calibrate:
 *  -Circle switches between legs.  As a new leg is selected, flash the leg color on the status LED.
 *  -Left / Right adjusts coxa joint for selected leg
 *  -Up / Down adjusts femur joint for selected leg
 *  -L1 / L2 adjusts tibia joint for selected leg
 *  -Triangle resets all values back to 0
 *  -X commits changes to EEPROM
 *  -Start exits calibration mode (regardless of whether or not it is committed)
 */
void calibration_init(){
	pwm_start();
	
	uint8_t l = 0;
	status_set_color(0xFF, 0x00, 0x00);
	uint16_t buttons = 0x0000;
	//Loop until Start is pressed
	while ((buttons & _BV(CONTROLLER_BUTTON_VALUE_START)) == 0){
		if (buttons != 0x0000){
			//Circle increments legs.
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_CIRCLE)) != 0) {
				l++;
				if (l >= LEG_COUNT) l = 0;
				
				if (l == 0) status_set_color(0xFF, 0x00, 0x00);
				else if (l == 1) status_set_color(0xFF, 0xFF, 0x00);
				else if (l == 2) status_set_color(0x00, 0xFF, 0x00);
				else if (l == 3) status_set_color(0xFF, 0xFF, 0xFF);
				else if (l == 4) status_set_color(0x00, 0x00, 0xFF);
				else if (l == 5) status_set_color(0xFF, 0x00, 0xFF);
			}
			
			//Left / Right arrows adjust coxa joint
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADRIGHT)) != 0) {
				legs[l].setOffset(COXA, legs[l].getOffset(COXA) + 1);
			}
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADLEFT)) != 0) {
				legs[l].setOffset(COXA, legs[l].getOffset(COXA) - 1);
			}
			
			//Up / Down arrows adjust femur joint
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADUP)) != 0) {
				legs[l].setOffset(FEMUR, legs[l].getOffset(FEMUR) + 1);
			}
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_PADDOWN)) != 0) {
				legs[l].setOffset(FEMUR, legs[l].getOffset(FEMUR) - 1);
			}
			
			//L1 / L2 buttons adjust the tibia joint
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT1)) != 0) {
				legs[l].setOffset(TIBIA, legs[l].getOffset(TIBIA) + 1);
			}
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)) != 0) {
				legs[l].setOffset(TIBIA, legs[l].getOffset(TIBIA) - 1);
			}
			
			//Triangle button to revert all calibration to 0
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
				for (uint8_t i = 0; i < LEG_COUNT; i++){
					for (uint8_t j = 0; j < JOINT_COUNT; j++){
						legs[i].setOffset(j, 0);
					}
				}
			}

			for (uint8_t i = 0; i < LEG_COUNT; i++){
				legs[i].resetPosition();
			}
			pwm_apply_batch();
			
			//X button to commit changes to EEPROM
			if ((buttons & _BV(CONTROLLER_BUTTON_VALUE_CROSS)) != 0) {
				//servo_save_calibration();
				//Flash green to indicate save
				status_set_color(0x00, 0xFF, 0x00);			//Green
				_delay_ms(500);
				status_set_color(0xFF, 0xFF, 0x00);			//Yellow
			}
		}
		buttons = comm_read_buttons();
	}
	
	//Reload calibration from EEPROM; if it was committed, this will essentially
	// be a NOP, otherwise we revert any changes done in this session.
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			legs[l].setOffset(j, eeprom_read_byte((uint8_t*) (l * JOINT_COUNT) + j));
		}
	}
	
	pwm_stop();
}
