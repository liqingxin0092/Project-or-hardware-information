#include "key.h"
#include "stm32f10x.h"

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//иою╜	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);					
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIOC->BSRR=1<<13;
}
