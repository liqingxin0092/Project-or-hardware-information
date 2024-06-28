#include "main.h"
int main(void)
{
    RCC->CR |= 1 << 16;//开启HSE
    while (((RCC->CR >> 17) & 1) == 0);//等待HSE准备

    FLASH->ACR |= 2;    //配置系统时钟与FLASH读取延时

    RCC->CFGR &= ~(1 << 7);    //AHB  不分频
    RCC->CFGR &= ~(1 << 13);   //APB2 不分频
    RCC->CFGR |= 4 << 8;       //APB1 2分频

    RCC->CFGR |= 1 << 16;   //选择HSE输入PLL
    RCC->CFGR |= 7 << 18;   //PLL9倍频

    RCC->CR |= 1 << 24;//开启PLL
    while (((RCC->CR >> 25) & 1) == 0);//等待PLL准备

    RCC->CFGR |= 2;//选择PLL作为系统时钟
    while (((RCC->CFGR >> 2) & 3) != 2);//等待PLL准备作为系统时钟
    

    RCC->APB2ENR|=0x0d;  //开AB时钟和AFIO！
    AFIO->MAPR|=2<<24;//重映射！！！
    
    
    GPIOA->CRL=0x22222222;  //A0~7口全输出2M  //对
    GPIOA->CRH=0x44444224;  //A9 A10输出  //对
    GPIOA->ODR|=1<<9;
    GPIOA->ODR|=1<<10;
    GPIOB->CRH&=~(1<<14);   //b11输出  //对
    GPIOB->CRH|=1<<13;       
    
    GPIOB->CRL&=~(1<<14);  //b3  下拉输入
    GPIOB->CRL|=1<<15;  //对
    
    GPIOB->CRL&=~(1<<18);  //b4    xia拉输入
    GPIOB->CRL|=1<<19;  //对
    
    
    GPIOB->ODR &= ~(1<<4);  //下拉
    GPIOB->ODR &= ~(1<<3);   //下拉
    
  
  while (1)
  {
     
  }
}