#include "tim_callback.h"
#include "main.h"
#include "tim.h"
extern volatile uint32_t pulse_count1 ;  // 左计数脉冲总数
extern volatile uint32_t pulse_count2 ;  // 右计数脉冲总数
float	distance_per_pulse =0;
 
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
		distance_per_pulse = WHEEL_CIRCUMFERENCE / ENCODER_PPR;
    if (htim->Instance == TIM2)
    {
        float timer_freq = (float)HAL_RCC_GetPCLK1Freq() * 2 / (htim2.Init.Prescaler + 1);

        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
						pulse_count1++;
						left_distance = pulse_count1 * distance_per_pulse;
            uint32_t current_capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            if (current_capture >= last_capture_ch1)
                pulse_interval_ch1 = current_capture - last_capture_ch1;
            else
                pulse_interval_ch1 = (0xFFFFFFFF - last_capture_ch1) + current_capture + 1;

            last_capture_ch1 = current_capture;

            motor_speed_rps_ch1 = timer_freq / (pulse_interval_ch1 * ENCODER_PPR);
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
						pulse_count2++;
				   	right_distance = pulse_count2 * distance_per_pulse;
            uint32_t current_capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

            if (current_capture >= last_capture_ch2)
                pulse_interval_ch2 = current_capture - last_capture_ch2;
            else
                pulse_interval_ch2 = (0xFFFFFFFF - last_capture_ch2) + current_capture + 1;

            last_capture_ch2 = current_capture;

            motor_speed_rps_ch2 = timer_freq / (pulse_interval_ch2 * ENCODER_PPR);
        }
    }
}
