#ifndef __FUNC_H__
#define __FUNC_H__

#include "stdint.h"

typedef enum
{
    Up = 1,
    RightUp = 2,
    RightDown = 4,
    Down = 8,
    LeftDown = 16,
    LeftUp = 32,
    Mid = 64,
    Point = 128,
} location;

#define DATA_INC_WRITE 0x40
#define DATA_INC_READ 0x42
#define DATA_FIX_WRITE 0x44
#define DATA_FIX_READ 0x46

#define ADD_00 0xc0
#define ADD_01 0xc1
#define ADD_02 0xc2
#define ADD_03 0xc3
#define ADD_04 0xc4
#define ADD_05 0xc5
#define ADD_06 0xc6
#define ADD_07 0xc7
#define ADD_08 0xc8
#define ADD_09 0xc9
#define ADD_0A 0xcA
#define ADD_0B 0xcB
#define ADD_0C 0xcC
#define ADD_0D 0xcD
#define ADD_0E 0xcE
#define ADD_0F 0xcF

#define SHOW_1_16 0x88
#define SHOW_2_16 0x89
#define SHOW_4_16 0x8A
#define SHOW_10_16 0x8B
#define SHOW_11_16 0x8C
#define SHOW_12_16 0x8D
#define SHOW_13_16 0x8E
#define SHOW_14_16 0x8F
#define SHOW_OFF 0x80

extern uint8_t SegNum[10];
extern uint8_t KeyBuf[4];

void ContactInit(void);
void SendByte(uint8_t byte);
void FixWrite(uint8_t add, uint8_t byte);
void ReadKey(void);

#endif
