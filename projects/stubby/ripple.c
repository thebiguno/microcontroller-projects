#include "ripple.h"

static double velocity;
static double direction;
static uint8_t special;
static uint8_t last_special;

//The order in which we lift legs
static uint8_t gait_leg_order[6] = {
	REAR_LEFT,
	FRONT_RIGHT,
	MIDDLE_LEFT,
	REAR_RIGHT,
	FRONT_LEFT,
	MIDDLE_RIGHT
};

/**
 * Take a ripple gait step with the specified leg.  This is the same as wave gait, but it only affects one side at a time, and 
 * each side is interleaved together.
 */
static void ripple_gait(uint8_t leg){
	//We control the direction of legs on each side based on the combination of velocity and direction values.
	double left_direction;
	double right_direction;
	
	//Base case - no direction component, velocity only.  Set each side to forward / backward motion
	if (fabs(direction) < 0.1){
		left_direction = velocity;
		right_direction = velocity;
	}
	//If there are both direction and velocity components, then still move forward / backward but veer.
	else if (fabs(direction) >= 0.1 && fabs(velocity) >= 0.1){
		if (direction < 0) {	//Veer left
			right_direction = velocity;
			left_direction = velocity + direction;
		}
		else {	//Veer right
			right_direction = velocity - direction;
			left_direction = velocity;
		}
	}
	//If there is only a direction component, then just turn in place.
	else {
		right_direction = direction * -1;
		left_direction = direction;
	}
	
	//Lift tibia (z)
	leg_set_current_position_relative(leg, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY * 2);
	
	//Which leg are we controlling: right or left?
	double relevant_direction = ((leg & 0x01) == 0x00) ? left_direction : right_direction;
	
	//Move the coxa forward to starting position if we are moving forward; if we are moving backward, move it to the reverse position.
	double starting_position = relevant_direction > 0 ? COXA_REVERSE : COXA_FORWARD;
	leg_set_current_position_relative(leg, 0, starting_position, 0);
	//leg_delay_ms(DELAY * 2);
	
	//... while moving the other coxas back or forward by STEP.
	for (uint8_t i = 0; i < LEG_COUNT; i++){
		if (i != leg){
			double coxa_step;
			if ((i & 0x01) == 0x00) {	//this is an even leg, therefore the left side
				coxa_step = left_direction * COXA_STEP;
			}
			else {
				coxa_step = right_direction * COXA_STEP;
			}
			leg_set_current_position_relative(i, 0, coxa_step, 0);
		}
	}
	leg_delay_ms(DELAY * 1);

	//Drop tibia to neutral position
	leg_set_current_position_absolute(leg, 0, starting_position, TIBIA_LOWERED);
	leg_delay_ms(DELAY * 1);
}

void ripple_reset(){
	leg_set_current_position_relative(REAR_LEFT, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(FRONT_RIGHT, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY / 2);
	leg_set_current_position_absolute(REAR_LEFT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_set_current_position_absolute(FRONT_RIGHT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_delay_ms(DELAY * 2);
	leg_set_current_position_absolute(REAR_LEFT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_set_current_position_absolute(FRONT_RIGHT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_delay_ms(DELAY / 4);

	leg_set_current_position_relative(MIDDLE_LEFT, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(MIDDLE_RIGHT, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY / 2);
	leg_set_current_position_absolute(MIDDLE_LEFT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_set_current_position_absolute(MIDDLE_RIGHT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_delay_ms(DELAY * 2);
	leg_set_current_position_absolute(MIDDLE_LEFT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_set_current_position_absolute(MIDDLE_RIGHT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_delay_ms(DELAY / 4);

	leg_set_current_position_relative(FRONT_LEFT, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(REAR_RIGHT, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY / 2);
	leg_set_current_position_absolute(FRONT_LEFT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_set_current_position_absolute(REAR_RIGHT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_delay_ms(DELAY * 2);
	leg_set_current_position_absolute(FRONT_LEFT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_set_current_position_absolute(REAR_RIGHT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_delay_ms(DELAY / 4);
}

void ripple_step(){
	comm_read(&velocity, &direction, &special);
	
	if ((special & SPECIAL_RESET) && !(last_special & SPECIAL_RESET)){
		ripple_reset();
		last_special |= SPECIAL_RESET;
	}
	else if (velocity < -0.1 || velocity > 0.1 || direction < -0.1 || direction > 0.1) {
		static uint8_t i = 0x00;
		ripple_gait(gait_leg_order[i++]);
		if (i >= LEG_COUNT) i = 0;
		last_special &= ~SPECIAL_RESET;
	}
	else {
		leg_delay_ms(DELAY);
	}
}