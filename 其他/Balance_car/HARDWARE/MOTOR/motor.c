#include "motor.h"


int motor1,motor2;
/*******************
 *  @brief  设置两个电机转速和方向
*  @param  motor1:电机B设置参数、motor2:设置参数
*  @param  motor1: 输入1~100 对应控制B电机正方向速度在1%-100%、输入-1~-100 对应控制B电机
反方向速度在1%-100%、motor2同理
*  @return  无
*
 *******************/
 void Motor_Set (int motor1, int motor2)
 {
//     motor1=motor1;
    //根据参数正负 设置选择方向
    if(motor1 < 0) BIN1_RESET;
       else      BIN1_SET;
    if(motor2 < 0) AIN1_RESET;
        else      AIN1_SET;
    
	 if(motor1 < 0)		    //motor1 设置电机B的转速
    {
        if(motor1 < -7200) motor1 = -7200;//超过PWM幅值
         __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, -motor1+100);//修改定时器1 通道1 PA8 Pulse改变占空比
    }
    else{
        if(motor1 > 7200) motor1 = 7200;
         __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 7200-motor1-100);//修改定时器1 通道1 PA8 Pulse改变占空比
    }
  
    if(motor2 < 0)    		//motor2 设置电机A的转速
    {
        if(motor2 < -7200) motor2 = -7200;//超过PWM幅值
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, -motor2+100);//修改定时器1 通道4 PA11 Pulse改变占空比
    }
    else
    {
        if(motor2 > 7200) motor2 = 7200;
         __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 7200-motor2-100);//修改定时器1 通道4 PA11 Pulse改变占空比
    }
    
 } 


