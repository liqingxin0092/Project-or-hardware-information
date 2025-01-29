/**
 ****************************************************************************************************
 * @file        btim.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-15
 * @brief       ������ʱ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211015
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __BTIM_H
#define __BTIM_H

#include "main.h"

/******************************************************************************************/
/* ������ʱ�� ���� */

/* TIMX �ж϶���
 * Ĭ�������TIM6/TIM7
 * ע��: ͨ���޸���4���궨��,����֧��TIM1~TIM8����һ����ʱ��.
 */

#define BTIM_TIM3_INT TIM3
#define BTIM_TIM3_INT_IRQn TIM3_IRQn
#define BTIM_TIM3_INT_IRQHandler TIM3_IRQHandler
#define BTIM_TIM3_INT_CLK_ENABLE()   \
    do                               \
    {                                \
        __HAL_RCC_TIM3_CLK_ENABLE(); \
    } while (0) /* TIM3 ʱ��ʹ�� */

/******************************************************************************************/

void btim_tim3_int_init(uint16_t arr, uint16_t psc);
void ConfigureTimeForRunTimeStats(void);

#endif
