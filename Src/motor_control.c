#include "motor_control.h"
#include "cmsis_os.h"

// 初始化电机控制相关引脚和PWM
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能L298N引脚对应的GPIO时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // 配置L298N的控制引脚为输出模式
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = L298N_INPUT1_PIN | L298N_INPUT2_PIN | L298N_INPUT3_PIN | L298N_INPUT4_PIN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // 启动TIM3的PWM输出（假设TIM3已在CubeMX中正确配置好PWM模式）
    HAL_TIM_PWM_Start(&MOTOR_PWM_TIMER, MOTOR_PWM_CHANNEL1);
    HAL_TIM_PWM_Start(&MOTOR_PWM_TIMER, MOTOR_PWM_CHANNEL2);
}

// 设置电机速度（通过改变PWM占空比实现，占空比范围0 - 100）
void Motor_SetSpeed(uint8_t speed1,uint8_t speed2)
{
    uint32_t pulse_value1;
	int32_t pulse_value2;
    // 根据速度计算PWM脉冲值（TIM3的ARR为999，预分频器为167，计数频率为1MHz）
    pulse_value1 = (uint32_t)(999 * speed1 / 100.0);
		pulse_value2 = (uint32_t)(999 * speed2 / 100.0);
    __HAL_TIM_SetCompare(&MOTOR_PWM_TIMER, MOTOR_PWM_CHANNEL1, pulse_value1);
    __HAL_TIM_SetCompare(&MOTOR_PWM_TIMER, MOTOR_PWM_CHANNEL2, pulse_value2);
}

// 电机正转
void Motor_Forward(void)
{
    HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_SET);//left
    HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_SET);//right
    HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_RESET);
}

// 电机反转
void Motor_Backward(void)
{
    HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_SET);
}

// 电机停止
void Motor_Stop(void)
{
    HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_RESET);
}
//// 电机速度渐变启动
//void Motor_SoftStart(uint8_t target_speed, uint16_t duration)
//{
//    uint16_t step = duration / target_speed;  // 计算速度变化的时间间隔
//    uint8_t current_speed = 0;
//    for (current_speed = 0; current_speed <= target_speed; current_speed++)
//    {
//        Motor_SetSpeed(current_speed,current_speed);
//        HAL_Delay(step);
//    }
//}

// 电机刹车（通过短接电机两端实现快速制动）
void Motor_Brake(void)
{
    HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_SET);
    HAL_Delay(200);  // 短接一段时间实现刹车效果
    Motor_Stop();

}
void Motor_TurnLeft(void)
{
    HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_RESET);  // 左轮反转
    HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_SET);    // 右轮正转
    HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_RESET);
}

// 原地右转
void Motor_TurnRight(void)
{
    HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_SET);    // 左轮正转
    HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_RESET);  // 右轮反转
    HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_SET);
}
// left_dir = 1 正转，0 停止，-1 反转
void Motor_Left_SetDirection(int left_dir)
{
    switch(left_dir)
    {
        case 1: // 正转
            HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_RESET);
            break;
        case -1: // 反转
            HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_SET);
            break;
        default: // 停止
            HAL_GPIO_WritePin(L298N_INPUT1_GPIO_PORT, L298N_INPUT1_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(L298N_INPUT2_GPIO_PORT, L298N_INPUT2_PIN, GPIO_PIN_RESET);
            break;
    }
}
// right_dir = 1 正转，0 停止，-1 反转
void Motor_Right_SetDirection(int right_dir)
{
    switch(right_dir)
    {
        case 1: // 正转
            HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_RESET);
            break;
        case -1: // 反转
            HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_SET);
            break;
        default: // 停止
            HAL_GPIO_WritePin(L298N_INPUT3_GPIO_PORT, L298N_INPUT3_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(L298N_INPUT4_GPIO_PORT, L298N_INPUT4_PIN, GPIO_PIN_RESET);
            break;
    }
}

