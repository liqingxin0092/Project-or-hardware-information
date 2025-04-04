/**
 *******************************************************************************
 * @file        usart_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-4
 * @brief       ̽��USART����
 *
 *****************************************************************************************
 */

#ifndef __USAR_M_H
#define __USAR_M_H

#include "stdint.h"
#include "stm32h750xx.h"
#include "data_structure_m.h"
#include "h7_core.h"

/*���ô���*/
#define USE_USART1 1
#define USE_USART2 0
#define USE_USART3 0
#define USE_USART4 1

/*ʹ�ô����ض���*/
#define USART_APP usart1_app_struct

/*usmartѡ���ĸ�����*/
//ע�����ѡ��������Ǹ�����һ��,usmart������������.
#define USART_USED_BY_USMART 1

/*�ض��򷽰�*/
// 0:��ѯ. 1:����Ӳ��fifo. 2:д���fifo�жϷ�,��������ڴ���handle��ʹ��.
#define FPUTC_MODE 2

/*����Ĳ���2�ĸ���ѡ��*/
//��������ͻ������Ƚ�С�������������������,��֤������ȫ����
//1:����ʱ�任ȡ������.0:���������Ȼ�ȡʱ��(���ͻ��������Ϳ��Ա���).
#define ALLOW_BLOCKING 0

/*���ڲ����ṹ��*/
// �����fifo,���������жϺ���ʹ��
typedef struct
{
    circle_buffer_t tx_fifo_struct; // ����fifo�ṹ��
    circle_buffer_t rx_fifo_struct; // ����fifo�ṹ��
    USART_TypeDef *USARTx;          // �Ĵ���������
} usart_t;

/*������Ա��ⲿ����*/
#if USE_USART1 == 1
extern usart_t usart1_app_struct;
#endif

#if USE_USART4 == 1
extern usart_t usart4_app_struct;
#endif

/*����һ���ַ�*/
// ������
static inline void usart_send_byte(usart_t *usart_app, uint8_t val)
{  
#if ALLOW_BLOCKING==1
    while(buffer_is_full(&usart_app->tx_fifo_struct));//�����������ȵ�
#endif
    write_buffer(&usart_app->tx_fifo_struct, val);
    usart_app->USARTx->CR1 |= 0x80; // ��TXFNF�ж�
}

/*����ʹ��*/
#define enable_usart(USARTx) USARTx->CR1 |= 1

void usart_init(USART_TypeDef *USARTx, uint16_t switch_state, uint8_t fifo_init_num);
void baud_rate_set(USART_TypeDef *USARTx, uint32_t current_clk_fz, uint8_t div, uint32_t baud_rate);
void usart_set_hardware_fifo(USART_TypeDef *USARTx, uint8_t tx_fifo, uint8_t rx_fifo);
uint8_t  usart_transmit_it(usart_t *usart_app, uint8_t *psrc, uint32_t lenth);
uint8_t usart_receive_char(usart_t *usart_app,uint8_t *ptr);
void usart_init_app(void);

#endif
