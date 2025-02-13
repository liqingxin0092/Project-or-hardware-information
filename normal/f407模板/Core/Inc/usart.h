#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define USART_REC_LEN   500  //接收缓冲区大小
#define RXBUFFERSIZE    1    // 缓存大小 
extern uint16_t g_usart_rx_sta;
extern uint8_t g_usart_rx_buf[USART_REC_LEN];
extern uint8_t g_rx_buffer[RXBUFFERSIZE];
extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

