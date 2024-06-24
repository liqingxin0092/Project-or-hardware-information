#ifndef __MATRIXKEY_H
#define __MATRIXKEY_H

#include "OLED.h"
uint8_t MatrixKey_GetValue(void);
int16_t Remapping(uint8_t keynum);
extern uint16_t num;
extern int8_t KeyArr[3];
extern uint8_t KeyNum;

void Get_Num(void);
#endif
