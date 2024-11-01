#include "motor.h"


int motor1,motor2;
/*******************
 *  @brief  �����������ת�ٺͷ���
*  @param  motor1:���B���ò�����motor2:���ò���
*  @param  motor1: ����1~100 ��Ӧ����B����������ٶ���1%-100%������-1~-100 ��Ӧ����B���
�������ٶ���1%-100%��motor2ͬ��
*  @return  ��
*
 *******************/
 void Motor_Set (int motor1, int motor2)
 {
//     motor1=motor1;
    //���ݲ������� ����ѡ����
    if(motor1 < 0) BIN1_RESET;
       else      BIN1_SET;
    if(motor2 < 0) AIN1_RESET;
        else      AIN1_SET;
    
	 if(motor1 < 0)		    //motor1 ���õ��B��ת��
    {
        if(motor1 < -7200) motor1 = -7200;//����PWM��ֵ
         __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, -motor1+100);//�޸Ķ�ʱ��1 ͨ��1 PA8 Pulse�ı�ռ�ձ�
    }
    else{
        if(motor1 > 7200) motor1 = 7200;
         __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 7200-motor1-100);//�޸Ķ�ʱ��1 ͨ��1 PA8 Pulse�ı�ռ�ձ�
    }
  
    if(motor2 < 0)    		//motor2 ���õ��A��ת��
    {
        if(motor2 < -7200) motor2 = -7200;//����PWM��ֵ
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, -motor2+100);//�޸Ķ�ʱ��1 ͨ��4 PA11 Pulse�ı�ռ�ձ�
    }
    else
    {
        if(motor2 > 7200) motor2 = 7200;
         __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 7200-motor2-100);//�޸Ķ�ʱ��1 ͨ��4 PA11 Pulse�ı�ռ�ձ�
    }
    
 } 


