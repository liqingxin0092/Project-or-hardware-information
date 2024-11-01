#include "pid.h"
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"

// 定义结构体类型变量
tPid pidMotorASpeed;
tPid pidMotorBSpeed;
// 给结构体变量赋初值
void PID_init() //{"p":12000,"i":80,"d":200,"a":0,"b":0}
{
	pidMotorASpeed.actual_val = 0.0;
	pidMotorASpeed.target_val = 3.0;
	pidMotorASpeed.err = 0.0;
	pidMotorASpeed.err_last = 0.0;
	pidMotorASpeed.err_sum = 0.0;
	pidMotorASpeed.Kp = 12000;
	pidMotorASpeed.Ki = 80;
	pidMotorASpeed.Kd = 200;

	pidMotorBSpeed.actual_val = 0.0;
	pidMotorBSpeed.target_val = 3.0;
	pidMotorBSpeed.err = 0.0;
	pidMotorBSpeed.err_last = 0.0;
	pidMotorBSpeed.err_sum = 0.0;
	pidMotorBSpeed.Kp = 12000;
	pidMotorBSpeed.Ki = 80;
	pidMotorBSpeed.Kd = 200;
}
// 比例P调节控制函数
float P_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;				  // 传递真实值
	pid->err = pid->target_val - pid->actual_val; // 当前误差=目标值减真实值
	// 比例控制调节 输出=Kp*当前误差
	pid->actual_val = pid->Kp * pid->err;
	return pid->actual_val;
}
// 比例P 积分I 控制函数
float PI_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;				  // 传递真实值
	pid->err = pid->target_val - pid->actual_val; // 当前误差=目标值减真实值
	pid->err_sum += pid->err;					  // 误差累计值=当前累计误差和
	// 使用PI控制 输出=Kp*当前误差+Ki*误差累计值
	pid->actual_val = pid->Kp * pid->err + pid->Ki * pid->err_sum;
	return pid->actual_val;
}
// PID控制函数
float PID_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;				  // 传递真实值
	pid->err = pid->target_val - pid->actual_val; // 当前误差=目标值-真实值
	pid->err_sum += pid->err;					  // 误差累计值=当前误差累计和
	// 使用PID控制 输出=Kp*当前误差+Ki*误差累计值+Kd*(当前误差-上次误差)
	pid->actual_val = pid->Kp * pid->err + pid->Ki * pid->err_sum + pid->Kd * (pid->err - pid->err_last);
	// 保存上次误差。这次误差赋值给上次
	pid->err_last = pid->err;
	return pid->actual_val;
}
