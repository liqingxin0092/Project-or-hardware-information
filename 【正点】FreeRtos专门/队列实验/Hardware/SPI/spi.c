#include "spi.h"

SPI_HandleTypeDef hspi1;

void SPI1_Init(void)
{
	__HAL_RCC_SPI1_CLK_ENABLE(); // ��SPIʱ��

	// PB3--SCK ,PB4--MISO,PB5--MOSI ��ʼ������
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;			// ��������
	GPIO_InitStruct.Speed = DMA_PRIORITY_VERY_HIGH; // 100MHz
	GPIO_InitStruct.Pull = GPIO_PULLUP;				// ����
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;		// ����SPI1
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();
    
	/*����SPI*/
	hspi1.Instance = SPI1;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;			  // ˫��ȫ˫��
	hspi1.Init.Mode = SPI_MODE_MASTER;						  // ��ģʽ
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;				  // 8λ
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;				  // ����ʱ�ߵ�ƽ
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;					  // �ڶ�������
	hspi1.Init.NSS = SPI_NSS_SOFT;							  // ���
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; // 2��Ƶ
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;					  // MSB
	hspi1.Init.CRCPolynomial = 7;
	HAL_SPI_Init(&hspi1);	  // ��ʼ��
	__HAL_SPI_ENABLE(&hspi1); // cmdʹ��
	u8 p=0x14;
	HAL_SPI_Transmit(&hspi1, &p, 1, 50); // ��������
}

// SPI1�ٶ����ú��� @ref SPI_BaudRate_Prescaler
// SPI�ٶ�=fAPB2/2^(SpeedSet+1)
// fAPB2ʱ��һ��Ϊ84Mhz
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	__HAL_SPI_DISABLE(&hspi1);			// cmdʹ��
	SPI1->CR1 &= 0XFFC7;				// λ3-5���㣬�������ò�����
	SPI1->CR1 |= SPI_BaudRatePrescaler; // ����SPI1�ٶ�
	__HAL_SPI_ENABLE(&hspi1);			// cmdʹ��
}

// SPI1 ��дһ���ֽ�
// TxData:Ҫд����ֽ�
// ����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
  while(!((SPI1->SR>>1)&1));
  SPI1->DR=TxData;
  while(!(SPI1->SR&1)); 
  return SPI1->DR;
}
