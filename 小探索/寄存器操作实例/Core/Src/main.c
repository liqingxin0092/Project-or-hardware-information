#include "main.h"
#include "smg.h"
uint16_t num;
void key1()
{

    if (((*(uint32_t *)(0X40010C00 + 0x08)) >> 3) & 1)
    {
        Delay_ms(20);
        if (((GPIOB->IDR) >> 3) & 1)
        {
            GPIOA->ODR &= ~(1 << 10);
            num += 1;
            if (num > 9)
                num = 0;

            Flash_write(0x0801FC00, num);

            while (((GPIOB->IDR) >> 3) & 1)
                ;
            GPIOA->ODR |= 1 << 10;
        }
        Delay_ms(30);
    }
}
void key2()
{
    if (((GPIOB->IDR) >> 4) & 1)
    {
        Delay_ms(30);
        if (((GPIOB->IDR) >> 4) & 1)
        {

            GPIOA->ODR &= ~(1 << 9);
            num--;
            if (num == 0xffff)
                num = 9;

            Flash_write(0x0801FC00, num);

            while (((GPIOB->IDR) >> 4) & 1)
                ;

            GPIOA->ODR |= 1 << 9;
        }
        Delay_ms(20);
    }
}

int main(void)
{
    RCC->CR |= 1 << 16; // ����HSE
    while (((RCC->CR >> 17) & 1) == 0)
        ; // �ȴ�HSE׼��

    FLASH->ACR |= 2; // ����ϵͳʱ����FLASH��ȡ��ʱ

    RCC->CFGR &= ~(1 << 7);  // AHB  ����Ƶ
    RCC->CFGR &= ~(1 << 13); // APB2 ����Ƶ
    RCC->CFGR |= 4 << 8;     // APB1 2��Ƶ

    RCC->CFGR |= 1 << 16; // ѡ��HSE����PLL
    RCC->CFGR |= 7 << 18; // PLL9��Ƶ

    RCC->CR |= 1 << 24; // ����PLL
    while (((RCC->CR >> 25) & 1) == 0)
        ; // �ȴ�PLL׼��

    RCC->CFGR |= 2; // ѡ��PLL��Ϊϵͳʱ��
    while (((RCC->CFGR >> 2) & 3) != 2)
        ; // �ȴ�PLL׼����Ϊϵͳʱ��

    RCC->APB2ENR |= 0x0d;  // ��ABʱ�Ӻ�AFIO��
    AFIO->MAPR |= 2 << 24; // ��ӳ�䣡����

    GPIOA->CRL = 0x22222222; // A0~7��ȫ���2M  //��
    GPIOA->CRH = 0x44444224; // A9 A10���  //��

    GPIOA->ODR |= 1 << 9;
    GPIOA->ODR |= 1 << 10;
    GPIOB->CRH &= ~(1 << 14); // b11���  //��
    GPIOB->CRH |= 1 << 13;

    GPIOB->CRL &= ~(1 << 14); // b3  ��������
    GPIOB->CRL |= 1 << 15;    // ��

    GPIOB->CRL &= ~(1 << 18); // b4    xia������
    GPIOB->CRL |= 1 << 19;    // ��

    GPIOB->ODR &= ~(1 << 4); // ����
    GPIOB->ODR &= ~(1 << 3); // ����

    num = FlashRead(0x0801FC00);
    if (num == 0xffff)
        num = 0;
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
        key1();
        key2();
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