#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "pid_ctrl.h"
#include "Motor.h"

extern float cur_angle;    //树莓派发的
static float last_angle;
float angular_speed;       //当前角速度
   
float k_opp;

extern short roll_speed; //陀螺仪
extern float roll;
extern float kd2; 
extern short acc;
       
/*电机的pid结构体*/
//输入:角速度  ---->  输出:CCR:  -1000~1000 
pid_t motor_pid_struct=
{
  /* kp */ 
    .kp= 0,
  /* ki */ 
    .ki=0.00, 
  /* kd */ 
    .kd=0.0,  
  /* tar_val */
    .tar_val= 0,    
  /* 积分误差累计值上限*/
    .inc_max= 0,
  /* 积分误差累计值下限*/
    .inc_min= 0,
  /* 积分加入的阈值*/
    .inc_use_threshold= 0,
  /* 输出值上限 */
    .out_max=999,
  /* 输出值下限 */
    .out_min=-999,
  /* 微分项过去权重 */
    .dpart=0,      
  /* 输出偏移 */
    .out_offset= 65,
  /* 输出死区(误差容忍) */
    .out_threshold= 0,             
};

//输入:角度  ---->  输出: 角速度 
pid_t angl_2_anglspeed_struct=
{
  /* kp */ 
    .kp= 0.0,
  /* ki */ 
    .ki=0.00, 
  /* kd */ 
    .kd=0.0,  
  /* tar_val */
    .tar_val= 0,       //目标角度是0
  /* 积分误差累计值上限*/
    .inc_max= 0,
  /* 积分误差累计值下限*/
    .inc_min= 0,
  /* 积分加入的阈值*/
    .inc_use_threshold= 0,
  /* 输出值上限 */
    .out_max=1,
  /* 输出值下限 */
    .out_min=-1,
  /* 微分项过去权重 */
    .dpart=0,      
  /* 输出偏移 */
    .out_offset= 0,
  /* 输出死区(误差容忍) */
    .out_threshold= 0,
};

/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);							
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
    
    GPIOB->ODR|=1<<14;
    GPIOB->ODR&=~(1<<15);
    
    GPIOA->ODR|=1<<6;
    GPIOA->ODR&=~(1<<7);
    
	PWM_Init();													
}

//-1000~1000
void Motor_SetSpeed1(int16_t Speed)
{
	if (Speed >= 0)							
	{
        GPIOB->ODR|=1<<15;
        GPIOB->ODR&=~(1<<14);
		TIM1->CCR1=	Speed;		
	}
	else								
	{
        GPIOB->ODR|=1<<14;
        GPIOB->ODR&=~(1<<15);
		TIM1->CCR1=	-Speed;	
	}
}

void Motor_SetSpeed4(int16_t Speed)
{
	if (Speed >= 0)							
	{
        GPIOA->ODR|=1<<6;
        GPIOA->ODR&=~(1<<7);
		TIM1->CCR4=	Speed;		
	}
	else								
	{
        GPIOA->ODR|=1<<7;
        GPIOA->ODR&=~(1<<6);
		TIM1->CCR4=	-Speed;	
	}
}

void Motor_SetSpeed(int16_t Speed)
{
//    Motor_SetSpeed1(Speed);
//    Motor_SetSpeed4(Speed);
    uint16_t temp=0;
	if (Speed >= 0)							
	{
        GPIOB->ODR|=1<<15;
        GPIOB->ODR&=~(1<<14);
		TIM1->CCR1=	Speed;
        TIM1->CCR4=	0;    
	}
    else
    {
        temp= -1.15*Speed;
        if(temp>999)
           temp = 999;
        
        GPIOA->ODR|=1<<7;
        GPIOA->ODR&=~(1<<6);
		TIM1->CCR4=	temp;
        TIM1->CCR1=	0;         
    }         
}
    
void Timer_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//开启TIM2的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 500 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);					//开启TIM2的更新中断
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//选择配置NVIC的TIM2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM2, ENABLE);			//使能TIM2，定时器开始运行
}

void TIM2_IRQHandler(void)     //0.005s        
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{		
//        angular_speed =cur_angle-last_angle;
//        last_angle=cur_angle;
        
        Motor_SetSpeed(pid_control_gunzi(&motor_pid_struct,kd2,roll,roll_speed,acc));
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
