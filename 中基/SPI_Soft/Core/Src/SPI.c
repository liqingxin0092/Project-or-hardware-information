#include "SPI.h"
#include "main.h"

// 实现SPI模式0
#define SPI_PORT GPIOA

#define SPI_CS GPIO_PIN_4
#define SPI_CLK GPIO_PIN_5
#define SPI_DI GPIO_PIN_6
#define SPI_DO GPIO_PIN_7

void SPI_Deal_CS(uint8_t bit)
{
    if (bit == 0)
        CLEAR_BIT(SPI_PORT->ODR, SPI_CS);
    else
        SET_BIT(SPI_PORT->ODR, SPI_CS);
}
void SPI_Deal_CLK(uint8_t bit)
{
    if (bit == 0)
        CLEAR_BIT(SPI_PORT->ODR, SPI_CLK);
    else
        SET_BIT(SPI_PORT->ODR, SPI_CLK);
}
void SPI_Deal_DO(uint8_t bit)
{
    if (bit == 0)
        CLEAR_BIT(SPI_PORT->ODR, SPI_DO);
    else
        SET_BIT(SPI_PORT->ODR, SPI_DO);
}
uint8_t SPI_Read_DI(void)
{
    return HAL_GPIO_ReadPin(SPI_PORT, SPI_DI);
}
void SPI_Contact_Init()
{
    SPI_Deal_CS(1);  // CS高电平不选中
    SPI_Deal_CLK(0); // 默认低电平
}
void SPI_Start()
{
    SPI_Deal_CS(0);
}
void SPI_Stop()
{
    SPI_Deal_CS(1);
}
uint8_t SPI_Swap(uint8_t byte)
{
    uint8_t ReceiveData = 0;
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        SPI_Deal_DO(byte << (7 - i));
        SPI_Deal_CLK(1);
        ReceiveData |= SPI_Read_DI() << i;
        SPI_Deal_CLK(0);
    }
    return ReceiveData;
}
uint8_t SPI_Swap_Plus(uint8_t byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        SPI_Deal_DO(byte & 0x80);
        byte <<= 1;
        SPI_Deal_CLK(1);
        byte |= SPI_Read_DI();
        SPI_Deal_CLK(0);
    }
    return byte;
}
