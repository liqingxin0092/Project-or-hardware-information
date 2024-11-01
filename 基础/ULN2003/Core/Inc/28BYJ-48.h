#ifndef __28BYJ_48_H
#define __28BYJ_48_H

#include "main.h"
struct ULN2003_t
{
    GPIO_TypeDef *ULN_PORT;
    uint16_t A_Pin;
    uint16_t B_Pin;
    uint16_t C_Pin;
    uint16_t D_Pin;
    void (*func)(struct ULN2003_t *temp, double angle, uint16_t external_delay_us, uint8_t turn_flag);
};
void ULN2003_Init(struct ULN2003_t *p);

void _3(struct ULN2003_t *temp, double angle, uint16_t external_delay_us, uint8_t turn_flag);

#endif
