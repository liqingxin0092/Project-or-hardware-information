#ifndef __BSP_H
#define __BSP_H


/*�û�������*/
#define USE_LCD            0    //ʹ��LCD
#define USE_EVENTRECORD    0    //ʹ��EVENTRECORDER 
#define USE_FATFS          0    //ʹ���ļ�ϵͳ 
#define USE_FATTESTER      0    //ʹ���ļ�ϵͳ����,��������ļ�ϵͳ���Ҳ��ر�
#define USE_BUFFER_KEY     0    //ʹ��buffer_key
#define USE_USMART         1    //ʹ��USMART
#define USE_MODBUS_M       0    //ʹ��MODBUS_M
#define UES_RAM_ECC        0    //ʹ��RAM_ECC,���Ǿ�������Ƭ������Ҫ�Լ���,Ӳ��ECC����������ʹ�ܵ�

//ע��Ҫ��rcc�Ǹ��ļ��￪����Ƶ
#define CPU_CLOCK    480   //�ṩ������480����600

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

