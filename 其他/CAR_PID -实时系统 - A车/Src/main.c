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
#include "cmsis_os.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "pid.h"
#include "motor.h"
#include "cJSON.h"
#include "IM948.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int32_t encoder_count_motor_a;
int32_t encoder_count_motor_b;
int32_t encoder_count_motor_c;
int32_t encoder_count_motor_d;

extern wheel_t LeftUp;
extern wheel_t LeftDown;
extern wheel_t RightUp;
extern wheel_t RightDown;

double ActDistance;                     // Current travel distance
extern uint8_t Need_To_Record_Distance; // Whether to record the distance

float p, i, d, a;                        // debug parameter
cJSON *cJsonData, *cJsonVlaue;           // debug
extern DMA_HandleTypeDef hdma_usart3_rx; // debug communication
uint8_t receivedata[50];

extern DMA_HandleTypeDef hdma_usart2_rx; // receive direction
extern F32 tmpZ;                         // direction
extern struct_Ram_Uart Uart;
uint8_t rx_byte;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart2) // direction
  {
    if (FifoSize > Uart.UartFifo.Cnt)
    {
      Uart.UartFifo.RxBuf[Uart.UartFifo.In] = rx_byte;
      if (++Uart.UartFifo.In >= FifoSize)
      {
        Uart.UartFifo.In = 0;
      }
      ++Uart.UartFifo.Cnt;
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, &rx_byte, 1);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
  }
  if (huart == &huart3) // debug
  {
    cJsonData = cJSON_Parse((const char *)receivedata);
    if (cJSON_GetObjectItem(cJsonData, "p") != NULL)
    {
      cJsonVlaue = cJSON_GetObjectItem(cJsonData, "p");
      p = cJsonVlaue->valuedouble;
//      LeftDown.pid_struct.p = LeftUp.pid_struct.p = RightDown.pid_struct.p = RightUp.pid_struct.p = p;
      LeftDown.pid_struct.p=p;
//      LeftUp.pid_struct.p=p;
//        RightDown.pid_struct.p=p;
    }
    if (cJSON_GetObjectItem(cJsonData, "i") != NULL)
    {
      cJsonVlaue = cJSON_GetObjectItem(cJsonData, "i");
      i = cJsonVlaue->valuedouble;
//      LeftDown.pid_struct.i = LeftUp.pid_struct.i = RightDown.pid_struct.i = RightUp.pid_struct.i = i;
      LeftDown.pid_struct.i=i;
//      LeftUp.pid_struct.i=i;
//        RightUp.pid_struct.i=i;
//        RightDown.pid_struct.i=i;
    }
    if (cJSON_GetObjectItem(cJsonData, "d") != NULL)
    {
      cJsonVlaue = cJSON_GetObjectItem(cJsonData, "d");
      d = cJsonVlaue->valuedouble;
//      LeftDown.pid_struct.d = LeftUp.pid_struct.d = RightDown.pid_struct.d = RightUp.pid_struct.d = d;
      LeftDown.pid_struct.d=d;
//      LeftUp.pid_struct.d=d;
//        RightDown.pid_struct.d=d;
    }
    if (cJSON_GetObjectItem(cJsonData, "a") != NULL)
    {
      cJsonVlaue = cJSON_GetObjectItem(cJsonData, "a");
      a = cJsonVlaue->valuedouble;
//      LeftDown.pid_struct.tar = RightDown.pid_struct.tar = LeftUp.pid_struct.tar = RightUp.pid_struct.tar = a;
      LeftDown.pid_struct.tar=a;
//      LeftUp.pid_struct.tar=a;
//        RightDown.pid_struct.tar=a;
    }
    if (cJsonData != NULL)
    {
      cJSON_Delete(cJsonData);
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, receivedata, 50);
    __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
  }
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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM9_Init();
  MX_TIM12_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_TIM7_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);            // motor_pwm
  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);            //
  HAL_TIM_PWM_Start(&MOTOR_B_TIMER, MOTOR_B_CHANNEL_1); // tim9
  HAL_TIM_PWM_Start(&MOTOR_B_TIMER, MOTOR_B_CHANNEL_2); //
  HAL_TIM_PWM_Start(&MOTOR_C_TIMER, MOTOR_C_CHANNEL_1); // tim1
  HAL_TIM_PWM_Start(&MOTOR_C_TIMER, MOTOR_C_CHANNEL_2); //
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);             // tim1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

  HAL_TIM_PWM_Start(&SENSOR, TIM_CHANNEL_2); // Servo engine

  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL); // speed
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);

  OLED_Init(); // OLED
  OLED_Clear();
  OLED_Display_On();

  HAL_TIM_Base_Start_IT(&htim7); // TIM_PeriodElapsedCallback,get speed,set speed

  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, receivedata, 50); // debug
  __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);

  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, &rx_byte, 200);
  __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT); // direction

  Wheels_Init(); // Wheels
  MIX_PID_Init();

  Cmd_03(); // direction
  Cmd_06(); // init yaw
  Cmd_12(5, 255, 0, 1, 3, 2, 2, 4, 9, 0x40);
  Cmd_19();

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6) // freeRtos Time
  {
    HAL_IncTick();
  }
  if (htim->Instance == TIM7) //  0.01s , get speed
  {
    encoder_count_motor_a = (short)__HAL_TIM_GET_COUNTER(&htim2);
    encoder_count_motor_b = (short)__HAL_TIM_GET_COUNTER(&htim3);
    encoder_count_motor_c = (short)__HAL_TIM_GET_COUNTER(&htim4);
    encoder_count_motor_d = (short)__HAL_TIM_GET_COUNTER(&htim5);

    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    __HAL_TIM_SET_COUNTER(&htim5, 0);
    LeftUp.speed = encoder_count_motor_c / 740.0f * 100;
    LeftDown.speed = encoder_count_motor_d / 740.0f * 100;
    RightDown.speed = -encoder_count_motor_a / 740.0f * 100;
    RightUp.speed = -encoder_count_motor_b / 1480.0f * 100;

    if (Need_To_Record_Distance)
      ActDistance += 0.01f * LeftUp.speed * 20.8f;
  }
}

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */

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
