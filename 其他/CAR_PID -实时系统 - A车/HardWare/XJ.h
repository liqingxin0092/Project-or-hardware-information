// XJ.h
#ifndef XJ_H
#define XJ_H

#include "stm32f4xx_hal.h"

// ����ѭ������������
#define TRACK_SENSOR_1_PIN GPIO_PIN_2
#define TRACK_SENSOR_2_PIN GPIO_PIN_3
#define TRACK_SENSOR_3_PIN GPIO_PIN_6
#define TRACK_SENSOR_4_PIN GPIO_PIN_7
#define TRACK_SENSOR_GPIO_PORT GPIOA

// ��ʼ��ѭ��������
void XJ_Init(void);

// ��ȡѭ��������״̬
uint8_t XJ_Read(void);

#endif // XJ_H
