#include "Stubby.h"

#include <dcutil/new.h>

#include "controllers/General.h"
#include "controllers/UniversalController.h"
#include "gait/gait.h"

#define COMM_TIMEOUT_PERIOD			5000

using namespace digitalcave;

//This cannot be a class variable, since it needs to be accessed by an ISR
SerialAVR serialAvr(38400);

FramedSerialMessage request(0,40);

//Reset WDT after system reset
void get_mcusr(void) __attribute__((naked))  __attribute__((used))  __attribute__((section(".init3")));
void get_mcusr(void) {
	MCUSR = 0;
	wdt_disable();
}

void stubby_main(){
	//Setup
	Stubby stubby;

	wdt_enable(WDTO_2S);
	servo_init(stubby.getLegs());
	battery_init();
	// magnetometer_init();
	// distance_init();
	timer0_init();
	// timer2_init();

	UniversalController universalController(&stubby);
	//Calibration calibration(&stubby);
	General general(&stubby);

	uint32_t time;
	uint32_t lastReceiveMessageTime = 0;

	//Main loop
	while(1){
		//Tell the watchdog we are still alive
		wdt_reset();

		//Loop time
		time = timer_millis();

		//Read incoming serial messages
		if (stubby.getProtocol()->read(&serialAvr, &request)) {
			uint8_t cmd = request.getCommand();

			if ((cmd & 0xF0) == 0x00){
				general.dispatch(&request);
			}
			else if ((cmd & 0xF0) == 0x10){
				universalController.dispatch(&request);
			}
			// else if ((cmd & 0xF0) == 0x30){
			// 	calibration.dispatch(&request);
			// }
			else {
				//TODO Send debug message 'unknown command' or similar
			}
			lastReceiveMessageTime = time;
		}
		else if ((time - lastReceiveMessageTime) > COMM_TIMEOUT_PERIOD) {
			if (stubby.getMode() == MODE_WALKING){
				stubby.sendStatus("Comm Timeout  ", 14);
			}
			stubby.setMode(MODE_UNARMED);
		}

		//Move legs according to state and time
		if (stubby.getMode() == MODE_WALKING){
			gait_step(&stubby);
		}
	}
}

Stubby::Stubby() :
	protocol(64),
	mode(MODE_UNARMED),
	linearAngle(0),
	linearVelocity(0),
	rotationalVelocity(0)
{
	//Set up the leg objects, including servo details and mounting angle.  Leg indices follow
	// DIP numbering format, starting at front left and proceeding counter clockwise around.
#if PCB_REVISION == 1
	legs[0] = new Leg(FRONT_LEFT,	&PORTB, PORTB0, &PORTB, PORTB1, &PORTB, PORTB2, 2 * LEG_MOUNTING_ANGLE, Point(-60, 104, 0));
	legs[1] = new Leg(MIDDLE_LEFT,	&PORTB, PORTB3, &PORTA, PORTA3, &PORTB, PORTB4, 3 * LEG_MOUNTING_ANGLE, Point(-120, 0, 0));
	legs[2] = new Leg(REAR_LEFT,	&PORTD, PORTD2, &PORTD, PORTD3, &PORTC, PORTC4, 4 * LEG_MOUNTING_ANGLE, Point(-60, -104, 0));
	legs[3] = new Leg(REAR_RIGHT,	&PORTC, PORTC3, &PORTC, PORTC2, &PORTD, PORTD7, 5 * LEG_MOUNTING_ANGLE, Point(60, -104, 0));
	legs[4] = new Leg(MIDDLE_RIGHT,	&PORTC, PORTC7, &PORTC, PORTC6, &PORTC, PORTC5, 0 * LEG_MOUNTING_ANGLE, Point(120, 0, 0));
	legs[5] = new Leg(FRONT_RIGHT,	&PORTA, PORTA4, &PORTA, PORTA5, &PORTA, PORTA6, 1 * LEG_MOUNTING_ANGLE, Point(60, 104, 0));
#elif PCB_REVISION == 2
	legs[0] = new Leg(FRONT_LEFT,	&PORTA, PORTA6, &PORTA, PORTA5, &PORTA, PORTA4, 2 * LEG_MOUNTING_ANGLE, Point(-60, 104, 0));
	legs[1] = new Leg(MIDDLE_LEFT,	&PORTA, PORTA3, &PORTB, PORTB0, &PORTB, PORTB1, 3 * LEG_MOUNTING_ANGLE, Point(-120, 0, 0));
	legs[2] = new Leg(REAR_LEFT,	&PORTB, PORTB2, &PORTB, PORTB3, &PORTB, PORTB4, 4 * LEG_MOUNTING_ANGLE, Point(-60, -104, 0));
	legs[3] = new Leg(REAR_RIGHT,	&PORTD, PORTD4, &PORTD, PORTD3, &PORTD, PORTD2, 5 * LEG_MOUNTING_ANGLE, Point(60, -104, 0));
	legs[4] = new Leg(MIDDLE_RIGHT,	&PORTD, PORTD7, &PORTD, PORTD6, &PORTD, PORTD5, 0 * LEG_MOUNTING_ANGLE, Point(120, 0, 0));
	legs[5] = new Leg(FRONT_RIGHT,	&PORTC, PORTC4, &PORTC, PORTC3, &PORTC, PORTC2, 1 * LEG_MOUNTING_ANGLE, Point(60, 104, 0));
#else
	#error Unsupported PCB_REVISION value.
#endif

	serial = &serialAvr;
}

//On ARM chips this function will be in the CubeMX-generated code, and will call xxx_main.
// For consistency we do the same thing here.
int main(void){
	stubby_main();
	return 0;
}

ISR(USART1_RX_vect){
	serialAvr.isr();
}






















// #include "Stubby.h"
//
// #include "controllers/Calibration.h"
// #include "controllers/UniversalController.h"
// #include "gait/gait.h"
// #include "hardware/magnetometer.h"
// #include "hardware/servo.h"
// #include "hardware/status.h"
// #include "hardware/timer2.h"
// #include "types/Point.h"
// #include "util/math.h"
//
// #include "Leg.h"
//
// using namespace digitalcave;
//
// // int main(void) __attribute__ ((noreturn));
// int main (void){
// 	wdt_enable(WDTO_2S);
//
// 	Stubby stubby;
// 	stubby.run();
// 	return 0;
// }



// void Stubby::run(){
// 	FramedSerialMessage request(0,40);
//
// 	servo_init(legs);
// 	battery_init();
// 	magnetometer_init();
// 	distance_init();
// 	timer0_init();
// 	timer2_init();
//
// 	UniversalController universalController(this);
// 	Calibration calibration(this);
//
// 	uint32_t time;
// 	uint32_t lastStepMoveTime = timer_millis();
//
// 	while (1){
// 		time = timer_millis();
// 		wdt_reset();
//
// 		//Listen for incoming commands
// 		if (protocol->read(&serial, &request)) {
// 			if ((request.getCommand() & 0xF0) == 0x00){
// 				//general.dispatch(&serial, &request);		//TODO
// 			}
// 			else if ((request.getCommand() & 0xF0) == 0x10){
// 				universalController.dispatch(&serial, &request);
// 			}
// 			else if ((request.getCommand() & 0xF0) == 0x30){
// 				calibration.dispatch(&serial, &request);
// 			}
// 		}
//
// 		//Perform gait according to current state.
// 		if (mode == MODE_WALKING){
// 			gait_step(stubby);
// 		}
// 	}
// }

// void Stubby::resetLegs(){
// 	for (uint8_t l = 0; l < LEG_COUNT; l+=2){
// 		legs[l]->setOffset(Point(0,0,30));
// 	}
// 	pwm_apply_batch();
// 	delay_ms(200);
//
// 	for (uint8_t l = 0; l < LEG_COUNT; l+=2){
// 		legs[l]->setOffset(Point(0,0,0));
// 	}
// 	pwm_apply_batch();
// 	delay_ms(200);
//
// 	for (uint8_t l = 1; l < LEG_COUNT; l+=2){
// 		legs[l]->setOffset(Point(0,0,30));
// 	}
// 	pwm_apply_batch();
// 	delay_ms(200);
//
// 	for (uint8_t l = 1; l < LEG_COUNT; l+=2){
// 		legs[l]->setOffset(Point(0,0,0));
// 	}
// 	pwm_apply_batch();
// 	delay_ms(200);
// }

// /*
//  * Called from ISR; keep things as short as possible.
//  */
// void protocol_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
// 	if (cmd == MESSAGE_ANNOUNCE_CONTROL_ID){
// 		if (message[0] == 'U'){
// 			controller = CONTROLLER_UC;
// 		}
// 		else if (message[0] == 'P'){
// 			controller = CONTROLLER_PROCESSING;
// 		}
// 		else if (message[0] == 'C'){
// 			controller = CONTROLLER_CALIBRATION;
// 		}
// 		//TODO Put any other supported control modes here.
// 		else {
// 			controller = CONTROLLER_NONE;
// 		}
// 	}
// 	else if (cmd == MESSAGE_REQUEST_ENABLE_DEBUG){
// 		debug = 0x01;
// 		doAcknowledgeCommand(MESSAGE_REQUEST_ENABLE_DEBUG);
// 	}
// 	else if (cmd == MESSAGE_REQUEST_DISABLE_DEBUG){
// 		debug = 0x00;
// 		doAcknowledgeCommand(MESSAGE_REQUEST_DISABLE_DEBUG);
// 	}
// 	//This is a Universal Controller message (namespace 0x1X)
// 	else if (controller == CONTROLLER_UC && (cmd & 0xF0) == 0x10){
// 		uc_dispatch_message(cmd, message, length);
// 	}
// 	//This is a Processing API message (namespace 0x2X)
// 	else if (controller == CONTROLLER_PROCESSING && (cmd & 0xF0) == 0x20){
// 		processing_dispatch_message(cmd, message, length);
// 	}
// 	//This is a Calibration API message (namespace 0x3X)
// 	else if (controller == CONTROLLER_CALIBRATION && (cmd & 0xF0) == 0x30){
// 		calibration_dispatch_message(cmd, message, length);
// 	}
// 	else {
// 		//TODO Send debug message 'unknown command' or similar
// 	}
// }
