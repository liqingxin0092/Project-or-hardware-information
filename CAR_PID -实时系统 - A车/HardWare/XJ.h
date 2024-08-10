// XJ.h
#ifndef XJ_H
#define XJ_H

#include "stm32f4xx_hal.h"

// 定义循迹传感器引脚
#define TRACK_SENSOR_1_PIN GPIO_PIN_2
#define TRACK_SENSOR_2_PIN GPIO_PIN_3
#define TRACK_SENSOR_3_PIN GPIO_PIN_6
#define TRACK_SENSOR_4_PIN GPIO_PIN_7
#define TRACK_SENSOR_GPIO_PORT GPIOA

// 初始化循迹传感器
void XJ_Init(void);

// 读取循迹传感器状态
uint8_t XJ_Read(void);

#endif // XJ_H
