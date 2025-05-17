#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#include "main.h"

// 定义风扇控制引脚
#define FAN_CONTROL_GPIO_PORT GPIOE
#define FAN_CONTROL_PIN GPIO_PIN_3

// 函数声明
void Fan_Init(void);
void Fan_TurnOn(void);
void Fan_TurnOff(void);

#endif // 添加此句，使预编译指令完整