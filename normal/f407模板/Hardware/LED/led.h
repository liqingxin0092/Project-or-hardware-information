#ifndef __LED_H
#define __LED_H

#include "main.h"

#define LED1 __BIT_READ(GPIOF_IDR_Add,r9)	  // D1
#define LED2 __BIT_READ(GPIOF_IDR_Addr,10)	// D2	

void LED_Init(void); // ≥ı ºªØ
void set_LED1(uint8_t state);
void set_LED2(uint8_t state);

#endif
