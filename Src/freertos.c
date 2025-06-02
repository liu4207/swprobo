/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uart_send.h"
#define UART2_QUEUE_LEN 10
//extern osMessageQId uartQueueHandle;

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
/* USER CODE BEGIN Variables */
//osThreadId UARTSendHandle;
// ������Ϣ���о����extern��
osMessageQId uart2_tx_queue;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId MotorHandle;
osThreadId IMUHandle;
osThreadId BrushHandle;
osThreadId PumpHandle;
osThreadId FanHandle;
osThreadId CrashHandle;
osThreadId BatteryHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void uart2_tx_task(void const * argument);
//extern void UART_Send_Task(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Motor_Task(void const * argument);
void IMU_Task(void const * argument);
void Brush_Task(void const * argument);
void Pump_Task(void const * argument);
void Fan_Task(void const * argument);
void Crash_Task(void const * argument);
void Battery_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
 /* --- ������Ϣ���� --- */
  osMessageQDef(uart2_tx_queue_def, UART2_QUEUE_LEN, uint32_t);
  uart2_tx_queue = osMessageCreate(osMessageQ(uart2_tx_queue_def), NULL);
  /* add queues, ... */
//	osMessageQDef(uartQueue, UART_QUEUE_LEN, uint32_t);
//	uartQueueHandle = osMessageCreate(osMessageQ(uartQueue), NULL);
	
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Motor */
  osThreadDef(Motor, Motor_Task, osPriorityHigh, 0, 512);
  MotorHandle = osThreadCreate(osThread(Motor), NULL);

  /* definition and creation of IMU */
  osThreadDef(IMU, IMU_Task, osPriorityRealtime, 0, 512);
  IMUHandle = osThreadCreate(osThread(IMU), NULL);

  /* definition and creation of Brush */
  osThreadDef(Brush, Brush_Task, osPriorityNormal, 0, 128);
  BrushHandle = osThreadCreate(osThread(Brush), NULL);

  /* definition and creation of Pump */
  osThreadDef(Pump, Pump_Task, osPriorityNormal, 0, 128);
  PumpHandle = osThreadCreate(osThread(Pump), NULL);

  /* definition and creation of Fan */
  osThreadDef(Fan, Fan_Task, osPriorityNormal, 0, 128);
  FanHandle = osThreadCreate(osThread(Fan), NULL);

  /* definition and creation of Crash */
  osThreadDef(Crash, Crash_Task, osPriorityHigh, 0, 128);
  CrashHandle = osThreadCreate(osThread(Crash), NULL);

  /* definition and creation of Battery */
  osThreadDef(Battery, Battery_Task, osPriorityAboveNormal, 0, 128);
  BatteryHandle = osThreadCreate(osThread(Battery), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	  osThreadDef(uart2TxTask, uart2_tx_task, osPriorityNormal, 0, 512);
  osThreadCreate(osThread(uart2TxTask), NULL);
  /* add threads, ... */
//	osThreadDef(UARTSend, UART_Send_Task, osPriorityNormal, 0, 512);
//	UARTSendHandle = osThreadCreate(osThread(UARTSend), NULL);

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Motor_Task */
/**
* @brief Function implementing the Motor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Motor_Task */
//void Motor_Task(void const * argument)
//{
//  /* USER CODE BEGIN Motor_Task */
////////  /* Infinite loop */
////////  for(;;)
////////  {
////////    osDelay(1);
////////  }
//  /* USER CODE END Motor_Task */
//}

/* USER CODE BEGIN Header_IMU_Task */
/**
* @brief Function implementing the IMU thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IMU_Task */
//void IMU_Task(void const * argument)
//{
//  /* USER CODE BEGIN IMU_Task */
////////  /* Infinite loop */
////////  for(;;)
////////  {
////////    osDelay(1);
////////  }
//  /* USER CODE END IMU_Task */
//}

/* USER CODE BEGIN Header_Brush_Task */
/**
* @brief Function implementing the Brush thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Brush_Task */
//void Brush_Task(void const * argument)
//{
//  /* USER CODE BEGIN Brush_Task */
////////  /* Infinite loop */
////////  for(;;)
////////  {
////////    osDelay(1);
////////  }
//  /* USER CODE END Brush_Task */
//}

/* USER CODE BEGIN Header_Pump_Task */
/**
* @brief Function implementing the Pump thread.
* @param argument: Not used
* @retval None
*/
///* USER CODE END Header_Pump_Task */
//void Pump_Task(void const * argument)
//{
//  /* USER CODE BEGIN Pump_Task */
////////  /* Infinite loop */
////////  for(;;)
////////  {
////////    osDelay(1);
////////  }
//  /* USER CODE END Pump_Task */
//}

/* USER CODE BEGIN Header_Fan_Task */

/**
* @brief Function implementing the Fan thread.
* @param argument: Not used
* @retval None
*/
///* USER CODE END Header_Fan_Task */
//void Fan_Task(void const * argument)
//{
//  /* USER CODE BEGIN Fan_Task */
////////  /* Infinite loop */
////////  for(;;)
////////  {
////////    osDelay(1);
////////  }
//  /* USER CODE END Fan_Task */
//}

/* USER CODE BEGIN Header_Crash_Task */
/**
* @brief Function implementing the Crash thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Crash_Task */
//void Crash_Task(void const * argument)
//{
//  /* USER CODE BEGIN Crash_Task */
//  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
//  /* USER CODE END Crash_Task */
//}

/* USER CODE BEGIN Header_Battery_Task */
/**
* @brief Function implementing the Battery thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Battery_Task */
//void Battery_Task(void const * argument)
//{
//  /* USER CODE BEGIN Battery_Task */
//////  /* Infinite loop */
//////  for(;;)
//////  {
//////    osDelay(1);
//////  }
//  /* USER CODE END Battery_Task */
//}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
