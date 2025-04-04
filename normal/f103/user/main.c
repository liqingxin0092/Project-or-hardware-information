#include "stm32f10x.h"                  // Device header
#include "Delay.h"


int main(void)
{
   RCC->APB2ENR|=2;  //  ¿ªa 
   GPIOA->CRL|= 1;   //
    
   while(1)
   {
      GPIOA->ODR^=1; 
      Delay_ms(100);  
   }    
}

