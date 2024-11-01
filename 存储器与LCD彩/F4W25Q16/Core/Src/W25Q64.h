#ifndef __W25Q64_H
#define __W25Q64_H

#include "stdint.h"

// command define

// A write disable state occurs upon power - up or after any of the following instructions : Write Disable, Page Program, Sector Erase, Block Erase, Chip Erase and Write Status Register.

#define W25Q64Data_Size 768

#define W25Q64_WRIGHT_ENABLE 0x06
#define W25Q64_READ_STATUS_REGISTER1 0x05
#define W25Q64_PAGE_PROGRAM 0x02
#define W25Q64_SECTOR_ERASE_4KB 0x20
#define W25Q64_JEDEC_ID 0x9f
#define W25Q64_READ_DATA 0x03
#define W25Q64_NULL 0

void W24Q64_Wait_Busy(void);
void W25Q64_Write_Enable(void);
void W25Q64_Sextor_Erase(uint32_t address);
void W25Q64_Write(uint32_t address, uint8_t *ptr, uint16_t count);

// 应用层
extern uint8_t W25Q64_Read_Data[W25Q64Data_Size];
void W25Q64_init(void);
void W25Q64_Read_ID(uint8_t *MANUFACTURER_ID, uint8_t *Memory_Type, uint8_t *Capcity);
void W25Q64_Read(uint32_t address, uint16_t count);
void W25Q64_Erase_Write(uint32_t address, uint8_t *ptr, uint16_t count);
void W25Q64_Erase_Write_Plus(uint32_t address, uint8_t *ptr, uint16_t count);

#endif
