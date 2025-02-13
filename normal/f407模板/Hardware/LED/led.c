#include "led.h"

void LED_Init(void)
{
  /*GPIOF9,10��ʼ��*/
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;        // ����
  GPIO_InitStruct.Pull = GPIO_NOPULL;                // ����
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 100MHz
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*������*/
  __BIT_SET(GPIOF_ODR_Addr, 9, 1);
  __BIT_SET(GPIOF_ODR_Addr, 10, 1);
}

void set_LED1(uint8_t state)
{
     __BIT_SET(GPIOF_ODR_Addr, 9, state);
}
void set_LED2(uint8_t state)
{
     __BIT_SET(GPIOF_ODR_Addr, 10, state);
}

