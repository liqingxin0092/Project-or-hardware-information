#include "main.h"
#include "dzp.h"
#include "Delay.h"
// PA0~Din,PA1~CS,PA2~CLK
enum Pin
{
    Din = 1,
    CS = 1 << 1,
    CLK = 1 << 2,
};
void Contact_Init()
{
    SET_BIT(GPIOA->ODR, CS);    // cs默认高电平
    CLEAR_BIT(GPIOA->ODR, CLK); // clk默认低电平
}
void SendByte_Add(uint8_t add, uint8_t byte)
{
    int8_t i;
    CLEAR_BIT(GPIOA->ODR, CS); // CS下降沿开启一段时序
    for (i = 7; i >= 0; i--)
    {
        // 高位先行
        if ((add >> i) & 1)
            SET_BIT(GPIOA->ODR, Din);
        else
            CLEAR_BIT(GPIOA->ODR, Din);

        SET_BIT(GPIOA->ODR, CLK); // clk上升沿发送数据
        Delay_us(1);
        CLEAR_BIT(GPIOA->ODR, CLK);
    }
    for (i = 7; i >= 0; i--)
    {
        // 高位先行
        if ((byte >> i) & 1)
            SET_BIT(GPIOA->ODR, Din);
        else
            CLEAR_BIT(GPIOA->ODR, Din);

        SET_BIT(GPIOA->ODR, CLK); // clk上升沿发送数据
        Delay_us(1);
        CLEAR_BIT(GPIOA->ODR, CLK);
    }
    SET_BIT(GPIOA->ODR, CS); // cs上升沿结束时序
    Delay_us(1);
}
