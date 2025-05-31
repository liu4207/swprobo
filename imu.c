#include "imu.h"
#include "main.h"
#include "cmsis_os.h"      // for osDelay, osMessageCreate, osMessageQDef
#include <stdio.h>  //printf
/**
 * @brief       例程演示入口函数
 * @param       无
 * @retval      无
 */
void IMU_EstimateWheelSpeed(IMU_Data_t imu, float *v_left, float *v_right)
{
    // 角速度转换为 rad/s
    float omega = deg2rad(imu.gyro_z_deg_s);

    // 加速度转 m/s^2（单位g -> m/s^2）
    float acc = imu.acc_x_g * GRAVITY;

    // 简单积分速度（累积）(注意长期漂移需滤波修正)
    velocity += acc * DT;

    // 计算左右轮速度
    *v_left  = velocity - (WHEEL_DISTANCE / 2.0f) * omega;
    *v_right = velocity + (WHEEL_DISTANCE / 2.0f) * omega;
}


 /**
 * @brief       按键0功能，获取并显示ATK-MS601M数据
 * @param       无
 * @retval      无
 */
extern osMessageQId uart2_tx_queue;
/* IMU任务内部静态发送缓冲区 */
static char imu_print_buf[UART2_TX_BUF_SIZE];
/* 当前发送的动态缓冲区指针 */
char *uart2_current_tx_ptr = NULL;

static void demo_key0_fun(void)
{
    atk_ms601m_attitude_data_t attitude_dat;           /* 姿态角数据 */
    atk_ms601m_gyro_data_t gyro_dat;                   /* 陀螺仪数据 */
    atk_ms601m_accelerometer_data_t accelerometer_dat; /* 加速度计数据 */
	  atk_ms601m_quaternion_data_t quaternion_dat;       /* 四元数数据 */
    
    /* 获取ATK-MS901数据 */
		atk_ms601m_get_quaternion(&quaternion_dat, 100);

    atk_ms601m_get_attitude(&attitude_dat, 100);                            /* 获取姿态角数据 */
    atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 100);  /* 获取陀螺仪、加速度计数据 */
    
	    IMU_Data_t imu_data;
    imu_data.gyro_z_deg_s = gyro_dat.z;       // 角速度Z轴，度/秒
    imu_data.acc_x_g = accelerometer_dat.x;   // X轴加速度，单位g
	
	    float v_left, v_right;
    IMU_EstimateWheelSpeed(imu_data, &v_left, &v_right);
	
	int len = snprintf(imu_print_buf, sizeof(imu_print_buf),//静态分配
        "Quaternion: q0=%.4f q1=%.4f q2=%.4f q3=%.4f\r\n"
        "Gx: %.02f Gy: %.02f Gz: %.02f \r\n"
        "Ax: %.02fG Ay: %.02fG Az: %.02fG\r\n"
				"LeftDist: %.4f RightDist: %.4f LinVel: %.4f AngVel: %.4f\r\n",
        quaternion_dat.q0, quaternion_dat.q1, quaternion_dat.q2, quaternion_dat.q3,
        gyro_dat.x, gyro_dat.y, gyro_dat.z,
        accelerometer_dat.x, accelerometer_dat.y, accelerometer_dat.z,
				left_distance, right_distance, linear_velocity, angular_velocity_z);

    // 发送字符串指针到队列（指向全局缓冲区）
    osMessagePut(uart2_tx_queue, (uint32_t)imu_print_buf, 0);//到这里
	
    /* 分配动态内存 */
//    char *buf = pvPortMalloc(200);  // 200字节足够显示三行IMU数据
//    if (buf == NULL)
//    {
//        printf("pvPortMalloc failed!\r\n");
//        return;
//    }
	

//    /* 格式化填入数据 */
//    snprintf(buf, 200,
//        "Roll: %.02f° Pitch: %.02f° Yaw: %.02f°\r\n"
//        "Gx: %.02f°/s Gy: %.02f°/s Gz: %.02f°/s\r\n"
//        "Ax: %.02fG Ay: %.02fG Az: %.02fG\r\n",
//        attitude_dat.roll, attitude_dat.pitch, attitude_dat.yaw,
//        gyro_dat.x, gyro_dat.y, gyro_dat.z,
//        accelerometer_dat.x, accelerometer_dat.y, accelerometer_dat.z);

//    /* 发送到串口2发送任务 */
//    osMessagePut(uart2_tx_queue, (uint32_t)buf, 0);
		

    
    /* 串口打印数据 */ //直接用串口可能会卡 但是可以应急
//    uart2_printf("Roll: %.02f° Pitch: %.02f° Yaw: %.02f°\r\n", attitude_dat.roll, attitude_dat.pitch, attitude_dat.yaw);
//    uart2_printf("Gx: %.02f°/s Gy: %.02f°/s Gz: %.02f°/s\r\n", gyro_dat.x, gyro_dat.y, gyro_dat.z);
//    uart2_printf("Ax: %.02fG Ay: %.02fG Az: %.02fG\r\n", accelerometer_dat.x, accelerometer_dat.y, accelerometer_dat.z);
//    uart2_printf("****************************************\r\n\r\n");
//    uart_msg_t msg;


}


void demo_run(void)
{
//    uint8_t ret;
// //   uint8_t key;
//    
//    /* 初始化ATK-MS601M */
//    ret = atk_ms601m_init(115200);
//    if (ret != 0)
//    {
//        printf("ATK-MS601M init failed!\r\n");
//        while (1)
//        {
//           // LED0_TOGGLE();
//           // delay_ms(200);
//					HAL_Delay(200);
//        }
//    }
//    
//    printf("ATK-MS601M init success!\r\n\n");

//    /* LCD UI初始化 */
//    demo_lcd_ui_init();
    
//    while (1)
//    {
      //  key = key_scan(0);
        
        switch (1)
        {
            case 1:
            {
                /* 获取并显示ATK-MS901数据 */
                demo_key0_fun();
                break;
            }
            default:
            {
                break;
            }
        }
        HAL_Delay(10);
        //delay_ms(10);
//    }
}

void IMU_Task(void const * argument)
{
  /* USER CODE BEGIN IMU_Task */
		atk_ms601m_uart_init();
	    uint8_t ret;
 //   uint8_t key;
    
    /* 初始化ATK-MS601M */
    ret = atk_ms601m_init(115200);
    if (ret != 0)
    {
        printf("ATK-MS601M init failed!\r\n");
        while (1)
        {
           // LED0_TOGGLE();
           // delay_ms(200);
					HAL_Delay(200);
        }
    }
    
    printf("ATK-MS601M init success!\r\n\n");
  /* Infinite loop */
  for(;;)
  {
		demo_run(); 
		
    osDelay(100);
  }
  /* USER CODE END IMU_Task */
}



void uart2_tx_task(void const * argument)
{
   // char *msg_ptr;
    osEvent evt;

    for(;;)
    {
        evt = osMessageGet(uart2_tx_queue, osWaitForever);
        if(evt.status == osEventMessage)
        {
					  char *msg_ptr = (char *)evt.value.p;
            //msg_ptr = (char *)evt.value.p;
					
					
            if(msg_ptr != NULL)
            {
							
								uart2_dma_send(msg_ptr, strlen(msg_ptr));
                // 这里假设 DMA 发送完成回调后负责释放/管理内存
            }
        }
        osDelay(10);
    }
}


