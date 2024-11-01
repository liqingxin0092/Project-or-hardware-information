#include "main.h"
#include "func.h"
#include "Delay.h"
// 地址偶数控制数码管，数码管是共阴的，奇数控制LED。

//  example:
// ContactInit();
// SendByte(SHOW_14_16);
// SendByte(DATA_FIX_WRITE);

// FixWrite(ADD_00, SegNum[0]);//依次控制数码管
// FixWrite(ADD_02, SegNum[1]);//SegNum[0~9],依次代表可以显示的数字
// FixWrite(ADD_04, SegNum[2]);
// FixWrite(ADD_06, SegNum[3]);
// FixWrite(ADD_08, SegNum[4]);
// FixWrite(ADD_0A, SegNum[5]);
// FixWrite(ADD_0C, SegNum[6]);
// FixWrite(ADD_0E, SegNum[7]);

// FixWrite(ADD_01, 0);
// FixWrite(ADD_03, 0);
// FixWrite(ADD_05, 0);
// FixWrite(ADD_07, 0);
// FixWrite(ADD_09, 0);
// FixWrite(ADD_0B, 0);
// FixWrite(ADD_0D, 0);
// FixWrite(ADD_0F, 0);

// KeyBuf[0]    0x01(s1)    0x10(s5)
// KeyBuf[1]    0x01(s2)    0x10(s6)
// KeyBuf[2]    0x01(s3)    0x10(s7)
// KeyBuf[3]    0x01(s4)    0x10(s8)

uint8_t SegNum[10] = {
    Up | RightUp | RightDown | Down | LeftDown | LeftUp,       // 0
    RightDown | RightUp,                                       // 1
    Up | RightUp | Mid | LeftDown | Down,                      // 2
    Up | RightUp | Mid | RightDown | Down,                     // 3
    RightUp | Mid | RightDown | LeftUp,                        // 4
    Up | LeftUp | Mid | RightDown | Down,                      // 5
    Up | RightDown | Mid | LeftDown | Down | LeftUp,           // 6
    Up | RightUp | RightDown,                                  // 7
    Up | RightUp | RightDown | Mid | LeftDown | LeftUp | Down, // 8
    Up | RightUp | Mid | RightDown | LeftUp | Down             // 9

};

// PA0~STB PA1~CLK PA2~DIO
void ContactInit()
{
    GPIOA->ODR |= 1 << 0; // STB空闲时高电平
    GPIOA->ODR |= 1 << 1; // CLK空闲时高电平
}
// 低位先行
void SendByte(uint8_t byte)
{
    uint8_t i;
    GPIOA->ODR &= ~1; // 片选下降沿开始时序
    for (i = 0; i < 8; i++)
    {
        GPIOA->ODR &= ~(1 << 1); // CLK下降沿开始准备数据
        if (byte & (1 << i))
            GPIOA->ODR |= 1 << 2; // 如果第i位是1,则发送1
        else
            GPIOA->ODR &= ~(1 << 2); // 否则发送0
        GPIOA->ODR |= 1 << 1;        // CLK上升沿TM1638接收数据
    }
    GPIOA->ODR |= 1; // 片选上升沿结束时序
}

void FixWrite(uint8_t add, uint8_t byte)
{
    uint8_t i;
    GPIOA->ODR &= ~1; // 片选下降沿开始时序
    for (i = 0; i < 8; i++)
    {
        GPIOA->ODR &= ~(1 << 1); // CLK下降沿开始准备数据
        if (add & (1 << i))
            GPIOA->ODR |= 1 << 2; // 如果第i位是1,则发送1
        else
            GPIOA->ODR &= ~(1 << 2); // 否则发送0
        GPIOA->ODR |= 1 << 1;        // CLK上升沿TM1638接收数据
    }
    for (i = 0; i < 8; i++)
    {
        GPIOA->ODR &= ~(1 << 1); // CLK下降沿开始准备数据
        if (byte & (1 << i))
            GPIOA->ODR |= 1 << 2; // 如果第i位是1,则发送1
        else
            GPIOA->ODR &= ~(1 << 2); // 否则发送0
        GPIOA->ODR |= 1 << 1;        // CLK上升沿TM1638接收数据
    }
    GPIOA->ODR |= 1; // 片选上升沿结束时序
}

uint8_t KeyBuf[4];
void ReadKey()
{
    uint8_t i, j;
    GPIOA->ODR &= ~1; // 片选下降沿开始时序
    for (i = 0; i < 8; i++)
    {
        GPIOA->ODR &= ~(1 << 1); // CLK下降沿开始准备数据
        if (DATA_FIX_READ & (1 << i))
            GPIOA->ODR |= 1 << 2; // 如果第i位是1,则发送1
        else
            GPIOA->ODR &= ~(1 << 2); // 否则发送0
        GPIOA->ODR |= 1 << 1;        // CLK上升沿TM1638接收数据
    }

    Delay_us(1);
    GPIOA->CRL = 0x44444433; // DIO切换为输入模式
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 8; j++)
        {
            GPIOA->ODR &= ~(1 << 1); // CLK下降沿TM1638准备数据
            Delay_us(1);
            GPIOA->ODR |= 1 << 1; // CLK上升沿主机接收数据
            if (((GPIOA->IDR >> 2) & 1))
                KeyBuf[i] |= 1 << j; // 接收数据
            else
                KeyBuf[i] &= ~(1 << j); // 接收数据
        }
    }
    GPIOA->ODR |= 1;         // 片选上升沿结束时序
    GPIOA->CRL = 0x44444333; // DIO切换为输出模式
}
