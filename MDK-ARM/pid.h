#ifndef __PID_H
#define __PID_H

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float last_error;
    float output_min;
    float output_max;
} PID_HandleTypeDef;

void PID_Init(PID_HandleTypeDef *pid, float Kp, float Ki, float Kd, float out_min, float out_max);
float PID_Update(PID_HandleTypeDef *pid, float setpoint, float measurement, float dt);
void PID_Reset(PID_HandleTypeDef *pid);
#endif
