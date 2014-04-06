#include "ripple.h"

static double velocity;
static double direction;

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
	//Lift tibia (z)
	leg_set_current_position_relative(leg, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY / 4);
	
	//Move the coxa forward to neutral
	leg_set_current_position_absolute(leg, 0, 0, TIBIA_STEP);
	leg_delay_ms(DELAY * 2);
	
	//... while moving the other coxas back by STEP.
	for (uint8_t i = 0; i < LEG_COUNT; i++){
		if (i != leg){
			double coxa_step = COXA_STEP * -1;
			if ((i & 0x01) == 0x00){	//left side
				if (direction < -0.5) coxa_step = coxa_step / 2;
				leg_set_current_position_relative(i, 0, coxa_step, 0);
			}
			else {	//right side
				if (direction > 0.5) coxa_step = coxa_step / 2;
				leg_set_current_position_relative(i, 0, coxa_step, 0);
			}
		}
	}
	leg_delay_ms(DELAY / 4);

	//Drop tibia to neutral position
	leg_set_current_position_absolute(leg, 0, 0, 0);
	leg_delay_ms(DELAY / 4);
}

void ripple_step(){
	comm_read(&velocity, &direction);
	
	if (velocity > 0.1) {
		static uint8_t i = 0x00;
		ripple_gait(gait_leg_order[i++]);
		if (i >= LEG_COUNT) i = 0;
	}
	else {
		leg_delay_ms(DELAY);
	}
}