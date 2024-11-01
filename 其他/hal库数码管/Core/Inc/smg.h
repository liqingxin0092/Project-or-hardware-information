#ifndef __SMG_H__
#define __SMG_H__

#include "main.h"

//0ÊÇÁÁ £¬1ÊÇÃð
#define RIGHT_UP_1()  GPIOA->ODR|=1<<0         //
#define RIGHT_UP_0()  GPIOA->ODR&=~(1<<0) 
#define UP_1()  GPIOA->ODR|=1<<1           //
#define UP_0()  GPIOA->ODR&=~(1<<1) 
#define LEFT_UP_1()  GPIOA->ODR|=1<<2 //
#define LEFT_UP_0()  GPIOA->ODR&=~(1<<2)
#define MID_1()  GPIOA->ODR|=1<<3       //
#define MID_0()  GPIOA->ODR&=~(1<<3)

#define POINT_1()  GPIOA->ODR|=1<<0   //
#define POINT_0()  GPIOA->ODR&=~(1<<0)

#define RIGHT_DOWN_1()  GPIOA->ODR|=1<<6//
#define RIGHT_DOWN_0()  GPIOA->ODR&=~(1<<6)

#define DOWN_1()  GPIOA->ODR|=1<<4 //
#define DOWN_0()  GPIOA->ODR&=~(1<<4)

#define LEFT_DOWN_1()  GPIOB->ODR|=1<<11  //
#define LEFT_DOWN_0()  GPIOB->ODR&=~(1<<11)

void zero(void);
void one(void);
void two(void);
void three(void);
void four(void);
void five(void);
void six(void);
void seven(void);
void eight(void);
void nine(void);
uint16_t  FlashRead(uint32_t add);
void Flash_write(uint32_t add,uint16_t data  );
void Delay_us(uint32_t xus);
void Delay_ms(uint32_t xms);
#endif
