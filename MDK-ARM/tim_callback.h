#ifndef __TIM_CALLBACK_H
#define __TIM_CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
// 输入捕获回调函数声明
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
extern volatile uint32_t pulse_count1 ;  // 左计数脉冲总数
extern volatile uint32_t pulse_count2 ;  // 右计数脉冲总数

#ifdef __cplusplus
}
#endif

#endif /* __TIM_CALLBACK_H */
