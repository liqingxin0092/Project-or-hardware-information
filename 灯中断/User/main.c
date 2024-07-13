#include "stm32f10x.h"                  // Device header
#include "Delay.h"
int main(void)
{
    RCC->APB2ENR|=RCC_APB2ENR_IOPBEN|RCC_APB2ENR_AFIOEN|RCC_APB2ENR_IOPAEN;  //开启GPIOA,GPIOB的时钟
    
    AFIO->MAPR|=2<<24;//重映射开时钟
    
    GPIOB->CRH=0x44448444; // PB11上拉输入
    GPIOB->ODR|=1<<11;
    
    GPIOA->CRH=0x24444444;// PA15推挽2MHZ
   
    AFIO->EXTICR[2] |=1<<12;//选择PB11引脚
    
    EXTI->IMR|=1<<11;//开放线11上的中断请求
    EXTI->FTSR|=1<<11;//选择线11上的下降沿触发
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//注意分组就一次
        
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
