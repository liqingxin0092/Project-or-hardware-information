#include "main.h"
#include "gpio.h"
#include "stdio.h"
#include "delay.h"
#include "my_freertos.h"
#include "usart.h"
#include "usmart.h"
#include "LTDC.h"
#include "SDRAM.h"
#include "fmc.h"
#include "rtc.h"

void SystemClock_Config(void);
uint8_t hour, min, sec, ampm;
uint8_t year, month, date, week;
int main(void)
{
  HAL_Init();                       // HAL库初始化
  SystemClock_Config();             // 时钟配置
  MX_USART1_UART_Init();            // 串口初始化
  MX_GPIO_Init();                   // SDRAM GPIO时钟配置,LCD背光IO配置
  delay_init(180);                  // 初始化延时
  usmart_dev.init(90);              // 初始化USMART,这里是定时器频率
  MX_FMC_Init();                    // SDRAM接口初始化
  SDRAM_Init(&__SDRAM_handle, 677); // 初始化SDRAM
  MX_LTDC_Init(BLACK);              // 初始化LTDC

  char p[30] = {0};
  sprintf(p, "rtc_state(1,failed;0,succeed):%d;", rtc_init()); // 初始化rtc
  LTDC_ShowString(0, 0, 1024, 24, 24, p, RED, 0);

  sprintf(p, "bkp0 data:%#x;", rtc_read_bkr(0)); //读备份
  LTDC_ShowString(650, 0, 1024, 24, 24, p, RED, 0);

  rtc_write_bkr(5, 45256);//写备份
  sprintf(p, "bkp5 data:%d;", rtc_read_bkr(5));
  LTDC_ShowString(396, 0, 1024, 24, 24, p, RED, 0);

  rtc_set_wakeup(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);//设置唤醒中断(秒中断)
  rtc_set_alarma(7, 3, 7, 0); /* 设置闹钟 */
  while (1)
  {
    delay_ms(500);

    rtc_get_time(&hour, &min, &sec, &ampm);
    rtc_get_date(&year, &month, &date, &week);

    sprintf(p, "%2d:%2d:%2d ", hour, min, sec);
    LTDC_ShowString(0, 24, 1024, 24, 24, p, RED, 0);
    sprintf(p, "%d-%d-%d  ", year + 2000, month, date);
    LTDC_ShowString(0, 48, 1024, 24, 24, p, GREEN, 0);
  }
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
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
