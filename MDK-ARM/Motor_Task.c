#include "motor_control.h"
#include "cmsis_os.h"
#include "motor_task.h"
#include "usart.h"
#include "pid.h"
#include "tim_callback.h"
#include "main.h"
#include "tim.h"
#include <math.h>
#include "device_control.h"
#include "fan_control.h"

#define WHEEL_BASE 0.185f            // 轮距，米（左右轮中心距离）
#define CONTROL_PERIOD_MS    100
#define SPEED_PULSE_INTERVAL_MAX 2000000
#define PWM_DEADBAND 20  // PWM最低有效门槛，低于此值电机不转
#define SPEED_STOP_THRESHOLD 0.01f  // 速度低于该值认为停止
volatile int control_mode = 1;  // 1: 自动模式；0: 手动模式

volatile int32_t encoder_left_pulse_count = 0;
volatile int32_t encoder_right_pulse_count = 0;
volatile uint32_t pulse_count1 = 0;  // 左计数脉冲总数
volatile uint32_t pulse_count2 = 0;  // 右计数脉冲总数
//float distance_per_pulse=0; // = WHEEL_CIRCUMFERENCE / ENCODER_PPR;
float left_distance = 0;
float right_distance = 0;
float linear_velocity=0;
float angular_velocity_z=0;

  extern void Motor_Init(void);
  extern void Motor_Forward(void);
  extern void Motor_Backward(void);
  extern void Motor_Stop(void);
  extern void Motor_SetSpeed(uint8_t speed1,uint8_t speed2);
  extern void Motor_SoftStart(uint8_t target_speed, uint16_t duration);
  extern void Motor_Brake(void);
  static PID_HandleTypeDef pid_left;
static PID_HandleTypeDef pid_right;

volatile float pwm_left=0,pwm_right=0,speed_left=0,speed_right=0,target_speed_rps=0,target_linear_speed=0,v,w,v_left,v_right,target_rps_left,target_rps_right;
	
	#define FILTER_LEN 5
float speed_left_buf[FILTER_LEN] = {0};
float speed_right_buf[FILTER_LEN] = {0};
int filter_index_left = 0;
int filter_index_right = 0;

float FilterSpeed(float *buf, int *index, float new_val)
{
    buf[*index] = new_val;
    *index = (*index + 1) % FILTER_LEN;

    float sum = 0;
    for (int i = 0; i < FILTER_LEN; i++) sum += buf[i];
    return sum / FILTER_LEN;
}

// 全局变量，导航传下来的速度（由navigation模块或者上位机赋值）
volatile float nav_linear_velocity = 0.2f;   // m/s least boom up 0.16
volatile float nav_angular_velocity = 0.0f; // rad/s
volatile float filtered_speed_left;
volatile float filtered_speed_right;

extern volatile uint8_t fan_enabled;
extern volatile uint8_t brush_enabled;
extern volatile uint8_t pump_enabled;

	int temp=0;
	void uart2_rx_callback(uint8_t *buf, uint16_t len)
{
    for (int i = 0; i < len; i++)
    {
				char msg[50];  // 存放打印信息
        switch (buf[i])
        {
            case '1': temp = 1; break; // 前进
            case '2': temp = 2; break; // 后退
            case '3': temp = 3; break; // 左转
            case '4': temp = 4; break; // 右转
					
            case 'a': fan_enabled = 1; break;
            case 'x': fan_enabled = 0; break;

            case 'b': brush_enabled = 1; break;
            case 'y': brush_enabled = 0; break;

            case 'c': pump_enabled = 1; break;
            case 'z': pump_enabled = 0; break;
					
					  case '8': control_mode = 0; break; // 切换到手动模式
            case '9': control_mode = 1; break; // 切换回自动模式
            default: break;
        }
				HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }
}


  void Motor_Task(void const * argument)
  {
      Motor_Init();
		
		  PID_Init(&pid_left, 20.0f, 1.5f, 0.1f, 0.0f, 100.0f);
      PID_Init(&pid_right, 20.0f, 1.5f, 0.1f, 0.0f, 100.0f);
		  uint32_t last_tick = osKernelSysTick();
//		int autonav=1;
      //Motor_SetSpeed(80);  // 默认设置速度为 80% 
      /* 电机任务主循环 */
      while(1)
      {
				uint32_t current_tick = osKernelSysTick();
				// 计算时间间隔，防止溢出安全计算
        uint32_t delta_tick = current_tick - last_tick;
        float dt = delta_tick / 1000.0f;

        if (dt >= (CONTROL_PERIOD_MS / 1000.0f))
        {
            last_tick = current_tick;
					 if (control_mode == 1)  // 自动导航模式
					{
            // 读取导航速度指令（可能为负）
            v = nav_linear_velocity;      // m/s，允许正负
            w = nav_angular_velocity;     // rad/s，允许正负
					
					  // 速度接近零，立即刹车并停止PWM输出
            if (fabsf(v) < SPEED_STOP_THRESHOLD && fabsf(w) < SPEED_STOP_THRESHOLD)
            {
                Motor_Brake();
                Motor_SetSpeed(0, 0);
                Motor_Stop();
                PID_Reset(&pid_left);
                PID_Reset(&pid_right);
                continue; // 本周期不做PID控制
            }
						
					            // 计算左右轮目标线速度
             v_left = v - (w * WHEEL_BASE / 2.0f);
             v_right = v + (w * WHEEL_BASE / 2.0f);
					
					            // 转为轮转速 rps
             target_rps_left = fabsf(v_left / WHEEL_CIRCUMFERENCE);
             target_rps_right = fabsf(v_right / WHEEL_CIRCUMFERENCE);//根据上位机给定的线速度和角速度，结合底盘几何算出的左右轮目标转速。
					
            //float speed_left = 0.0f, speed_right = 0.0f;
            if (pulse_interval_ch1 < SPEED_PULSE_INTERVAL_MAX)//如果编码器读数值正常  开始按照指定速度转
                speed_left = motor_speed_rps_ch1;//保存**“实际”**的左轮转速反馈 在闭环控制里用它来告诉 PID 控制器
						 else {
						speed_left = 0.0f;  // 无脉冲时，实际速度应该是 0
						}
            if (pulse_interval_ch2 < SPEED_PULSE_INTERVAL_MAX)//如果超过最高值 就意味着 编码器没在出脉冲，可能轮子停了或者传感器读不到
                speed_right = motor_speed_rps_ch2;
						 else {
						speed_right = 0.0f;  // 无脉冲时，实际速度应该是 0
						}
						

            // 对速度做滤波处理
            filtered_speed_left = FilterSpeed(speed_left_buf, &filter_index_left, speed_left);//用长度为 5 的循环缓冲区做简单平均，抑制编码器读数波动，让 PID 输入更平滑。
            filtered_speed_right = FilterSpeed(speed_right_buf, &filter_index_right, speed_right);
						
						linear_velocity =(filtered_speed_left + filtered_speed_right)*WHEEL_CIRCUMFERENCE / 2.0f;
						angular_velocity_z=(filtered_speed_right - filtered_speed_left)* WHEEL_CIRCUMFERENCE / WHEEL_BASE;
						

            // PID计算PWM
            pwm_left = PID_Update(&pid_left, target_rps_left, filtered_speed_left, dt);
            pwm_right = PID_Update(&pid_right, target_rps_right, filtered_speed_right, dt);

            // PWM死区处理，保证低速能起动电机
            // PWM绝对值，死区处理
            float abs_pwm_left = (pwm_left < 0) ? -pwm_left : pwm_left;
            float abs_pwm_right = (pwm_right < 0) ? -pwm_right : pwm_right;

            if (abs_pwm_left > 0 && abs_pwm_left < PWM_DEADBAND) abs_pwm_left = PWM_DEADBAND;
            else if (abs_pwm_left <= 0) abs_pwm_left = 0;

            if (abs_pwm_right > 0 && abs_pwm_right < PWM_DEADBAND) abs_pwm_right = PWM_DEADBAND;
            else if (abs_pwm_right <= 0) abs_pwm_right = 0;

            if (abs_pwm_left > 100) abs_pwm_left = 100;
            if (abs_pwm_right > 100) abs_pwm_right = 100;

            // 设置PWM占空比
            Motor_SetSpeed((uint8_t)abs_pwm_left, (uint8_t)abs_pwm_right);

            // 设置方向：正负号控制方向，0为停止
            Motor_Left_SetDirection((v_left > 0) ? 1 : ((v_left < 0) ? -1 : 0));
            Motor_Right_SetDirection((v_right > 0) ? 1 : ((v_right < 0) ? -1 : 0));

					}
				
				else{
				//客户端控制
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
			}	 
			//}
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
			}
          osDelay(50);
      }
  }
