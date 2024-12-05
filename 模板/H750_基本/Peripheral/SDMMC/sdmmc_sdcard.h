#ifndef _SDMMC_SDCARD_H
#define _SDMMC_SDCARD_H

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"

#define SD_TIMEOUT      ((uint32_t)100000000)       //超时时间
#define SD_DMA_MODE     0	                        //1：DMA模式，0：查询模式   

/* 根据 SD_HandleTypeDef 定义的宏，用于快速计算容量 */
#define SD_TOTAL_SIZE_BYTE(__Handle__)  (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 0)
#define SD_TOTAL_SIZE_KB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 10)
#define SD_TOTAL_SIZE_MB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 20)
#define SD_TOTAL_SIZE_GB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 30)


extern SD_HandleTypeDef        SDCARD_Handler;      //SD卡句柄
extern HAL_SD_CardInfoTypeDef  SDCardInfo;          //SD卡信息结构体

u8 SD_Init(void);
u8 SD_ReadDisk(u8* buf,u32 sector,u8 cnt);
u8 SD_WriteDisk(u8 *buf,u32 sector,u8 cnt);
u8 SD_ReadBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt);
u8 SD_WriteBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt);
void show_sdcard_info(void);

#endif
