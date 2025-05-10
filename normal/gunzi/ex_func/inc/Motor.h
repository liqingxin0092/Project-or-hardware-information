#ifndef __MOTOR_H
#define __MOTOR_H

#include "stdint.h"
#include "pid_ctrl.h"

extern pid_t motor_pid_struct;
extern pid_t angl_2_anglspeed_struct;
extern float k_opp;

void Motor_Init(void);

void Motor_SetSpeed1(int16_t Speed);
void Motor_SetSpeed4(int16_t Speed);
 
void Motor_SetSpeed(int16_t Speed);

void Timer_Init(void);

#endif
