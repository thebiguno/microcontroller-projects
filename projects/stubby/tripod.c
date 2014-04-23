#include "gait.h"
#define DELAY 		80

/**
 * Take a tripod gait step with the specified index.
 */
void gait_step(double velocity, double direction){
	static uint8_t i = 0x00;

	//Alternate between 0 and 1.  When 0, we move left front + rear and right middle; when 1 
	// we move right front and rear and left middle.
	i ^= 0x01;
	
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
	
	//leg_a_X are the legs which are being lifted and moved; leg_b_X are the legs which move without lifting, thus propelling the robot.
	uint8_t leg_a_0, leg_a_1, leg_a_2, leg_b_0, leg_b_1, leg_b_2;
	if (i == 0){
		leg_a_0 = FRONT_LEFT;
		leg_a_1 = MIDDLE_RIGHT;
		leg_a_2 = REAR_LEFT;
		
		leg_b_0 = FRONT_RIGHT;
		leg_b_1 = MIDDLE_LEFT;
		leg_b_2 = REAR_RIGHT;
	}
	else {
		leg_a_0 = FRONT_RIGHT;
		leg_a_1 = MIDDLE_LEFT;
		leg_a_2 = REAR_RIGHT;

		leg_b_0 = FRONT_LEFT;
		leg_b_1 = MIDDLE_RIGHT;
		leg_b_2 = REAR_LEFT;
	}
	
	//Lift tibia (z)
	leg_set_current_position_relative(leg_a_0, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(leg_a_1, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(leg_a_2, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY * 2);
	
	//Which leg are we controlling: right or left?
	double relevant_direction_a_0 = ((leg_a_0 & 0x01) == 0x00) ? left_direction : right_direction;
	double relevant_direction_a_1 = ((leg_a_1 & 0x01) == 0x00) ? left_direction : right_direction;
	double relevant_direction_a_2 = ((leg_a_2 & 0x01) == 0x00) ? left_direction : right_direction;

	double relevant_direction_b_0 = ((leg_b_0 & 0x01) == 0x00) ? left_direction : right_direction;
	double relevant_direction_b_1 = ((leg_b_1 & 0x01) == 0x00) ? left_direction : right_direction;
	double relevant_direction_b_2 = ((leg_b_2 & 0x01) == 0x00) ? left_direction : right_direction;
	
	//Move the coxa forward to starting position if we are moving forward; if we are moving backward, move it to the reverse position.
	leg_set_current_position_absolute(leg_a_0, 0, relevant_direction_a_0 > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_RAISED);
	leg_set_current_position_absolute(leg_a_1, 0, relevant_direction_a_1 > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_RAISED);
	leg_set_current_position_absolute(leg_a_2, 0, relevant_direction_a_2 > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_RAISED);
	
	//... while moving the other coxas back or forward by STEP.
	leg_set_current_position_absolute(leg_b_0, 0, relevant_direction_b_0 < 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_LOWERED);
	leg_set_current_position_absolute(leg_b_1, 0, relevant_direction_b_1 < 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_LOWERED);
	leg_set_current_position_absolute(leg_b_2, 0, relevant_direction_b_2 < 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_LOWERED);
	leg_delay_ms(DELAY * 2);

	//Drop tibia to neutral position
	leg_set_current_position_absolute(leg_a_0, 0, relevant_direction_a_0 > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_LOWERED);
	leg_set_current_position_absolute(leg_a_1, 0, relevant_direction_a_1 > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_LOWERED);
	leg_set_current_position_absolute(leg_a_2, 0, relevant_direction_a_2 > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_LOWERED);
	leg_delay_ms(DELAY * 1);
}

void gait_init(){
	leg_set_current_position_relative(FRONT_LEFT, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(MIDDLE_RIGHT, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(REAR_LEFT, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY * 2);
	leg_set_current_position_absolute(FRONT_LEFT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_set_current_position_absolute(MIDDLE_RIGHT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_set_current_position_absolute(REAR_LEFT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_delay_ms(DELAY * 2);
	leg_set_current_position_absolute(FRONT_LEFT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_set_current_position_absolute(MIDDLE_RIGHT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_set_current_position_absolute(REAR_LEFT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_delay_ms(DELAY * 1);

	leg_set_current_position_relative(FRONT_RIGHT, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(MIDDLE_LEFT, 0, 0, TIBIA_STEP);
	leg_set_current_position_relative(REAR_RIGHT, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY * 2);
	leg_set_current_position_absolute(FRONT_RIGHT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_set_current_position_absolute(MIDDLE_LEFT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_set_current_position_absolute(REAR_RIGHT, 0, COXA_NEUTRAL, TIBIA_RAISED);
	leg_delay_ms(DELAY * 2);
	leg_set_current_position_absolute(FRONT_RIGHT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_set_current_position_absolute(MIDDLE_LEFT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_set_current_position_absolute(REAR_RIGHT, 0, COXA_NEUTRAL, TIBIA_LOWERED);
	leg_delay_ms(DELAY * 1);
}
