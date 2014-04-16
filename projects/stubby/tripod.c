
static void tripod_gait(uint8_t forward1, uint8_t forward2, uint8_t forward3, uint8_t backward1, uint8_t backward2, uint8_t backward3){
	//Move three legs forward (lifting tibias), while moving the others backward
	//Lift three tibias
	leg_position[forward1 + 1] = NEUTRAL + leg_neutral_offset[forward1 + 1] + (TRIPOD_TIBIA_RAISED * leg_rotation_direction[forward1 + 1]);
	leg_position[forward2 + 1] = NEUTRAL + leg_neutral_offset[forward2 + 1] + (TRIPOD_TIBIA_RAISED * leg_rotation_direction[forward2 + 1]);
	leg_position[forward3 + 1] = NEUTRAL + leg_neutral_offset[forward3 + 1] + (TRIPOD_TIBIA_RAISED * leg_rotation_direction[forward3 + 1]);
	update_leg_position(DELAY_MEDIUM);
	
	//Move three legs forward
	leg_position[forward1] = NEUTRAL + leg_neutral_offset[forward1] + (TRIPOD_COXA_STEP * leg_rotation_direction[forward1]);
	leg_position[forward2] = NEUTRAL + leg_neutral_offset[forward2] + (TRIPOD_COXA_STEP * leg_rotation_direction[forward2]);
	leg_position[forward3] = NEUTRAL + leg_neutral_offset[forward3] + (TRIPOD_COXA_STEP * leg_rotation_direction[forward3]);
	
	//Move three legs backward
	leg_position[backward1] = NEUTRAL + leg_neutral_offset[backward1] - (TRIPOD_COXA_STEP * leg_rotation_direction[backward1]);
	leg_position[backward2] = NEUTRAL + leg_neutral_offset[backward2] - (TRIPOD_COXA_STEP * leg_rotation_direction[backward2]);
	leg_position[backward3] = NEUTRAL + leg_neutral_offset[backward3] - (TRIPOD_COXA_STEP * leg_rotation_direction[backward3]);
	update_leg_position(DELAY_SHORT);
	
	//Drop tibias
	leg_position[forward1 + 1] = NEUTRAL + leg_neutral_offset[forward1 + 1];
	leg_position[forward2 + 1] = NEUTRAL + leg_neutral_offset[forward2 + 1];
	leg_position[forward3 + 1] = NEUTRAL + leg_neutral_offset[forward3 + 1];
	update_leg_position(DELAY_MEDIUM);
}

//Position the legs in a correct starting position for tripod gait
void tripod_gait_init(){
	
}

void tripod_step(){
	tripod_gait(FRONT_LEFT, MIDDLE_RIGHT, REAR_LEFT, FRONT_RIGHT, MIDDLE_LEFT, REAR_RIGHT);
	tripod_gait(FRONT_RIGHT, MIDDLE_LEFT, REAR_RIGHT, FRONT_LEFT, MIDDLE_RIGHT, REAR_LEFT);	
}