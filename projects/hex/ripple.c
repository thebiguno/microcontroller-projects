#include "ripple.h"

/**
 * Take a ripple gait step with the specified leg.  This is the same as wave gait, but it only affects one side at a time, and 
 * each side is interleaved together.
 */
static void ripple_gait(uint8_t leg){
	//Lift tibia (z)
	leg_set_current_position_relative(leg, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY);
	
	//Move the coxa forward to neutral
	leg_set_current_position_absolute(leg, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY);
	
	//... while moving the other coxas back by STEP.
	for (uint8_t i = 0; i < LEG_COUNT; i++){
		if (i != leg){
			leg_set_current_position_relative(i, 0, COXA_STEP * -1, 0);
		}
	}
	leg_delay_ms(DELAY);

	//Drop tibia to neutral position
	leg_set_current_position_absolute(leg, 0, 0, 0);
	leg_delay_ms(DELAY);
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
	leg_set_desired_position_absolute(REAR_LEFT,	0, 0, TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(MIDDLE_RIGHT,	0, 0, TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(FRONT_LEFT,	0, 0, TIBIA_STEP, DELAY);
	leg_delay_ms(DELAY);
	leg_set_desired_position_absolute(REAR_LEFT,	0, COXA_STEP * -5,	TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(MIDDLE_RIGHT,	0, COXA_STEP *  0,	TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(FRONT_LEFT,	0, COXA_STEP * -1,	TIBIA_STEP, DELAY);
	leg_delay_ms(DELAY);
	leg_set_desired_position_absolute(REAR_LEFT,	0, COXA_STEP * -5, 0, DELAY);
	leg_set_desired_position_absolute(MIDDLE_RIGHT,	0, COXA_STEP *  0, 0, DELAY);
	leg_set_desired_position_absolute(FRONT_LEFT,	0, COXA_STEP * -1, 0, DELAY);
	leg_delay_ms(DELAY);

	leg_set_desired_position_absolute(REAR_RIGHT,	0, 0, TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(MIDDLE_LEFT,	0, 0, TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(FRONT_RIGHT,	0, 0, TIBIA_STEP, DELAY);
	leg_delay_ms(DELAY);
	leg_set_desired_position_absolute(REAR_RIGHT,	0, COXA_STEP * -2,	TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(MIDDLE_LEFT,	0, COXA_STEP * -3,	TIBIA_STEP, DELAY);
	leg_set_desired_position_absolute(FRONT_RIGHT,	0, COXA_STEP * -4,	TIBIA_STEP, DELAY);
	leg_delay_ms(DELAY);
	leg_set_desired_position_absolute(REAR_RIGHT,	0, COXA_STEP * -2,	0, DELAY);
	leg_set_desired_position_absolute(MIDDLE_LEFT,	0, COXA_STEP * -3,	0, DELAY);
	leg_set_desired_position_absolute(FRONT_RIGHT,	0, COXA_STEP * -4,	0, DELAY);
	leg_delay_ms(DELAY);
}
