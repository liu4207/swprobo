#ifndef __BATTERY_TASK_H
#define __BATTERY_TASK_H

#include "stm32f4xx_hal.h"

// 启动电池电压采集DMA
void battery_task_start_dma(void);

// 获取ADC采样平均值
uint16_t battery_task_get_adc_average(void);

// 读取电池电压，单位伏特
float battery_task_read_voltage(void);

void Battery_Task(void const * argument);

#endif /* __BATTERY_TASK_H */
