#include "main.h"
int main(void)
{
    RCC->CR |= 1 << 16;//����HSE
    while (((RCC->CR >> 17) & 1) == 0);//�ȴ�HSE׼��

    FLASH->ACR |= 2;    //����ϵͳʱ����FLASH��ȡ��ʱ

    RCC->CFGR &= ~(1 << 7);    //AHB  ����Ƶ
    RCC->CFGR &= ~(1 << 13);   //APB2 ����Ƶ
    RCC->CFGR |= 4 << 8;       //APB1 2��Ƶ

    RCC->CFGR |= 1 << 16;   //ѡ��HSE����PLL
    RCC->CFGR |= 7 << 18;   //PLL9��Ƶ

    RCC->CR |= 1 << 24;//����PLL
    while (((RCC->CR >> 25) & 1) == 0);//�ȴ�PLL׼��

    RCC->CFGR |= 2;//ѡ��PLL��Ϊϵͳʱ��
    while (((RCC->CFGR >> 2) & 3) != 2);//�ȴ�PLL׼����Ϊϵͳʱ��
    

    RCC->APB2ENR|=0x0d;  //��ABʱ�Ӻ�AFIO��
    AFIO->MAPR|=2<<24;//��ӳ�䣡����
    
    
    GPIOA->CRL=0x22222222;  //A0~7��ȫ���2M  //��
    GPIOA->CRH=0x44444224;  //A9 A10���  //��
    GPIOA->ODR|=1<<9;
    GPIOA->ODR|=1<<10;
    GPIOB->CRH&=~(1<<14);   //b11���  //��
    GPIOB->CRH|=1<<13;       
    
    GPIOB->CRL&=~(1<<14);  //b3  ��������
    GPIOB->CRL|=1<<15;  //��
    
    GPIOB->CRL&=~(1<<18);  //b4    xia������
    GPIOB->CRL|=1<<19;  //��
    
    
    GPIOB->ODR &= ~(1<<4);  //����
    GPIOB->ODR &= ~(1<<3);   //����
    
  
  while (1)
  {
     
  }
}