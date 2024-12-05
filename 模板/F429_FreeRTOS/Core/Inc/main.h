#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx_hal.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef __IO uint32_t vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t vu8;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;


/*IO映射*/
#define GPIOA_ODR_Addr 0x40020014
#define GPIOB_ODR_Addr 0x40020414
#define GPIOC_ODR_Addr 0x40020814
#define GPIOD_ODR_Addr 0x40020C14
#define GPIOE_ODR_Addr 0x40021014
#define GPIOF_ODR_Addr 0x40021414
#define GPIOG_ODR_Addr 0x40021814
#define GPIOH_ODR_Addr 0x40021C14
#define GPIOI_ODR_Addr 0x40022014

#define GPIOA_IDR_Addr 0x40020010
#define GPIOB_IDR_Addr 0x40020410
#define GPIOC_IDR_Addr 0x40020810
#define GPIOD_IDR_Addr 0x40020C10
#define GPIOE_IDR_Addr 0x40021010
#define GPIOF_IDR_Addr 0x40021410
#define GPIOG_IDR_Addr 0x40021810
#define GPIOH_IDR_Addr 0x40021C10
#define GPIOI_IDR_Addr 0x40022010

/*位域操作,注意这个只针对0x4000 0000的区域!*/
#define __BIT_SET(WORD_ADDR, BIT_ADDR, BIT) *(volatile uint32_t *)(0x42000000 + (((WORD_ADDR) & 0x0fffffff) << 5) + ((BIT_ADDR) << 2)) = (BIT)
#define __BIT_READ(WORD_ADDR, BIT_ADDR) *(volatile uint32_t *)(0x42000000 + (((WORD_ADDR) & 0x0fffffff) << 5) + ((BIT_ADDR) << 2))
#define __BIT(WORD_ADDR, BIT_ADDR) *(volatile uint32_t *)(0x42000000 + (((WORD_ADDR) & 0x0fffffff) << 5) + ((BIT_ADDR) << 2))
    
#define KEY0 __BIT(GPIOH_IDR_Addr, 3)

void Error_Handler(void);


#endif /* __MAIN_H */
