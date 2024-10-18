#ifndef __NAND_FLASH_H
#define __NAND_FLASH_H

#include "main.h"

/**   编程/使用注意!!！
 *  1.读取IDR!  输出ODR!
 *  2.信号输入引脚的上下拉属性应配置为与触发时相悖.
 *  3.擦除真的是一擦一个块.
 *  4.注意 标志位返回函数返回的是什么,具体某一位还是一个字节.
 *  5.读写以字节为单位.
 **/

/**
 * 注意b是位,B是字节.
 * 型号 : Samsung-K9F2G08U0C
 * ID数组 : 0xEC, 0xDA, 0x10, 0x95, 0x44.
 * 解析得到 : 此型号不支持cache编程, 不支持在多个芯片间交错编程, 可同时编程两页.
 * 2 Level Cell, *Internal Chip Number 1
 * 串行访问最小时间25ns, 8位数据, 块大小128Kb
 * 冗余区域大小(byte / 512byte) : 16
 * Page Size : 2KB
 * Plane Size : 1Gb = 1024Mb = 128MB
 * Plane Number : 2
 * 该器件一共2个Plane,每个Plane有1024个Block,每个Block有64Page,每个Page有(2048+64)Byte.
 * 2 * 1024 * 64 * ( 2048 + 64 ) = 2048 * 64 * 2112 = 128 * 2112 k = 264 MByte
 * 内存一共 264MB
 **/

/**初始化位带操作
 *
 *(uint32_t *)(0x42000000 + ((0x40020014 & 0x0fffffff) << 5) + (1 << 2)) = 0;
 *
 **/

/*坏块  (0~2023)  */
// 274,559,1049,1361,1646

/*当前映射*/
// D0 ~ PD14
// D1 ~ PD15
// D2 ~ PD0
// D3 ~ PD1
// D4 ~ PE7
// D5 ~ PE8
// D6 ~ PE9
// D7 ~ PE10
// CLE ~ PD11
// ALE ~ PD12
// WE ~ PD5
// RE ~ PD4
// CE ~ PG9
// RB ~ PD6

/*PORT基地址*/
// GPIOD ~ 0x4002 0C00
// GPIOG ~ 0x4002 1800
// GPIOE ~ 0x4002 1000
// ODR ~ 0x14
// IDR ~ 0x10

#define GPIOD_ODR_M 0x40020C14
#define GPIOG_ODR_M 0x40021814
#define GPIOE_ODR_M 0x40021014

#define GPIOD_IDR_M 0x40020C10
#define GPIOE_IDR_M 0x40021010

#define GPIOD_MODER_M 0x40020C00
#define GPIOE_MODER_M 0x40021000
#define GPIOD_PUPDR_M 0x40020C0c
#define GPIOE_PUPDR_M 0x4002100c

#define D0_PORT GPIOD_ODR_M
#define D1_PORT GPIOD_ODR_M
#define D2_PORT GPIOD_ODR_M
#define D3_PORT GPIOD_ODR_M
#define D4_PORT GPIOE_ODR_M
#define D5_PORT GPIOE_ODR_M
#define D6_PORT GPIOE_ODR_M
#define D7_PORT GPIOE_ODR_M
#define CLE_PORT GPIOD_ODR_M
#define ALE_PORT GPIOD_ODR_M
#define WE_PORT GPIOD_ODR_M
#define RE_PORT GPIOD_ODR_M
#define CE_PORT GPIOG_ODR_M
#define RB_PORT GPIOD_ODR_M

#define D0_Pin 14
#define D1_Pin 15
#define D2_Pin 0
#define D3_Pin 1
#define D4_Pin 7
#define D5_Pin 8
#define D6_Pin 9
#define D7_Pin 10
#define CLE_Pin 11
#define ALE_Pin 12
#define WE_Pin 5
#define RE_Pin 4
#define CE_Pin 9
#define RB_Pin 6

#define __GPIO_OUT(PORT, PIN, BIT) *(uint32_t *)(0x42000000 + (((PORT) & 0x0fffffff) << 5) + ((PIN) << 2)) = (BIT)
#define __IO_SET(BYTE)                                  \
    do                                                  \
    {                                                   \
        __GPIO_OUT(D0_PORT, D0_Pin, (BYTE) & 1);        \
        __GPIO_OUT(D1_PORT, D1_Pin, ((BYTE) >> 1) & 1); \
        __GPIO_OUT(D2_PORT, D2_Pin, ((BYTE) >> 2) & 1); \
        __GPIO_OUT(D3_PORT, D3_Pin, ((BYTE) >> 3) & 1); \
        __GPIO_OUT(D4_PORT, D4_Pin, ((BYTE) >> 4) & 1); \
        __GPIO_OUT(D5_PORT, D5_Pin, ((BYTE) >> 5) & 1); \
        __GPIO_OUT(D6_PORT, D6_Pin, ((BYTE) >> 6) & 1); \
        __GPIO_OUT(D7_PORT, D7_Pin, ((BYTE) >> 7) & 1); \
    } while (0)

#define __IO_IN_MODE()                            \
    do                                            \
    {                                             \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D0_Pin, 0); \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D1_Pin, 0); \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D2_Pin, 0); \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D3_Pin, 0); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D4_Pin, 0); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D5_Pin, 0); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D6_Pin, 0); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D7_Pin, 0); \
    } while (0)

#define __IO_OUT_MODE()                           \
    do                                            \
    {                                             \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D0_Pin, 1); \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D1_Pin, 1); \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D2_Pin, 1); \
        __GPIO_OUT(GPIOD_MODER_M, 2 * D3_Pin, 1); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D4_Pin, 1); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D5_Pin, 1); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D6_Pin, 1); \
        __GPIO_OUT(GPIOE_MODER_M, 2 * D7_Pin, 1); \
    } while (0)

#define __SEND_COMMAND(command)           \
    do                                    \
    {                                     \
        __GPIO_OUT(CLE_PORT, CLE_Pin, 1); \
        __GPIO_OUT(WE_PORT, WE_Pin, 0);   \
        __IO_SET((command));              \
        __GPIO_OUT(WE_PORT, WE_Pin, 1);   \
        __GPIO_OUT(CLE_PORT, CLE_Pin, 0); \
    } while (0)

#define __SEND_ADDRESS(page_addr, byte_addr) \
    do                                       \
    {                                        \
        __GPIO_OUT(ALE_PORT, ALE_Pin, 1);    \
        __GPIO_OUT(WE_PORT, WE_Pin, 0);      \
        __IO_SET((byte_addr));               \
        __GPIO_OUT(WE_PORT, WE_Pin, 1);      \
        __GPIO_OUT(WE_PORT, WE_Pin, 0);      \
        __IO_SET((byte_addr) >> 8);          \
        __GPIO_OUT(WE_PORT, WE_Pin, 1);      \
        __GPIO_OUT(WE_PORT, WE_Pin, 0);      \
        __IO_SET((page_addr));               \
        __GPIO_OUT(WE_PORT, WE_Pin, 1);      \
        __GPIO_OUT(WE_PORT, WE_Pin, 0);      \
        __IO_SET((page_addr) >> 8);          \
        __GPIO_OUT(WE_PORT, WE_Pin, 1);      \
        __GPIO_OUT(WE_PORT, WE_Pin, 0);      \
        __IO_SET((page_addr) >> 16);         \
        __GPIO_OUT(WE_PORT, WE_Pin, 1);      \
        __GPIO_OUT(ALE_PORT, ALE_Pin, 0);    \
    } while (0)

#define NAND_READ_ID 0x90
#define NAND_STATUS1 0x70
#define NAND_STATUS2 0xf1

void NAND_Init(void);
void NAND_read_ID(uint8_t arr[]);
// 一页2048+64kb.
void NAND_page_read(uint32_t page_addr, uint16_t byte_addr, uint8_t buffer[], uint16_t length);
uint8_t NAND_page_write(uint32_t page_addr, uint16_t byte_addr, uint8_t buffer[], uint16_t length);
uint8_t NAND_block_erase(uint16_t page_addr);
void NAND_test(uint16_t *ptr);

#endif
