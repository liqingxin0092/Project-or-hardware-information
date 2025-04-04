#include "main.h"

void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 180 * xus;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x00000005;
	while (!(SysTick->CTRL & (1 << 16)))
		;
	SysTick->CTRL = 0x00000004;
}
void Delay_ms(uint32_t xms)
{
	uint32_t i = xms;
	while (i--)
	{
		Delay_us(1000);
	}
}

