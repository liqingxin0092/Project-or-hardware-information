#ifndef __USART_H__
#define __USART_H__

#include "gd32f4xx.h"



#define USB_USART_DMA	0

#define  ARRAYNUM(arr_name)   (uint32_t)(sizeof(arr_name) / sizeof(*(arr_name)))

#define BSP_USART_TX_RCU  		RCU_GPIOA   		// ����TX�Ķ˿�ʱ��
#define BSP_USART_RX_RCU  		RCU_GPIOA   		// ����RX�Ķ˿�ʱ��
#define BSP_USART_RCU     		RCU_USART0  		// ����0��ʱ��

#define BSP_USART_TX_PORT 		GPIOA				// ����TX�Ķ˿�
#define BSP_USART_RX_PORT 		GPIOA				// ����RX�Ķ˿�
#define BSP_USART_AF 			GPIO_AF_7   		// ����0�ĸ��ù���
#define BSP_USART_TX_PIN  		GPIO_PIN_9  		// ����TX������
#define BSP_USART_RX_PIN  		GPIO_PIN_10 		// ����RX������

#define BSP_USART 				USART0      		// ����0
#define BSP_USART_IRQ     		USART0_IRQn 		// ����0�ж�
#define BSP_USART_IRQHandler  	USART0_IRQHandler	// ����0�жϷ�����


#define BSP_DMA_CH					DMA_CH2
#define BSP_DMA_CH_IRQ              DMA1_Channel2_IRQn  // DMA�ж�
#define BSP_DMA_CH_IRQHandler       DMA1_Channel2_IRQHandler  // DMA�жϷ�����

/* ���ڻ����������ݳ��� */
#define USART_RECEIVE_LENGTH  4096

extern uint8_t  g_recv_buff[USART_RECEIVE_LENGTH]; // ���ջ�����
extern uint16_t g_recv_length;										 // �������ݳ���
extern uint8_t  g_recv_complete_flag; 						 // ������ɱ�־λ

void usart_gpio_config(uint32_t band_rate);  			 // ���ô���
void usart_send_data(uint8_t ucch);          			 // ����һ���ַ�
void usart_send_string(uint8_t *ucstr);      			 // ����һ���ַ���
void usart_dma_config(void);
void BSP_DMA_CH_IRQHandler(void);

#endif