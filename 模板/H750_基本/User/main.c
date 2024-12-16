#include "sdmmc_sdcard.h"
#include "usmart.h"
#include "delay.h"
#include "my_freertos.h"
#include "qspi_w25q64.h"
#include "lcd_spi_154.h"

/*不用的功能注意注释*/
int main()
{
  HAL_Init();
  SystemClock_Config();  // 时钟初始化
  RCC_GPIO_Init();       // 初始化GPIO时钟
  MX_USART1_UART_Init(); // 串口初始化
  delay_init(480);       // 初始化延时
  usmart_dev.init(240);  // 初始化USMART,这里是定时器频率
  QSPI_W25Qxx_Init();    // 初始化W25Q64
  SPI_LCD_Init();        // 液晶屏以及SPI初始化
  SD_Init();             // 初始化SD卡.
  LCD_DisplayText(0, 0, "hello");
  freertos_demo();
}

void RCC_GPIO_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_Struct = {0};
  GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Struct.Pin = GPIO_PIN_13;
  GPIO_Struct.Pull = GPIO_NOPULL;
  GPIO_Struct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_Struct);
}

void Error_Handler(void);
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
   */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }
  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
// GPIO复用设置
// GPIOx:GPIOA~GPIOI.
// BITx:0~15,代表IO引脚编号.
// AFx:0~15,代表AF0~AF15.
// AF0~15设置情况(这里仅是列出常用的,详细的请见429/746数据手册,Table 12):
// AF0:MCO/SWD/SWCLK/RTC   		AF1:TIM1/TIM2;            		AF2:TIM3~5;               		AF3:TIM8~11
// AF4:I2C1~I2C4;          		AF5:SPI1~SPI6;            		AF6:SPI3/SAI1;            	 	AF7:SPI2/3/USART1~3/UART5/SPDIFRX;
// AF8:USART4~8/SPDIFRX/SAI2; 	AF9;CAN1~2/TIM12~14/LCD/QSPI; 	AF10:USB_OTG/USB_HS/SAI2/QSPI  	AF11:ETH
// AF12:FMC/SDMMC/OTG/HS   		AF13:DCIM                 		AF14:LCD;                  		AF15:EVENTOUT
void GPIO_AF_Set(GPIO_TypeDef *GPIOx, u8 BITx, u8 AFx)
{
  GPIOx->AFR[BITx >> 3] &= ~(0X0F << ((BITx & 0X07) * 4));
  GPIOx->AFR[BITx >> 3] |= (u32)AFx << ((BITx & 0X07) * 4);
}
// GPIO通用设置
// GPIOx:GPIOA~GPIOI.
// BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
// MODE:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
// OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
// OSPEED:0~3;输出速度设置,0,低速;1,中速;2,快速;3,高速.
// PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
// 注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
void GPIO_Set(GPIO_TypeDef *GPIOx, u32 BITx, u32 MODE, u32 OTYPE, u32 OSPEED, u32 PUPD)
{
  u32 pinpos = 0, pos = 0, curpin = 0;
  for (pinpos = 0; pinpos < 16; pinpos++)
  {
    pos = 1 << pinpos;   // 一个个位检查
    curpin = BITx & pos; // 检查引脚是否要设置
    if (curpin == pos)   // 需要设置
    {
      GPIOx->MODER &= ~(3 << (pinpos * 2)); // 先清除原来的设置
      GPIOx->MODER |= MODE << (pinpos * 2); // 设置新的模式
      if ((MODE == 0X01) || (MODE == 0X02)) // 如果是输出模式/复用功能模式
      {
        GPIOx->OSPEEDR &= ~(3 << (pinpos * 2));     // 清除原来的设置
        GPIOx->OSPEEDR |= (OSPEED << (pinpos * 2)); // 设置新的速度值
        GPIOx->OTYPER &= ~(1 << pinpos);            // 清除原来的设置
        GPIOx->OTYPER |= OTYPE << pinpos;           // 设置新的输出模式
      }
      GPIOx->PUPDR &= ~(3 << (pinpos * 2)); // 先清除原来的设置
      GPIOx->PUPDR |= PUPD << (pinpos * 2); // 设置新的上下拉
    }
  }
}
