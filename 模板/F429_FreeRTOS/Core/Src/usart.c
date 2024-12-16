#include "usart.h"
#include "stdio.h"

UART_HandleTypeDef huart1;
uint8_t uart_send_data;
uint16_t g_usart_rx_sta = 0; //状态
uint8_t g_rx_buffer[RXBUFFERSIZE];      /* HAL库使用的串口接收缓冲 */
uint8_t g_usart_rx_buf[USART_REC_LEN];//接收缓冲区

void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);
  HAL_NVIC_SetPriority(USART1_IRQn, 2, 0); 
  HAL_NVIC_EnableIRQ(USART1_IRQn);  
  HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);

  /*发送DMA控制*/ //USART1_TX DMA通道 DMA2,channel 4,stream7.
  USART1->CR3|=1<<7;//使能串口1的DMA模式
  __HAL_RCC_DMA2_CLK_ENABLE();//开DMA2时钟
  uint32_t temp=0;
  temp|=4<<25; //选择通道4
  temp|=3<<16; //优先级非常高
  temp|=1<<6;  //从内存到外设
  DMA2_Stream7->CR = temp;//控制寄存器赋值
  DMA2_Stream7->NDTR=1;//传输一个byte
  DMA2_Stream7->PAR=0x40011004;//外设地址 UART1->DR
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}
//int fputc(int ch, FILE *f)
//{
//  USART1->DR = ch;
//  while (((USART1->SR >> 7) & 1) != 1)
//    ;
//  return ch;
//}

//无论哪种方式都离不了while剐蹭CPU
int fputc(int ch, FILE *f)
{
  DMA2_Stream7->M0AR=(uint32_t)&ch; //源地址
  DMA2_Stream7->CR |=1;//启动DMA传输
  while(!(DMA2->HISR&0x8000000));//等待标志位为0
  DMA2->HIFCR|=1<<27;//清除标志位
  return ch;
}
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)             /* 如果是串口1 */
    {
        if((g_usart_rx_sta & 0x8000) == 0)      /* 接收未完成 */
        {
            if(g_usart_rx_sta & 0x4000)         /* 接收到了0x0d */
            {
                if(g_rx_buffer[0] != 0x0a) 
                {
                    g_usart_rx_sta = 0;         /* 接收错误,重新开始 */
                }
                else 
                {
                    g_usart_rx_sta |= 0x8000;   /* 接收完成了 */
                }
            }
            else                                /* 还没收到0X0D */
            {
                if(g_rx_buffer[0] == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0] ;
                    g_usart_rx_sta++;
                    if(g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;     /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, 1);
    }
}
