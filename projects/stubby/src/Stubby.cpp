#include "Stubby.h"

#include "controllers/Calibration.h"
#include "controllers/UniversalController.h"
#include "gait/gait.h"
#include "hardware/magnetometer.h"
#include "hardware/servo.h"
#include "hardware/status.h"
#include "hardware/timer2.h"
#include "types/Point.h"
#include "util/math.h"

#include "Leg.h"

using namespace digitalcave;

//This cannot be a class variable, since it needs to be accessed by an ISR
SerialAVR serial(38400);

//Stubby state variables
static volatile uint8_t power = 0x00;					//0 is off, 1 is on
volatile uint8_t controller = 0x00;						//0 is no controller, 1 is Universal Controller, 2 is Processing API, 3 is Calibration API
volatile uint8_t debug = 0x00;							//0 is no debug, 1 is show debug
volatile uint8_t pending_acknowledge = 0x00;			//When set, we will send a message in the delay code
volatile uint8_t pending_complete = 0x00;				//When set, we will send a message in the delay code

//Reset WDT after system reset
void get_mcusr(void) __attribute__((naked))  __attribute__((used))  __attribute__((section(".init3")));
void get_mcusr(void) {
	MCUSR = 0;
	wdt_disable();
}

int main(void) __attribute__ ((noreturn));
int main (void){
	wdt_enable(WDTO_2S);

	Stubby stubby;
	stubby.run();
}

Stubby::Stubby() :
	//Set up the leg objects, including servo details and mounting angle.  Leg indices follows
	// // DIP numbering format, starting at front left and proceeding counter clockwise around.
	legs[LEG_COUNT]({
	#if PCB_REVISION == 1
		Leg(FRONT_LEFT,		&PORTB, PORTB0, &PORTB, PORTB1, &PORTB, PORTB2, 2 * LEG_MOUNTING_ANGLE, Point(-60, 104, 0)),
		Leg(MIDDLE_LEFT,	&PORTB, PORTB3, &PORTA, PORTA3, &PORTB, PORTB4, 3 * LEG_MOUNTING_ANGLE, Point(-120, 0, 0)),
		Leg(REAR_LEFT,		&PORTD, PORTD2, &PORTD, PORTD3, &PORTC, PORTC4, 4 * LEG_MOUNTING_ANGLE, Point(-60, -104, 0)),
		Leg(REAR_RIGHT,		&PORTC, PORTC3, &PORTC, PORTC2, &PORTD, PORTD7, 5 * LEG_MOUNTING_ANGLE, Point(60, -104, 0)),
		Leg(MIDDLE_RIGHT,	&PORTC, PORTC7, &PORTC, PORTC6, &PORTC, PORTC5, 0 * LEG_MOUNTING_ANGLE, Point(120, 0, 0)),
		Leg(FRONT_RIGHT,	&PORTA, PORTA4, &PORTA, PORTA5, &PORTA, PORTA6, 1 * LEG_MOUNTING_ANGLE, Point(60, 104, 0))
	#elif PCB_REVISION == 2
		Leg(FRONT_LEFT,		&PORTA, PORTA6, &PORTA, PORTA5, &PORTA, PORTA4, 2 * LEG_MOUNTING_ANGLE, Point(-60, 104, 0)),
		Leg(MIDDLE_LEFT,	&PORTA, PORTA3, &PORTB, PORTB0, &PORTB, PORTB1, 3 * LEG_MOUNTING_ANGLE, Point(-120, 0, 0)),
		Leg(REAR_LEFT,		&PORTB, PORTB2, &PORTB, PORTB3, &PORTB, PORTB4, 4 * LEG_MOUNTING_ANGLE, Point(-60, -104, 0)),
		Leg(REAR_RIGHT,		&PORTD, PORTD4, &PORTD, PORTD3, &PORTD, PORTD2, 5 * LEG_MOUNTING_ANGLE, Point(60, -104, 0)),
		Leg(MIDDLE_RIGHT,	&PORTD, PORTD7, &PORTD, PORTD6, &PORTD, PORTD5, 0 * LEG_MOUNTING_ANGLE, Point(120, 0, 0)),
		Leg(FRONT_RIGHT,	&PORTC, PORTC4, &PORTC, PORTC3, &PORTC, PORTC2, 1 * LEG_MOUNTING_ANGLE, Point(60, 104, 0))
	#else
		#error Unsupported PCB_REVISION value.
	#endif
	}),
	state(STATE_POWER_OFF)
{

}

void Stubby::run(){
	FramedSerialMessage request(0,40);
	
	servo_init(legs);
	battery_init();
	magnetometer_init();
	distance_init();
	timer0_init();
	timer2_init();
	
	Calibration calibration(this);
	
	uint32_t time;
	uint32_t lastStepMoveTime;

	while (1){
		time = timer_millis();
		wdt_reset();
		
		//Listen for incoming commands
		if (protocol.read(&serial, &request)) {
			if (request.getCommand() & 0xF0 == 0x30){
				calibration.dispatch(&serial, &request);
			}
			else if (controller == CONTROLLER_PROCESSING){
				processing_command_executor();
			}
			else if (controller == CONTROLLER_CALIBRATION){
				calibration_command_executor();
			}
		}
		
		//Perform gait according to current state.
		if (state == STATE_WALKING){
			if ((time -lastStepMoveTime) > 5){
				if (rotational_velocity <= 0.3 && rotational_velocity > -0.3) rotational_velocity = 0;
				if (linear_velocity <= 0.3) linear_velocity = 0;
				if (linear_velocity != 0 || rotational_velocity != 0){
					static int8_t step_index = 0;
					
					for (uint8_t l = 0; l < LEG_COUNT; l++){
						Point step = gait_step(legs[l], step_index, linear_velocity, linear_angle, rotational_velocity);
						legs[l].setOffset(step);
					}
					step_index++;
					if (step_index > gait_step_count()){
						step_index = 0;
					}
				}
				else {
					for (uint8_t l = 0; l < LEG_COUNT; l++){
						legs[l].setOffset(Point(0,0,0));
					}
				}
				pwm_apply_batch();
				lastStepMoveTime = time;
			}
		}
	}
}

void Stubby::sendDebug(char* message){
	if (debug){
		FramedSerialMessage response(MESSAGE_SEND_DEBUG, (uint8_t*) message, strlen(message));
		protocol.write(&serial, response);
	}
}

void Stubby::resetLegs(){
	for (uint8_t l = 0; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,30));
	}
	pwm_apply_batch();
	delay_ms(200);

	for (uint8_t l = 0; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,0));
	}
	pwm_apply_batch();
	delay_ms(200);

	for (uint8_t l = 1; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,30));
	}
	pwm_apply_batch();
	delay_ms(200);

	for (uint8_t l = 1; l < LEG_COUNT; l+=2){
		legs[l].setOffset(Point(0,0,0));
	}
	pwm_apply_batch();
	delay_ms(200);
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
		else if (message[0] == 'C'){
			controller = CONTROLLER_CALIBRATION;
		}
		//TODO Put any other supported control modes here.
		else {
			controller = CONTROLLER_NONE;
		}
	}
	else if (cmd == MESSAGE_REQUEST_ENABLE_DEBUG){
		debug = 0x01;
		doAcknowledgeCommand(MESSAGE_REQUEST_ENABLE_DEBUG);
	}
	else if (cmd == MESSAGE_REQUEST_DISABLE_DEBUG){
		debug = 0x00;
		doAcknowledgeCommand(MESSAGE_REQUEST_DISABLE_DEBUG);
	}
	//This is a Universal Controller message (namespace 0x1X)
	else if (controller == CONTROLLER_UC && (cmd & 0xF0) == 0x10){
		uc_dispatch_message(cmd, message, length);
	}
	//This is a Processing API message (namespace 0x2X)
	else if (controller == CONTROLLER_PROCESSING && (cmd & 0xF0) == 0x20){
		processing_dispatch_message(cmd, message, length);
	}
	//This is a Calibration API message (namespace 0x3X)
	else if (controller == CONTROLLER_CALIBRATION && (cmd & 0xF0) == 0x30){
		calibration_dispatch_message(cmd, message, length);
	}
	else {
		//TODO Send debug message 'unknown command' or similar
	}
}

FramedSerialProtocol* Stubby::getProtocol(){
	return &protocol;
}

ISR(USART1_RX_vect){
	serial.isr();
}