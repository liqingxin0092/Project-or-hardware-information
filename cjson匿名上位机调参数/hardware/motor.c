#include "motor.h"
#include "tim.h"
#include "pid.h"
#include "stm32f4xx_it.h"

extern tPid pidMotorASpeed;
extern tPid pidMotorBSpeed;

void Motor_Set(int MotorA,int MotorB)//���֣�����
{
  //Forward and reverse control of motor
	//�������ת����
	if(MotorA<0) //��		
	{	
		if(MotorA<-16799)
			MotorA = -16799;
		__HAL_TIM_SET_COMPARE(&htim10,TIM_CHANNEL_1,0);
		__HAL_TIM_SET_COMPARE(&htim11,TIM_CHANNEL_1,-MotorA);
	}
	else 	
	{
		if(MotorA>16799)
			MotorA = 16799;
		__HAL_TIM_SET_COMPARE(&htim11,TIM_CHANNEL_1,0);
		__HAL_TIM_SET_COMPARE(&htim10,TIM_CHANNEL_1,MotorA);
	}
	
	//Forward and reverse control of motor
	//�������ת����	
	if(MotorB<0)		//��
	{	
        if(MotorB<-16799)
			MotorB = -16799;
		__HAL_TIM_SET_COMPARE(&htim9,TIM_CHANNEL_1,0);
		__HAL_TIM_SET_COMPARE(&htim9,TIM_CHANNEL_2,-MotorB);
	}
	else 	
	{
		if(MotorB>16799)
			MotorB = 16799;
		__HAL_TIM_SET_COMPARE(&htim9,TIM_CHANNEL_2,0);
		__HAL_TIM_SET_COMPARE(&htim9,TIM_CHANNEL_1,MotorB);
	}
  
}

void motorPidSetSpeed(float MotorASetSpeed,float MotorBSetSpeed)    //(���֣�����)
{
    //����pidĿ��ת��
  	pidMotorASpeed.target_val=MotorBSetSpeed;  //����
  	pidMotorBSpeed.target_val=MotorASetSpeed;  //����
    //pid������Ƶ�� //                          ����                                          ����
    Motor_Set((int)PID_realize(&pidMotorASpeed,MotorBSpeed),(int)PID_realize(&pidMotorBSpeed,MotorASpeed));
} //                                       ����                                        ����

