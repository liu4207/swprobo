#include "adc.h"
#include "cmsis_os.h"
#include "battery_task.h"
#include <stdio.h>  //printf
#define BATTERY_ADC_BUF_LEN  10

static uint16_t battery_task_adc_buffer[BATTERY_ADC_BUF_LEN];

void battery_task_start_dma(void)
{
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)battery_task_adc_buffer, BATTERY_ADC_BUF_LEN) != HAL_OK)
    {
        Error_Handler();
    }
}

uint16_t battery_task_get_adc_average(void)
{
    uint32_t sum = 0;
    for (int i = 0; i < BATTERY_ADC_BUF_LEN; i++)
    {
        sum += battery_task_adc_buffer[i];
    }
    return (uint16_t)(sum / BATTERY_ADC_BUF_LEN);
}

float battery_task_read_voltage(void)
{
    const float VREF = 3.3f;
    const float R1 = 30000.0f;
    const float R2 = 7500.0f;

    uint16_t adc_avg = battery_task_get_adc_average();
    float adc_voltage = ((float)adc_avg / 4095.0f) * VREF;
    float battery_voltage = adc_voltage * (R1 + R2) / R2;

    return battery_voltage;
}

void Battery_Task(void const * argument)
{
  /* USER CODE BEGIN Battery_Task */
	battery_task_start_dma();
  /* Infinite loop */
  for(;;)
  {
		float voltage = battery_task_read_voltage();
  //  printf("Battery Voltage: %.2f V\r\n", voltage);
    osDelay(1000);
  }
  /* USER CODE END Battery_Task */
}
