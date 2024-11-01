#include "encoder.h"
#include "main.h"
#include "stm32f1xx_it.h"
#include "tim.h"

short Encode1Count = 0; // ������������ֵ
short Encode2Count = 0;
float Motor1Speed = 0.00;
float Motor2Speed = 0.00;
uint16_t TimerCount = 0;
extern float pitch, roll, yaw;	  // ŷ����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim1) // htim1 500HZ  2ms �ж�һ��
	{
		TimerCount++;
		if (TimerCount % 5 == 0) // ÿmsִ��һ��
		{
            Encode1Count = (short)__HAL_TIM_GET_COUNTER(&htim4);
            Motor1Speed = Encode1Count /1560.0f*100;
            Encode1Count = (short)__HAL_TIM_GET_COUNTER(&htim2);
            Motor1Speed = Encode1Count /-1560.0f*100;
            TIM4->CNT=0;
            TIM2->CNT=0;
            TimerCount = 0;
		}
	}
}
