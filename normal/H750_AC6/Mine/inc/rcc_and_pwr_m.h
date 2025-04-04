/**
 *******************************************************************************
 * @file        rcc_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       探究一下rcc
 *              
 *****************************************************************************************
 */
#ifndef __RCC_AND_PWR_M_H
#define __RCC_AND_PWR_M_H

#include "stdint.h"

/*定义一下HSE时钟,单位MHZ*/
#define HSE_CLOCK 25

/*允许超频*/
#define USE_OVERCLOCK 1

/*仅cpu超频配置*/
//注意systic也超了
//外设时钟没超
//超到600M
//5,240,2,0xff,0xff
//1,4,2,2,2,2,0

/*正常480配置*/
//5,192,2,0xff,0xff
//1,2,2,2,2,2,0
void rcc_hse_pll1_init(uint8_t DIVM1,uint16_t DIVN1,uint8_t DIVP1,uint8_t DIVR1,uint8_t DIVQ1);
void rcc_cpu_periph_clk_init(uint16_t D1CPRE,uint16_t HPRE,uint8_t D1PPRE,uint8_t D2PPRE1,uint8_t D2PPRE2,uint8_t D3PPRE,uint8_t TIMPRE);
uint32_t rcc_get_system_clk(void);

#endif



