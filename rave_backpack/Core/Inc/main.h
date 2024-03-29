/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;

typedef enum {
	IDLE, STATIC, BREATHE, RAINBOW, METEOR
}pattern_state;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define up_arrow_Pin GPIO_PIN_6
#define up_arrow_GPIO_Port GPIOA
#define up_arrow_EXTI_IRQn EXTI9_5_IRQn
#define left_arrow_Pin GPIO_PIN_7
#define left_arrow_GPIO_Port GPIOA
#define left_arrow_EXTI_IRQn EXTI9_5_IRQn
#define dn_arrow_Pin GPIO_PIN_4
#define dn_arrow_GPIO_Port GPIOC
#define dn_arrow_EXTI_IRQn EXTI4_IRQn
#define right_arrow_Pin GPIO_PIN_5
#define right_arrow_GPIO_Port GPIOC
#define right_arrow_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */
#define FREQ 45000000
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
