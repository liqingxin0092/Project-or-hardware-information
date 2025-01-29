#ifndef _SYS_H
#define _SYS_H
#include "stm32h7xx.h"
#include "core_cm7.h"
#include "stm32h7xx_hal.h"
#include "stdint.h"
// 0,不支持os
// 1,支持os
#define SYSTEM_SUPPORT_OS 1 // 定义系统文件夹是否支持OS
///////////////////////////////////////////////////////////////////////////////////
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

#define GPIO_A 0
#define GPIO_B 1
#define GPIO_C 2
#define GPIO_D 3
#define GPIO_E 4
#define GPIO_F 5
#define GPIO_G 6
#define GPIO_H 7
#define GPIO_I 8

#define FTIR 1 // 下降沿触发
#define RTIR 2 // 上升沿触发

// GPIO设置专用宏定义
#define GPIO_MODE_IN 0	// 普通输入模式
#define GPIO_MODE_OUT 1 // 普通输出模式
#define GPIO_MODE_AF 2	// AF功能模式
#define GPIO_MODE_AIN 3 // 模拟输入模式

#define GPIO_SPEED_5M 0	  // GPIO速度(低速,5M)
#define GPIO_SPEED_25M 1  // GPIO速度(中速,25M)
#define GPIO_SPEED_50M 2  // GPIO速度(快速,50M)
#define GPIO_SPEED_100M 3 // GPIO速度(高速,100M)

#define GPIO_PUPD_NONE 0 // 不带上下拉
#define GPIO_PUPD_PU 1	 // 上拉
#define GPIO_PUPD_PD 2	 // 下拉
#define GPIO_PUPD_RES 3	 // 保留

#define GPIO_OTYPE_PP 0 // 推挽输出
#define GPIO_OTYPE_OD 1 // 开漏输出

// GPIO引脚编号定义
#define PIN0 1 << 0
#define PIN1 1 << 1
#define PIN2 1 << 2
#define PIN3 1 << 3
#define PIN4 1 << 4
#define PIN5 1 << 5
#define PIN6 1 << 6
#define PIN7 1 << 7
#define PIN8 1 << 8
#define PIN9 1 << 9
#define PIN10 1 << 10
#define PIN11 1 << 11
#define PIN12 1 << 12
#define PIN13 1 << 13
#define PIN14 1 << 14
#define PIN15 1 << 15
void GPIO_AF_Set(GPIO_TypeDef *GPIOx, u8 BITx, u8 AFx); // 注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
void GPIO_Set(GPIO_TypeDef *GPIOx, u32 BITx, u32 MODE, u32 OTYPE, u32 OSPEED, u32 PUPD);

#define ON 1
#define OFF 0
#define Write_Through() (*(__IO uint32_t *)0XE000EF9C = 1UL << 2) // Cache透写模式

void Cache_Enable(void);									   // 使能STM32H7的L1-Cahce
void Stm32_Clock_Init(u32 plln, u32 pllm, u32 pllp, u32 pllq); // 配置系统时钟
void RCC_GPIO_Init(void);
void SystemClock_Config(void);
u8 Get_ICahceSta(void); // 判断I_Cache是否打开
u8 Get_DCahceSta(void); // 判断I_Dache是否打开
void Sys_Soft_Reset(void);
void MCO_test(void);
void MCO_test_IO_Init(void);
void JumpToBootloader(void);

#if defined(__clang__) // 使用V6编译器(clang)
void __attribute__((noinline)) WFI_SET(void);
void __attribute__((noinline)) INTX_DISABLE(void);
void __attribute__((noinline)) INTX_ENABLE(void);
void __attribute__((noinline)) MSR_MSP(u32 addr);

#elif defined(__CC_ARM) // 使用V5编译器(ARMCC)
// 以下为汇编函数
void WFI_SET(void);		 // 执行WFI指令
void INTX_DISABLE(void); // 关闭所有中断
void INTX_ENABLE(void);	 // 开启所有中断
void MSR_MSP(u32 addr);	 // 设置堆栈地址
#endif

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
