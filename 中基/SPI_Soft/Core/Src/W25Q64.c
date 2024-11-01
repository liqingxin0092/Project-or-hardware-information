#include "SPI.h"
#include "W25Q64.h"

// example:
// W25Q64_init();
// uint8_t test[4] = {0x11, 0x22, 0x33, 0x44};
// W25Q64_Erase_Write(0, test, 4);
// W25Q64_Read(0, 4);

void W25Q64_init()
{
    SPI_Contact_Init();
}
void W25Q64_Read_ID(uint8_t *MANUFACTURER_ID, uint8_t *Memory_Type, uint8_t *Capcity)
{
    SPI_Start();

    SPI_Swap_Plus(W25Q64_JEDEC_ID);                // 发送指令
    *MANUFACTURER_ID = SPI_Swap_Plus(W25Q64_NULL); // 读取，换取
    *Memory_Type = SPI_Swap_Plus(W25Q64_NULL);
    *Capcity = SPI_Swap_Plus(W25Q64_NULL);

    SPI_Stop();
}
void W24Q64_Wait_Busy()
{
    SPI_Start();

    SPI_Swap_Plus(W25Q64_READ_STATUS_REGISTER1); // 发送读状态寄存器指令
    do
        (SPI_Swap_Plus(W25Q64_NULL)); // 换取状态寄存器值
    while (SPI_Swap_Plus(W25Q64_NULL) & 1);

    SPI_Stop();
}

void W25Q64_Write_Enable(void)
{
    SPI_Start();
    SPI_Swap_Plus(W25Q64_WRIGHT_ENABLE); // 发送读状态寄存器指令
    SPI_Stop();
}

void W25Q64_Sextor_Erase(uint32_t address)
{
    W24Q64_Wait_Busy();
    W25Q64_Write_Enable();

    SPI_Start();

    SPI_Swap_Plus(W25Q64_SECTOR_ERASE_4KB); // 命令
    SPI_Swap_Plus(address >> 16);           // 地址
    SPI_Swap_Plus(address >> 8);
    SPI_Swap_Plus(address);

    SPI_Stop();
}

void W25Q64_Write(uint32_t address, uint8_t *ptr, uint16_t count)
{
    uint16_t i;
    W24Q64_Wait_Busy();
    W25Q64_Write_Enable();

    SPI_Start();

    SPI_Swap_Plus(W25Q64_PAGE_PROGRAM); // 命令
    SPI_Swap_Plus(address >> 16);       // 地址
    SPI_Swap_Plus(address >> 8);
    SPI_Swap_Plus(address);

    for (i = 0; i < count; i++) // 数据
    {
        SPI_Swap_Plus(ptr[i]);
    }

    SPI_Stop();
}
uint8_t W25Q64_Read_Data[W25Q64Data_Size];
void W25Q64_Read(uint32_t address, uint16_t count)
{
    uint16_t i;
    W24Q64_Wait_Busy();

    SPI_Start();

    SPI_Swap_Plus(W25Q64_READ_DATA); // 命令
    SPI_Swap_Plus(address >> 16);    // 地址
    SPI_Swap_Plus(address >> 8);     //
    SPI_Swap_Plus(address);          //

    for (i = 0; i < count; i++)
    {
        W25Q64_Read_Data[i] = SPI_Swap_Plus(W25Q64_NULL);
    }

    SPI_Stop();
}
void W25Q64_Erase_Write(uint32_t address, uint8_t *ptr, uint16_t count)
{
    W25Q64_Sextor_Erase(address);
    W25Q64_Write(address, ptr, count);
}
