#include "device_control.h"
#include "stm32f4xx_hal_gpio.h"
// 初始化左侧刷子控制引脚
void Left_Brush_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 配置引脚为输出模式
    GPIO_InitStruct.Pin = LEFT_BRUSH_CONTROL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LEFT_BRUSH_CONTROL_GPIO_PORT, &GPIO_InitStruct);

    // 初始化为关闭状态
    Turn_Off_Left_Brush();
}

// 初始化右侧刷子控制引脚
void Right_Brush_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOE时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();

    // 配置引脚为输出模式
    GPIO_InitStruct.Pin = RIGHT_BRUSH_CONTROL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RIGHT_BRUSH_CONTROL_GPIO_PORT, &GPIO_InitStruct);

    // 初始化为关闭状态
    Turn_Off_Right_Brush();
}

// 初始化水泵控制引脚
void Pump_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOE时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();

    // 配置引脚为输出模式
    GPIO_InitStruct.Pin = PUMP_CONTROL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PUMP_CONTROL_GPIO_PORT, &GPIO_InitStruct);

    // 初始化为关闭状态
    Turn_Off_Pump();
}


// 打开左侧刷子
void Turn_On_Left_Brush(void)
{
    HAL_GPIO_WritePin(LEFT_BRUSH_CONTROL_GPIO_PORT, LEFT_BRUSH_CONTROL_PIN, GPIO_PIN_SET);
}

// 关闭左侧刷子
void Turn_Off_Left_Brush(void)
{
    HAL_GPIO_WritePin(LEFT_BRUSH_CONTROL_GPIO_PORT, LEFT_BRUSH_CONTROL_PIN, GPIO_PIN_RESET);
}

// 打开右侧刷子
void Turn_On_Right_Brush(void)
{
    HAL_GPIO_WritePin(RIGHT_BRUSH_CONTROL_GPIO_PORT, RIGHT_BRUSH_CONTROL_PIN, GPIO_PIN_SET);
}

// 关闭右侧刷子
void Turn_Off_Right_Brush(void)
{
    HAL_GPIO_WritePin(RIGHT_BRUSH_CONTROL_GPIO_PORT, RIGHT_BRUSH_CONTROL_PIN, GPIO_PIN_RESET);
}

// 打开水泵
void Turn_On_Pump(void)
{
    HAL_GPIO_WritePin(PUMP_CONTROL_GPIO_PORT, PUMP_CONTROL_PIN, GPIO_PIN_SET);
}

// 关闭水泵
void Turn_Off_Pump(void)
{
    HAL_GPIO_WritePin(PUMP_CONTROL_GPIO_PORT, PUMP_CONTROL_PIN, GPIO_PIN_RESET);
}