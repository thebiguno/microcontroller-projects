#include "gait.h"
#define DELAY 		80
#define COXA_STEP (COXA_FORWARD - COXA_REVERSE) / 5


#define PRIMARY_CUTOFF 0.3
#define SECONDARY_CUTOFF 0.2

//The order in which we lift legs
static uint8_t gait_leg_order[6] = {
	REAR_LEFT,
	FRONT_RIGHT,
	MIDDLE_LEFT,
	REAR_RIGHT,
	FRONT_LEFT,
	MIDDLE_RIGHT
};

static inline double get_delay_multiplier(double *velocity, double *direction, uint8_t measurement){
	
	comm_read(velocity, direction);

	double value = 0;
	if (measurement == 0) value = fabs(*velocity);
	else if (measurement == 1) value = fabs(*direction);
	
	if (value >= 0.8) return 1;
	else if (value >= 0.7) return 1.2;
	else if (value >= 0.6) return 1.4;
	else return 1.8;
}

void ripple_gait_step(){
	static uint8_t i = 0x00;

	//Movement variables
	double velocity;
	double direction;
	
	//We control the direction of legs on each side based on the combination of velocity and direction values.
	double left_direction;
	double right_direction;
	double delay_multiplier;	//The larger the multiplier, the slower it goes
	double delay;				//Temp variable to avoid multiple re-calculations
	
	uint8_t measurement;
	
	comm_read(&velocity, &direction);
	
	uint8_t leg = gait_leg_order[i];
	if (direction > 0){
		i++;
		if (i >= LEG_COUNT) i = 0;
	}
	else {
		i--;
		if (i >= LEG_COUNT) i = LEG_COUNT - 1;	//i is unsigned, so will loop to 255.
	}
	
	//Base case - no direction component, velocity only.  Set each side to forward / backward motion
	if (fabs(velocity) >= PRIMARY_CUTOFF && fabs(direction) < SECONDARY_CUTOFF){
		left_direction = velocity;
		right_direction = velocity;
		
		measurement = 0;
	}
	//Velocity is greater than direction, but both are present
	else if (fabs(velocity) >= PRIMARY_CUTOFF && fabs(direction) >= SECONDARY_CUTOFF && fabs(velocity) > fabs(direction)){
		if (direction < 0) {	//Veer left
			right_direction = velocity;
			left_direction = velocity + (velocity > 0 ? 1 : -1) * fmin(1, (direction * 2));
		}
		else {	//Veer right
			right_direction = velocity - (velocity > 0 ? 1 : -1) * fmin(1, (direction * 2));
			left_direction = velocity;
		}
		measurement = 0;
	}
	//Direction is greater than velocity, but both are present
	else if (fabs(direction) >= PRIMARY_CUTOFF && fabs(velocity) >= SECONDARY_CUTOFF && fabs(direction) > fabs(velocity)){
		if (velocity < 0) {	//Veer left
			right_direction = direction * -1 + velocity;
			left_direction = direction;
		}
		else {	//Veer right
			right_direction = direction * -1 - velocity;
			left_direction = direction;
		}
		measurement = 1;
	}
	//If there is only a direction component, then just turn in place.
	else if (fabs(direction) >= PRIMARY_CUTOFF && fabs(velocity) < SECONDARY_CUTOFF){
		right_direction = direction * -1;
		left_direction = direction;
		
		measurement = 1;
	}
	else {
		//Everything is 0
		return;
	}
	
	//Lift tibia (z)
	delay_multiplier = get_delay_multiplier(&velocity, &direction, measurement);
	delay = DELAY * 2 * delay_multiplier;
	leg_set_desired_position_relative(leg, 0, 0, TIBIA_STEP, delay * 1.5);
	leg_delay_ms(delay);
	
	//Which leg are we controlling: right or left?
	double relevant_direction = ((leg & 0x01) == 0x00) ? left_direction : right_direction;
	
	//Move the coxa forward to starting position if we are moving forward; if we are moving backward, move it to the reverse position.
	delay_multiplier = get_delay_multiplier(&velocity, &direction, measurement);
	delay = DELAY * 2 * delay_multiplier;
	leg_set_desired_position_absolute(leg, 0, relevant_direction > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_RAISED, delay * 1.5);
	
	//... while moving the other coxas back or forward; how much depends on velocity and direction.
	// Here we assume that COXA_REVERSE == COXA_FORWARD * -1; in the event that this is not the case, you will
	// have to handle each case separately.
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		if (l != leg){
			leg_set_desired_position_absolute(i, 0, COXA_STEP * ((l & 0x01) == 0x00 ? left_direction : right_direction), TIBIA_LOWERED, delay);
		}
	}	
	leg_delay_ms(delay);

	//Drop tibia to neutral position
	delay_multiplier = get_delay_multiplier(&velocity, &direction, measurement);
	delay = DELAY * 1 * delay_multiplier;
	leg_set_desired_position_absolute(leg, 0, relevant_direction > 0 ? COXA_REVERSE : COXA_FORWARD, TIBIA_LOWERED, delay * 1.2);
	leg_delay_ms(delay);
}