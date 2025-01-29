#include "spi.h"

SPI_HandleTypeDef hspi1;

void SPI1_Init(void)
{
	__HAL_RCC_SPI1_CLK_ENABLE(); // 开SPI时钟

	// PB3--SCK ,PB4--MISO,PB5--MOSI 初始化设置
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;			// 复用推挽
	GPIO_InitStruct.Speed = DMA_PRIORITY_VERY_HIGH; // 100MHz
	GPIO_InitStruct.Pull = GPIO_PULLUP;				// 上拉
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;		// 复用SPI1
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();
    
	/*配置SPI*/
	hspi1.Instance = SPI1;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;			  // 双线全双工
	hspi1.Init.Mode = SPI_MODE_MASTER;						  // 主模式
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;				  // 8位
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;				  // 空闲时高电平
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;					  // 第二个边沿
	hspi1.Init.NSS = SPI_NSS_SOFT;							  // 软件
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; // 2分频
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;					  // MSB
	hspi1.Init.CRCPolynomial = 7;
	HAL_SPI_Init(&hspi1);	  // 初始化
	__HAL_SPI_ENABLE(&hspi1); // cmd使能
	u8 p=0x14;
	HAL_SPI_Transmit(&hspi1, &p, 1, 50); // 启动传输
}

// SPI1速度设置函数 @ref SPI_BaudRate_Prescaler
// SPI速度=fAPB2/2^(SpeedSet+1)
// fAPB2时钟一般为84Mhz
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	__HAL_SPI_DISABLE(&hspi1);			// cmd使能
	SPI1->CR1 &= 0XFFC7;				// 位3-5清零，用来设置波特率
	SPI1->CR1 |= SPI_BaudRatePrescaler; // 设置SPI1速度
	__HAL_SPI_ENABLE(&hspi1);			// cmd使能
}

// SPI1 读写一个字节
// TxData:要写入的字节
// 返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
  while(!((SPI1->SR>>1)&1));
  SPI1->DR=TxData;
  while(!(SPI1->SR&1)); 
  return SPI1->DR;
}
