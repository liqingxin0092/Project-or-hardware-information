#include "control.h"
#include "stm32f1xx_hal.h"
#include "motor.h"
#include "OLED.h"
#include "inv_mpu.h"
#include "pid.h"
float Vertical_Kp = -200; //-1200��d
float Vertical_Kd = -0.2;

float Velocity_Kp = 0;
float Velocity_Ki = 0;

extern float Motor1Speed ;
extern float Motor2Speed ;
int32_t PWM_OUT = 0;

extern float pitch, roll, yaw;	  // ŷ����
extern short gyrox, gyroy, gyroz; // ������ԭʼ����

float Med_Angle = 0;
int Vertical_out, Velocity_out, Turn_out;

/*****************************************
�ⲿ�жϻص�����
******************************************/
extern short gyro[3];
extern pid_t wheel;
extern float Motor1Speed;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_15)
	{
		mpu_dmp_get_data(&pitch, &roll, &yaw); // ��ȡmpu6050�Ƕ�����
		Vertical_out=Vertical(Med_Angle,roll,gyroy);       //ֱ����
		Velocity_out=Velocity(Motor2Speed,Motor1Speed);  //�ٶȻ�
		Turn_out=Turn(gyroz);                              //ת��
		
		PWM_OUT=Vertical_out-Vertical_Kp*Velocity_out;

		//3.�ѿ�����������ص������

//		MOTOR_1=PWM_OUT-Turn_out;
//		MOTOR_2=PWM_OUT+Turn_out;

		Motor_Set (PWM_OUT,PWM_OUT);
	}
}

/*****************************************
ֱ����PD��������Kp*Ek+Kd*Ek_D
��ڣ������Ƕȡ���ʵ�Ƕȡ���ʵ���ٶ�
���ڣ�ֱ�������
******************************************/
int Vertical(float Med, float Angle, float gyro_Y)
{
	int PWM_out;
	PWM_out = Vertical_Kp * Angle + Vertical_Kd * (gyro_Y - 0);
	return PWM_out;
}

/*****************************************
ֱ����PI:Kp*Ek+Ki*Ek_S
��ͨ�˲���ʹ���θ���ƽ����������Ƶ���ţ���ֹͻ��
******************************************/
int Velocity(int Encoder_left, int Encoder_right)
{
	static int PWM_out, Encoder_Err, Encoder_S;
	static int EnC_Err_Lowout, EnC_Err_Lowout_Last;
	float a = 0.7;

	Encoder_Err = (Encoder_left + Encoder_right) - 0;
	// ���ٶȽ��е�ͨ�˲�
	// low_out=(1-a)*Ek+a*low_out_last
	EnC_Err_Lowout = (1 - a) * Encoder_Err + a * EnC_Err_Lowout_Last;
	EnC_Err_Lowout_Last = EnC_Err_Lowout;
	// ���ٶ�ƫ����л���
	Encoder_S += EnC_Err_Lowout;
	Encoder_S = Encoder_S > 10000 ? 10000 : (Encoder_S < -10000 ? -10000 : Encoder_S);
	// �ٶȻ������������
	PWM_out = Velocity_Kp * Encoder_Err + Velocity_Ki * Encoder_S;
	return PWM_out;
}

/*****************************************
ת�򻷣�ϵ��*Z��Ƕ�
******************************************/
int Turn(int gyro_Z)
{
	int PWM_out;
	PWM_out = (-0.5) * gyro_Z;
	return PWM_out;
}
