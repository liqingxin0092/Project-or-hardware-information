#include "stm32f10x.h"                  // Device header
#include "math.h"
#include "delay.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

int main(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC->APB2ENR|=1;         
    AFIO->MAPR|= 0x2000000;	    //解除jtag
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Config(72000);//1ms
    
    USART1_Config();  

	while (1)
	{
  
         
	}
}
