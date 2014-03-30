#include "wave.h"

/**
 * Take a wave gait step with the specified leg.  All other legs will move backwards a small amount.  The specified leg should 
 * be the index to the coxa PWM array; the associated femur is implied as coxa + 1.
 */
static void wave_gait(uint8_t leg){
	//Lift tibia to absolute position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (WAVE_TIBIA_RAISED * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);
	
	//Move the coxa forward to neutral + 3x STEP...
	leg_position[leg] = NEUTRAL + leg_neutral_offset[leg] + (WAVE_COXA_STEP * leg_rotation_direction[leg]);
	//... while moving the other coxas back by STEP.
	for (uint8_t i = 0; i < 12; i+=2){
		if (i != leg){
			leg_position[i] -= WAVE_COXA_STEP * leg_rotation_direction[i];
		}
	}
	update_leg_position(DELAY_MEDIUM);

	//Drop tibia to neutral position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1];
	update_leg_position(DELAY_SHORT);
}


void wave_step(){
	wave_gait(REAR_LEFT);
	wave_gait(MIDDLE_LEFT);
	wave_gait(FRONT_LEFT);
	
	wave_gait(REAR_RIGHT);
	wave_gait(MIDDLE_RIGHT);
	wave_gait(FRONT_RIGHT);
}


void wave_gait_init(){
	lift_and_position_leg(REAR_LEFT, WAVE_COXA_STEP * -2, 0);
	lift_and_position_leg(MIDDLE_LEFT, WAVE_COXA_STEP * -1, 0);
	lift_and_position_leg(FRONT_LEFT, WAVE_COXA_STEP * 0, 0);
	lift_and_position_leg(REAR_RIGHT, WAVE_COXA_STEP * 1, 0);
	lift_and_position_leg(MIDDLE_RIGHT, WAVE_COXA_STEP * 2, 0);
	lift_and_position_leg(FRONT_RIGHT, WAVE_COXA_STEP * 3, 0);
}