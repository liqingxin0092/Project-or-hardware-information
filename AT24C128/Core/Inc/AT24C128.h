#ifndef __AT24C128_H
#define __AT24C128_H

#include "stdint.h"

// 低电平允许编程
#define AT24C128_WP_PORT GPIOA
#define AT24C128_WP_Pin GPIO_PIN_0

extern uint8_t AT24C128ReceiveData[64];
void AT24C128_Test(uint8_t DeviceAddress);
void AT24C128_Write(uint8_t DeviceAddress, uint8_t PageAddress, uint8_t ByteAddress, uint8_t *ptr, uint8_t lenth);
void AT24C128_Read(uint8_t DeviceAddress, uint8_t PageAddress, uint8_t ByteAddress, uint8_t lenth);

#endif
