#include "processing.h"

extern volatile uint8_t debug;

static volatile uint8_t heading_confirmation_required = 0x00;
static volatile uint8_t power_change_required = 0x00;
static volatile uint8_t move_required = 0x00;
static volatile uint8_t turn_required = 0x00;
static volatile uint8_t translation_required = 0x00;

static volatile double desired_move_heading;
static volatile double desired_move_angle;
static volatile double desired_move_velocity;
static volatile double desired_move_distance;

static volatile double desired_turn_heading;
static volatile double desired_turn_angle;
static volatile double desired_turn_velocity;

static volatile int8_t bodyTranslationX = 0;
static volatile int8_t bodyTranslationY = 0;
static volatile int8_t bodyTranslationZ = 0;

//In the current implementation of gait_tripod, we move 5mm with each iteration of the
// step procedure at maximum velocity (and the distance scales linearly with velocity).
//Note: Through experimentation we find that each step is slightly smaller than 5mm... not 
// sure if this is due to slippage, bad measurements, not enough timing, or something else.
// Regardless, by making this number smaller, we end up with the right measurement in real
// world applications.  Yay for fudge!
#define STEP_DISTANCE					4.0

#define VEER_CORRECTION_MULTIPLIER		5
#define VEER_CORRECTION_EXPONENT		3

#define HEADING_MARGIN_OF_ERROR			0.5

extern Leg legs[LEG_COUNT];

void processing_command_executor(){
	wdt_reset();
	
	if (heading_confirmation_required){
		static uint8_t count = 0;
		static double last_heading = 0;
		
		double heading = magnetometer_read_heading();
		if (fabs(heading - last_heading) < HEADING_MARGIN_OF_ERROR){
			count++;
		}
		else {
			last_heading = heading;
			count = 0;
		}
		
		if (count >= 5){
			if (move_required){
				desired_move_heading = heading;
			}
			if (turn_required){
				desired_turn_heading = normalize_angle(desired_turn_angle + heading);
			}

			if (debug){
				char temp[64];
				sprintf(temp, "Confirmed heading: %1.3f", heading);
				doSendDebug(temp, 64);
			}

			
			heading_confirmation_required = 0x00;
			count = 0;
			last_heading = 0;
		}
		
		delay_ms(50);
	}
	else {
		if (power_change_required){
			if (get_power()){
				pwm_start();
				status_set_color(0x00, 0xFF, 0x00);
				doResetLegs();
				battery_enable_status();
			}
			else {
				battery_disable_status();
				status_set_color(0x00, 0x00, 0x00);
				delay_ms(200);

				pwm_stop();
			}
			power_change_required = 0x00;
		}
		
		if (move_required){
			double current_heading = magnetometer_read_heading();
			double veer_correction = difference_between_angles(desired_move_heading, current_heading);
			veer_correction = pow(veer_correction * VEER_CORRECTION_MULTIPLIER, VEER_CORRECTION_EXPONENT);
			if (veer_correction > 0.5) veer_correction = 0.5;
			else if (veer_correction < -0.5) veer_correction = -0.5;

			static uint8_t step_index = 0;
			
			if (desired_move_distance < (STEP_DISTANCE * desired_move_velocity)){
				move_required = 0x00;
				veer_correction = 0;
				doResetLegs();
				doCompleteCommand(MESSAGE_REQUEST_MOVE);
			}
			else {
				if ((step_index & 0x01) == 0x00){
					char temp[8];
					sprintf(temp, "%1.3f", veer_correction);
					doSendDebug(temp, 8);
				}

				for (uint8_t l = 0; l < LEG_COUNT; l++){
					Point step = gait_step(legs[l], step_index, desired_move_velocity, desired_move_angle, veer_correction);
					step.add(Point(bodyTranslationX, bodyTranslationY, fmin(3, bodyTranslationZ)));
					legs[l].setOffset(step);
				}
				step_index++;
				if (step_index > gait_step_count()){
					step_index = 0;
				}
				
				pwm_apply_batch();
				
				desired_move_distance -= (STEP_DISTANCE * desired_move_velocity);
			}
		}
		else if (turn_required){
			static uint8_t step_index = 0;
			double current_heading = magnetometer_read_heading();
			
			double difference = difference_between_angles(desired_turn_heading, current_heading);
			if (fabs(difference) <= HEADING_MARGIN_OF_ERROR){
				turn_required = 0x00;
				doResetLegs();
				doCompleteCommand(MESSAGE_REQUEST_MOVE);
			}
			else {
				double velocity = fmax(0.1, fmin(desired_turn_velocity, fabs(difference) / 1.3));
				if (difference < 0) velocity *= -1;

				for (uint8_t l = 0; l < LEG_COUNT; l++){
					Point step = gait_step(legs[l], step_index, 0, 0, velocity);
					step.add(Point(bodyTranslationX, bodyTranslationY, fmin(3, bodyTranslationZ)));
					legs[l].setOffset(step);
				}
				if (step_index == 0 && debug){
					char temp[64];
					sprintf(temp, "current: %1.3f, desired: %1.3f, diff: %1.3f", current_heading, desired_turn_heading, difference);
					doSendDebug(temp, 64);
				}
				
				step_index++;
				if (step_index > gait_step_count()){
					step_index = 0;
				}
			}
			
			pwm_apply_batch();
		}
		else if (translation_required){
			translation_required = 0x00;
			
			for (uint8_t l = 0; l < LEG_COUNT; l++){
				legs[l].setOffset(Point(bodyTranslationX, bodyTranslationY, bodyTranslationZ));
			}
			
			pwm_apply_batch();
		}
		

	}
	
#if F_CPU == 12000000
	delay_ms(1);
#elif F_CPU == 20000000
	delay_ms(5);
#else
#warning Non-standard CPU speed.  Please specify a delay.
#endif
}

void processing_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	if (cmd == MESSAGE_REQUEST_POWER_ON){
		set_power(POWER_ON);
		power_change_required = 0x01;
		doAcknowledgeCommand(MESSAGE_REQUEST_POWER_ON);
	}
	else if (cmd == MESSAGE_REQUEST_POWER_OFF){
		set_power(POWER_OFF);
		power_change_required = 0x01;
		doAcknowledgeCommand(MESSAGE_REQUEST_POWER_OFF);
	}
	else if (cmd == MESSAGE_REQUEST_HEADING){
		uint8_t data[1];
		data[0] = convert_radian_to_byte(magnetometer_read_heading());
		protocol_send_message(MESSAGE_SEND_HEADING, data, 1);
	}
	else if (cmd == MESSAGE_REQUEST_DISTANCE){
		uint8_t data[2];
		uint16_t distance = distance_read();
		data[0] = (distance >> 8) & 0xFF;
		data[1] = distance & 0xFF;
		protocol_send_message(MESSAGE_SEND_DISTANCE, data, 2);
	}
	else if (cmd == MESSAGE_REQUEST_MOVE){
		if (length == 4){
			move_required = 0x01;
			heading_confirmation_required = 0x01;
			
			desired_move_angle = convert_byte_to_radian(message[0]);
			desired_move_velocity = message[1] / 255.0;
			desired_move_distance = message[2] << 8 | message[3];
			
			doAcknowledgeCommand(MESSAGE_REQUEST_MOVE);
		}
	}
	else if (cmd == MESSAGE_REQUEST_TURN){
		if (length == 2){
			turn_required = 0x01;
			heading_confirmation_required = 0x01;
			
			desired_turn_angle = convert_byte_to_radian(message[0]);
			desired_turn_velocity = message[1] / 255.0;
			
			doAcknowledgeCommand(MESSAGE_REQUEST_TURN);
		}
	}
	else if (cmd == MESSAGE_REQUEST_TRANSLATE){
		if (length == 3){
			translation_required = 0x01;
			
			bodyTranslationX = message[0];
			bodyTranslationY = message[1];
			bodyTranslationZ = message[2];
			
			doAcknowledgeCommand(MESSAGE_REQUEST_TRANSLATE);
		}
	}
}