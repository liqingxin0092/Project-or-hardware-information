#ifndef __SPI_H
#define __SPI_H

#include "stdint.h"

// 工具层
void SPI_CS(uint8_t bit);
void SPI_Deal_CLK(uint8_t bit);
void SPI_Deal_DO(uint8_t bit);
uint8_t SPI_Read_DI(void);
// 时序层
void SPI_Start(void);
void SPI_Stop(void);
uint8_t SPI_Swap(uint8_t byte);
uint8_t SPI_Swap_Plus(uint8_t byte);

// 应用层
void SPI_Contact_Init(void);

#endif
