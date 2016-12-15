#include "motor.h"
#include "../Chiindii.h"

static uint32_t maxPwm = 0;

//Defined in CubeMX code
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim5;

/*
 * We assign MOTOR1 through MOTOR4 to OCR1B, OCR1C, OCR3B, OCR3C respectively
 */
void motor_start() {
	//Set all values to low
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);

	//Turn on PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

	maxPwm = htim1.Init.Period;
}

void motor_stop(){
	//Set all values to low
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, 0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, 0);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);

	//Turn off PWM
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
}

void motor_set(float* motorValues){
	uint16_t pwm[8];
	for(uint8_t i = 0; i < 8; i++){
		pwm[i] = motorValues[i] * 1024;
		if (pwm[i] >= 1024){
			pwm[i] = 1023;
		}
	}

	//Set all values according to hardware
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, pwm[2]);		//Motor 3
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, pwm[6]);		//Motor 7
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, pwm[5]);		//Motor 6
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, pwm[1]);		//Motor 2
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, pwm[0]);		//Motor 1
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, pwm[4]);		//Motor 5
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_3, pwm[7]);		//Motor 8
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, pwm[3]);		//Motor 4
}
