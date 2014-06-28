#include "Stubby.h"

#ifndef DEBUG_SIMULATION
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "status.h"
#include "comm.h"
#include "lib/serial/serial.h"
#include "lib/pwm/pwm.h"
#endif

#include "gait.h"
#include "trig.h"
#include "Leg.h"


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
	
	mode_select();
}

void mode_select(){
	//Hit start to begin controlling the robot, or hit select to calibrate joints
	while (1){
		uint16_t buttons = comm_read_pressed_buttons();
	
		if (buttons & _BV(CONTROLLER_BUTTON_VALUE_SELECT)){
			mode_calibration();
		}
		
		if (buttons & _BV(CONTROLLER_BUTTON_VALUE_START)){
			mode_remote_control();
		}
		
		_delay_ms(20);
	}
}

void mode_remote_control(){
	pwm_start();
	status_set_color(0x00, 0xFF, 0x00);
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].resetPosition();
	}
	pwm_apply_batch();
	
	//status_init();
	
	int8_t step_index = 0;

	//Hit Start to exit remote control mode, and go back to startup
	while(1){
		Point left_stick = comm_read_left();
		Point right_stick = comm_read_right();
		uint16_t pressed = comm_read_pressed_buttons();
		uint16_t held = comm_read_held_buttons();

		//Hit start to exit remote control mode
		if (pressed & _BV(CONTROLLER_BUTTON_VALUE_START)){
			break;
		}

		Point elevation_offset(0,0,0);
		if (held & _BV(CONTROLLER_BUTTON_VALUE_RIGHT1)) {
			elevation_offset.add(Point(0,0,-20));
		} 
		else if (held & _BV(CONTROLLER_BUTTON_VALUE_RIGHT2)){
			elevation_offset.add(Point(0,0,20));
		}
		
		if (held & _BV(CONTROLLER_BUTTON_VALUE_CIRCLE)){
			//We use the angle of the left stick to determine which way to lean
			//double left_angle = atan2(left_stick.y, left_stick.x);

			//Use pythagorean theorem to find the velocity, in the range [0..1].
			//double left_amount = fmin(1.0, fmax(0.0, sqrt((left_stick.x * left_stick.x) + (left_stick.y * left_stick.y)) / 15.0));
			
			for (uint8_t l = 0; l < LEG_COUNT; l++){

				
			}
		}
		else {
			double linear_angle = atan2(left_stick.y, left_stick.x);

			//Use pythagorean theorem to find the velocity, in the range [0..1].
			double linear_velocity = fmin(1.0, fmax(0.0, sqrt((left_stick.x * left_stick.x) + (left_stick.y * left_stick.y)) / 15.0));
			
			//We only care about the X axis for right (rotational) stick
			double rotational_velocity = right_stick.x / 15.0;
		
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				Point step = gait_step(legs[l], step_index, linear_velocity, linear_angle, rotational_velocity);
				step.add(elevation_offset);
				legs[l].setOffset(step);
			}
			step_index++;
			if (step_index > gait_step_count()){
				step_index = 0;
			}
		}
		pwm_apply_batch();
		
		_delay_ms(5);
	}
	
	//Reset legs to neutral position
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].resetPosition();
	}
	pwm_apply_batch();
	
	status_set_color(0x00, 0x00, 0x00);
	_delay_ms(200);

	pwm_stop();
}

/*
 * Calibration mode.  Use the following buttons to calibrate:
 *  -Square switches between legs.  As a new leg is selected, flash the leg color on the status LED.
 *  -Left / Right adjusts coxa joint for selected leg
 *  -Up / Down adjusts femur joint for selected leg
 *  -L1 / L2 adjusts tibia joint for selected leg
 *  -Circle + Triangle resets all values back to 0 (flash red)
 *  -Triangle by itself resets all values back to last saved (flash yellow)
 *  -Cross by itself saves changes to EEPROM (flash green)
 *  -Start exits calibration mode, discarding unsaved changes
 */
void mode_calibration(){
	pwm_start();

	status_flash(0x00, 0x00, 0xFF, 5);
	uint8_t l = 0;
	uint16_t held_buttons = 0x00;
	uint16_t pressed_buttons = 0x00;
	
	//Loop until Start is pressed
	while (1){
		held_buttons = comm_read_held_buttons();
		pressed_buttons = comm_read_pressed_buttons();
		
		_delay_ms(20);
		
		if (held_buttons != 0x00 || pressed_buttons != 0x00){
			//Start exits calibration mode (whether or not changes were made / applied / saved).
			if (pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_START)){
				break;
			}

			//Blink lights out every button press
			status_set_color(0x00, 0x00, 0x00);
			_delay_ms(60);
		
			//Square increments legs.
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_SQUARE)) != 0) {
				l++;
				if (l >= LEG_COUNT) l = 0;
			}
			
			//Left / Right arrows adjust coxa joint
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADRIGHT)) != 0) {
				legs[l].setCalibration(COXA, legs[l].getCalibration(COXA) + 1);
			}
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADLEFT)) != 0) {
				legs[l].setCalibration(COXA, legs[l].getCalibration(COXA) - 1);
			}
			
			//Up / Down arrows adjust femur joint
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADUP)) != 0) {
				legs[l].setCalibration(FEMUR, legs[l].getCalibration(FEMUR) + 1);
			}
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADDOWN)) != 0) {
				legs[l].setCalibration(FEMUR, legs[l].getCalibration(FEMUR) - 1);
			}
			
			//L1 / L2 adjust the tibia joint
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT1)) != 0) {
				legs[l].setCalibration(TIBIA, legs[l].getCalibration(TIBIA) + 1);
			}
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)) != 0) {
				legs[l].setCalibration(TIBIA, legs[l].getCalibration(TIBIA) - 1);
			}
			
			//Hold Circle + press Triangle to revert all calibration to 0
			if ((held_buttons & _BV(CONTROLLER_BUTTON_VALUE_CIRCLE)) != 0 && (pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
				for (uint8_t i = 0; i < LEG_COUNT; i++){
					for (uint8_t j = 0; j < JOINT_COUNT; j++){
						legs[i].setCalibration(j, 0);
					}
				}
				//Flash red 5 times to indicate revert
				status_flash(0xFF, 0x00, 0x00, 5);
			}
			
			//Triangle button to revert all calibration to last saved
			else if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
				for (uint8_t l = 0; l < LEG_COUNT; l++){
					for (uint8_t j = 0; j < JOINT_COUNT; j++){
						legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * JOINT_COUNT) + j));
					}
				}
				//Flash yellow 5 times to indicate revert
				status_flash(0xFF, 0xFF, 0x00, 5);
			}

			//X button to commit changes to EEPROM
			else if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_CROSS)) != 0) {
				for (uint8_t l = 0; l < LEG_COUNT; l++){
					for (uint8_t j = 0; j < JOINT_COUNT; j++){
						eeprom_update_byte((uint8_t*) (l * JOINT_COUNT) + j, legs[l].getCalibration(j));
					}
				}
				//Flash green 5 times to indicate save
				status_flash(0x00, 0xFF, 0x00, 5);
			}


			for (uint8_t i = 0; i < LEG_COUNT; i++){
				legs[i].resetPosition();
			}
			
			pwm_apply_batch();
		}
		
		if (l == 0) status_set_color(0xFF, 0x00, 0x00);
		else if (l == 1) status_set_color(0xFF, 0xFF, 0x00);
		else if (l == 2) status_set_color(0x00, 0xFF, 0x00);
		else if (l == 3) status_set_color(0x00, 0xFF, 0xFF);
		else if (l == 4) status_set_color(0x00, 0x00, 0xFF);
		else if (l == 5) status_set_color(0xFF, 0x00, 0xFF);
	}
	
	//Reload calibration from EEPROM; if it was committed, this will do nothing,
	// otherwise we revert any changes done in this session.
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * JOINT_COUNT) + j));
		}
		legs[l].resetPosition();
	}
	pwm_apply_batch();
	
	status_set_color(0x00, 0x00, 0x00);
	_delay_ms(200);
	pwm_stop();
}

void servo_init(){
	//Set up the servos using the leg details + status LEDs
	volatile uint8_t *ports[PWM_COUNT];
	uint8_t pins[PWM_COUNT];

	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			ports[(l * JOINT_COUNT) + j] = legs[l].getPort(j);
			pins[(l * JOINT_COUNT) + j] = legs[l].getPin(j);
			
			legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * JOINT_COUNT) + j));
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

