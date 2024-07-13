#include "stm32f10x.h"                  // Device header
#include "Delay.h"
int main(void)
{
    RCC->APB2ENR|=RCC_APB2ENR_IOPBEN|RCC_APB2ENR_AFIOEN|RCC_APB2ENR_IOPAEN;  //����GPIOA,GPIOB��ʱ��
    
    AFIO->MAPR|=2<<24;//��ӳ�俪ʱ��
    
    GPIOB->CRH=0x44448444; // PB11��������
    GPIOB->ODR|=1<<11;
    
    GPIOA->CRH=0x24444444;// PA15����2MHZ
   
    AFIO->EXTICR[2] |=1<<12;//ѡ��PB11����
    
    EXTI->IMR|=1<<11;//������11�ϵ��ж�����
    EXTI->FTSR|=1<<11;//ѡ����11�ϵ��½��ش���
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//ע������һ��
        
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    while (1)
    {
        
        
    }
}
void EXTI15_10_IRQHandler()
{
    if(!((GPIOB->IDR>>11)&1))
    {
        Delay_ms(20);
        if(!((GPIOB->IDR>>11)&1))
        {
            GPIOA->ODR^=1<<15;
            while(!((GPIOB->IDR>>11)&1));
        }
        Delay_ms(20);
    } 
    EXTI->PR|=1<<11;
}
