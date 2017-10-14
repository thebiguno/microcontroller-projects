/*
 * Stm32 HAL implementation of Button library.
 */

#ifndef ENCODER_HAL_H
#define ENCODER_HAL_H

#include "stm32f4xx_hal.h"

namespace digitalcave {

	class EncoderHAL {
		private:
			TIM_Encoder_InitTypeDef encoder;
			TIM_HandleTypeDef timer;

		public:
			/*
			 * See datasheet Table 9: Alternate function mapping
			 * TIM1 -> PA8/PA9
			 * TIM2 -> PA0/PA1
			 * TIM3 -> PA6/PA7 or PB4/PB5 or PC6/PC7
			 * TIM4 -> PB6/PB7 or PD12/PD13
			 * TIM5 -> PA0/PA1
			 * port is only used for timers 3 and 4
			 */
			EncoderHAL(TIM_TypeDef* instance, GPIO_TypeDef* port);

			/* must be called from TIMn_IRQHandler */
			void poll();
			uint16_t read();
	};
}

#endif
