/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define WS2812_Pin GPIO_PIN_13
#define WS2812_GPIO_Port GPIOC
#define RGB_A_Pin GPIO_PIN_0
#define RGB_A_GPIO_Port GPIOC
#define RGB_B_Pin GPIO_PIN_1
#define RGB_B_GPIO_Port GPIOC
#define RGB_C_Pin GPIO_PIN_2
#define RGB_C_GPIO_Port GPIOC
#define RGB_D_Pin GPIO_PIN_3
#define RGB_D_GPIO_Port GPIOC
#define ENC1A_Pin GPIO_PIN_0
#define ENC1A_GPIO_Port GPIOA
#define ENC1B_Pin GPIO_PIN_1
#define ENC1B_GPIO_Port GPIOA
#define RGB_STB_Pin GPIO_PIN_4
#define RGB_STB_GPIO_Port GPIOC
#define RGB_OE_Pin GPIO_PIN_5
#define RGB_OE_GPIO_Port GPIOC
#define IR_DI_Pin GPIO_PIN_0
#define IR_DI_GPIO_Port GPIOB
#define PZ_DO_Pin GPIO_PIN_1
#define PZ_DO_GPIO_Port GPIOB
#define R0_Pin GPIO_PIN_6
#define R0_GPIO_Port GPIOC
#define G0_Pin GPIO_PIN_7
#define G0_GPIO_Port GPIOC
#define B0_Pin GPIO_PIN_8
#define B0_GPIO_Port GPIOC
#define SD_DET_Pin GPIO_PIN_8
#define SD_DET_GPIO_Port GPIOA
#define EN2_SW_Pin GPIO_PIN_11
#define EN2_SW_GPIO_Port GPIOA
#define EN1_SW_Pin GPIO_PIN_12
#define EN1_SW_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_10
#define R1_GPIO_Port GPIOC
#define G1_Pin GPIO_PIN_11
#define G1_GPIO_Port GPIOC
#define B1_Pin GPIO_PIN_12
#define B1_GPIO_Port GPIOC

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
