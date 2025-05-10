#ifndef __BUZZER_H
#define __BUZZER_H

#include "FreeRTOS.h"

#define BUZZER_ON()  GPIOB->BRR = 0x8
#define BUZZER_OFF() GPIOB->BSRR= 0x8

#define BUZZER_ALARM(time) do{\
GPIOB->BRR = 0x8;\
vTaskDelay(time);\
GPIOB->BSRR= 0x8; \
}while(0)                    

void buzzer_init(void);

#endif 
