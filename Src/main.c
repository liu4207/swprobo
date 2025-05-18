/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "atk_ms601m.h"
#include "atk_ms601m_uart.h"
#include "device_control.h"
#include "motor_control.h"
#include "fan_control.h"
#include "imu.h"
#include <stdio.h>  // 添加stdio.h头文件以使用printf函数
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t rx_byte;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	DWT_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
		MX_GPIO_Init();
    printf("GPIO 初始化完成\n");
    MX_TIM3_Init();
    printf("TIM3 初始化完成\n");
    MX_UART4_Init();
    printf("UART4 初始化完成\n");
    MX_USART1_UART_Init();
    printf("USART1 初始化完成\n");
    MX_USART2_UART_Init();
    printf("USART2 初始化完成\n");
  /* USER CODE BEGIN 2 */
	atk_ms601m_uart_init();  // 初始化 FIFO 和使能中断
	Motor_Init();  // 初始化电机
	  Left_Brush_Init();
  Right_Brush_Init();
  Pump_Init();
	Fan_Init();
	printf("外设初始化完成\n");
//	IMU_Init(&huart1);
	
	HAL_UART_Receive_IT(&huart1, &rx_byte, 1);//开启中断
HAL_GPIO_WritePin(user_led_GPIO_Port, user_led_Pin, GPIO_PIN_SET); // 点亮LED指示程序运行
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
		int counter = 0; // 用于打印的计数器
//		IMU_Data_t imu_data = {0};
		 demo_run(); 
  while (1)
  {
//		IMU_Process();
//		IMU_GetData(&imu_data);
//		printf("姿态角: 俯仰=%.2f°, 横滚=%.2f°, 偏航=%.2f°\n",
//               imu_data.pitch, imu_data.roll, imu_data.yaw);
//		
		 Turn_On_Left_Brush();
    Turn_On_Right_Brush();
//		 Turn_On_Pump();
//		Fan_TurnOff();
		Motor_SoftStart(100,2000);
		
		Motor_Forward();
    HAL_Delay(2000);
		 Motor_Brake();
		    HAL_Delay(2000);
		
		printf("Number: %d\n", counter++); // 打印递增的数字
    HAL_Delay(1000); // 每秒打印一次
		HAL_GPIO_TogglePin(user_led_GPIO_Port, user_led_Pin); // 翻转LED
//		 Motor_Forward();  // 电机正转
//     Motor_SetSpeed(50);  // 设置电机速度为50%
//     HAL_Delay(2000);  // 运行2秒
//    Fan_TurnOn();
//		HAL_Delay(800);
		

//     Motor_Stop();  // 停止电机
//     HAL_Delay(1000);  // 停顿1秒

//     Motor_Backward();  // 电机反转
//     Motor_SetSpeed(30);  // 设置电机速度为30%
//     HAL_Delay(2000);  // 运行2秒

//     Motor_Stop();  // 停止电机
//     HAL_Delay(1000);  // 停顿1秒
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
	printf("错误: 系统发生错误，进入死循环\n");
  while (1)
  {
		HAL_GPIO_TogglePin(user_led_GPIO_Port, user_led_Pin);
        HAL_Delay(500); /* 错误时 LED 闪烁 */
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
