#ifndef _SYS_H
#define _SYS_H

#include "stm32h7xx_hal.h"

// ����һЩ���õ��������Ͷ̹ؼ���
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t sc8;

typedef __IO int32_t vs32;
typedef __IO int16_t vs16;
typedef __IO int8_t vs8;

typedef __I int32_t vsc32;
typedef __I int16_t vsc16;
typedef __I int8_t vsc8;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t uc8;

typedef __IO uint32_t vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t vu8;

typedef __I uint32_t vuc32;
typedef __I uint16_t vuc16;
typedef __I uint8_t vuc8;
    
void Stm32_Clock_Init(u32 plln, u32 pllm, u32 pllp, u32 pllq); // ����ϵͳʱ��
void SystemClock_Config(void);
u8 Get_ICahceSta(void); // �ж�I_Cache�Ƿ��
u8 Get_DCahceSta(void); // �ж�I_Dache�Ƿ��
void Sys_Soft_Reset(void);
void MCO_test(void);
void MCO_test_IO_Init(void);
void JumpToBootloader(void);
void MSR_MSP(u32 addr);	 // ���ö�ջ��ַ
void set_votor(uint32_t addr);

/*event recorderʹ��*/
//EventRecorderInitialize(EventRecordAll, 1U);
//EventRecorderStart();


/*�����¼�*/
// EventStartA(0);
// �����Ĵ��벿��
// EventStopA(0);

/*��������*/
// uint32_t EventRecord2(uint32_t id,uint32_t val1,uint32_t val2 );

#endif
