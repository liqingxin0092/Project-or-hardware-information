#ifndef __SDRAM_H
#define __SDRAM_H

#include "main.h"

/*对外用这个宏,保障移植性*/
#define __SDRAM_handle hsdram1
extern SDRAM_HandleTypeDef __SDRAM_handle;

#define SDRAM_BASE 0xc0000000

// SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x0200)

void SDRAM_Init(SDRAM_HandleTypeDef *hsdram, uint16_t freq_count);//freq_count:677
void SDRAM_fmc_write(uint8_t *ptr, uint32_t addr, uint32_t lenth);
void SDRAM_fmc_read(uint8_t *ptr, uint32_t addr, uint32_t lenth);

#endif
