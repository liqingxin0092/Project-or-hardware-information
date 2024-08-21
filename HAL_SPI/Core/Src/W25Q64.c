#include "W25Q64.h"
#include "main.h"
extern SPI_HandleTypeDef hspi1;

// __HAL_SPI_ENABLE(&hspi1);
// HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
// HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);

// example:
// W25Q64_init();
// uint8_t test[4] = {0x11, 0x22, 0x33, 0x44};
// W25Q64_Erase_Write(0, test, 4);
// W25Q64_Read(0, 4);

#define CS_PORT GPIOA
#define CS_Pin GPIO_PIN_4
#define SPI_Start()                      \
    do                                   \
        CLEAR_BIT(CS_PORT->ODR, CS_Pin); \
    while (0)

#define SPI_Stop()                     \
    do                                 \
        SET_BIT(CS_PORT->ODR, CS_Pin); \
    while (0)

void W25Q64_init()
{
    __HAL_SPI_ENABLE(&hspi1);
    SET_BIT(CS_PORT->ODR, CS_Pin);
}

uint8_t W25Q64_Read_Data[W25Q64Data_Size];
void W25Q64_Read_ID(uint8_t *MANUFACTURER_ID, uint8_t *Memory_Type, uint8_t *Capcity)
{
    SPI_Start();

    uint8_t SendData = W25Q64_JEDEC_ID;
    uint8_t ReceiveData[3];
    HAL_SPI_Transmit(&hspi1, &SendData, 1, 50); // 发送指令

    HAL_SPI_Receive(&hspi1, ReceiveData, 3, 50); // 读

    SPI_Stop();

    *MANUFACTURER_ID = ReceiveData[0];
    *Memory_Type = ReceiveData[1];
    *Capcity = ReceiveData[2];
}
void W24Q64_Wait_Busy()
{
    SPI_Start();

    uint8_t SendData = W25Q64_READ_STATUS_REGISTER1;
    HAL_SPI_Transmit(&hspi1, &SendData, 1, 50); // 发送指令

    uint8_t ReceiveData;
    do
    {
        HAL_SPI_Receive(&hspi1, &ReceiveData, 1, 50); // 读
    } // 换取状态寄存器值
    while (ReceiveData & 1);

    SPI_Stop();
}

void W25Q64_Write_Enable(void)
{
    SPI_Start();
    uint8_t SendData = W25Q64_WRIGHT_ENABLE;
    HAL_SPI_Transmit(&hspi1, &SendData, 1, 50); // 发送指令
    SPI_Stop();
}

void W25Q64_Sextor_Erase(uint32_t address)
{
    W24Q64_Wait_Busy();
    W25Q64_Write_Enable();

    SPI_Start();

    uint8_t SendData[4] = {W25Q64_SECTOR_ERASE_4KB, address >> 16, address >> 8, address};
    HAL_SPI_Transmit(&hspi1, SendData, 4, 50); // 发送指令

    SPI_Stop();
}

void W25Q64_Write(uint32_t address, uint8_t *ptr, uint16_t count)
{
    W24Q64_Wait_Busy();
    W25Q64_Write_Enable();

    SPI_Start();

    uint8_t SendData[4] = {W25Q64_PAGE_PROGRAM, address >> 16, address >> 8, address};
    HAL_SPI_Transmit(&hspi1, SendData, 4, 50); // 发送指令

    HAL_SPI_Transmit(&hspi1, ptr, count, 50); // 发送指令

    SPI_Stop();
}

void W25Q64_Read(uint32_t address, uint16_t count)
{
    W24Q64_Wait_Busy();

    SPI_Start();

    uint8_t SendData[4] = {W25Q64_READ_DATA, address >> 16, address >> 8, address};
    HAL_SPI_Transmit(&hspi1, SendData, 4, 50); // 发送指令

    HAL_SPI_Receive(&hspi1, W25Q64_Read_Data, count, 50); // 读

    SPI_Stop();
}
void W25Q64_Erase_Write(uint32_t address, uint8_t *ptr, uint16_t count)
{
    W25Q64_Sextor_Erase(address);
    W25Q64_Write(address, ptr, count);
}
