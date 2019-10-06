/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RGB_A_Pin GPIO_PIN_0
#define RGB_A_GPIO_Port GPIOC
#define RGB_B_Pin GPIO_PIN_1
#define RGB_B_GPIO_Port GPIOC
#define RGB_C_Pin GPIO_PIN_2
#define RGB_C_GPIO_Port GPIOC
#define RGB_D_Pin GPIO_PIN_3
#define RGB_D_GPIO_Port GPIOC
#define ENC_P_Pin GPIO_PIN_0
#define ENC_P_GPIO_Port GPIOA
#define ENC_N_Pin GPIO_PIN_1
#define ENC_N_GPIO_Port GPIOA
#define SW1_Pin GPIO_PIN_4
#define SW1_GPIO_Port GPIOA
#define SW2_Pin GPIO_PIN_5
#define SW2_GPIO_Port GPIOA
#define RGB_STB_Pin GPIO_PIN_4
#define RGB_STB_GPIO_Port GPIOC
#define RGB_OE_Pin GPIO_PIN_5
#define RGB_OE_GPIO_Port GPIOC
#define WS_DO_Pin GPIO_PIN_12
#define WS_DO_GPIO_Port GPIOB
#define PZ_DO_Pin GPIO_PIN_13
#define PZ_DO_GPIO_Port GPIOB
#define IR_DI_Pin GPIO_PIN_14
#define IR_DI_GPIO_Port GPIOB
#define RGB_R0_Pin GPIO_PIN_6
#define RGB_R0_GPIO_Port GPIOC
#define RGB_G0_Pin GPIO_PIN_7
#define RGB_G0_GPIO_Port GPIOC
#define RGB_B0_Pin GPIO_PIN_8
#define RGB_B0_GPIO_Port GPIOC
#define RGB_R1_Pin GPIO_PIN_10
#define RGB_R1_GPIO_Port GPIOC
#define RGB_G1_Pin GPIO_PIN_11
#define RGB_G1_GPIO_Port GPIOC
#define RGB_B1_Pin GPIO_PIN_12
#define RGB_B1_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
