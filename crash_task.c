//#include "stm32f4xx_hal.h"   // 修改成你的芯片头文件
//#include "cmsis_os.h"
//#include "stdio.h"           // 用于调试打印（串口）
//#include "crash_task.h"
//// 全局变量，保存跌落状态，1表示跌落检测到，0表示正常
//volatile uint8_t IR_SD_FallFlag = 0;
//volatile uint8_t IR_BT_R_FallFlag = 0;
//volatile uint8_t IR_CC_R1_FallFlag = 0;  

//// 读取IR_SD_（PE10）引脚状态
//GPIO_PinState IR_SD_Read(void)
//{
//    return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10);
//}

//// 读取IR_BT_R（PE11）引脚状态
//GPIO_PinState IR_BT_R_Read(void)
//{
//    return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11);
//}
//GPIO_PinState IR_CC_R1_Read(void)
//{
//		return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12);
//}
//void Crash_Task(void const * argument)
//{
//    for(;;)
//    {
////        GPIO_PinState sd_state = IR_SD_Read();
////        GPIO_PinState bt_state = IR_BT_R_Read();
////				GPIO_PinState ccr1_state = IR_CC_R1_Read();
////        if(sd_state == GPIO_PIN_RESET)
////            IR_SD_FallFlag = 1;   // 跌落检测到
////        else
////            IR_SD_FallFlag = 0;   // 正常

////        if(bt_state == GPIO_PIN_RESET)
////            IR_BT_R_FallFlag = 1;
////        else
////            IR_BT_R_FallFlag = 0;

////				if(ccr1_state == GPIO_PIN_RESET)
////            IR_CC_R1_FallFlag = 1;
////        else
////						IR_CC_R1_FallFlag = 0;	
//        osDelay(10);
//    }
//}