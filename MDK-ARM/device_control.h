#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include "main.h"
#include "stm32f4xx_hal_gpio.h"
// 定义左侧刷子控制引脚
#define LEFT_BRUSH_CONTROL_GPIO_PORT GPIOB
#define LEFT_BRUSH_CONTROL_PIN GPIO_PIN_6

// 定义右侧刷子控制引脚
#define RIGHT_BRUSH_CONTROL_GPIO_PORT GPIOE
#define RIGHT_BRUSH_CONTROL_PIN GPIO_PIN_9

// 定义水泵控制引脚
#define PUMP_CONTROL_GPIO_PORT GPIOE
#define PUMP_CONTROL_PIN GPIO_PIN_2

// 函数声明
void Left_Brush_Init(void);
void Right_Brush_Init(void);
void Pump_Init(void);
void Turn_On_Left_Brush(void);
void Turn_Off_Left_Brush(void);
void Turn_On_Right_Brush(void);
void Turn_Off_Right_Brush(void);
void Turn_On_Pump(void);
void Turn_Off_Pump(void);

#endif