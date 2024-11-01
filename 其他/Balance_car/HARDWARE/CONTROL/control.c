#include "control.h"
#include "stm32f1xx_hal.h"
#include "motor.h"
#include "OLED.h"
#include "inv_mpu.h"
#include "pid.h"
float Vertical_Kp = -200; //-1200无d
float Vertical_Kd = -0.2;

float Velocity_Kp = 0;
float Velocity_Ki = 0;

extern float Motor1Speed ;
extern float Motor2Speed ;
int32_t PWM_OUT = 0;

extern float pitch, roll, yaw;	  // 欧拉角
extern short gyrox, gyroy, gyroz; // 陀螺仪原始数据

float Med_Angle = 0;
int Vertical_out, Velocity_out, Turn_out;

/*****************************************
外部中断回调函数
******************************************/
extern short gyro[3];
extern pid_t wheel;
extern float Motor1Speed;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_15)
	{
		mpu_dmp_get_data(&pitch, &roll, &yaw); // 获取mpu6050角度数据
		Vertical_out=Vertical(Med_Angle,roll,gyroy);       //直立环
		Velocity_out=Velocity(Motor2Speed,Motor1Speed);  //速度环
		Turn_out=Turn(gyroz);                              //转向环
		
		PWM_OUT=Vertical_out-Vertical_Kp*Velocity_out;

		//3.把控制输出量加载到电机上

//		MOTOR_1=PWM_OUT-Turn_out;
//		MOTOR_2=PWM_OUT+Turn_out;

		Motor_Set (PWM_OUT,PWM_OUT);
	}
}

/*****************************************
直立环PD控制器：Kp*Ek+Kd*Ek_D
入口：期望角度、真实角度、真实角速度
出口：直立环输出
******************************************/
int Vertical(float Med, float Angle, float gyro_Y)
{
	int PWM_out;
	PWM_out = Vertical_Kp * Angle + Vertical_Kd * (gyro_Y - 0);
	return PWM_out;
}

/*****************************************
直立环PI:Kp*Ek+Ki*Ek_S
低通滤波：使波形更加平滑，消除高频干扰，防止突变
******************************************/
int Velocity(int Encoder_left, int Encoder_right)
{
	static int PWM_out, Encoder_Err, Encoder_S;
	static int EnC_Err_Lowout, EnC_Err_Lowout_Last;
	float a = 0.7;

	Encoder_Err = (Encoder_left + Encoder_right) - 0;
	// 对速度进行低通滤波
	// low_out=(1-a)*Ek+a*low_out_last
	EnC_Err_Lowout = (1 - a) * Encoder_Err + a * EnC_Err_Lowout_Last;
	EnC_Err_Lowout_Last = EnC_Err_Lowout;
	// 对速度偏差进行积分
	Encoder_S += EnC_Err_Lowout;
	Encoder_S = Encoder_S > 10000 ? 10000 : (Encoder_S < -10000 ? -10000 : Encoder_S);
	// 速度环控制输出计算
	PWM_out = Velocity_Kp * Encoder_Err + Velocity_Ki * Encoder_S;
	return PWM_out;
}

/*****************************************
转向环：系数*Z轴角度
******************************************/
int Turn(int gyro_Z)
{
	int PWM_out;
	PWM_out = (-0.5) * gyro_Z;
	return PWM_out;
}
