#include "74hc595.h"
#include "main.h"

#define IC_PORT GPIOA
#define IC_DIO GPIO_PIN_0  // DS，
#define IC_RCLK GPIO_PIN_1 // 锁存时钟,ST_CP
#define IC_SCLK GPIO_PIN_2 // 移位时钟,SH_CP
// 先入先出
void WriteByte(char *p)
{
    int8_t i;
    for (i = 0; i < 8; i++)
    {
        if (p[i] - '0')
            SET_BIT(IC_PORT->ODR, IC_DIO);
        else
            CLEAR_BIT(IC_PORT->ODR, IC_DIO);

        CLEAR_BIT(IC_PORT->ODR, IC_SCLK); // 上升沿移位
        SET_BIT(IC_PORT->ODR, IC_SCLK);
    }
    CLEAR_BIT(IC_PORT->ODR, IC_RCLK); // 上升沿锁存
    SET_BIT(IC_PORT->ODR, IC_RCLK);
}
void WriteHalfWord(char *p)
{
    int8_t i;
    for (i = 0; i < 16; i++)
    {
        if (p[i] - '0')
            SET_BIT(IC_PORT->ODR, IC_DIO);
        else
            CLEAR_BIT(IC_PORT->ODR, IC_DIO);

        CLEAR_BIT(IC_PORT->ODR, IC_SCLK); // 上升沿移位
        SET_BIT(IC_PORT->ODR, IC_SCLK);
    }
    CLEAR_BIT(IC_PORT->ODR, IC_RCLK); // 上升沿锁存
    SET_BIT(IC_PORT->ODR, IC_RCLK);
}
void WriteMix(uint8_t num, char *p)
{
    int8_t i;
    for (i = 7; i >= 0; i--)
    {
        if (num & (1 << i))
            SET_BIT(IC_PORT->ODR, IC_DIO);
        else
            CLEAR_BIT(IC_PORT->ODR, IC_DIO);

        CLEAR_BIT(IC_PORT->ODR, IC_SCLK); // 上升沿移位
        SET_BIT(IC_PORT->ODR, IC_SCLK);
    }
    for (i = 0; i < 8; i++)
    {
        if (p[i] - '0')
            SET_BIT(IC_PORT->ODR, IC_DIO);
        else
            CLEAR_BIT(IC_PORT->ODR, IC_DIO);

        CLEAR_BIT(IC_PORT->ODR, IC_SCLK); // 上升沿移位
        SET_BIT(IC_PORT->ODR, IC_SCLK);
    }
    CLEAR_BIT(IC_PORT->ODR, IC_RCLK); // 上升沿锁存
    SET_BIT(IC_PORT->ODR, IC_RCLK);
}