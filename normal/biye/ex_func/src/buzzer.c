#include "stm32f10x.h" 
#include "buzzer.h"
 
void buzzer_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    BUZZER_OFF();
}

