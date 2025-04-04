/**
 *******************************************************************************
 * @file        dma_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-22
 * @brief       dma常用功能
 *              
 *****************************************************************************************
 */
#ifndef __DMA_M_H
#define __DMA_M_H

#include "dma_m.h"
#include "stm32h750xx.h"

void bdam_trans(BDMA_Channel_TypeDef* bdma_chx,uint32_t p_addr,uint32_t m_addr,uint16_t lenth,uint16_t switch_state);


#endif 
