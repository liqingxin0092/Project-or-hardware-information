#include "gpio.h"
#include "my_freertos.h"
#include "usmart.h"
#include "SDRAM.h"
#include "fmc.h"
#include "touch.h"
#include "ftl.h"
#include "w25qxx.h"
#include "TF-card.h"
#include "exfuns.h"
#include "fonts.h"
#include "text.h"

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();                       // HAL���ʼ��
  SystemClock_Config();             // ʱ������
  MX_USART1_UART_Init();            // ���ڳ�ʼ��
  MX_GPIO_Init();                   // SDRAM GPIOʱ������,LCD����IO����
  delay_init(180);                  // ��ʼ����ʱ
  usmart_dev.init(90);              // ��ʼ��USMART,�����Ƕ�ʱ��Ƶ��
  MX_FMC_Init();                    // SDRAM��NAND FLASH��ʼ��
  SDRAM_Init(&__SDRAM_handle, 250); // ��ʼ��SDRAM
  MX_LTDC_Init(0);                  // ��ʼ��LTDC
  Touch_init();                     // ��ʼ������IC
  W25QXX_Init();                    // spi flash��ʼ��
  exfuns_init();                    // Ϊfatfs��ر��������ڴ�
  f_mount(fs[0], "0:", 1);          // ����SD��
  f_mount(fs[1], "1:", 1);          // ����NAND FLASH
  fonts_init();                     // �ֿ��ʼ��
  
  freertos_start();                 // FreeRTOS��ʼ����
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
