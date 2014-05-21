#include "gait.h"
#define DELAY 		80

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
