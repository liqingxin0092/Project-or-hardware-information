#include "pid.h"
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"

// ����ṹ�����ͱ���
tPid pidMotorASpeed;
tPid pidMotorBSpeed;
// ���ṹ���������ֵ
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
// ����P���ڿ��ƺ���
float P_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;				  // ������ʵֵ
	pid->err = pid->target_val - pid->actual_val; // ��ǰ���=Ŀ��ֵ����ʵֵ
	// �������Ƶ��� ���=Kp*��ǰ���
	pid->actual_val = pid->Kp * pid->err;
	return pid->actual_val;
}
// ����P ����I ���ƺ���
float PI_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;				  // ������ʵֵ
	pid->err = pid->target_val - pid->actual_val; // ��ǰ���=Ŀ��ֵ����ʵֵ
	pid->err_sum += pid->err;					  // ����ۼ�ֵ=��ǰ�ۼ�����
	// ʹ��PI���� ���=Kp*��ǰ���+Ki*����ۼ�ֵ
	pid->actual_val = pid->Kp * pid->err + pid->Ki * pid->err_sum;
	return pid->actual_val;
}
// PID���ƺ���
float PID_realize(tPid *pid, float actual_val)
{
	pid->actual_val = actual_val;				  // ������ʵֵ
	pid->err = pid->target_val - pid->actual_val; // ��ǰ���=Ŀ��ֵ-��ʵֵ
	pid->err_sum += pid->err;					  // ����ۼ�ֵ=��ǰ����ۼƺ�
	// ʹ��PID���� ���=Kp*��ǰ���+Ki*����ۼ�ֵ+Kd*(��ǰ���-�ϴ����)
	pid->actual_val = pid->Kp * pid->err + pid->Ki * pid->err_sum + pid->Kd * (pid->err - pid->err_last);
	// �����ϴ��������ֵ���ϴ�
	pid->err_last = pid->err;
	return pid->actual_val;
}
