#ifndef __NAND_FSMC_H
#define __NAND_FSMC_H

#include "main.h"

#define NAND_MAX_PAGE_SIZE 2048
#define NAND_ECC_SECTOR_SIZE 512 // 执行ECC计算的单元大小，默认512字节

// NAND属性结构体
typedef struct
{
    uint16_t page_totalsize;                                       // 每页总大小，main区和spare区总和
    uint16_t page_mainsize;                                        // 每页的main区大小
    uint16_t page_sparesize;                                       // 每页的spare区大小
    uint8_t block_pagenum;                                         // 每个块包含的页数量
    uint16_t plane_blocknum;                                       // 每个plane包含的块数量
    uint16_t block_totalnum;                                       // 总的块数量
    uint16_t good_blocknum;                                        // 好块数量
    uint16_t valid_blocknum;                                       // 有效块数量(供文件系统使用的好块数量)
    uint16_t *lut;                                                 // LUT表，用作逻辑块-物理块转换
    uint32_t ecc_hard;                                             // 硬件计算出来的ECC值
    uint32_t ecc_hdbuf[NAND_MAX_PAGE_SIZE / NAND_ECC_SECTOR_SIZE]; // ECC硬件计算值缓冲区
    uint32_t ecc_rdbuf[NAND_MAX_PAGE_SIZE / NAND_ECC_SECTOR_SIZE]; // ECC读取的值缓冲区
} nand_attriute;

extern nand_attriute nand_dev; // nand重要参数结构体

#define NAND_RB_PORT GPIOD
#define NAND_RB_PIN 6
#define NAND_read_RB (((NAND_RB_PORT->IDR) >> NAND_RB_PIN) & 1) // 读取单引脚

#define NAND_BASE 0x80000000
#define nand_cmd (1 << 16)
#define nand_addr (1 << 17)
#define nand_data 0

/*某些指令*/
#define NAND_READ_ID 0x90
#define NAND_STATUS1 0x70
#define NAND_STATUS2 0xf1

/*某些返回值*/
#define NSTA_READY 0X40      // nand已经准备好
#define NSTA_ERROR 0X01      // nand错误
#define NSTA_ECC1BITERR 0X03 // ECC 1bit错误
#define NSTA_ECC2BITERR 0X04 // ECC 2bit以上错误

uint8_t NAND_ReadStatus(void);
void NAND_wait_busy(void);
void NAND_read_id(void);
void NAND_Init(void);
uint8_t NAND_page_read(uint32_t page_add, uint16_t byte_add, uint8_t *buffer, uint16_t num_byte_to_read);
uint8_t NAND_Write_Page(uint32_t page_add, uint16_t byte_add, uint8_t *buffer, uint16_t num_byte_to_read);
int8_t NAND_CopyPageWithoutWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum);
int8_t NAND_CopyPageWithWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum, uint16_t byte_add, uint8_t *pBuffer, uint16_t NumByteToWrite);
uint8_t NAND_EraseBlock(uint32_t BlockNum);
uint8_t NAND_ReadPageComp(uint32_t PageNum, uint16_t ColNum, uint32_t CmpVal, uint16_t NumByteToRead, uint16_t *NumByteEqual);
uint8_t NAND_WritePageConst(uint32_t PageNum, uint16_t ColNum, uint32_t cval, uint16_t NumByteToWrite);
void NAND_EraseChip(void);

#endif
