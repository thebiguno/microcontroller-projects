#include "EncoderHAL.h"

using namespace digitalcave;

EncoderHAL::EncoderHAL(TIM_TypeDef* instance, GPIO_TypeDef* port)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 1);

	timer.Instance = instance;
	timer.Init.Period = 0xFFFF;
	timer.Init.CounterMode = TIM_COUNTERMODE_UP;
	timer.Init.Prescaler = 0;
	timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	encoder.EncoderMode = TIM_ENCODERMODE_TI12;

	encoder.IC1Filter = 0x0F;
	encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
	encoder.IC1Prescaler = TIM_ICPSC_DIV4;
	encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;

	encoder.IC2Filter = 0x0F;
	encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	encoder.IC2Prescaler = TIM_ICPSC_DIV4;
	encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;

	GPIO_InitTypeDef GPIO_InitStruct;

	uint32_t alternate;
	uint16_t pins;

	if (instance == TIM1) {
		__TIM1_CLK_ENABLE();
		alternate = GPIO_AF1_TIM1;
		port = GPIOA;
		pins = GPIO_PIN_8 | GPIO_PIN_9;
		HAL_NVIC_SetPriority(TIM1_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(TIM1_IRQn);
	} else if (instance == TIM2) {
		__TIM2_CLK_ENABLE();
		alternate = GPIO_AF2_TIM1;
		port = GPIOA;
		pins = GPIO_PIN_0 | GPIO_PIN_1;
		HAL_NVIC_SetPriority(TIM2_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	} else if (instance == TIM3) {
		__TIM3_CLK_ENABLE();

		if (port == GPIOA) {
			pins = GPIO_PIN_6 | GPIO_PIN_7;
		} else if (port == GPIOB) {
			pins = GPIO_PIN_4 | GPIO_PIN_5;
		} else if (port == GPIOC) {
			pins = GPIO_PIN_6 | GPIO_PIN_7;
		}

		HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
	} else if (instance == TIM4) {
		__TIM4_CLK_ENABLE();

		if (port == GPIOB) {
			pins = GPIO_PIN_6 | GPIO_PIN_7;
		} else if (port == GPIOD) {
			pins = GPIO_PIN_12 | GPIO_PIN_13;
		}
		HAL_NVIC_SetPriority(TIM4_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
	} else if (instance == TIM5) {
		__TIM5_CLK_ENABLE();

		port = GPIOA;
		pins = GPIO_PIN_0 | GPIO_PIN_1;
		HAL_NVIC_SetPriority(TIM5_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(TIM5_IRQn);
	}

	if (port == GPIOA) {
		__GPIOA_CLK_ENABLE();
	} else if (port == GPIOB) {
		__GPIOB_CLK_ENABLE();
	} else if (port == GPIOC) {
		__GPIOC_CLK_ENABLE();
	} else if (port == GPIOD) {
		__GPIOD_CLK_ENABLE();
	}

	GPIO_InitStruct.Pin = pins;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = alternate;
	HAL_GPIO_Init(port, &GPIO_InitStruct);

	if (HAL_TIM_Encoder_Init(&timer, &encoder) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_TIM_Encoder_Start_IT(&timer, TIM_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}
}

void EncoderHAL::poll() {
	HAL_TIM_IRQHandler(&timer);
}
uint16_t EncoderHAL::read() {
	return timer->instance->CNT;
}
