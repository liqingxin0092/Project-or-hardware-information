/**
 *******************************************************************************
 * @file        usart_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-4
 * @brief       探究USART功能
 *
 *****************************************************************************************
 */

#ifndef __USAR_M_H
#define __USAR_M_H

#include "stdint.h"
#include "stm32h750xx.h"
#include "data_structure_m.h"
#include "h7_core.h"

/*启用串口*/
#define USE_USART1 1
#define USE_USART2 0
#define USE_USART3 0
#define USE_USART4 1

/*使用串口重定向*/
#define USART_APP usart1_app_struct

/*usmart选择哪个串口*/
//注意这个选项和上面那个保持一致,usmart才能正常工作.
#define USART_USED_BY_USMART 1

/*重定向方案*/
// 0:轮询. 1:串口硬件fifo. 2:写软件fifo中断发,这个不能在错误handle中使用.
#define FPUTC_MODE 2

/*上面的参数2的附加选项*/
//在软件发送缓冲区比较小的情况下允许阻塞发送,保证内容完全发送
//1:牺牲时间换取完整度.0:牺牲完整度换取时间(发送缓冲区大点就可以避免).
#define ALLOW_BLOCKING 0

/*串口操作结构体*/
// 带软件fifo,被函数和中断函数使用
typedef struct
{
    circle_buffer_t tx_fifo_struct; // 发送fifo结构体
    circle_buffer_t rx_fifo_struct; // 发送fifo结构体
    USART_TypeDef *USARTx;          // 寄存器操作体
} usart_t;

/*句柄可以被外部引用*/
#if USE_USART1 == 1
extern usart_t usart1_app_struct;
#endif

#if USE_USART4 == 1
extern usart_t usart4_app_struct;
#endif

/*发送一个字符*/
// 缓冲区
static inline void usart_send_byte(usart_t *usart_app, uint8_t val)
{  
#if ALLOW_BLOCKING==1
    while(buffer_is_full(&usart_app->tx_fifo_struct));//如果满了在这等的
#endif
    write_buffer(&usart_app->tx_fifo_struct, val);
    usart_app->USARTx->CR1 |= 0x80; // 开TXFNF中断
}

/*串口使能*/
#define enable_usart(USARTx) USARTx->CR1 |= 1

void usart_init(USART_TypeDef *USARTx, uint16_t switch_state, uint8_t fifo_init_num);
void baud_rate_set(USART_TypeDef *USARTx, uint32_t current_clk_fz, uint8_t div, uint32_t baud_rate);
void usart_set_hardware_fifo(USART_TypeDef *USARTx, uint8_t tx_fifo, uint8_t rx_fifo);
uint8_t  usart_transmit_it(usart_t *usart_app, uint8_t *psrc, uint32_t lenth);
uint8_t usart_receive_char(usart_t *usart_app,uint8_t *ptr);
void usart_init_app(void);

#endif
