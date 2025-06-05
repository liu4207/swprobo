#include "stm32f4xx_hal.h"   // 修改成你的芯片头文件
#include "cmsis_os.h"
#include "stdio.h"           // 用于调试打印（串口）
#include "crash_task.h"
#include "motor_control.h"  // ?? 控制后退电机的函数定义
#include "pid.h"
//extern static PID_HandleTypeDef pid_left;
//extern static PID_HandleTypeDef pid_right;
// 全局变量，保存跌落状态，1表示跌落检测到，0表示正常
volatile uint8_t IR_SD_FallFlag = 0;
volatile uint8_t IR_BT_R_FallFlag = 0;
volatile uint8_t IR_CC_R1_FallFlag = 0;  
volatile uint8_t IR_BT_F_Flag   = 0;
volatile uint8_t IR_BT_L_Flag   = 0;
//光电门
volatile uint8_t IRHIT_L_Flag = 0;
volatile uint8_t IRHIT_R_Flag = 0;
extern volatile uint8_t motor_locked;


// 读取IR_SD_（PE10）引脚状态
GPIO_PinState IR_SD_Read(void)
{
    return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);
}

// 读取IR_BT_R（PE11）引脚状态
GPIO_PinState IR_BT_R_Read(void)
{
    return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11);
}
GPIO_PinState IR_CC_R1_Read(void)
{
		return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12);
}
void Crash_Task(void const * argument)
{
		static uint32_t last_escape_time = 0;
    for(;;)
    {
        GPIO_PinState sd_state = IR_SD_Read();
        GPIO_PinState bt_state = IR_BT_R_Read();
				GPIO_PinState ccr1_state = IR_CC_R1_Read();
        if(sd_state == GPIO_PIN_RESET)
            IR_SD_FallFlag = 1;   // 跌落检测到 side 可以避障 1是正常 0是有东西
        else
            IR_SD_FallFlag = 0;   // 

        if(bt_state == GPIO_PIN_RESET)
            IR_BT_R_FallFlag = 1;// 0正常 1是底下高 防跌落
        else
            IR_BT_R_FallFlag = 0;

				if(ccr1_state == GPIO_PIN_RESET)
            IR_CC_R1_FallFlag = 1;//没有用
        else
						IR_CC_R1_FallFlag = 0;	
				IR_BT_F_Flag   = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)  == GPIO_PIN_RESET) ? 1 : 0;//0正常 1是底下高 防跌落
        IR_BT_L_Flag   = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)  == GPIO_PIN_RESET) ? 1 : 0;
				
				IRHIT_L_Flag = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET) ? 1 : 0;//0正常 1碰撞
				IRHIT_R_Flag = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET) ? 1 : 0;
				
				 uint32_t now = HAL_GetTick();
        if (
            (IR_SD_FallFlag == 0) || 
            (IR_BT_R_FallFlag == 1) || 
            (IR_BT_F_Flag == 1) || 
         
            (IRHIT_L_Flag == 1) || 
            (IRHIT_R_Flag == 1)
        )
				{
            if (now - last_escape_time > 2000) // 避免短时间内重复后退
            {
                last_escape_time = now;
								motor_locked = 1;  // 锁定电机控制权
                //printf("避障触发，执行后退...\n");
								Motor_Brake();
								Motor_SetSpeed(50, 50);
                Motor_Backward();
                
                osDelay(500);  // 后退时间

                Motor_Stop();
							Motor_SetSpeed(0, 0);
							osDelay(1000);
							motor_locked = 0;  // 解锁
//                PID_Reset(&pid_left);
//                PID_Reset(&pid_right);
            }
					}
        osDelay(10);
    }
}
