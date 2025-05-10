#include "motor.h"
#include "stm32f10x.h"
#include "pid_ctrl.h"

/*�����pid�ṹ��*/
//ת��:0~100   ---->  CCR:0~100
pid_t motor_pid_struct=
{
  /* kp */ 
    .kp= 1.09,
  /* ki */ 
    .ki=0.0014, 
  /* kd */ 
    .kd=0.05,  
  /* tar_val */
    .tar_val= 0,    
  /* ��������ۼ�ֵ����*/
    .inc_max= 0,
  /* ��������ۼ�ֵ����*/
    .inc_min= 0,
  /* ���ּ������ֵ*/
    .inc_use_threshold= 40,
  /* ���ֵ���� */
    .out_max=40,
  /* ���ֵ���� */
    .out_min= 0,
  /* ΢�����ȥȨ�� */
    .dpart=0,      
  /* ���ƫ�� */
    .out_offset= 0,
  /* �������(�������) */
    .out_threshold= 0,
};

void motor_init(void)
{
	/*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//����TIM2��ʱ��
    
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						
																	//��������Ƶ����ţ�����Ҫ����Ϊ����ģʽ	
    GPIOA->ODR|=1<<12;
    GPIOA->ODR&=~(1<<15);
	/*����ʱ��Դ*/
	TIM_InternalClockConfig(TIM2);		//ѡ��TIM2Ϊ�ڲ�ʱ�ӣ��������ô˺�����TIMĬ��ҲΪ�ڲ�ʱ��
	
	/*ʱ����Ԫ��ʼ��*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//����ṹ�����
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //ʱ�ӷ�Ƶ��ѡ�񲻷�Ƶ���˲������������˲���ʱ�ӣ���Ӱ��ʱ����Ԫ����
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //������ģʽ��ѡ�����ϼ���
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                 //�������ڣ���ARR��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;               //Ԥ��Ƶ������PSC��ֵ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //�ظ����������߼���ʱ���Ż��õ�
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);             //���ṹ���������TIM_TimeBaseInit������TIM2��ʱ����Ԫ
	
	/*����Ƚϳ�ʼ��*/ 
	TIM_OCInitTypeDef TIM_OCInitStructure;							//����ṹ�����
	TIM_OCStructInit(&TIM_OCInitStructure);                         //�ṹ���ʼ�������ṹ��û��������ֵ
	                                                                //�����ִ�д˺��������ṹ�����г�Ա����һ��Ĭ��ֵ
	                                                                //����ṹ���ֵ��ȷ��������
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //����Ƚ�ģʽ��ѡ��PWMģʽ1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //������ԣ�ѡ��Ϊ�ߣ���ѡ����Ϊ�ͣ�������ߵ͵�ƽȡ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //���ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;								//��ʼ��CCRֵ
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);                        //���ṹ���������TIM_OC3Init������TIM2������Ƚ�ͨ��3
	
	/*TIMʹ��*/
	TIM_Cmd(TIM2, ENABLE);			//ʹ��TIM2����ʱ����ʼ����
}


void encoder_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM3->ARR=0xffff;
//    TIM3->PSC=1;
    TIM3->CCMR1|=1      //IC1ӳ����TI1,��Ҫ��ѡ������
               |(0xf<<4);  //�����˲�
//    TIM3->CCER |=1<<1;  //�����½���
    TIM3->SMCR |=7      //�ⲿʱ��ģʽ1
               |(5<<4); //ѡ��TI1FP1
    TIM3->CR1  |=1;//en
}   

