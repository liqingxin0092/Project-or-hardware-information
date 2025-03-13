#ifndef _SYS_H
#define _SYS_H

#include "stm32h7xx_hal.h"

// 定义一些常用的数据类型短关键字
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
    
void Stm32_Clock_Init(u32 plln, u32 pllm, u32 pllp, u32 pllq); // 配置系统时钟
void SystemClock_Config(void);
u8 Get_ICahceSta(void); // 判断I_Cache是否打开
u8 Get_DCahceSta(void); // 判断I_Dache是否打开
void Sys_Soft_Reset(void);
void MCO_test(void);
void MCO_test_IO_Init(void);
void JumpToBootloader(void);
void MSR_MSP(u32 addr);	 // 设置堆栈地址
void set_votor(uint32_t addr);

/*event recorder使能*/
//EventRecorderInitialize(EventRecordAll, 1U);
//EventRecorderStart();


/*测量事件*/
// EventStartA(0);
// 测量的代码部分
// EventStopA(0);

/*发送数字*/
// uint32_t EventRecord2(uint32_t id,uint32_t val1,uint32_t val2 );

#endif
