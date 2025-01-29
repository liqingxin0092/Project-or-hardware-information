#include "usart.h"
#include "stdio.h"

UART_HandleTypeDef huart1;

uint16_t g_usart_rx_sta = 0; //״̬
uint8_t g_rx_buffer[RXBUFFERSIZE];      /* HAL��ʹ�õĴ��ڽ��ջ��� */
uint8_t g_usart_rx_buf[USART_REC_LEN];//���ջ�����

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

int fputc(int ch, FILE *f)
{
  USART1->DR = ch;
  while (((USART1->SR >> 7) & 1) != 1)
    ;
  return ch;
}
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)             /* ����Ǵ���1 */
    {
        if((g_usart_rx_sta & 0x8000) == 0)      /* ����δ��� */
        {
            if(g_usart_rx_sta & 0x4000)         /* ���յ���0x0d */
            {
                if(g_rx_buffer[0] != 0x0a) 
                {
                    g_usart_rx_sta = 0;         /* ���մ���,���¿�ʼ */
                }
                else 
                {
                    g_usart_rx_sta |= 0x8000;   /* ��������� */
                }
            }
            else                                /* ��û�յ�0X0D */
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
                        g_usart_rx_sta = 0;     /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)g_rx_buffer, 1);
    }
}
