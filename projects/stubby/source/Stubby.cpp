#include "Stubby.h"

#include "controllers/processing.h"
#include "controllers/universal_controller.h"
#include "gait/gait.h"
#include "hardware/servo.h"
#include "hardware/status.h"
#include "util/math.h"

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

//Stubby state variables
static uint8_t power = 0x00;			//0 is off, 1 is on
static uint8_t controller = 0x00;		//0 is no controller, 1 is Universal Controller, 2 is Processing API

int main (void){
	wdt_enable(WDTO_2S);
	
	servo_init(legs);
	serial_init_b(38400);
	status_init();
	
	while (1){
		wdt_reset();

		//_delay_ms(100);
		//PORTD ^= _BV(PORTD4);

		if (controller == CONTROLLER_UC){
			//PORTD ^= _BV(PORTD5);
			uc_command_executor();
		}
		else if (controller == CONTROLLER_PROCESSING){
			processing_command_executor();
		}
	}
}

uint8_t get_power(){
	return power;
}
void set_power(uint8_t _power){
	power = _power;
}
uint8_t get_controller(){
	return controller;
}

void doAcknowledgeCommand(uint8_t command){
	uint8_t data[1];
	data[0] = command;
	protocol_send_message(MESSAGE_SEND_ACKNOWLEDGE, data, 1);
}

void doCompleteCommand(uint8_t command){
	uint8_t data[1];
	data[0] = command;
	protocol_send_message(MESSAGE_SEND_COMPLETE, data, 1);
}

void doResetLegs(){
	for (uint8_t l = 0; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,30));
	}
	pwm_apply_batch();
	_delay_ms(200);

	for (uint8_t l = 0; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,0));
	}
	pwm_apply_batch();
	_delay_ms(200);

	for (uint8_t l = 1; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,30));
	}
	pwm_apply_batch();
	_delay_ms(200);

	for (uint8_t l = 1; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,0));
	}
	pwm_apply_batch();
	_delay_ms(200);
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
	_delay_ms(3);
	
	//TODO In the current implementation of gait_tripod, we move 5mm with each iteration of the
	// step procedure at maximum velocity (and the distance scales linearly with velocity).
	// Eventually we need to add something to the gait code where we can query this value; for
	// now, a constant with linear scaling factor is fine.
	//Note: Through experimentation we find that each step is slightly smaller than 5mm... not 
	// sure if this is due to slippage, bad measurements, not enough timing, or something else.
	// Regardless, by making this number smaller, we end up with the right measurement in real
	// world applications.  Yay for fudge!
	return 4.5 * linear_velocity;
}

/*
 * Performs a single turn operation.  Returns the angle moved (radians).
 */
double doTurn(double rotational_velocity){
	static int8_t step_index = 0;
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		Point step = gait_step(legs[l], step_index, 0, 0, rotational_velocity);
		legs[l].setOffset(step);
	}
	step_index++;
	if (step_index > gait_step_count()){
		step_index = 0;
	}
	
	pwm_apply_batch();
	_delay_ms(5);
	
	//How much of an angle it has moved in this step operation.  This multiplier was
	// arrived at by trial and error.  It rotates faster one way than the other.  Yeah, baby!
	return ((rotational_velocity < 0 ? 1.6 : 1.3) * M_PI / 180) * rotational_velocity;
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

/*
 * Performs a single translation operation instantaneously (no stepping / smoothing).
 */
void doTranslate(uint16_t x, uint16_t y, uint16_t z){
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		Point translate(x, y, z);
		legs[l].setOffset(translate);
	}
	
	pwm_apply_batch();
	_delay_ms(5);
}

/*
 * Called from ISR; keep things as short as possible.
 */
void protocol_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	if (cmd == MESSAGE_ANNOUNCE_CONTROL_ID){
		if (message[0] == 'U'){
			controller = CONTROLLER_UC;
		}
		else if (message[0] == 'P'){
			controller = CONTROLLER_PROCESSING;
		}
		//TODO Put any other supported control modes here.
		else {
			controller = CONTROLLER_NONE;
		}
	}
	//This is a Universal Controller message (namespace 0x1X)
	else if (controller == CONTROLLER_UC && (cmd & 0xF0) == 0x10){
		uc_dispatch_message(cmd, message, length);
	}
	//This is a Processing API message (namespace 0x2X)
	else if (controller == CONTROLLER_PROCESSING && (cmd & 0xF0) == 0x20){
		processing_dispatch_message(cmd, message, length);
	}
	else {
		//TODO Send debug message 'unknown command' or similar
	}
}