/**
 *******************************************************************************
 * @file        gpio.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-20
 * @brief       配置,控制GPIO
 *              
 *****************************************************************************************
 */
 
#ifndef __GPIO_H
#define __GPIO_H

#include "stdint.h"

typedef struct
{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint16_t BSR;
    volatile uint16_t BRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFRL;
    volatile uint32_t AFRH;
}gpio_t;

typedef struct 
{
  gpio_t *GPIOx;
  uint8_t pin;
}gpio_group_t;

#define GPIO_A ((gpio_t *)0x58020000)
#define GPIO_B ((gpio_t *)0x58020400)
#define GPIO_C ((gpio_t *)0x58020800)
#define GPIO_D ((gpio_t *)0x58020c00)
#define GPIO_E ((gpio_t *)0x58021000)
#define GPIO_F ((gpio_t *)0x58021400)
#define GPIO_G ((gpio_t *)0x58021800)
#define GPIO_H ((gpio_t *)0x58021c00)
#define GPIO_I ((gpio_t *)0x58022000)
#define GPIO_J ((gpio_t *)0x58022400)
#define GPIO_K ((gpio_t *)0x58022800)

/*设置引脚输出*/
#define GPIO_OUT(GPIOx,pin,num) (num)?(GPIOx->BSR=1<<(pin)):(GPIOx->BRR=1<<(pin))

/*设置引脚输入*/
#define GPIO_IN(GPIOx,pin)  (uint8_t)(((GPIOx->IDR)&(1<<(pin)))?1:0)

/*设置引脚*/
void gpio_init_pin(gpio_t* GPIOx,uint8_t pin,uint8_t mode,uint8_t out_type,uint8_t speed,uint8_t up_or_down,uint8_t alterfun);
void gpio_lock_pin(gpio_t* GPIOx,uint16_t pin_group);

/*应用层*/
void RCC_GPIO_Init(void);

#endif
