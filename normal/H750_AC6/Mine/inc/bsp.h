#ifndef __BSP_H
#define __BSP_H

/*�û�������*/
#define USE_LCD            0   //ʹ��LCD
#define USE_EVENTRECORD    0    //ʹ��EVENTRECORDER 
#define USE_FATFS          0    //ʹ���ļ�ϵͳ 
#define USE_FATTESTER      0    //ʹ���ļ�ϵͳ����,��������ļ�ϵͳ���Ҳ��ر�
#define USE_BUFFER_KEY     0    //ʹ��buffer_key
#define USE_USMART         1    //ʹ��USMART
#define USE_MODBUS_M       0    //ʹ��MODBUS_M
#define UES_RAM_ECC        0    //ʹ��RAM_ECC,���Ǿ�������Ƭ������Ҫ�Լ���,Ӳ��ECC����������ʹ�ܵ�

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

