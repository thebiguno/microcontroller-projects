/*
*
* Copyright (C) Patryk Jaworski <regalis@regalis.com.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
#include <stm32f4xx.h>
 
#define LED_PIN 5
#define LED_ON() GPIOA->BSRRL |= (1 << LED_PIN)
#define LED_OFF() GPIOA->BSRRH |= (1 << LED_PIN)
 
int main() {
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// Enable GPIOA clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		// enable TIM2 clock
	
	GPIOA->MODER |= (1 << (LED_PIN << 1)); //Configure GPIOA pin 5 as output
	
    NVIC->ISER[0] |= 1<< (TIM2_IRQn); // enable the TIM2 IRQ

	//Timer init stuff
	TIM2->PSC = 0x0; // no prescaler, timer counts up in sync with the peripheral clock
	TIM2->DIER |= TIM_DIER_UIE; // enable update interrupt
	TIM2->ARR = 0x01; // count to 1 (autoreload value 1)
	TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; // autoreload on, counter enabled
	TIM2->EGR = 1; // trigger update event to reload timer registers

    while (1);
}

void TIM2_IRQHandler(void) {
	// flash on update event
	if (TIM2->SR & TIM_SR_UIF) GPIOA->ODR ^= (1 << LED_PIN);

	TIM2->SR = 0x0; // reset the status register
}
