#ifndef __USART_H__
#define __USART_H__
    
#include "stm32h7xx.h"
    
#define USART_REC_LEN   500  //���ջ�������С
#define RXBUFFERSIZE    1    // �����С 
extern uint16_t g_usart_rx_sta;
extern uint8_t g_usart_rx_buf[USART_REC_LEN];
extern uint8_t g_rx_buffer[RXBUFFERSIZE];
extern UART_HandleTypeDef huart1;



void MX_USART1_UART_Init(void);



#endif /* __USART_H__ */

