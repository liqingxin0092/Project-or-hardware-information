#ifndef __TF_CARD_H
#define __TF_CARD_H

#include "main.h"

extern SD_HandleTypeDef g_sdcard_handler;
extern HAL_SD_CardInfoTypeDef g_sd_card_info_handle; /* SD卡信息结构体 */


#define SD_TIMEOUT             ((uint32_t)100000000)    /* 超时时间 */
#define SD_TRANSFER_OK         ((uint8_t)0x00)
#define SD_TRANSFER_BUSY       ((uint8_t)0x01)

/* 根据 SD_HandleTypeDef 定义的宏，用于快速计算容量 */
#define SD_TOTAL_SIZE_BYTE(__Handle__)  (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 0)
#define SD_TOTAL_SIZE_KB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 10)
#define SD_TOTAL_SIZE_MB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 20)
#define SD_TOTAL_SIZE_GB(__Handle__)    (((uint64_t)((__Handle__)->SdCard.LogBlockNbr) * ((__Handle__)->SdCard.LogBlockSize)) >> 30)

/*IO映射*/
// D0  ~ PC8
// D1  ~ PC9
// D2  ~ PC10
// D3  ~ PC11
// SCk ~ PC12
// CMD ~ PD2

extern SD_HandleTypeDef g_sdcard_handler;

uint8_t sd_init(void);
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);
void show_sdcard_info(void);

#endif
