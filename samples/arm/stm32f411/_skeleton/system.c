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
 
/* Helpers for SystemInitError() */
#define SYSTEM_INIT_ERROR_FLASH 0x01
#define SYSTEM_INIT_ERROR_PLL 0x02
#define SYSTEM_INIT_ERROR_CLKSRC 0x04
#define SYSTEM_INIT_ERROR_HSI 0x08

void SystemInitError(uint8_t error_source) {
	while(1);
}

void SystemInit() {
	/* Enable Power Control clock */
	RCC->APB1ENR |= RCC_APB1LPENR_PWRLPEN;
	/* Regulator voltage scaling output selection: Scale 2 */
	PWR->CR |= PWR_CR_VOS_1;
 
	/* Wait until HSI ready */
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);
 
	/* Store calibration value */
	PWR->CR |= (uint32_t)(16 << 3);
 
	/* Disable main PLL */
	RCC->CR &= ~(RCC_CR_PLLON);
	/* Wait until PLL ready (disabled) */
	while ((RCC->CR & RCC_CR_PLLRDY) != 0);
 
	/*
	 * Configure Main PLL
	 * HSI as clock input
	 * fvco = 336MHz
	 * fpllout = 84MHz
	 * fusb = 48MHz
	 * PLLM = 16
	 * PLLN = 336
	 * PLLP = 4
	 * PLLQ = 7
	 */
	RCC->PLLCFGR = (uint32_t)((uint32_t)0x20000000 | (uint32_t)(16 << 0) | (uint32_t)(336 << 6) | 
					RCC_PLLCFGR_PLLP_0 | (uint32_t)(7 << 24));
 
	/* PLL On */
	RCC->CR |= RCC_CR_PLLON;
	/* Wait until PLL is locked */
	while ((RCC->CR & RCC_CR_PLLRDY) == 0);
 
	/* 
	 * FLASH configuration block
	 * enable instruction cache
	 * enable prefetch
	 * set latency to 2WS (3 CPU cycles)
	 */
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_2WS;
 
	/* Check flash latency */
	if ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_2WS) {
		SystemInitError(SYSTEM_INIT_ERROR_FLASH);
	}
 
	/* Set clock source to PLL */
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	/* Check clock source */
	while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
 
	/* Set HCLK (AHB1) prescaler (DIV1) */
	RCC->CFGR &= ~(RCC_CFGR_HPRE);
 
	/* Set APB1 Low speed prescaler (APB1) DIV2 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
 
	/* SET APB2 High speed srescaler (APB2) DIV1 */
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
}
