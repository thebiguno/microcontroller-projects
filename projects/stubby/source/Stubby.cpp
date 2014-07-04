#include "Stubby.h"

#ifndef DEBUG_SIMULATION
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "status.h"
#include "lib/universal_controller/client.h"
#include "lib/serial/serial.h"
#include "lib/pwm/pwm.h"
#endif

#include "gait.h"
#include "trig.h"
#include "Leg.h"
#include "Point.h"


//Set up the leg objects, including servo details and mounting angle.  Leg indices follows
// DIP numbering format, starting at front left and proceeding counter clockwise around.
Leg legs[LEG_COUNT] = {
	Leg(FRONT_LEFT,		&PORTB, PORTB0, &PORTB, PORTB1, &PORTB, PORTB2, 2 * LEG_MOUNTING_ANGLE, Point(-60, 104, 0)),
	Leg(MIDDLE_LEFT,	&PORTB, PORTB3, &PORTA, PORTA3, &PORTB, PORTB4, 3 * LEG_MOUNTING_ANGLE, Point(-120, 0, 0)),
	Leg(REAR_LEFT,		&PORTD, PORTD2, &PORTD, PORTD3, &PORTC, PORTC4, 4 * LEG_MOUNTING_ANGLE, Point(-60, -104, 0)),
	Leg(REAR_RIGHT,		&PORTC, PORTC3, &PORTC, PORTC2, &PORTD, PORTD7, 5 * LEG_MOUNTING_ANGLE, Point(60, -104, 0)),
	Leg(MIDDLE_RIGHT,	&PORTC, PORTC7, &PORTC, PORTC6, &PORTC, PORTC5, 0 * LEG_MOUNTING_ANGLE, Point(120, 0, 0)),
	Leg(FRONT_RIGHT,	&PORTA, PORTA4, &PORTA, PORTA5, &PORTA, PORTA6, 1 * LEG_MOUNTING_ANGLE, Point(60, 104, 0))
};

int main (void){
	wdt_enable(WDTO_2S);
	
	servo_init();
	
	uc_init();
	
	status_init();
	
	mode_select();
}

void mode_select(){
	//Hit start to begin controlling the robot, or hit select to calibrate joints
	while (1){
		wdt_reset();
		
		uint16_t buttons = uc_read_pressed_buttons();
	
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
	
	status_enable_battery();
	
	int8_t step_index = 0;

	//Hit Start to exit remote control mode, and go back to startup
	while(1){
		wdt_reset();
		
		uc_stick_t left_stick = uc_read_left();
		uc_stick_t right_stick = uc_read_right();
		uint16_t pressed = uc_read_pressed_buttons();
		uint16_t held = uc_read_held_buttons();

		//Hit start to exit remote control mode
		if (pressed & _BV(CONTROLLER_BUTTON_VALUE_START)){
			break;
		}

		//Translation: XY (left stick) and Z (right stick)
		if (held & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				Point translate(left_stick.x * -1.5, left_stick.y * -1.5, 0);
				if (right_stick.y > 10 || right_stick.y < -10){
					translate.add(Point(0, 0, right_stick.y * -0.75));
				}
				legs[l].setOffset(translate);
			}
		}
		//Rotation: Pitch / Roll (left stick) and Yaw (right stick)
		else if (held & _BV(CONTROLLER_BUTTON_VALUE_RIGHT2)){
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				legs[l].setOffset(Point(0,0,0));
				Point p = legs[l].getPosition();
				p.rotateXY(right_stick.x * M_PI / 180);
				p.rotateYZ(left_stick.y * M_PI / 180);
				p.rotateXZ(left_stick.x * -M_PI / 180);
				legs[l].setPosition(p);
			}
		}
		//Normal movement
		else {
			double linear_angle = 0;
			if (left_stick.x == 0 && left_stick.y > 0){
				linear_angle = M_PI / 2;
			}
			else if (left_stick.x == 0 && left_stick.y < 0){
				linear_angle = M_PI / -2;
			}
			else if (left_stick.x != 0 || left_stick.y != 0){
				linear_angle = atan2(left_stick.y, left_stick.x);
			}

			//Use pythagorean theorem to find the velocity, in the range [0..1].
			double linear_velocity = fmin(1.0, fmax(0.0, sqrt((left_stick.x * left_stick.x) + (left_stick.y * left_stick.y)) / 15.0));
			
			//We only care about the X axis for right (rotational) stick
			double rotational_velocity = right_stick.x / 15.0;
		
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				Point step = gait_step(legs[l], step_index, linear_velocity, linear_angle, rotational_velocity);
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
	
	status_disable_battery();
	
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
	uint8_t mode = 0x00;		//switch between calibration modes: neutral angle offset (for joint calibration), and position calibration (for X,Y,Z positioning calibration)
	uint16_t held_buttons = 0x00;
	uint16_t pressed_buttons = 0x00;
	
	//We calibrate against the servos in neutral phase, trying to match the angles as stated in Leg.h:
	// TIBIA_NEUTRAL_SERVO_ANGLE, FEMUR_NEUTRAL_SERVO_ANGLE, and MOUNTING_ANGLE (yeah, coxa is different
	// from the others).
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			pwm_set_phase_batch(l * JOINT_COUNT + j, PHASE_NEUTRAL + (legs[l].getCalibration(j) * 10));
		}
	}
	pwm_apply_batch();
	
	//Loop until Start is pressed
	while (1){
		wdt_reset();
		held_buttons = uc_read_held_buttons();
		pressed_buttons = uc_read_pressed_buttons();
		
		_delay_ms(20);
		
		if (pressed_buttons != 0x00){
			//Start exits calibration mode (whether or not changes were made / applied / saved).
			if (pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_START)){
				break;
			}

			//Blink lights out every button press
			status_set_color(0x00, 0x00, 0x00);
			_delay_ms(60);
			
			//Select changes modes.  Flash 5 purple for mode == 1, 5 blue for mode == 0.
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_SELECT)) != 0) {
				mode ^= 0x01;
				status_flash((mode ? 0xFF : 0x00), 0x00, 0xFF, 5);
			}

		
			//Square increments legs.
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_SQUARE)) != 0) {
				l++;
				if (l >= LEG_COUNT) l = 0;
			}
			
			//Left / Right arrows adjust coxa joint / X position
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADRIGHT)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_X : COXA, legs[l].getCalibration(mode ? CALIBRATION_X : COXA) + 1);
			}
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADLEFT)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_X : COXA, legs[l].getCalibration(mode ? CALIBRATION_X : COXA) - 1);
			}
			
			//Up / Down arrows adjust femur joint / Y position
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADUP)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Y : FEMUR, legs[l].getCalibration(mode ? CALIBRATION_Y : FEMUR) + 1);
			}
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_PADDOWN)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Y : FEMUR, legs[l].getCalibration(mode ? CALIBRATION_Y : FEMUR) - 1);
			}
			
			//L1 / L2 adjust the tibia joint / Z position
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT1)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Z : TIBIA, legs[l].getCalibration(mode ? CALIBRATION_Z : TIBIA) + 1);
			}
			if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Z : TIBIA, legs[l].getCalibration(mode ? CALIBRATION_Z : TIBIA) - 1);
			}
			
			//Hold Circle + press Triangle to revert all calibration (in the current mode) to 0
			if ((held_buttons & _BV(CONTROLLER_BUTTON_VALUE_CIRCLE)) != 0 && (pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
				if (mode){
					for (uint8_t i = 0; i < LEG_COUNT; i++){
						for (uint8_t j = JOINT_COUNT; j < CALIBRATION_COUNT; j++){
							legs[i].setCalibration(j, 0);
						}
					}
				}
				else {
					for (uint8_t i = 0; i < LEG_COUNT; i++){
						for (uint8_t j = 0; j < JOINT_COUNT; j++){
							legs[i].setCalibration(j, 0);
						}
					}
				}
				//Flash red 5 times to indicate revert
				status_flash(0xFF, 0x00, 0x00, 5);
			}
			
			//Triangle button to revert all calibration (in all modes) to last saved
			else if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
				for (uint8_t l = 0; l < LEG_COUNT; l++){
					for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
						legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
					}
				}
				
				//Flash yellow 5 times to indicate revert
				status_flash(0xFF, 0xFF, 0x00, 5);
			}

			//X button to commit all changes (in all modes) to EEPROM
			else if ((pressed_buttons & _BV(CONTROLLER_BUTTON_VALUE_CROSS)) != 0) {
				for (uint8_t l = 0; l < LEG_COUNT; l++){
					for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
						eeprom_update_byte((uint8_t*) (l * CALIBRATION_COUNT) + j, legs[l].getCalibration(j));
					}
				}
				//Flash green 5 times to indicate save
				status_flash(0x00, 0xFF, 0x00, 5);
			}

			if (mode){
				for (uint8_t l = 0; l < LEG_COUNT; l++){
					legs[l].resetPosition();
				}
			}
			else {
				for (uint8_t l = 0; l < LEG_COUNT; l++){
					for (uint8_t j = 0; j < JOINT_COUNT; j++){
						pwm_set_phase_batch(l * JOINT_COUNT + j, PHASE_NEUTRAL + (legs[l].getCalibration(j) * 10));
					}
				}
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
		for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
			legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
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
		}
		
		for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
			legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
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

