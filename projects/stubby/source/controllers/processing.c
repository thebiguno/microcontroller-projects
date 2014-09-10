#include "processing.h"

static volatile uint8_t power_change_required = 0x00;
static volatile uint8_t movement_required = 0x00;
static volatile uint8_t movement_ack_required = 0x00;
static volatile double desired_linear_angle;
static volatile double desired_rotational_angle;
static volatile double desired_linear_velocity;
static volatile double desired_rotational_velocity;
static volatile uint16_t desired_distance = 0;

extern Leg legs[LEG_COUNT];

void processing_command_executor(){
	wdt_reset();
	
	if (power_change_required){
		if (get_power()){
			doAcknowledgeCommand(POWER_ON);
			pwm_start();
			status_set_color(0x00, 0xFF, 0x00);
			doResetLegs();
			status_enable_battery();
		}
		else {
			doAcknowledgeCommand(POWER_OFF);
			status_disable_battery();
			doResetLegs();
			status_set_color(0x00, 0x00, 0x00);
			_delay_ms(200);

			pwm_stop();
		}
		power_change_required = 0x00;
	}
	
	if (movement_ack_required){
		doAcknowledgeCommand(MESSAGE_REQUEST_MOVE);
		movement_ack_required = 0x00;
	}
	
	if (movement_required){
		if (desired_distance > 0){
			uint8_t step_distance = doMove(desired_linear_angle, desired_linear_velocity, desired_rotational_velocity);
			if (step_distance < desired_distance){
				desired_distance -= step_distance;
			}
			else {
				desired_distance = 0;
				movement_required = 0x00;
				doResetLegs();
				doCompleteCommand(MESSAGE_REQUEST_MOVE);
			}
		}
		else if (desired_rotational_angle > 0.01 || desired_rotational_angle < -0.01){
			
		}
	}
}

void processing_dispatch_message(uint8_t cmd, uint8_t *message, uint8_t length){
	if (cmd == MESSAGE_REQUEST_POWER_ON){
		set_power(POWER_ON);
		power_change_required = 0x01;
	}
	else if (cmd == MESSAGE_REQUEST_POWER_OFF){
		set_power(POWER_OFF);
		power_change_required = 0x01;
	}
	else if (cmd == MESSAGE_REQUEST_MOVE){
		if (length == 2){		//Only linear angle and rotational angle
			movement_required = 0x01;
			desired_linear_angle = convert_byte_to_radian(message[0]);
			desired_rotational_angle = convert_byte_to_radian(message[1]);
			desired_linear_velocity = 1.0;
			desired_rotational_velocity = 0.0;		//TODO 
			desired_distance = 0xFFFF;
		}
		else if (length == 4){	//Previous 2 arguments plus linear / rotational velocity
			movement_required = 0x01;
			desired_linear_angle = convert_byte_to_radian(message[0]);
			desired_rotational_angle = convert_byte_to_radian(message[1]);
			desired_linear_velocity = message[2] / 255.0;
			desired_rotational_velocity = message[3] / 255.0;
			desired_distance = 0xFFFF;
		}
		else if (length == 6){	//Previous 4 arguments plus distance (2 bytes)
			movement_required = 0x01;
			desired_linear_angle = convert_byte_to_radian(message[0]);
			desired_rotational_angle = convert_byte_to_radian(message[1]);
			desired_linear_velocity = message[2] / 255.0;
			desired_rotational_velocity = message[3] / 255.0;
			desired_distance = message[4] << 8 | message[5];
		}
		movement_ack_required = 0x01;
	}
}