  #include "motor_control.h"
  #include "cmsis_os.h"
  #include "motor_task.h"
	#include "usart.h"
  extern void Motor_Init(void);
  extern void Motor_Forward(void);
  extern void Motor_Backward(void);
  extern void Motor_Stop(void);
  extern void Motor_SetSpeed(uint8_t speed);
  extern void Motor_SoftStart(uint8_t target_speed, uint16_t duration);
  extern void Motor_Brake(void);
  
	int temp=0;
	void uart2_rx_callback(uint8_t *buf, uint16_t len)
{
    for (int i = 0; i < len; i++)
    {
        switch (buf[i])
        {
            case '1':
                // 前进
                temp=1;
                break;
            case '2':
                // 后退
                temp=2;
                break;
            case '3':
                // 左转
                temp=3;
                break;
            case '4':
                // 右转
                temp=4;
                break;
            default:
                break;
        }
    }
}


  void Motor_Task(void const * argument)
  {
      Motor_Init();
      Motor_SetSpeed(80);  // 默认设置速度为 80% 
      /* 电机任务主循环 */
      while(1)
      {
				int command = temp;
				temp=0;
				 switch (command)
				 {
            case 1:
                Motor_Forward();
						// osDelay(5000);
                break;
						
            case 2:
                Motor_Backward();
                break;
            case 3:
                Motor_TurnLeft();//left
                break;
            case 4:
                Motor_TurnRight();//right
                break;
            default:
                Motor_Stop();
                break;
        }
//          // 示例操作逻辑（可根据需要替换为实际控制逻辑）
//          Motor_SoftStart(80, 1000);   // 平滑启动到80%速度
//          Motor_Forward();            // 向前运行
//          osDelay(2000);              // 保持运行2秒
//  
//          Motor_Brake();              // 快速刹车
//          osDelay(500);
//  
//          Motor_Backward();           // 反转
//          Motor_SetSpeed(60);         // 设置速度为60%
//          osDelay(2000);
//  
//          Motor_Stop();               // 停止
          osDelay(100);
      }
  }
