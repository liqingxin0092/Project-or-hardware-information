#ifndef __BSP_H
#define __BSP_H

/*用户配置区*/
#define USE_LCD            0   //使用LCD
#define USE_EVENTRECORD    0    //使用EVENTRECORDER 
#define USE_FATFS          0    //使用文件系统 
#define USE_FATTESTER      0    //使用文件系统测试,如果不用文件系统这个也请关闭
#define USE_BUFFER_KEY     0    //使用buffer_key
#define USE_USMART         1    //使用USMART
#define USE_MODBUS_M       0    //使用MODBUS_M
#define UES_RAM_ECC        0    //使用RAM_ECC,但是具体检测哪片区域需要自己配,硬件ECC控制器总是使能的

#include "my_freertos.h"
#include "qspi_w25q64.h"
#include "piclib.h"
#include "soft_timer.h"
#include "test.h"
#include "fattester.h"
#include "gpio.h"
#include "ecc_detection.h"
#include "flash_m.h"

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

void bsp_init(void);

#endif

