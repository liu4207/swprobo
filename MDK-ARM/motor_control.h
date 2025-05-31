#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "main.h"

// L298N控制引脚（假设连接到这些GPIO，根据实际硬件连接调整）
#define L298N_INPUT1_GPIO_PORT GPIOD  // 假设连接到PD13，对应LWI1
#define L298N_INPUT1_PIN GPIO_PIN_13
#define L298N_INPUT2_GPIO_PORT GPIOD  // 假设连接到PD12，对应LWI2
#define L298N_INPUT2_PIN GPIO_PIN_12
#define L298N_INPUT3_GPIO_PORT GPIOD  // 假设连接到PD14，对应RWI1
#define L298N_INPUT3_PIN GPIO_PIN_14
#define L298N_INPUT4_GPIO_PORT GPIOD  // 假设连接到PD15，对应RWI2
#define L298N_INPUT4_PIN GPIO_PIN_15

// TIM3相关定义
extern TIM_HandleTypeDef htim3;
#define MOTOR_PWM_TIMER htim3
#define MOTOR_PWM_CHANNEL1 TIM_CHANNEL_3
#define MOTOR_PWM_CHANNEL2 TIM_CHANNEL_4

// 函数声明
void Motor_Init(void);
void Motor_SetSpeed(uint8_t speed1,uint8_t speed2);
void Motor_Forward(void);
void Motor_Backward(void);
void Motor_Stop(void);
void Motor_SoftStart(uint8_t target_speed, uint16_t duration);
void Motor_Brake(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_Right_SetDirection(int right_dir);
void Motor_Left_SetDirection(int left_dir);

#endif
