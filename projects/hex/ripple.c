#include "ripple.h"

/**
 * Take a ripple gait step with the specified leg.  This is the same as wave gait, but it only affects one side at a time, and 
 * each side is interleaved together.
 */
static void ripple_gait(uint8_t leg){
	//Lift tibia to absolute position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (RIPPLE_TIBIA_RAISED * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);
	
	//Move the coxa forward to neutral
	leg_position[leg] = NEUTRAL + leg_neutral_offset[leg];
	//... while moving the other coxas back by STEP.
	for (uint8_t i = 0; i < 12; i+=2){
		if (i != leg){
			leg_position[i] -= RIPPLE_COXA_STEP * leg_rotation_direction[i];
		}
	}
	update_leg_position(DELAY_SHORT);

	//Drop tibia to neutral position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1];
	update_leg_position(DELAY_SHORT);
}

void ripple_step(){
	ripple_gait(REAR_LEFT);
	ripple_gait(FRONT_RIGHT);
	ripple_gait(MIDDLE_LEFT);
	ripple_gait(REAR_RIGHT);
	ripple_gait(FRONT_LEFT);
	ripple_gait(MIDDLE_RIGHT);
}

void ripple_gait_init(){
	lift_and_position_leg(REAR_LEFT, RIPPLE_COXA_STEP * -5, 0);
	lift_and_position_leg(MIDDLE_LEFT, RIPPLE_COXA_STEP * -3, 0);
	lift_and_position_leg(FRONT_LEFT, RIPPLE_COXA_STEP * -1, 0);
	
	lift_and_position_leg(REAR_RIGHT, RIPPLE_COXA_STEP * -2, 0);
	lift_and_position_leg(MIDDLE_RIGHT, RIPPLE_COXA_STEP * 0, 0);
	lift_and_position_leg(FRONT_RIGHT, RIPPLE_COXA_STEP * -4, 0);
}
