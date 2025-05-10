#include "stm32f10x.h"                  // Device header
#include "uart.h"
#include "usb_device.h"

int main(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    RCC->APB2ENR|=1;         
    AFIO->MAPR|= 0x2000000;	    //½â³ýjtag
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Config(72000);//1ms
    
    USART1_Config(115200);
    usb_init();
    
	while (1)
	{
		
	}
}
