/**
 *******************************************************************************
 * @file        gpio.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-20
 * @brief       ����,����GPIO
 *              
 *****************************************************************************************
 */
#include "gpio.h"
#include "stm32h7xx_hal.h"

/*����һ������*/
//mode:00,����ģʽ
//     01,���ģʽ
//     10,����ģʽ
//     11,ģ��ģʽ
//out_type:0,�������
//         1,��©���
//speed:00,low
//      01,medium
//      10,high
//      11,very high
//up_or_down:00,����
//           01,����
//           10,����
//alterfun:0000~1111 --> AF0~AF15
void gpio_init_pin(gpio_t* GPIOx,uint8_t pin,uint8_t mode,uint8_t out_type,uint8_t speed,uint8_t up_or_down,uint8_t alterfun)
{
    GPIOx->MODER&=~(3<<(pin<<1));//ģʽ��Ӧλ�����λֵ
    GPIOx->MODER|=mode<<(pin<<1);//ģʽ����
    GPIOx->OTYPER|=out_type<<pin;//�����������
    GPIOx->OSPEEDR&=~(3<<(pin<<1));//�ٶȶ�Ӧλ�����λֵ
    GPIOx->OSPEEDR|=speed<<(pin<<1);//�ٶ�����
    GPIOx->PUPDR&=~(3<<(pin<<1));//��������Ӧλ�����λֵ
    GPIOx->PUPDR|=up_or_down<<(pin<<1);//����������
    if(0<=pin&&pin<=7)
        GPIOx->AFRL|=alterfun<<(pin<<2);
    else 
        GPIOx->AFRH|=alterfun<<((pin-8)<<2);
}
 
/*������������(������Ӧ���ƺ͹��ܼĴ���)*/
//pin_group: msb ~~ lsb , 1111 1111 1111 1111 ,���ĸ�,��Ӧλ��1
void gpio_lock_pin(gpio_t* GPIOx,uint16_t pin_group)
{
    uint32_t key=0x10000|(uint32_t)pin_group;
    GPIOx->LCKR=key;
    GPIOx->LCKR=(uint32_t)pin_group;
    GPIOx->LCKR=key;
    (void)GPIOx->LCKR;
    while(!GPIOx->LCKR);
}

/*Ӧ�ò�*/
void RCC_GPIO_Init(void)
{
    /*IOʱ��ʹ��*/
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    gpio_init_pin(GPIO_C,13,1,0,2,0,0);//gpioc 13 �������,���Ե�
    
}



