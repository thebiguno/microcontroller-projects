#ifndef HEX_LEG
#define HEX_LEG

#define __DELAY_BACKWARD_COMPATIBLE__ 

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

typedef struct leg_t {
	volatile uint8_t *port[3];
	uint8_t pin[3];

	int16_t offset[3];
	int8_t direction[3];
	
	//The current angle in radians
	double current[3];
	
	//The desired angle in radians
	double desired[3];
	
	//The step in radians to be moved each iteration through the busy/wait loop.
	double step[3];
} leg_t;

#include "servo.h"

#define FRONT_LEFT		0
#define FRONT_RIGHT		1
#define MIDDLE_LEFT		2
#define MIDDLE_RIGHT	3
#define REAR_LEFT		4
#define REAR_RIGHT		5

#define COXA			0
#define FEMUR			1
#define TIBIA			2

#define DELAY_STEP		16

//Initialize memory for an array of legs.  By convention, the resulting array is ordered
// from left to right, front to back.  I.e. Left Front, Right Front, Left Middle, Right Middle, etc. 
//The implementation of leg_init MUST be hardware-specific, as part of the init is port / pin 
// combinations for the servo drivers.
void leg_init(uint8_t count);

//Set the leg position in X,Y,Z co-ordinate space, and specify how long (millis) it should 
// take to move it to that position.
//TODO For a 2 DOF leg, having an X,Y,Z co-ordinate space doesn't really make sense, as there
// is only the partial shell of a sphere that the foot can travel.  For now, we only specify
// Y and Z, and these are radian values which are passed through as-is to the servos.
void leg_set_position(uint8_t leg, double x, double y, double z, uint16_t time);

//Busy/wait loop which moves each legs step as needed, and waits one millisecond for each.
void leg_delay(uint16_t delay);

//Set the function pointer of a callback to be executed after every millisecond of the busy/wait 
// loop.  This function should be where the user interface is polled (serial port, whatever).
void leg_set_polling_callback();

#endif