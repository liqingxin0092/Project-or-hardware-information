#ifndef  __pid_H
#define  __pid_H

#include "main.h"

//�����ṹ������
typedef struct
{
	float target_val;//Ŀ��ֵ
	float actual_val;//ʵ��ֵ
	float err;//ƫ��
	float err_last;//��һ��ƫ��
	float err_sum;//����ۼ�ֵ
	float Kp,Ki,Kd;//���������֣�΢��ϵ��
} tPid;

//��������
float P_realize(tPid*pid,float actual_val);
void PID_init(void);
float PI_realize(tPid*pid,float actual_val);
float PID_realize(tPid*pid,float actual_val);

#endif

