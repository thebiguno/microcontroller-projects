#include "motor.h"
#include "../Chiindii.h"

// volatile uint8_t motorPinMask1 = 0;
// volatile uint8_t motorPinMask2 = 0;
// volatile uint8_t motorPinMask3 = 0;
// volatile uint8_t motorPinMask4 = 0;

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

void motor_set(double motorValue1, double motorValue2, double motorValue3, double motorValue4, double motorValue5, double motorValue6, double motorValue7, double motorValue8){
	// if (motorValue1 > PWM_MAX) motorValue1 = PWM_MAX;
	// if (motorValue2 > PWM_MAX) motorValue2 = PWM_MAX;
	// if (motorValue3 > PWM_MAX) motorValue3 = PWM_MAX;
	// if (motorValue4 > PWM_MAX) motorValue4 = PWM_MAX;
	//
	// OCR1B = motorValue1;
	// OCR1C = motorValue2;
	// OCR3B = motorValue3;
	// OCR3C = motorValue4;
	//
	// //Anything less than PWM_MIN turns the motors off completely; this is so that we don't miss
	// // low values when the compares fire before the overflow is completed
	// if (motorValue1 < PWM_MIN) motorPinMask1 = 0x00;
	// else motorPinMask1 = _BV(MOTOR1_PIN);
	// if (motorValue2 < PWM_MIN) motorPinMask2 = 0x00;
	// else motorPinMask2 = _BV(MOTOR2_PIN);
	// if (motorValue3 < PWM_MIN) motorPinMask3 = 0x00;
	// else motorPinMask3 = _BV(MOTOR3_PIN);
	// if (motorValue4 < PWM_MIN) motorPinMask4 = 0x00;
	// else motorPinMask4 = _BV(MOTOR4_PIN);
}

// //Turn on pins at overflow
// ISR(TIMER1_OVF_vect){
// 	MOTOR1_PORT |= motorPinMask1;
// 	MOTOR2_PORT |= motorPinMask2;
// }
// ISR(TIMER3_OVF_vect){
// 	MOTOR3_PORT |= motorPinMask3;
// 	MOTOR4_PORT |= motorPinMask4;
// }

// //Turn off pins on compare match
// ISR(TIMER1_COMPB_vect){
// 	MOTOR1_PORT &= ~_BV(MOTOR1_PIN);
// }
// ISR(TIMER1_COMPC_vect){
// 	MOTOR2_PORT &= ~_BV(MOTOR2_PIN);
// }
// ISR(TIMER3_COMPB_vect){
// 	MOTOR3_PORT &= ~_BV(MOTOR3_PIN);
// }
// ISR(TIMER3_COMPC_vect){
// 	MOTOR4_PORT &= ~_BV(MOTOR4_PIN);
// }
