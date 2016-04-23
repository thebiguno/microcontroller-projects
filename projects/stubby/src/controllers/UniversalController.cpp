#include "UniversalController.h"

using namespace digitalcave;

UniversalController::UniversalController(Stubby* stubby) : 
	stubby(stubby)
{
	;
}

void UniversalController::dispatch(Stream* serial, FramedSerialMessage* message){
	
}





// extern Leg legs[LEG_COUNT];
// 
// static volatile uc_stick_t left_stick;
// static volatile uc_stick_t right_stick;
// 
// static volatile uint16_t uc_buttons_pressed;
// static volatile uint16_t uc_buttons_released;
// static volatile uint16_t uc_buttons_held;
// 
// uint16_t uc_read_pressed_buttons(){
// 	if (uc_buttons_pressed == 0x00) return 0x00;
// 	
// 	uint16_t result = uc_buttons_pressed;
// 	uc_buttons_pressed = 0x00;
// 	return result;
// }
// 
// uint16_t uc_read_released_buttons(){
// 	if (uc_buttons_released == 0x00) return 0x00;
// 	
// 	uint16_t result = uc_buttons_released;
// 	uc_buttons_released = 0x00;
// 	return result;
// }
// 
// uint16_t uc_read_held_buttons(){
// 	return uc_buttons_held;
// }
// 
// uc_stick_t uc_read_left(){
// 	uc_stick_t result;
// 	result.x = left_stick.x;
// 	result.y = left_stick.y;
// 	return result;
// }
// uc_stick_t uc_read_right(){
// 	uc_stick_t result;
// 	result.x = right_stick.x;
// 	result.y = right_stick.y;
// 	return result;
// }
// 
// void uc_remote_control(){
// 	pwm_start();
// 	status_set_color(0x00, 0xFF, 0x00);
// 	battery_enable_status();
// 	doResetLegs();
// 	
// 	//Hit Start to exit remote control mode, and go back to startup
// 	while(1){
// 		wdt_reset();
// 		
// 		//Exit when start is pressed again (or if UC loses controller state)
// 		if (!(get_power() & POWER_ON)){
// 			break;
// 		}
// 		else if (get_controller() != CONTROLLER_UC){
// 			break;
// 		}
// 
// 		
// 		uint16_t buttons_pressed = uc_read_pressed_buttons();
// 		uint16_t buttons_held = uc_read_held_buttons();
// 		uc_stick_t left_stick = uc_read_left();
// 		uc_stick_t right_stick = uc_read_right();
// 
// 		if (buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_CROSS)){
// 			doResetLegs();
// 		}
// 		//Translation: XY (left stick) and Z (right stick)
// 		else if (buttons_held & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)){
// 			uint16_t x = left_stick.x * -0.2;
// 			uint16_t y = left_stick.y * -0.2;
// 			uint16_t z = (right_stick.y > 50 || right_stick.y < -50) ? right_stick.y * -0.1 : 0;
// 
// 			for (uint8_t l = 0; l < LEG_COUNT; l++){
// 				Point translate(x, y, z);
// 				legs[l].setOffset(translate);
// 			}
// 			
// 			pwm_apply_batch();
// #if F_CPU == 12000000
// 			delay_ms(5);
// #elif F_CPU == 18432000
// 			delay_ms(8);
// #elif F_CPU == 20000000
// 			delay_ms(10);
// #else
// #warning Non-standard CPU speed.  Please specify a delay.
// #endif
// 		}
// 		//Rotation: Pitch / Roll (left stick) and Yaw (right stick)
// 		else if (buttons_held & _BV(CONTROLLER_BUTTON_VALUE_RIGHT2)){
// 			//We get the magic number of 1440 by multiplying 180 by 8... it is identical to saying right_stick.x / 8 * (M_PI / 180), but faster
// 			double pitch = left_stick.y * M_PI / 1440;
// 			double roll = left_stick.x * -M_PI / 1440;
// 			double yaw = right_stick.x * M_PI / 1440;
// 			
// 			for (uint8_t l = 0; l < LEG_COUNT; l++){
// 				legs[l].setOffset(Point(0,0,0));
// 				Point p = legs[l].getPosition();
// 				p.rotateXY(yaw);	
// 				p.rotateYZ(pitch);
// 				p.rotateXZ(roll);
// 				legs[l].setPosition(p);
// 			}
// 			
// 			pwm_apply_batch();
// #if F_CPU == 12000000
// 			delay_ms(5);
// #elif F_CPU == 18432000
// 			delay_ms(8);
// #elif F_CPU == 20000000
// 			delay_ms(10);
// #else
// #warning Non-standard CPU speed.  Please specify a delay.
// #endif
// 		}
// 		//Normal movement
// 		else {
// 			double linear_angle = 0;
// 			if (left_stick.x == 0 && left_stick.y > 0){
// 				linear_angle = M_PI / 2;
// 			}
// 			else if (left_stick.x == 0 && left_stick.y < 0){
// 				linear_angle = M_PI / -2;
// 			}
// 			else if (left_stick.x != 0 || left_stick.y != 0){
// 				linear_angle = atan2(left_stick.y, left_stick.x);
// 			}
// 			
// 
// 			//Use pythagorean theorem to find the velocity, in the range [0..1].
// 			double linear_velocity = fmin(1.0, fmax(0.0, sqrt((left_stick.x * left_stick.x) + (left_stick.y * left_stick.y)) / 127.0));
// 			
// 			//We only care about the X axis for right (rotational) stick
// 			double rotational_velocity = right_stick.x / 127.0;
// 
// 			if (rotational_velocity <= 0.3 && rotational_velocity > -0.3) rotational_velocity = 0;
// 			if (linear_velocity <= 0.3) linear_velocity = 0;
// 			if (linear_velocity != 0 || rotational_velocity != 0){
// 				static int8_t step_index = 0;
// 				
// 				for (uint8_t l = 0; l < LEG_COUNT; l++){
// 					Point step = gait_step(legs[l], step_index, linear_velocity, linear_angle, rotational_velocity);
// 					legs[l].setOffset(step);
// 				}
// 				step_index++;
// 				if (step_index > gait_step_count()){
// 					step_index = 0;
// 				}
// 			}
// 			else {
// 				for (uint8_t l = 0; l < LEG_COUNT; l++){
// 					legs[l].setOffset(Point(0,0,0));
// 				}
// 			}
// 			pwm_apply_batch();
// #if F_CPU == 12000000
// 			delay_ms(5);
// #elif F_CPU == 18432000
// 			delay_ms(7);
// #elif F_CPU == 20000000
// 			delay_ms(8);
// #else
// #warning Non-standard CPU speed.  Please specify a delay.
// #endif
// 
// 		}
// 	}
// 	
// 	battery_disable_status();
// 	doResetLegs();
// 	status_set_color(0x00, 0x00, 0x00);
// 	delay_ms(200);
// 
// 	pwm_stop();
// }

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
// void uc_calibration(){
// 	pwm_start();
// 
// 	status_flash(0x00, 0x00, 0xFF, 5);
// 	uint8_t l = 0;
// 	uint8_t mode = 0x00;		//switch between calibration modes: neutral angle offset (for joint calibration), and position calibration (for X,Y,Z positioning calibration)
// 	
// 	//We calibrate against the servos in neutral phase, trying to match the angles as stated in Leg.h:
// 	// TIBIA_NEUTRAL_SERVO_ANGLE, FEMUR_NEUTRAL_SERVO_ANGLE, and MOUNTING_ANGLE (yeah, coxa is different
// 	// from the others).
// 	for (uint8_t l = 0; l < LEG_COUNT; l++){
// 		for (uint8_t j = 0; j < JOINT_COUNT; j++){
// 			pwm_set_phase_batch(l * JOINT_COUNT + j, PHASE_NEUTRAL + (legs[l].getCalibration(j) * 10));
// 		}
// 	}
// 	pwm_apply_batch();
// 	
// 	//Loop until Start is pressed (or UC loses controller state)
// 	while (1){
// 		wdt_reset();
// 		
// 		delay_ms(20);
// 		
// 		uint16_t buttons_pressed = uc_read_pressed_buttons();
// 		
// 		if (buttons_pressed != 0x00){
// 			//Start exits calibration mode (whether or not changes were made / applied / saved).
// 			if (buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_START)){
// 				break;
// 			}
// 			else if (get_controller() != CONTROLLER_UC){
// 				break;
// 			}
// 
// 			//Blink lights out every button press
// 			status_set_color(0x00, 0x00, 0x00);
// 			delay_ms(60);
// 			
// 			//Select changes modes.  Flash 5 purple for mode == 1, 5 blue for mode == 0.
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_SELECT)) != 0) {
// 				mode ^= 0x01;
// 				status_flash((mode ? 0xFF : 0x00), 0x00, 0xFF, 5);
// 			}
// 
// 		
// 			//Square increments legs.
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_SQUARE)) != 0) {
// 				l++;
// 				if (l >= LEG_COUNT) l = 0;
// 			}
// 			
// 			//Left / Right arrows adjust coxa joint / X position
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADRIGHT)) != 0) {
// 				legs[l].setCalibration(mode ? CALIBRATION_X : COXA, legs[l].getCalibration(mode ? CALIBRATION_X : COXA) + 1);
// 			}
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADLEFT)) != 0) {
// 				legs[l].setCalibration(mode ? CALIBRATION_X : COXA, legs[l].getCalibration(mode ? CALIBRATION_X : COXA) - 1);
// 			}
// 			
// 			//Up / Down arrows adjust femur joint / Y position
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADUP)) != 0) {
// 				legs[l].setCalibration(mode ? CALIBRATION_Y : FEMUR, legs[l].getCalibration(mode ? CALIBRATION_Y : FEMUR) + 1);
// 			}
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_PADDOWN)) != 0) {
// 				legs[l].setCalibration(mode ? CALIBRATION_Y : FEMUR, legs[l].getCalibration(mode ? CALIBRATION_Y : FEMUR) - 1);
// 			}
// 			
// 			//L1 / L2 adjust the tibia joint / Z position
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_LEFT1)) != 0) {
// 				legs[l].setCalibration(mode ? CALIBRATION_Z : TIBIA, legs[l].getCalibration(mode ? CALIBRATION_Z : TIBIA) + 1);
// 			}
// 			if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_LEFT2)) != 0) {
// 				legs[l].setCalibration(mode ? CALIBRATION_Z : TIBIA, legs[l].getCalibration(mode ? CALIBRATION_Z : TIBIA) - 1);
// 			}
// 			
// 			//Hold Circle + press Triangle to revert all calibration (in the current mode) to 0
// 			if ((uc_read_held_buttons() & _BV(CONTROLLER_BUTTON_VALUE_CIRCLE)) != 0 && (buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
// 				if (mode){
// 					for (uint8_t i = 0; i < LEG_COUNT; i++){
// 						for (uint8_t j = JOINT_COUNT; j < CALIBRATION_COUNT; j++){
// 							legs[i].setCalibration(j, 0);
// 						}
// 					}
// 				}
// 				else {
// 					for (uint8_t i = 0; i < LEG_COUNT; i++){
// 						for (uint8_t j = 0; j < JOINT_COUNT; j++){
// 							legs[i].setCalibration(j, 0);
// 						}
// 					}
// 				}
// 				//Flash red 5 times to indicate revert
// 				status_flash(0xFF, 0x00, 0x00, 5);
// 			}
// 			
// 			//Triangle button to revert all calibration (in all modes) to last saved
// 			else if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_TRIANGLE)) != 0) {
// 				for (uint8_t l = 0; l < LEG_COUNT; l++){
// 					for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
// 						legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
// 					}
// 				}
// 				
// 				//Flash yellow 5 times to indicate revert
// 				status_flash(0xFF, 0xFF, 0x00, 5);
// 			}
// 
// 			//X button to commit all changes (in all modes) to EEPROM
// 			else if ((buttons_pressed & _BV(CONTROLLER_BUTTON_VALUE_CROSS)) != 0) {
// 				for (uint8_t l = 0; l < LEG_COUNT; l++){
// 					for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
// 						eeprom_update_byte((uint8_t*) (l * CALIBRATION_COUNT) + j, legs[l].getCalibration(j));
// 					}
// 				}
// 				//Flash green 5 times to indicate save
// 				status_flash(0x00, 0xFF, 0x00, 5);
// 			}
// 
// 			if (mode){
// 				for (uint8_t l = 0; l < LEG_COUNT; l++){
// 					legs[l].setOffset(Point(0,0,0));
// 				}
// 			}
// 			else {
// 				for (uint8_t l = 0; l < LEG_COUNT; l++){
// 					for (uint8_t j = 0; j < JOINT_COUNT; j++){
// 						pwm_set_phase_batch(l * JOINT_COUNT + j, PHASE_NEUTRAL + (legs[l].getCalibration(j) * 10));
// 					}
// 				}
// 			}
// 			
// 			pwm_apply_batch();
// 		}
// 		
// 		if (l == 0) status_set_color(0xFF, 0x00, 0x00);
// 		else if (l == 1) status_set_color(0xFF, 0xFF, 0x00);
// 		else if (l == 2) status_set_color(0x00, 0xFF, 0x00);
// 		else if (l == 3) status_set_color(0x00, 0xFF, 0xFF);
// 		else if (l == 4) status_set_color(0x00, 0x00, 0xFF);
// 		else if (l == 5) status_set_color(0xFF, 0x00, 0xFF);
// 	}
// 	
// 	//Reload calibration from EEPROM; if it was committed, this will do nothing,
// 	// otherwise we revert any changes done in this session.
// 	for (uint8_t l = 0; l < LEG_COUNT; l++){
// 		for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
// 			legs[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
// 		}
// 		legs[l].resetPosition();
// 	}
// 	pwm_apply_batch();
// 	
// 	status_set_color(0x00, 0x00, 0x00);
// 	delay_ms(200);
// 	pwm_stop();
// }
// 
// void uc_command_executor(){
// 	wdt_reset();
// 	
// 	//Hit start to begin controlling the robot, or hit select to calibrate joints
// 	if (uc_read_pressed_buttons() & _BV(CONTROLLER_BUTTON_VALUE_SELECT)){
// 		uc_calibration();
// 	}
// 	else if (get_power() & POWER_ON){
// 		uc_remote_control();
// 	}
// 	
// 	delay_ms(20);
// }

/*
 * UC-specific message dispatching.  This is called from an ISR, so keep things as short as possible here.
 */
// void uc_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
// 	if (cmd == MESSAGE_UC_BUTTON_PUSH){
// 		if (message[0] == CONTROLLER_BUTTON_VALUE_START || message[0] == CONTROLLER_BUTTON_VALUE_SQUARE){
// 			set_power(get_power() ^ POWER_ON);
// 		}
// 
// 		uint16_t pressed_value = _BV(message[0]);
// 		uc_buttons_pressed |= pressed_value;	//Mark as pressed; this is cleared when they are read.
// 		uc_buttons_held |= pressed_value;		//Mark as held; this is cleared when released
// 	}
// 	else if (cmd == MESSAGE_UC_BUTTON_RELEASE){
// 		uint16_t released_value = _BV(message[0]);
// 		uc_buttons_released |= released_value;	//Mark as released; this is cleared when they are read.
// 		uc_buttons_held &= ~released_value;		//Mark as no longer held
// 	}
// 	else if (cmd == MESSAGE_UC_JOYSTICK_MOVE){
// 		left_stick.x = message[0] - 127;
// 		left_stick.y = 127 - message[1];
// 		right_stick.x = message[2] - 127;
// 		right_stick.y = 127 - message[3];
// 	}
// }