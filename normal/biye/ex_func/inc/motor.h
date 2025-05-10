#ifndef __MOTOR_H
#define __MOTOR_H

#include "pid_ctrl.h"

#define MOTOR_SET_CCR(ccr) TIM2->CCR1=(ccr)

extern pid_t motor_pid_struct;

void motor_init(void);
void encoder_init(void);

#endif
