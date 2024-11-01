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

// example:
//  W25Q64_init();
//  uint8_t test[768];
//  uint16_t i;
//  for (i = 0; i < 256; i++)
//  {
//    test[i] = i;
//  }
//  for(i = 0; i < 256; i++)
//  {
//      test[256+i] = i;
//  }
//    for(i = 0; i <256 ; i++)
//  {
//      uint8_t a=i%3;
//      test[512+i] = a;
//  }
//  W25Q64_Erase_Write_Plus(0, test, 768);
//  W25Q64_Read(0, 768);

#define CS_PORT GPIOA
#define CS_Pin GPIO_PIN_15
#define SPI_X hspi1
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
    __HAL_SPI_ENABLE(&SPI_X);
    SET_BIT(CS_PORT->ODR, CS_Pin);
}

uint8_t W25Q64_Read_Data[W25Q64Data_Size];
void W25Q64_Read_ID(uint8_t *MANUFACTURER_ID, uint8_t *Memory_Type, uint8_t *Capcity)
{
    SPI_Start();

    uint8_t SendData = W25Q64_JEDEC_ID;
    uint8_t ReceiveData[3];
    HAL_SPI_Transmit(&SPI_X, &SendData, 1, 50); // 发送指令

    HAL_SPI_Receive(&SPI_X, ReceiveData, 3, 50); // 读

    SPI_Stop();

    *MANUFACTURER_ID = ReceiveData[0];
    *Memory_Type = ReceiveData[1];
    *Capcity = ReceiveData[2];
}
void W24Q64_Wait_Busy()
{
    SPI_Start();

    uint8_t SendData = W25Q64_READ_STATUS_REGISTER1;
    HAL_SPI_Transmit(&SPI_X, &SendData, 1, 50); // 发送指令

    uint8_t ReceiveData;
    do
    {
        HAL_SPI_Receive(&SPI_X, &ReceiveData, 1, 50); // 读
    } // 换取状态寄存器值
    while (ReceiveData & 1);

    SPI_Stop();
}

void W25Q64_Write_Enable(void)
{
    SPI_Start();
    uint8_t SendData = W25Q64_WRIGHT_ENABLE;
    HAL_SPI_Transmit(&SPI_X, &SendData, 1, 50); // 发送指令
    SPI_Stop();
}

void W25Q64_Sextor_Erase(uint32_t address)
{
    W24Q64_Wait_Busy();
    W25Q64_Write_Enable();

    SPI_Start();

    uint8_t SendData[4] = {W25Q64_SECTOR_ERASE_4KB, address >> 16, address >> 8, address};
    HAL_SPI_Transmit(&SPI_X, SendData, 4, 50); // 发送指令

    SPI_Stop();
}

void W25Q64_Write(uint32_t address, uint8_t *ptr, uint16_t count)
{
    W24Q64_Wait_Busy();
    W25Q64_Write_Enable();

    SPI_Start();

    uint8_t SendData[4] = {W25Q64_PAGE_PROGRAM, address >> 16, address >> 8, address};
    HAL_SPI_Transmit(&SPI_X, SendData, 4, 50); // 发送指令

    HAL_SPI_Transmit(&SPI_X, ptr, count, 50); // 发送指令

    SPI_Stop();
}

void W25Q64_Read(uint32_t address, uint16_t count)
{
    W24Q64_Wait_Busy();

    SPI_Start();

    uint8_t SendData[4] = {W25Q64_READ_DATA, address >> 16, address >> 8, address};
    HAL_SPI_Transmit(&SPI_X, SendData, 4, 50); // 发送指令

    HAL_SPI_Receive(&SPI_X, W25Q64_Read_Data, count, 50); // 读

    SPI_Stop();
}
void W25Q64_Erase_Write(uint32_t address, uint8_t *ptr, uint16_t count)
{
    W25Q64_Sextor_Erase(address);
    W25Q64_Write(address, ptr, count);
}
// 这个函数在一个扇区内畅通无阻
void W25Q64_Erase_Write_Plus(uint32_t address, uint8_t *ptr, uint16_t count)
{
    W25Q64_Sextor_Erase(address); // 擦除这一片扇区

    uint16_t number_can_be_written = 0x100 - (address & 0xff); // 记录初始页还能写多少
    uint16_t Number_of_current_writes;                         // 本次写多少
    // 第一次特殊操作：
    // 计算第一次写多少
    if (count > number_can_be_written)
        Number_of_current_writes = number_can_be_written;
    else
        Number_of_current_writes = count;
    W25Q64_Write(address, ptr, Number_of_current_writes);
    // 第一次处理
    count -= Number_of_current_writes;
    address &= ~(0xff);
    address += 0x100;
    ptr += Number_of_current_writes;
    // 计算后续
    // 理论上是页开头
    while (1)
    {
        // 计算本次写多少
        if (count > 256)
        {
            Number_of_current_writes = 256;
        }
        else
        {
            Number_of_current_writes = count;
        }
        // 写
        W25Q64_Write(address, ptr, Number_of_current_writes);

        // 后续处理
        if (Number_of_current_writes == 256)
        {
            address += 0x100;
            ptr += Number_of_current_writes;
            count -= Number_of_current_writes;
        }
        else
            break;
    }
}
