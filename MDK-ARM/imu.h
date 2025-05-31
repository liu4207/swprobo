#ifndef __DEMO_H
#define __DEMO_H

#include "atk_ms601m.h"
#include "atk_ms601m_uart.h"
#include "usart.h"
#include <string.h>
#include <math.h>


#define UART2_QUEUE_LEN 10
#define UART2_TX_BUF_SIZE 250
void demo_run(void);
void IMU_Task(void const * argument);

// 轮距，单位米，需根据实际机器人测量
#define WHEEL_DISTANCE 0.20f  

// 重力加速度，单位 m/s^2
#define GRAVITY 9.80665f         // 重力加速度 m/s^2

// IMU数据结构
typedef struct {
    float gyro_z_deg_s;   // 陀螺仪Z轴角速度，单位度/秒
    float acc_x_g;        // 加速度X轴，单位g
} IMU_Data_t;

// 全局变量，积分得到的线速度
static float velocity = 0.0f;
// 采样周期（秒）
#define DT 0.01f
static float deg2rad(float deg) {
    return deg * 3.14159265f / 180.0f;
}
void IMU_EstimateWheelSpeed(IMU_Data_t imu, float *v_left, float *v_right);

#endif
