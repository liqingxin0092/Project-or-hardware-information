#include "main.h"
#include "smg.h"
uint16_t num;
int main(void)
{
    RCC->CR|=1<<16;
    while(((RCC->CR>>17)&1)==0);
    FLASH->ACR |= 2; 
    RCC->CFGR &= ~(1<<7);   
    RCC->CFGR &= ~(1<<13);   
    RCC->CFGR |= 4<<8; 
    RCC->CFGR|=1<<16;  
    RCC->CFGR|=7<<18;   
    RCC->CR|=1<<24;
    while(((RCC->CR>>25)&1)==0);
    RCC->CFGR|=2;
    while(((RCC->CFGR>>2)&3)!=2);
    RCC->APB2ENR|=0x0c; 
    GPIOA->CRL=0x22222222;
    GPIOA->CRH=0x44444422;
    GPIOB->CRH&=~(1<<14);
    GPIOB->CRH|=1<<13; 
    GPIOB->CRL&=~(1<<14);
    GPIOB->CRL=1<<15;
    GPIOB->CRL&=~(1<<14);
    GPIOB->CRL=1<<15;
    GPIOB->CRL&=~(1<<18);
    GPIOB->CRL=1<<19;
    num=  FlashRead(0x0801FC00);
    if(num==0xffff)
        num=0;
      switch (num)
      {
          case 0:
              zero();
          break;
          case 1:
              one();
          break;
          case 2:
              two();
          break;
          case 3:
              three();
          break;
          case 4:
              four();
          break;
          case 5:
              five();
          break;
          case 6:
              six();
          break;
          case 7:
              seven();
          break;
          case 8:
              eight();
          break;
          case 9:
              nine();
          break;
      }
  while (1)
  {
    if((((GPIOB->IDR)>>3)&1)==1)
    {
        Delay_ms(20);
        if((((GPIOB->IDR)>>3)&1)==1)
        {
            GPIOA->ODR&=~(1<<10);
            num++;
            if(num==10)
                num=0;
        Flash_write(0x0801FC00,num);
            while((((GPIOB->IDR)>>3)&1)==1);
            GPIOA->ODR|=1<<10;
        }
        Delay_ms(30);
    }
    if((((GPIOB->IDR)>>4)&1)==1)
    {
        Delay_ms(30);
        if((((GPIOB->IDR)>>4)&1)==1)
        {
            GPIOA->ODR&=~(1<<9);
            num--;
            if(num==0xffff)
                num=9;
            Flash_write(0x0801FC00,num);
            while((((GPIOB->IDR)>>4)&1)==1);
            GPIOA->ODR|=1<<9;
        }
        Delay_ms(20);
    }
      switch (num)
      {
          case 0:
              zero();
          break;
          case 1:
              one();
          break;
          case 2:
              two();
          break;
          case 3:
              three();
          break;
          case 4:
              four();
          break;
          case 5:
              five();
          break;
          case 6:
              six();
          break;
          case 7:
              seven();
          break;
          case 8:
              eight();
          break;
          case 9:
              nine();
          break;
      }
  }
}