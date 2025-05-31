#include "pid.h"

void PID_Init(PID_HandleTypeDef *pid, float Kp, float Ki, float Kd, float out_min, float out_max)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0;
    pid->last_error = 0;
    pid->output_min = out_min;
    pid->output_max = out_max;
}

float PID_Update(PID_HandleTypeDef *pid, float setpoint, float measurement, float dt)
{
    float error = setpoint - measurement;
    pid->integral += error * dt;

    // 限制积分项范围，避免积分风暴
    if (pid->integral > 100.0f) pid->integral = 100.0f;
    else if (pid->integral < -100.0f) pid->integral = -100.0f;

    float derivative = (error - pid->last_error) / dt;
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;

    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    pid->last_error = error;
    return output;
}
void PID_Reset(PID_HandleTypeDef *pid)
{
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
}