#ifndef __BSP_H
#define __BSP_H


/*用户配置区*/
#define USE_LCD            0    //使用LCD
#define USE_EVENTRECORD    0    //使用EVENTRECORDER 
#define USE_FATFS          0    //使用文件系统 
#define USE_FATTESTER      0    //使用文件系统测试,如果不用文件系统这个也请关闭
#define USE_BUFFER_KEY     0    //使用buffer_key
#define USE_USMART         1    //使用USMART
#define USE_MODBUS_M       0    //使用MODBUS_M
#define UES_RAM_ECC        0    //使用RAM_ECC,但是具体检测哪片区域需要自己配,硬件ECC控制器总是使能的

//注意要在rcc那个文件里开允许超频
#define CPU_CLOCK    480   //提供了两种480或者600

#include "my_freertos.h"
#include "qspi_w25q64.h"
#include "piclib.h"
#include "timer_m.h"
#include "test.h"
#include "fattester.h"
#include "gpio.h"
#include "flash_m.h"
#include "rcc_and_pwr_m.h"
#include "usart_m.h"
#include "dma_m.h"

#if USE_MODBUS_M==1
#include "modbus_m.h"
#endif

#if USE_LCD==1
#include "lcd.h"
#endif

#if USE_EVENTRECORD==1
#include "EventRecorder.h"
#endif

#if USE_BUFFER_KEY==1
#include "buffer_key.h"
#endif

#if USE_USMART==1
#include "usmart.h"
#endif

#if  UES_RAM_ECC==1
#include "ecc_detection.h"
#endif 

void bsp_init(void);
void JumpToBootloader(void);

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

