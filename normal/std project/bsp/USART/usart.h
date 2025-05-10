#ifndef __USART_H__
#define __USART_H__

#include "gd32f4xx.h"



#define USB_USART_DMA	0

#define  ARRAYNUM(arr_name)   (uint32_t)(sizeof(arr_name) / sizeof(*(arr_name)))

#define BSP_USART_TX_RCU  		RCU_GPIOA   		// 串口TX的端口时钟
#define BSP_USART_RX_RCU  		RCU_GPIOA   		// 串口RX的端口时钟
#define BSP_USART_RCU     		RCU_USART0  		// 串口0的时钟

#define BSP_USART_TX_PORT 		GPIOA				// 串口TX的端口
#define BSP_USART_RX_PORT 		GPIOA				// 串口RX的端口
#define BSP_USART_AF 			GPIO_AF_7   		// 串口0的复用功能
#define BSP_USART_TX_PIN  		GPIO_PIN_9  		// 串口TX的引脚
#define BSP_USART_RX_PIN  		GPIO_PIN_10 		// 串口RX的引脚

#define BSP_USART 				USART0      		// 串口0
#define BSP_USART_IRQ     		USART0_IRQn 		// 串口0中断
#define BSP_USART_IRQHandler  	USART0_IRQHandler	// 串口0中断服务函数


#define BSP_DMA_CH					DMA_CH2
#define BSP_DMA_CH_IRQ              DMA1_Channel2_IRQn  // DMA中断
#define BSP_DMA_CH_IRQHandler       DMA1_Channel2_IRQHandler  // DMA中断服务函数

/* 串口缓冲区的数据长度 */
#define USART_RECEIVE_LENGTH  4096

extern uint8_t  g_recv_buff[USART_RECEIVE_LENGTH]; // 接收缓冲区
extern uint16_t g_recv_length;										 // 接收数据长度
extern uint8_t  g_recv_complete_flag; 						 // 接收完成标志位

void usart_gpio_config(uint32_t band_rate);  			 // 配置串口
void usart_send_data(uint8_t ucch);          			 // 发送一个字符
void usart_send_string(uint8_t *ucstr);      			 // 发送一个字符串
void usart_dma_config(void);
void BSP_DMA_CH_IRQHandler(void);

#endif