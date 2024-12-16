#include "sdmmc_sdcard.h"
#include "usmart.h"
#include "delay.h"
#include "my_freertos.h"
#include "qspi_w25q64.h"
#include "lcd_spi_154.h"

/*���õĹ���ע��ע��*/
int main()
{
  HAL_Init();
  SystemClock_Config();  // ʱ�ӳ�ʼ��
  RCC_GPIO_Init();       // ��ʼ��GPIOʱ��
  MX_USART1_UART_Init(); // ���ڳ�ʼ��
  delay_init(480);       // ��ʼ����ʱ
  usmart_dev.init(240);  // ��ʼ��USMART,�����Ƕ�ʱ��Ƶ��
  QSPI_W25Qxx_Init();    // ��ʼ��W25Q64
  SPI_LCD_Init();        // Һ�����Լ�SPI��ʼ��
  SD_Init();             // ��ʼ��SD��.
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
// GPIO��������
// GPIOx:GPIOA~GPIOI.
// BITx:0~15,����IO���ű��.
// AFx:0~15,����AF0~AF15.
// AF0~15�������(��������г����õ�,��ϸ�����429/746�����ֲ�,Table 12):
// AF0:MCO/SWD/SWCLK/RTC   		AF1:TIM1/TIM2;            		AF2:TIM3~5;               		AF3:TIM8~11
// AF4:I2C1~I2C4;          		AF5:SPI1~SPI6;            		AF6:SPI3/SAI1;            	 	AF7:SPI2/3/USART1~3/UART5/SPDIFRX;
// AF8:USART4~8/SPDIFRX/SAI2; 	AF9;CAN1~2/TIM12~14/LCD/QSPI; 	AF10:USB_OTG/USB_HS/SAI2/QSPI  	AF11:ETH
// AF12:FMC/SDMMC/OTG/HS   		AF13:DCIM                 		AF14:LCD;                  		AF15:EVENTOUT
void GPIO_AF_Set(GPIO_TypeDef *GPIOx, u8 BITx, u8 AFx)
{
  GPIOx->AFR[BITx >> 3] &= ~(0X0F << ((BITx & 0X07) * 4));
  GPIOx->AFR[BITx >> 3] |= (u32)AFx << ((BITx & 0X07) * 4);
}
// GPIOͨ������
// GPIOx:GPIOA~GPIOI.
// BITx:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.����0X0101,����ͬʱ����Px0��Px8.
// MODE:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
// OTYPE:0/1;�������ѡ��,0,�������;1,��©���.
// OSPEED:0~3;����ٶ�����,0,����;1,����;2,����;3,����.
// PUPD:0~3:����������,0,����������;1,����;2,����;3,����.
// ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч!!
void GPIO_Set(GPIO_TypeDef *GPIOx, u32 BITx, u32 MODE, u32 OTYPE, u32 OSPEED, u32 PUPD)
{
  u32 pinpos = 0, pos = 0, curpin = 0;
  for (pinpos = 0; pinpos < 16; pinpos++)
  {
    pos = 1 << pinpos;   // һ����λ���
    curpin = BITx & pos; // ��������Ƿ�Ҫ����
    if (curpin == pos)   // ��Ҫ����
    {
      GPIOx->MODER &= ~(3 << (pinpos * 2)); // �����ԭ��������
      GPIOx->MODER |= MODE << (pinpos * 2); // �����µ�ģʽ
      if ((MODE == 0X01) || (MODE == 0X02)) // ��������ģʽ/���ù���ģʽ
      {
        GPIOx->OSPEEDR &= ~(3 << (pinpos * 2));     // ���ԭ��������
        GPIOx->OSPEEDR |= (OSPEED << (pinpos * 2)); // �����µ��ٶ�ֵ
        GPIOx->OTYPER &= ~(1 << pinpos);            // ���ԭ��������
        GPIOx->OTYPER |= OTYPE << pinpos;           // �����µ����ģʽ
      }
      GPIOx->PUPDR &= ~(3 << (pinpos * 2)); // �����ԭ��������
      GPIOx->PUPDR |= PUPD << (pinpos * 2); // �����µ�������
    }
  }
}
