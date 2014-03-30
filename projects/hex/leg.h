#ifndef HEX_LEG
#define HEX_LEG

#define __DELAY_BACKWARD_COMPATIBLE__ 

#include <avr/io.h>
#include <avr/io.h>
#include <util/delay.h>

#include "hex.h"

//Front legs
#define FRONT_LEFT		0
#define FL_COXA			0
#define FL_TIBIA		1
#define FRONT_RIGHT		2
#define FR_COXA			2
#define FR_TIBIA		3

//Middle legs
#define MIDDLE_LEFT		4
#define ML_COXA			4
#define ML_TIBIA		5
#define MIDDLE_RIGHT	6
#define MR_COXA			6
#define MR_TIBIA		7

//Rear legs
#define REAR_LEFT		8
#define RL_COXA			8
#define RL_TIBIA		9
#define REAR_RIGHT		10
#define RR_COXA			10
#define RR_TIBIA		11

#define TIBIA_RAISED	600

#define DELAY_SHORT			60
#define DELAY_MEDIUM		100

extern uint16_t leg_position[12];
extern int16_t leg_neutral_offset[12];
extern int8_t leg_rotation_direction[12];

void lift_and_position_leg(uint8_t leg, int16_t coxa_offset, int16_t tibia_offset);
void update_leg_position(uint16_t delay);
void leg_init();


#endif