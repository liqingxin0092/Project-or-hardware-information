/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdarg.h>     
#include <string.h>
#include <stdio.h>
#include "IM948.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern DMA_HandleTypeDef hdma_usart1_rx;
extern struct_Ram_Uart Uart;
extern   F32 tmpX, tmpY, tmpZ, tmpAbs;
uint8_t rel;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
char String[50];
void USART_Printf( UART_HandleTypeDef * chuankou,char *format, ...)
{
	va_list arg;							//定义可变参数列表数据类型的变量arg
	va_start(arg, format);					//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);			//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);						     	//结束变量arg
	HAL_UART_Transmit(chuankou, (uint8_t*)String, strlen(String), 200);
}
/* USER CODE END Variables */
osThreadId OLEDHandle;
osThreadId ANAHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void OLEDF(void const * argument);
void ANAF(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of OLED */
  osThreadDef(OLED, OLEDF, osPriorityNormal, 0, 128);
  OLEDHandle = osThreadCreate(osThread(OLED), NULL);

  /* definition and creation of ANA */
  osThreadDef(ANA, ANAF, osPriorityNormal, 0, 128);
  ANAHandle = osThreadCreate(osThread(ANA), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_OLEDF */
/**
  * @brief  Function implementing the OLED thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_OLEDF */
void OLEDF(void const * argument)
{
  /* USER CODE BEGIN OLEDF */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END OLEDF */
}

/* USER CODE BEGIN Header_ANAF */
/**
* @brief Function implementing the ANA thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ANAF */
void ANAF(void const * argument)
{
  /* USER CODE BEGIN ANAF */
  /* Infinite loop */
  for(;;)
  {
    		        // 处理模块发过来的数据----------------------------------
		U8 rxByte;
		int i;
		for (i = 0; i < 1000; i++)
		{
			if (Uart.UartFifo. Cnt > 0)
			{// 从fifo获取串口发来的数据
					rxByte = Uart.UartFifo.RxBuf[Uart.UartFifo.Out];
					if (++Uart.UartFifo.Out >= FifoSize)
					{
							Uart.UartFifo.Out = 0;
					}
					__disable_irq();
					--Uart.UartFifo.Cnt;
					__enable_irq();					
					 rel=Cmd_GetPkt(rxByte); // 移植 每收到1字节数据都填入该函数，当抓取到有效的数据包就会回调进入 Cmd_RxUnpack(U8 *buf, U8 DLen) 函数处理
			}
		}		
		 USART_Printf( &huart2,"rel=%d  X=%.1f  Y=%.1f  Z=%.1f  \r\n",rel,tmpX ,tmpY,tmpZ);
    osDelay(500);
  }
  /* USER CODE END ANAF */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

