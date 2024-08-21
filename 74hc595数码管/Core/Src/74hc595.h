#ifndef __74HC595_H
#define __74HC595_H

#include "stdint.h"
// example:共阳数码管
// WriteHalfWord("10101010 10000000");//左边段码,右边选中
//                  ^
//                 /|\
//                  |
//                  |
//     0       1    2       3         4     5          6        7
//     point   mid  leftup  leftdown  down  rightdown  rightup  up
//     0点亮 1熄灭

enum SegmentNum
{
    one = 0xf9,
    two = 0xa4,
    three = 0xb0,
    four = 0x99,
    five = 0x92,
    six = 0x82,
    seven = 0xf8,
    eight = 0x80,
    nine = 0x90,
};

void WriteByte(char *p);
void WriteHalfWord(char *p);
void WriteMix(uint8_t num, char *p);

#endif
