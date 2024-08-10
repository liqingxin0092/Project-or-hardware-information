/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "func.h"
#include "string.h"
#include "Delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t NumBuf[8];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Show()
{
  FixWrite(ADD_00, SegNum[NumBuf[0]]);
  FixWrite(ADD_02, SegNum[NumBuf[1]]);
  FixWrite(ADD_04, SegNum[NumBuf[2]]);
  FixWrite(ADD_06, SegNum[NumBuf[3]]);
  FixWrite(ADD_08, SegNum[NumBuf[4]]);
  FixWrite(ADD_0A, SegNum[NumBuf[5]]);
  FixWrite(ADD_0C, SegNum[NumBuf[6]]);
  FixWrite(ADD_0E, SegNum[NumBuf[7]]);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  ContactInit();
  SendByte(SHOW_10_16);
  SendByte(DATA_FIX_WRITE);

  FixWrite(ADD_01, 1);
  FixWrite(ADD_03, 1);
  FixWrite(ADD_05, 1);
  FixWrite(ADD_07, 1);
  FixWrite(ADD_09, 1);
  FixWrite(ADD_0B, 1);
  FixWrite(ADD_0D, 1);
  FixWrite(ADD_0F, 1);
  Show();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    ReadKey();

    for (uint8_t i = 0; i < 4; i++)
    {
      uint16_t count = 0;
      if (KeyBuf[i] == 0x01)
      {
        NumBuf[i]++;
        if (NumBuf[i] == 10) // 保证数据范围
          NumBuf[i] = 0;
        Show();

        if (NumBuf[i] % 2 == 0) // 设置LED
          FixWrite(0xc0 | (2 * i + 1), 1);
        else
          FixWrite(0xc0 | (2 * i + 1), 0);

        while (KeyBuf[i] == 0x01) // 等待长摁
        {
          ReadKey();

          Delay_ms(100);
          count++;
          if (count > 3)
          {
            NumBuf[i]++;
            if (NumBuf[i] == 10) // 保证数据范围
              NumBuf[i] = 0;

            if (NumBuf[i] % 2 == 0) // 设置LED
              FixWrite(0xc0 | (2 * i + 1), 1);
            else
              FixWrite(0xc0 | (2 * i + 1), 0);

            Show();
          }
        }
      }
      else if (KeyBuf[i] == 0x10)
      {
        NumBuf[i + 4]++;
        if (NumBuf[i + 4] == 10)
          NumBuf[i + 4] = 0;
        Show();

        if (NumBuf[i + 4] % 2 == 0) // 设置LED
          FixWrite(0xc0 | (2 * (i + 4) + 1), 1);
        else
          FixWrite(0xc0 | (2 * (i + 4) + 1), 0);

        while (KeyBuf[i] == 0x10)
        {
          ReadKey();

          Delay_ms(100);
          count++;
          if (count > 3)
          {
            NumBuf[i + 4]++;
            if (NumBuf[i + 4] == 10) // 保证数据范围
              NumBuf[i + 4] = 0;

            if (NumBuf[i + 4] % 2 == 0) // 设置LED
              FixWrite(0xc0 | (2 * (i + 4) + 1), 1);
            else
              FixWrite(0xc0 | (2 * (i + 4) + 1), 0);

            Show();
          }
        }
      }
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
