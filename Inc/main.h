/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define PUMP_I_Pin GPIO_PIN_2
#define PUMP_I_GPIO_Port GPIOE
#define FAN_PWM_Pin GPIO_PIN_3
#define FAN_PWM_GPIO_Port GPIOE
#define FAN_FB_Pin GPIO_PIN_4
#define FAN_FB_GPIO_Port GPIOE
#define MAGNET_Pin GPIO_PIN_5
#define MAGNET_GPIO_Port GPIOE
#define BMQ_R1_Pin GPIO_PIN_0
#define BMQ_R1_GPIO_Port GPIOB
#define RB_I_Pin GPIO_PIN_9
#define RB_I_GPIO_Port GPIOE
#define IR_SD_Pin GPIO_PIN_10
#define IR_SD_GPIO_Port GPIOE
#define IR_BT_R_Pin GPIO_PIN_11
#define IR_BT_R_GPIO_Port GPIOE
#define IR_CC_R1_Pin GPIO_PIN_12
#define IR_CC_R1_GPIO_Port GPIOE
#define IRHIT_L_Pin GPIO_PIN_12
#define IRHIT_L_GPIO_Port GPIOB
#define user_led_Pin GPIO_PIN_15
#define user_led_GPIO_Port GPIOB
#define IR_CC_R2_Pin GPIO_PIN_10
#define IR_CC_R2_GPIO_Port GPIOD
#define IR_CC_L2_Pin GPIO_PIN_11
#define IR_CC_L2_GPIO_Port GPIOD
#define LWI2_Pin GPIO_PIN_12
#define LWI2_GPIO_Port GPIOD
#define LWI1_Pin GPIO_PIN_13
#define LWI1_GPIO_Port GPIOD
#define RWI2_Pin GPIO_PIN_14
#define RWI2_GPIO_Port GPIOD
#define RWI1_Pin GPIO_PIN_15
#define RWI1_GPIO_Port GPIOD
#define IRHIT_R_Pin GPIO_PIN_15
#define IRHIT_R_GPIO_Port GPIOA
#define BMQ_L1_Pin GPIO_PIN_0
#define BMQ_L1_GPIO_Port GPIOD
#define IR_CC_L1_Pin GPIO_PIN_3
#define IR_CC_L1_GPIO_Port GPIOB
#define IR_BT_F_Pin GPIO_PIN_4
#define IR_BT_F_GPIO_Port GPIOB
#define IR_BT_L_Pin GPIO_PIN_5
#define IR_BT_L_GPIO_Port GPIOB
#define LB_I_Pin GPIO_PIN_6
#define LB_I_GPIO_Port GPIOB
#define user_button_Pin GPIO_PIN_0
#define user_button_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

extern volatile uint32_t last_capture_ch1;
extern volatile uint32_t pulse_interval_ch1;
extern volatile float motor_speed_rps_ch1;

extern volatile uint32_t last_capture_ch2;
extern volatile uint32_t pulse_interval_ch2;
extern volatile float motor_speed_rps_ch2;


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
