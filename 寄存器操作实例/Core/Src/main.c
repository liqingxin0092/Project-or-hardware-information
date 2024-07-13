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
    RCC->CR |= 1 << 16; // 开启HSE
    while (((RCC->CR >> 17) & 1) == 0)
        ; // 等待HSE准备

    FLASH->ACR |= 2; // 配置系统时钟与FLASH读取延时

    RCC->CFGR &= ~(1 << 7);  // AHB  不分频
    RCC->CFGR &= ~(1 << 13); // APB2 不分频
    RCC->CFGR |= 4 << 8;     // APB1 2分频

    RCC->CFGR |= 1 << 16; // 选择HSE输入PLL
    RCC->CFGR |= 7 << 18; // PLL9倍频

    RCC->CR |= 1 << 24; // 开启PLL
    while (((RCC->CR >> 25) & 1) == 0)
        ; // 等待PLL准备

    RCC->CFGR |= 2; // 选择PLL作为系统时钟
    while (((RCC->CFGR >> 2) & 3) != 2)
        ; // 等待PLL准备作为系统时钟

    RCC->APB2ENR |= 0x0d;  // 开AB时钟和AFIO！
    AFIO->MAPR |= 2 << 24; // 重映射！！！

    GPIOA->CRL = 0x22222222; // A0~7口全输出2M  //对
    GPIOA->CRH = 0x44444224; // A9 A10输出  //对

    GPIOA->ODR |= 1 << 9;
    GPIOA->ODR |= 1 << 10;
    GPIOB->CRH &= ~(1 << 14); // b11输出  //对
    GPIOB->CRH |= 1 << 13;

    GPIOB->CRL &= ~(1 << 14); // b3  下拉输入
    GPIOB->CRL |= 1 << 15;    // 对

    GPIOB->CRL &= ~(1 << 18); // b4    xia拉输入
    GPIOB->CRL |= 1 << 19;    // 对

    GPIOB->ODR &= ~(1 << 4); // 下拉
    GPIOB->ODR &= ~(1 << 3); // 下拉

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