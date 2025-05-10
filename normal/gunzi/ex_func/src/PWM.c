#include "stm32f10x.h"                  // Device header

/**
  * 函    数：PWM初始化
  * 参    数：无
  * 返 回 值：无
  */
void PWM_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
	TIM_OCInitTypeDef  TIM_OCInitStructure={0};
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);	//使能定时器1时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11; //TIM_CH1  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM1
	TIM_TimeBaseStructure.TIM_Period = 1000-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =2-1; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
    
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
    
	//初始化TIM Channel 1-4 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity =   TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC1
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC4

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);//使能或者失能 TIMx 在 CCRx 上的预装载寄存器
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~100
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare);		//设置CCR3的值
}
