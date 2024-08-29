#include "AT24C128.h"
#include "main.h"
#include "i2c.h"
#include "Delay.h"

// HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
// HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

// example:
//   AT24C128_IIC_Init();
//   uint8_t test[4] = {0x20,0x04,0x02,0x25};
//   AT24C128_Write(0xa0, 0, 0, test, 4);
//   AT24C128_Read(0xa0, 0,0, 4);

// 写数据：设备地址(1010+A2+A1+A0+读写位)+字节地址+页地址+数据
uint8_t AT24C128_test;

extern I2C_HandleTypeDef hi2c1;
void AT24C128_IIC_Init()
{
    __HAL_I2C_ENABLE(&hi2c1);
}

// //  PageAddress范围0~255 ,ByteAddress范围0~63
void AT24C128_Write(uint8_t DeviceAddress, uint8_t PageAddress, uint8_t ByteAddress, uint8_t *ptr, uint8_t lenth)
{
    CLEAR_BIT(AT24C128_WP_PORT->ODR, AT24C128_WP_Pin); // 解除写保护

    uint8_t SendData[64] = {PageAddress >> 2, ByteAddress | ((PageAddress & 3) << 6)};
    for (uint8_t i = 0; i < lenth; i++)
        SendData[i + 2] = ptr[i];
    HAL_I2C_Master_Transmit(&hi2c1, DeviceAddress, SendData, lenth + 2, 50);

    Delay_ms(50);
    SET_BIT(AT24C128_WP_PORT->ODR, AT24C128_WP_Pin); // 写保护使能
}

uint8_t AT24C128ReceiveData[64];
void AT24C128_Read(uint8_t DeviceAddress, uint8_t PageAddress, uint8_t ByteAddress, uint8_t lenth)
{
    uint8_t SendData[64] = {PageAddress >> 2, ByteAddress | ((PageAddress & 3) << 6)};
    HAL_I2C_Master_Transmit(&hi2c1, DeviceAddress, SendData, 2, 50);

    HAL_I2C_Master_Receive(&hi2c1, DeviceAddress, AT24C128ReceiveData, lenth, 50);
}
