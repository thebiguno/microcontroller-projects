#ifndef HEX_LEG
#define HEX_LEG

#define __DELAY_BACKWARD_COMPATIBLE__ 

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "stubby.h"

typedef struct leg_t {
	volatile uint8_t *port[JOINT_COUNT];
	uint8_t pin[JOINT_COUNT];

	int16_t offset[JOINT_COUNT];
	int8_t direction[JOINT_COUNT];
	
	//The current angle in radians
	double current[JOINT_COUNT];
	
	//The desired angle in radians
	double desired[JOINT_COUNT];
	
	//The step in radians to be moved each iteration through the busy/wait loop
	double step[JOINT_COUNT];
} leg_t;

#include "servo.h"

#define FRONT_LEFT		0
#define FRONT_RIGHT		1
#define MIDDLE_LEFT		2
#define MIDDLE_RIGHT	3
#define REAR_LEFT		4
#define REAR_RIGHT		5

#define COXA			0
//#define FEMUR			1
#define TIBIA			1

//The maximum angle which we can move away from zero.  If you try to set a value higher (lower 
// for negatives) than this, we cap it at this value.
#define MAX_ANGLE		1.6

//The time (millis) during which we pause in leg_delay_ms.
#define DELAY_STEP		5

//The minimum allowable step (in radians).  Also the bottom edge of angular resolution available when 
// setting desired position (once the error falls below this, we consider it 'good enough').
#define MIN_STEP		0.001

//Position of raised / lowered tibia
#define TIBIA_LOWERED -MAX_ANGLE
#define TIBIA_RAISED MAX_ANGLE
//Position of nautral / forward / reverse coxa
#define COXA_NEUTRAL 0
#define COXA_FORWARD -1.0
#define COXA_REVERSE 1.0


//Initialize memory for an array of legs.  By convention, the resulting array is ordered
// from left to right, front to back.  I.e. Left Front, Right Front, Left Middle, Right Middle, etc. 
//The implementation of leg_init MUST be hardware-specific, as part of the init is port / pin 
// combinations for the servo drivers.
void leg_init();

//Immediately set the leg position in X,Y,Z co-ordinate space.
//TODO For a 2 DOF leg, having an X,Y,Z co-ordinate space doesn't really make sense, as there
// is only the partial shell of a sphere that the foot can travel.  For now, we only specify
// Y and Z, and these are radian values which are passed through as-is to the servos.
void leg_set_current_position_absolute(uint8_t leg, double x, double y, double z);

//Immediately set the leg position in X,Y,Z co-ordinate space, relative to current position.
//TODO For a 2 DOF leg, having an X,Y,Z co-ordinate space doesn't really make sense, as there
// is only the partial shell of a sphere that the foot can travel.  For now, we only specify
// Y and Z, and these are radian values which are passed through as-is to the servos.
void leg_set_current_position_relative(uint8_t leg, double x, double y, double z);

//Set the desired leg position in X,Y,Z co-ordinate space, with a step (in radians).  The current
// state is incremented by the step amount repeatedly when looping through leg_delay until 
// the two numbers are the same.  The smaller the step value, the slower the servo will change
// position.
//TODO For a 2 DOF leg, having an X,Y,Z co-ordinate space doesn't really make sense, as there
// is only the partial shell of a sphere that the foot can travel.  For now, we only specify
// Y and Z, and these are radian values which are passed through as-is to the servos.
void leg_set_desired_position_absolute(uint8_t leg, double x, double y, double z, uint16_t millis);

//Set the leg position in X,Y,Z co-ordinate space, relative to current position, with a 
// step (in radians).  The current state is incremented by the step amount repeatedly when 
// looping through leg_delay until the two numbers are the same.  The smaller the step value, 
// the slower the servo will change position.
//TODO For a 2 DOF leg, having an X,Y,Z co-ordinate space doesn't really make sense, as there
// is only the partial shell of a sphere that the foot can travel.  For now, we only specify
// Y and Z, and these are radian values which are passed through as-is to the servos.
void leg_set_desired_position_relative(uint8_t leg, double x, double y, double z, uint16_t millis);

//Busy / wait loop which moves each legs step as needed and injects delays.
// TODO poll serial port here as well?
void leg_delay_ms(uint16_t millis);

#endif
