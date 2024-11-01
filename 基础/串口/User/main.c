#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "UART.h"
#include "stdio.h"
uint8_t s;
int main(void)
{
    NVIC_Config();
    UART_Init();
    printf("hello world,%d %g ",50,3.6);
    
    while (1)
    {
       
    }
}
