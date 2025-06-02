#ifndef CRASH_TASK_H
#define CRASH_TASK_H
extern volatile uint8_t IR_SD_FallFlag;// PE10
extern volatile uint8_t IR_BT_R_FallFlag;// PE11
extern volatile uint8_t IR_CC_R1_FallFlag;// PE12
extern volatile uint8_t IR_BT_F_Flag;
extern volatile uint8_t IR_BT_L_Flag;
extern volatile uint8_t IRHIT_L_Flag;
extern volatile uint8_t IRHIT_R_Flag;

void Crash_Task(void const * argument);

#endif

