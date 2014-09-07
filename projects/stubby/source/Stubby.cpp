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
#include "universal_controller.h"
#include "trig.h"
#include "servo.h"
#include "Leg.h"
#include "Point.h"


//Set up the leg objects, including servo details and mounting angle.  Leg indices follows
// // DIP numbering format, starting at front left and proceeding counter clockwise around.
Leg legs[LEG_COUNT] = {
#if PCB_REVISION == 1
	Leg(FRONT_LEFT,		&PORTB, PORTB0, &PORTB, PORTB1, &PORTB, PORTB2, 2 * LEG_MOUNTING_ANGLE, Point(-60, 104, 0)),
	Leg(MIDDLE_LEFT,	&PORTB, PORTB3, &PORTA, PORTA3, &PORTB, PORTB4, 3 * LEG_MOUNTING_ANGLE, Point(-120, 0, 0)),
	Leg(REAR_LEFT,		&PORTD, PORTD2, &PORTD, PORTD3, &PORTC, PORTC4, 4 * LEG_MOUNTING_ANGLE, Point(-60, -104, 0)),
	Leg(REAR_RIGHT,		&PORTC, PORTC3, &PORTC, PORTC2, &PORTD, PORTD7, 5 * LEG_MOUNTING_ANGLE, Point(60, -104, 0)),
	Leg(MIDDLE_RIGHT,	&PORTC, PORTC7, &PORTC, PORTC6, &PORTC, PORTC5, 0 * LEG_MOUNTING_ANGLE, Point(120, 0, 0)),
	Leg(FRONT_RIGHT,	&PORTA, PORTA4, &PORTA, PORTA5, &PORTA, PORTA6, 1 * LEG_MOUNTING_ANGLE, Point(60, 104, 0))
#elif PCB_REVISION == 2
	#error TODO Add support for Rev2 PCB
#else
	#error Unsupported PCB_REVISION value.
#endif
};

//Stubby state, as bit masks:
#define STATE_POWER			0x01		// bit 1 is power (1 == on, 0 == off)
static uint8_t state;

int main (void){
	wdt_enable(WDTO_2S);
	
	servo_init(legs);
	serial_init_b(38400);
	status_init();
	
	mode_select();
}


void protocol_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	if (cmd == MESSAGE_REQUEST_POWER_ON){
		state |= STATE_POWER;
	}
	else if (cmd == MESSAGE_REQUEST_POWER_OFF){
		state &= ~STATE_POWER;
	}
	else if (cmd == MESSAGE_UC_BUTTON_PUSH && message[0] == CONTROLLER_BUTTON_VALUE_START){
		state ^= STATE_POWER;
	}
	//This is a Universal Controller message (namespace 0x1X)
	else if ((cmd & 0xF0) == 0x10){
		uc_dispatch_message(cmd, message, length);
	}
}

void mode_select(){
	//Hit start to begin controlling the robot, or hit select to calibrate joints
	while (1){
		wdt_reset();
		
		if (uc_read_pressed_buttons() & _BV(CONTROLLER_BUTTON_VALUE_SELECT)){
			mode_calibration();
		}
		
		if (state & STATE_POWER){
			mode_remote_control();
		}
		
		_delay_ms(20);
	}
}

/*
 * Performs a single move operation.  Returns the distance moved.
 */
uint8_t doMove(double linear_angle, double linear_velocity, double rotational_velocity){
	static int8_t step_index = 0;
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		Point step = gait_step(legs[l], step_index, linear_velocity, linear_angle, rotational_velocity);
		legs[l].setOffset(step);
	}
	step_index++;
	if (step_index > gait_step_count()){
		step_index = 0;
	}
	
	pwm_apply_batch();
	_delay_ms(5);
	
	//TODO In the current implementation of gait_tripod, we move 5mm with each iteration of the
	// step procedure at maximum velocity (and the distance scales linearly with velocity).
	// Eventually we need to add something to the gait code where we can query this value; for
	// now, a constant with linear scaling factor is fine.
	return 5 * linear_velocity;
}

/*
 * Performs a single rotation operation instantaneously (no stepping / smoothing).
 */
void doRotate(double pitch, double roll, double yaw){
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].setOffset(Point(0,0,0));
		Point p = legs[l].getPosition();
		p.rotateXY(yaw);	
		p.rotateYZ(pitch);
		p.rotateXZ(roll);
		legs[l].setPosition(p);
	}
	
	pwm_apply_batch();
	_delay_ms(5);
}

void doTranslate(uint16_t x, uint16_t y, uint16_t z){
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		Point translate(x, y, z);
		legs[l].setOffset(translate);
	}
	
	pwm_apply_batch();
	_delay_ms(5);
}

void mode_remote_control(){
	pwm_start();
	status_set_color(0x00, 0xFF, 0x00);
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs[l].resetPosition();
	}
	pwm_apply_batch();
	
	status_enable_battery();
	
	//Hit Start to exit remote control mode, and go back to startup
	while(1){
		wdt_reset();
		
		//Hit start to exit remote control mode
		//if (pressed & _BV(CONTROLLER_BUTTON_VALUE_START)){
		if (!(state & STATE_POWER)){
			break;
		}
		
		uint16_t buttons_held = uc_read_held_buttons();
		uc_stick_t left_stick = uc_read_left();
		uc_stick_t right_stick = uc_read_right();

		//Translation: XY (left stick) and Z (right stick)
		if (buttons_held & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)){
			uint16_t x = left_stick.x * -0.2;
			uint16_t y = left_stick.y * -0.2;
			uint16_t z = (right_stick.y > 50 || right_stick.y < -50) ? right_stick.y * -0.1 : 0;
			doTranslate(x, y, z);
		}
		//Rotation: Pitch / Roll (left stick) and Yaw (right stick)
		else if (buttons_held & _BV(CONTROLLER_BUTTON_VALUE_RIGHT2)){
			//We get the magic number of 1440 by multiplying 180 by 8... it is identical to saying right_stick.x / 8 * (M_PI / 180), but faster
			double pitch = left_stick.y * M_PI / 1440;
			double roll = left_stick.x * -M_PI / 1440;
			double yaw = right_stick.x * M_PI / 1440;
			doRotate(pitch, roll, yaw);
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
			double linear_velocity = fmin(1.0, fmax(0.0, sqrt((left_stick.x * left_stick.x) + (left_stick.y * left_stick.y)) / 127.0));
			
			//We only care about the X axis for right (rotational) stick
			double rotational_velocity = right_stick.x / 127.0;
			
			doMove(linear_angle, linear_velocity, rotational_velocity);
		}
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
		
		_delay_ms(20);
		
		uint16_t buttons_pressed = uc_read_pressed_buttons();
		
		if (buttons_pressed != 0x00){
			//Start exits calibration mode (whether or not changes were made / applied / saved).
			if (buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_START)){
				break;
			}

			//Blink lights out every button press
			status_set_color(0x00, 0x00, 0x00);
			_delay_ms(60);
			
			//Select changes modes.  Flash 5 purple for mode == 1, 5 blue for mode == 0.
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_SELECT)) != 0) {
				mode ^= 0x01;
				status_flash((mode ? 0xFF : 0x00), 0x00, 0xFF, 5);
			}

		
			//Square increments legs.
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_SQUARE)) != 0) {
				l++;
				if (l >= LEG_COUNT) l = 0;
			}
			
			//Left / Right arrows adjust coxa joint / X position
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADRIGHT)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_X : COXA, legs[l].getCalibration(mode ? CALIBRATION_X : COXA) + 1);
			}
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADLEFT)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_X : COXA, legs[l].getCalibration(mode ? CALIBRATION_X : COXA) - 1);
			}
			
			//Up / Down arrows adjust femur joint / Y position
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADUP)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Y : FEMUR, legs[l].getCalibration(mode ? CALIBRATION_Y : FEMUR) + 1);
			}
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADDOWN)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Y : FEMUR, legs[l].getCalibration(mode ? CALIBRATION_Y : FEMUR) - 1);
			}
			
			//L1 / L2 adjust the tibia joint / Z position
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_LEFT1)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Z : TIBIA, legs[l].getCalibration(mode ? CALIBRATION_Z : TIBIA) + 1);
			}
			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)) != 0) {
				legs[l].setCalibration(mode ? CALIBRATION_Z : TIBIA, legs[l].getCalibration(mode ? CALIBRATION_Z : TIBIA) - 1);
			}
			
			//Hold Circle + press Triangle to revert all calibration (in the current mode) to 0
			if ((uc_read_pressed_buttons() & _BV(CONTROLLER_BUTTON_VALUE_CIRCLE)) != 0 && (buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
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
			else if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
				for (uint8_t l = 0; l < LEG_COUNT; l++){
					for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
						legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
					}
				}
				
				//Flash yellow 5 times to indicate revert
				status_flash(0xFF, 0xFF, 0x00, 5);
			}

			//X button to commit all changes (in all modes) to EEPROM
			else if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_CROSS)) != 0) {
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
