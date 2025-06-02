#include "fan_control.h"
#include "cmsis_os.h"
// fan_control.c 顶部
volatile uint8_t fan_enabled = 0;

// 初始化风扇控制引脚
void Fan_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOE时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();

    // 配置引脚为输出模式
    GPIO_InitStruct.Pin = FAN_CONTROL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(FAN_CONTROL_GPIO_PORT, &GPIO_InitStruct);

    // 初始化为关闭状态
    Fan_TurnOff();
}

// 打开风扇（输出高电平）
void Fan_TurnOn(void)
{
	
    HAL_GPIO_WritePin(FAN_CONTROL_GPIO_PORT, FAN_CONTROL_PIN, GPIO_PIN_SET);
}

// 关闭风扇（输出低电平）
void Fan_TurnOff(void)
{
    HAL_GPIO_WritePin(FAN_CONTROL_GPIO_PORT, FAN_CONTROL_PIN, GPIO_PIN_RESET);
}

void Fan_Task(void const * argument)
{
  /* USER CODE BEGIN Fan_Task */
		Fan_Init();
  /* Infinite loop */
  for(;;)
  {
     if (fan_enabled)
         Fan_TurnOn();
     else
         Fan_TurnOff();
		
    osDelay(100);
  }
  /* USER CODE END Fan_Task */
}
