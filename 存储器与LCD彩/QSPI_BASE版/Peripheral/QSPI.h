#ifndef __QSPI_H
#define __QSPI_H

#include "stm32h7xx.h"

#define pass() while (0)

/*引脚配置宏*/
#define QUADSPI_CLK_PIN GPIO_PIN_2                           // QUADSPI_CLK 引脚
#define QUADSPI_CLK_PORT GPIOB                               // QUADSPI_CLK 引脚端口
#define QUADSPI_CLK_AF GPIO_AF9_QUADSPI                      // QUADSPI_CLK IO口复用
#define GPIO_QUADSPI_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE() // QUADSPI_CLK 引脚时钟

#define QUADSPI_BK1_NCS_PIN GPIO_PIN_6                           // QUADSPI_BK1_NCS 引脚
#define QUADSPI_BK1_NCS_PORT GPIOB                               // QUADSPI_BK1_NCS 引脚端口
#define QUADSPI_BK1_NCS_AF GPIO_AF10_QUADSPI                     // QUADSPI_BK1_NCS IO口复用
#define GPIO_QUADSPI_BK1_NCS_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE() // QUADSPI_BK1_NCS 引脚时钟

#define QUADSPI_BK1_IO0_PIN GPIO_PIN_11                          // QUADSPI_BK1_IO0 引脚
#define QUADSPI_BK1_IO0_PORT GPIOD                               // QUADSPI_BK1_IO0 引脚端口
#define QUADSPI_BK1_IO0_AF GPIO_AF9_QUADSPI                      // QUADSPI_BK1_IO0 IO口复用
#define GPIO_QUADSPI_BK1_IO0_ENABLE __HAL_RCC_GPIOD_CLK_ENABLE() // QUADSPI_BK1_IO0 引脚时钟

#define QUADSPI_BK1_IO1_PIN GPIO_PIN_12                          // QUADSPI_BK1_IO1 引脚
#define QUADSPI_BK1_IO1_PORT GPIOD                               // QUADSPI_BK1_IO1 引脚端口
#define QUADSPI_BK1_IO1_AF GPIO_AF9_QUADSPI                      // QUADSPI_BK1_IO1 IO口复用
#define GPIO_QUADSPI_BK1_IO1_ENABLE __HAL_RCC_GPIOD_CLK_ENABLE() // QUADSPI_BK1_IO1 引脚时钟

#define QUADSPI_BK1_IO2_PIN GPIO_PIN_2                           // QUADSPI_BK1_IO2 引脚
#define QUADSPI_BK1_IO2_PORT GPIOE                               // QUADSPI_BK1_IO2 引脚端口
#define QUADSPI_BK1_IO2_AF GPIO_AF9_QUADSPI                      // QUADSPI_BK1_IO2 IO口复用
#define GPIO_QUADSPI_BK1_IO2_ENABLE __HAL_RCC_GPIOE_CLK_ENABLE() // QUADSPI_BK1_IO2 引脚时钟

#define QUADSPI_BK1_IO3_PIN GPIO_PIN_13                          // QUADSPI_BK1_IO3 引脚
#define QUADSPI_BK1_IO3_PORT GPIOD                               // QUADSPI_BK1_IO3 引脚端口
#define QUADSPI_BK1_IO3_AF GPIO_AF9_QUADSPI                      // QUADSPI_BK1_IO3 IO口复用
#define GPIO_QUADSPI_BK1_IO3_ENABLE __HAL_RCC_GPIOD_CLK_ENABLE() // QUADSPI_BK1_IO3 引脚时钟

/*接口函数*/
void QSPI_RCC_Init(void);          // QSPI RCC配置
void QSPI_IO_Init(void);           // QSPI IO配置
void QSPI_and_NorFlash_Init(void); // 全配置
uint32_t W25Q64_read_ID(void);
#endif
