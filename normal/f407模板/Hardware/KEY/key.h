#ifndef __KEY_H
#define __KEY_H

#include "main.h"

/*��ȡIO*/
#define KEY0 __BIT_READ(GPIOB_IDR_Addr, 9)
#define KEY1 __BIT_READ(GPIOB_IDR_Addr, 8)
#define WK_UP __BIT_READ(GPIOA_IDR_Addr, 0)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define WKUP_PRES 4

void KEY_Init(void); // IO��ʼ��
u8 KEY_Scan(u8);     // ����ɨ�躯��

#endif
