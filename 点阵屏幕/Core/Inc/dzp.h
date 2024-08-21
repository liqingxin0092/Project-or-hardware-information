#ifndef __DZP_H
#define __DZP_H
#include "stdint.h"

enum digit
{
    digit0 = 1,
    digit1,
    digit2,
    digit3,
    digit4,
    digit5,
    digit6,
    digit7,
};

enum intensity
{
    _1_32,
    _3_32,
    _5_32,
    _7_32,
    _9_32,
    _11_32,
    _13_32,
    _15_32,
    _17_32,
    _19_32,
    _21_32,
    _23_32,
    _25_32,
    _27_32,
    _29_32,
    _31_32,
};

#define DECODEMODE 0x09  // 译码器模式
#define INTENSITY 0x0a   // 亮度控制
#define SCANLIMIT 0x0b   // 扫描控制寄存器
#define SHUTDOWN 0x0c    // 开关
#define DISPLAYTEST 0x0f // 显示检测

void Contact_Init(void);
void SendByte_Add(uint8_t add, uint8_t byte);

#endif
