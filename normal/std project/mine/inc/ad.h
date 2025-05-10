/**
 *******************************************************************************
 * @file        ad.h
 * @author      liqingxin0092
 * @version     V1.1
 * @date        2025-3-29
 * @brief       精细控制ad转换
 * 更新记录:    3.30 增加了硬件过采样相关控制
 *              4.1  增加了dma传输扫描多路ADC数据
 *****************************************************************************************
 */
#ifndef __AD_H
#define __AD_H

#include "stdint.h"

#define GPIO_PORT_chx(chx)  chx_to_io_table[(chx)].GPIOx
#define GPIO_PIN_chx(chx)  chx_to_io_table[(chx)].GPIO_PIN_x

#define ADCx  ADC0
#define AD_TO_VOL(ad_val)  ((ad_val)/4096.0f*3.0f)

#define AD_INT_REF_VOL(ad_val,inter_val) (1.2f*(ad_val)/(inter_val))

/*ad_chx 2 io 结构体*/
typedef struct __attribute__((packed, aligned(1)))
{
    uint32_t GPIOx;
    uint32_t GPIO_PIN_x;
}chx_io_t;

/*ad_chx结构体*/
typedef struct __attribute__((packed, aligned(1)))
{
    uint8_t chx_length;
    uint8_t *chx_buffer;
}chx_sets_t;

void ad_init(uint32_t periph_num, uint8_t src_div, uint8_t chx_lenth, uint64_t chx_sets, uint8_t hw_ovsmpl);
void ad_single_init(uint32_t periph_num, uint8_t chx, uint8_t smp_time);
void ad_dma_init(uint32_t periph_num);
void ad_scan_init(uint32_t periph_num, chx_sets_t * p_struct,uint8_t smp_time);

uint16_t ad_start_once(uint32_t periph_num);
void ad_scan_once(uint32_t periph_num,uint16_t * m_addr);

uint16_t ad_soft_filter(uint32_t periph_num, uint16_t inter_fre, uint16_t ex_fre);


#endif

