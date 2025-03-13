/**
 *******************************************************************************
 * @file        gpio.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-20
 * @brief       配置,控制GPIO
 *              
 *****************************************************************************************
 */
#include "gpio.h"
#include "stm32h7xx_hal.h"

/*设置一个引脚*/
//mode:00,输入模式
//     01,输出模式
//     10,复用模式
//     11,模拟模式
//out_type:0,推挽输出
//         1,开漏输出
//speed:00,low
//      01,medium
//      10,high
//      11,very high
//up_or_down:00,浮空
//           01,上拉
//           10,下拉
//alterfun:0000~1111 --> AF0~AF15
void gpio_init_pin(gpio_t* GPIOx,uint8_t pin,uint8_t mode,uint8_t out_type,uint8_t speed,uint8_t up_or_down,uint8_t alterfun)
{
    GPIOx->MODER&=~(3<<(pin<<1));//模式对应位清除复位值
    GPIOx->MODER|=mode<<(pin<<1);//模式设置
    GPIOx->OTYPER|=out_type<<pin;//输出类型设置
    GPIOx->OSPEEDR&=~(3<<(pin<<1));//速度对应位清除复位值
    GPIOx->OSPEEDR|=speed<<(pin<<1);//速度设置
    GPIOx->PUPDR&=~(3<<(pin<<1));//上下拉对应位清除复位值
    GPIOx->PUPDR|=up_or_down<<(pin<<1);//上下拉设置
    if(0<=pin&&pin<=7)
        GPIOx->AFRL|=alterfun<<(pin<<2);
    else 
        GPIOx->AFRH|=alterfun<<((pin-8)<<2);
}
 
/*锁定引脚配置(锁定对应控制和功能寄存器)*/
//pin_group: msb ~~ lsb , 1111 1111 1111 1111 ,锁哪个,对应位置1
void gpio_lock_pin(gpio_t* GPIOx,uint16_t pin_group)
{
    uint32_t key=0x10000|(uint32_t)pin_group;
    GPIOx->LCKR=key;
    GPIOx->LCKR=(uint32_t)pin_group;
    GPIOx->LCKR=key;
    (void)GPIOx->LCKR;
    while(!GPIOx->LCKR);
}

/*应用层*/
void RCC_GPIO_Init(void)
{
    /*IO时钟使能*/
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    gpio_init_pin(GPIO_C,13,1,0,2,0,0);//gpioc 13 推挽输出,测试灯
    
}



