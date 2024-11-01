#include "main.h"
#include "Delay.h"

//  用户只能自定义字模7个格子数据
// 设定CGRAM地址
// 地址的最后三位偏移0~7是行地址,每行显示什么得给数据----
// WriteCommand(0x40 + 0x00); // 设置CGRAM地址0         |
//                                                      |
// WriteData(0x00); // 自定义字模数据                   |
// WriteData(0x0e);                                     |
// WriteData(0x15);                 <--------------------
// WriteData(0x1f);
// WriteData(0x15);
// WriteData(0x12);
// WriteData(0x0e);

// WriteCommand(0x40 + 0x08); // 设置CGRAM地址1
// WriteCommand(0x40 + 0x10); // 设置CGRAM地址2
// WriteCommand(0x40 + 0x18); // 设置CGRAM地址3
// WriteCommand(0x40 + 0x20); // 设置CGRAM地址4
// WriteCommand(0x40 + 0x28); // 设置CGRAM地址5
// WriteCommand(0x40 + 0x30); // 设置CGRAM地址6
// WriteCommand(0x40 + 0x38); // 设置CGRAM地址7
uint8_t temp;
uint8_t ReadBusy()
{

    SET_BIT(RW_GPIO_Port->ODR, RW_Pin);   // RW高电平代表读操作
    CLEAR_BIT(RS_GPIO_Port->ODR, RS_Pin); // RS高电平代表指令

    CLEAR_BIT(EN_GPIO_Port->ODR, EN_Pin); // 为提供上升沿作准备
    Delay_us(1);

    // D7(B4)口转为输入模式 ,0011 -> 0100
    GPIOB->CRL = 0x44443444;

    // 制造下降沿
    SET_BIT(EN_GPIO_Port->ODR, EN_Pin);
    Delay_us(2);
    // 读
    if (D7_GPIO_Port->ODR & D7_Pin)
    {
        temp = 1;
    }
    else
        temp = 0;
    CLEAR_BIT(EN_GPIO_Port->ODR, EN_Pin);

    // D7(B4)口恢复输出模式 ,0100 -> 0011
    GPIOB->CRL = 0x44433444;

    return temp;
}
void WriteData(uint8_t data)
{
    Delay_ms(1);

    CLEAR_BIT(RW_GPIO_Port->ODR, RW_Pin); // RW低电平代表写操作
    SET_BIT(RS_GPIO_Port->ODR, RS_Pin);   // RS高电平代表数据

    CLEAR_BIT(EN_GPIO_Port->ODR, EN_Pin); // 为提供上升沿作准备
    Delay_us(1);

    SET_BIT(EN_GPIO_Port->ODR, EN_Pin); // 上升沿
    if (data & (1 << 7))                // 并口发送数据
        SET_BIT(D7_GPIO_Port->ODR, D7_Pin);
    else
        CLEAR_BIT(D7_GPIO_Port->ODR, D7_Pin); // 第7位

    if (data & (1 << 6)) // 并口发送数据
        SET_BIT(D6_GPIO_Port->ODR, D6_Pin);
    else
        CLEAR_BIT(D6_GPIO_Port->ODR, D6_Pin); // 第6位

    if (data & (1 << 5)) // 并口发送数据
        SET_BIT(D5_GPIO_Port->ODR, D5_Pin);
    else
        CLEAR_BIT(D5_GPIO_Port->ODR, D5_Pin); // 第5位

    if (data & (1 << 4)) // 并口发送数据
        SET_BIT(D4_GPIO_Port->ODR, D4_Pin);
    else
        CLEAR_BIT(D4_GPIO_Port->ODR, D4_Pin); // 第4位

    if (data & (1 << 3)) // 并口发送数据
        SET_BIT(D3_GPIO_Port->ODR, D3_Pin);
    else
        CLEAR_BIT(D3_GPIO_Port->ODR, D3_Pin); // 第3位

    if (data & (1 << 2)) // 并口发送数据
        SET_BIT(D2_GPIO_Port->ODR, D2_Pin);
    else
        CLEAR_BIT(D2_GPIO_Port->ODR, D2_Pin); // 第2位

    if (data & (1 << 1)) // 并口发送数据
        SET_BIT(D1_GPIO_Port->ODR, D1_Pin);
    else
        CLEAR_BIT(D1_GPIO_Port->ODR, D1_Pin); // 第1位

    if (data & (1 << 0)) // 并口发送数据
        SET_BIT(D0_GPIO_Port->ODR, D0_Pin);
    else
        CLEAR_BIT(D0_GPIO_Port->ODR, D0_Pin); // 第0位
    Delay_us(2);
    CLEAR_BIT(EN_GPIO_Port->ODR, EN_Pin); // EN下降沿结束通讯
}

void WriteCommand(uint8_t command)
{
    Delay_ms(1);
    CLEAR_BIT(RW_GPIO_Port->ODR, RW_Pin); // RW低电平代表写操作
    CLEAR_BIT(RS_GPIO_Port->ODR, RS_Pin); // RS高电平代表指令

    CLEAR_BIT(EN_GPIO_Port->ODR, EN_Pin); // 为提供上升沿作准备
    Delay_us(1);

    SET_BIT(EN_GPIO_Port->ODR, EN_Pin); // 上升沿

    if (command & (1 << 7)) // 并口发送数据
        SET_BIT(D7_GPIO_Port->ODR, D7_Pin);
    else
        CLEAR_BIT(D7_GPIO_Port->ODR, D7_Pin); // 第7位

    if (command & (1 << 6)) // 并口发送数据
        SET_BIT(D6_GPIO_Port->ODR, D6_Pin);
    else
        CLEAR_BIT(D6_GPIO_Port->ODR, D6_Pin); // 第6位

    if (command & (1 << 5)) // 并口发送数据
        SET_BIT(D5_GPIO_Port->ODR, D5_Pin);
    else
        CLEAR_BIT(D5_GPIO_Port->ODR, D5_Pin); // 第5位

    if (command & (1 << 4)) // 并口发送数据
        SET_BIT(D4_GPIO_Port->ODR, D4_Pin);
    else
        CLEAR_BIT(D4_GPIO_Port->ODR, D4_Pin); // 第4位

    if (command & (1 << 3)) // 并口发送数据
        SET_BIT(D3_GPIO_Port->ODR, D3_Pin);
    else
        CLEAR_BIT(D3_GPIO_Port->ODR, D3_Pin); // 第3位

    if (command & (1 << 2)) // 并口发送数据
        SET_BIT(D2_GPIO_Port->ODR, D2_Pin);
    else
        CLEAR_BIT(D2_GPIO_Port->ODR, D2_Pin); // 第2位

    if (command & (1 << 1)) // 并口发送数据
        SET_BIT(D1_GPIO_Port->ODR, D1_Pin);
    else
        CLEAR_BIT(D1_GPIO_Port->ODR, D1_Pin); // 第1位

    if (command & (1 << 0)) // 并口发送数据
        SET_BIT(D0_GPIO_Port->ODR, D0_Pin);
    else
        CLEAR_BIT(D0_GPIO_Port->ODR, D0_Pin); // 第0位

    Delay_us(2);
    CLEAR_BIT(EN_GPIO_Port->ODR, EN_Pin); // EN下降沿结束通讯
}
void WriteString(char *ptr)
{
    uint8_t i;
    for (i = 0; ptr[i] != '\0'; i++)
    {
        WriteData(ptr[i]);
    }
}
