/**
 *******************************************************************************
 * @file        rcc_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       ̽��һ��rcc
 *              
 *****************************************************************************************
 */
#ifndef __RCC_AND_PWR_M_H
#define __RCC_AND_PWR_M_H

#include "stdint.h"

/*����һ��HSEʱ��,��λMHZ*/
#define HSE_CLOCK 25

/*����Ƶ*/
#define USE_OVERCLOCK 1

/*��cpu��Ƶ����*/
//ע��systicҲ����
//����ʱ��û��
//����600M
//5,240,2,0xff,0xff
//1,4,2,2,2,2,0

/*����480����*/
//5,192,2,0xff,0xff
//1,2,2,2,2,2,0
void rcc_hse_pll1_init(uint8_t DIVM1,uint16_t DIVN1,uint8_t DIVP1,uint8_t DIVR1,uint8_t DIVQ1);
void rcc_cpu_periph_clk_init(uint16_t D1CPRE,uint16_t HPRE,uint8_t D1PPRE,uint8_t D2PPRE1,uint8_t D2PPRE2,uint8_t D3PPRE,uint8_t TIMPRE);
uint32_t rcc_get_system_clk(void);

#endif



