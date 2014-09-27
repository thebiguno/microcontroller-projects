#include "processing.h"

static volatile uint8_t power_change_required = 0x00;
static volatile uint8_t move_required = 0x00;
static volatile uint8_t turn_required = 0x00;

static volatile double desired_move_heading;
static volatile double desired_move_angle;
static volatile double desired_move_velocity;
static volatile uint16_t desired_move_distance;

static volatile double desired_turn_heading;
static volatile double desired_turn_velocity;

extern Leg legs[LEG_COUNT];

void processing_command_executor(){
	wdt_reset();
	
	if (power_change_required){
		if (get_power()){
			pwm_start();
			status_set_color(0x00, 0xFF, 0x00);
			doResetLegs();
			status_enable_battery();
		}
		else {
			status_disable_battery();
			status_set_color(0x00, 0x00, 0x00);
			_delay_ms(200);

			pwm_stop();
		}
		power_change_required = 0x00;
	}
	
	if (move_required){
		double current_heading = magnetometer_read_heading();
		double veer_correction = (desired_move_heading - current_heading);

		//In the current implementation of gait_tripod, we move 5mm with each iteration of the
		// step procedure at maximum velocity (and the distance scales linearly with velocity).
		//Note: Through experimentation we find that each step is slightly smaller than 5mm... not 
		// sure if this is due to slippage, bad measurements, not enough timing, or something else.
		// Regardless, by making this number smaller, we end up with the right measurement in real
		// world applications.  Yay for fudge!
		#define STEP_DISTANCE 4.5
		
		static int8_t step_index = 0;
		
		if (desired_move_distance < (STEP_DISTANCE * desired_move_velocity)){
			move_required = 0x00;
			veer_correction = 0;
			doResetLegs();
			doCompleteCommand(MESSAGE_REQUEST_MOVE);
		}
		else {
			if (step_index == 0){
				char temp[64];
				sprintf(temp, "current: %1.3f, desired: %1.3f, diff: %1.3f", current_heading, desired_move_heading, veer_correction);
				doSendDebug(temp, 64);
			}

			for (uint8_t l = 0; l < LEG_COUNT; l++){
				Point step = gait_step(legs[l], step_index, desired_move_velocity, desired_move_angle, veer_correction);
				legs[l].setOffset(step);
			}
			step_index++;
			if (step_index > gait_step_count()){
				step_index = 0;
			}
			
			pwm_apply_batch();
			_delay_ms(3);
			
			desired_move_distance -= (STEP_DISTANCE * desired_move_velocity);
		}
	}
	
	if (turn_required){
		static int8_t step_index = 0;
		double current_heading = magnetometer_read_heading();
		
		if (fabs(normalize_angle(desired_turn_heading - current_heading)) <= 0.04){
			turn_required = 0x00;
			doResetLegs();
			doCompleteCommand(MESSAGE_REQUEST_MOVE);
		}
		else {
			if (step_index == 0){
				char temp[64];
				sprintf(temp, "current: %1.3f, desired: %1.3f, diff: %1.3f", current_heading, desired_turn_heading, fabs(normalize_angle(desired_turn_heading - current_heading)));
				doSendDebug(temp, 64);
			}

			for (uint8_t l = 0; l < LEG_COUNT; l++){
				Point step = gait_step(legs[l], step_index, 0, 0, desired_turn_velocity);
				legs[l].setOffset(step);
			}
			step_index++;
			if (step_index > gait_step_count()){
				step_index = 0;
			}
		}
		
		pwm_apply_batch();
		_delay_ms(5);
	}
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
	else if (cmd == MESSAGE_REQUEST_MOVE){
		if (length == 4){
			move_required = 0x01;
			desired_move_angle = convert_byte_to_radian(message[0]);
			desired_move_velocity = message[1] / 255.0;
			desired_move_distance = message[2] << 8 | message[3];
			
			desired_move_heading = magnetometer_read_heading();
			
			doAcknowledgeCommand(MESSAGE_REQUEST_MOVE);
		}
	}
	else if (cmd == MESSAGE_REQUEST_TURN){
		if (length == 2){
			turn_required = 0x01;
			double current_heading = magnetometer_read_heading();
			double desired_angle = convert_byte_to_radian(message[0]);
			
			desired_turn_heading = normalize_angle(desired_angle + current_heading);
			desired_turn_velocity = message[1] / 255.0;
			if (desired_angle < 0) desired_turn_velocity *= -1;
			
			doAcknowledgeCommand(MESSAGE_REQUEST_TURN);
		}
	}
}