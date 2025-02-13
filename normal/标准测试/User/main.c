#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t s;
int main(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_Initstructure;
    GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Initstructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_Initstructure);
    while (1)
    {
        GPIOC->ODR^= 1 << 13;
    }
	
}
