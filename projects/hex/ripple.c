#include "ripple.h"

static uint8_t gait_leg_order[6] = {
	REAR_LEFT,
	FRONT_RIGHT,
	MIDDLE_LEFT,
	REAR_RIGHT,
	FRONT_LEFT,
	MIDDLE_RIGHT
};


static uint16_t get_delay(){
//	return (uint16_t) ((MIN_DELAY - MAX_DELAY) * get_velocity()) + MAX_DELAY;
	return MIN_DELAY;
}

/**
 * Take a ripple gait step with the specified leg.  This is the same as wave gait, but it only affects one side at a time, and 
 * each side is interleaved together.
 */
static void ripple_gait(uint8_t leg){
	//Lift tibia (z)
	leg_set_current_position_relative(leg, 0, 0, TIBIA_STEP);
	leg_delay_ms(get_delay() / 4);
	
	//Move the coxa forward to neutral
	leg_set_current_position_absolute(leg, 0, 0, TIBIA_STEP);
	leg_delay_ms(get_delay() * 2);
	
	//... while moving the other coxas back by STEP.
	for (uint8_t i = 0; i < LEG_COUNT; i++){
		if (i != leg){
			double coxa_step = COXA_STEP * -1;
			if ((i & 0x01) == 0x00){	//left side
				if (get_turn() < -0.5) coxa_step = coxa_step / 2;
				leg_set_current_position_relative(i, 0, coxa_step, 0);
			}
			else {	//right side
				if (get_turn() > 0.5) coxa_step = coxa_step / 2;
				leg_set_current_position_relative(i, 0, coxa_step, 0);
			}
		}
	}
	leg_delay_ms(get_delay() / 4);

	//Drop tibia to neutral position
	leg_set_current_position_absolute(leg, 0, 0, 0);
	leg_delay_ms(get_delay() / 4);
}

void ripple_step(){
	static uint8_t i = 0x00;
	if (get_velocity() > 0.001) ripple_gait(gait_leg_order[i]);
	i++;
	if (i >= LEG_COUNT) i = 0;
}

void ripple_gait_init(){
	leg_set_desired_position_absolute(REAR_LEFT,	0, 0, TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(MIDDLE_RIGHT,	0, 0, TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(FRONT_LEFT,	0, 0, TIBIA_STEP, MIN_DELAY);
	leg_delay_ms(MIN_DELAY);
	leg_set_desired_position_absolute(REAR_LEFT,	0, COXA_STEP * -5,	TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(MIDDLE_RIGHT,	0, COXA_STEP *  0,	TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(FRONT_LEFT,	0, COXA_STEP * -1,	TIBIA_STEP, MIN_DELAY);
	leg_delay_ms(MIN_DELAY);
	leg_set_desired_position_absolute(REAR_LEFT,	0, COXA_STEP * -5, 0, MIN_DELAY);
	leg_set_desired_position_absolute(MIDDLE_RIGHT,	0, COXA_STEP *  0, 0, MIN_DELAY);
	leg_set_desired_position_absolute(FRONT_LEFT,	0, COXA_STEP * -1, 0, MIN_DELAY);
	leg_delay_ms(MIN_DELAY);

	leg_set_desired_position_absolute(REAR_RIGHT,	0, 0, TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(MIDDLE_LEFT,	0, 0, TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(FRONT_RIGHT,	0, 0, TIBIA_STEP, MIN_DELAY);
	leg_delay_ms(MIN_DELAY);
	leg_set_desired_position_absolute(REAR_RIGHT,	0, COXA_STEP * -2,	TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(MIDDLE_LEFT,	0, COXA_STEP * -3,	TIBIA_STEP, MIN_DELAY);
	leg_set_desired_position_absolute(FRONT_RIGHT,	0, COXA_STEP * -4,	TIBIA_STEP, MIN_DELAY);
	leg_delay_ms(MIN_DELAY);
	leg_set_desired_position_absolute(REAR_RIGHT,	0, COXA_STEP * -2,	0, MIN_DELAY);
	leg_set_desired_position_absolute(MIDDLE_LEFT,	0, COXA_STEP * -3,	0, MIN_DELAY);
	leg_set_desired_position_absolute(FRONT_RIGHT,	0, COXA_STEP * -4,	0, MIN_DELAY);
	leg_delay_ms(MIN_DELAY);
}
