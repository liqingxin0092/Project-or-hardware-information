#include "main.h"
#include "led.h"  
#include "usart.h"
#include "qspi_w25q64.h"
#include <string.h>


typedef  void (*pFunction)(void);
pFunction JumpToApplication;//定义一个函数指针

void SystemClock_Config(void);	// 系统时钟初始化


int main(void)
{
	SCB_EnableICache();
	SCB_EnableDCache();

	HAL_Init();             // 初始化HAL库
	SystemClock_Config();   // 配置系统时钟，主频480MH

	QSPI_W25Qxx_Init();	  				 // 初始化W25Q64
	QSPI_W25Qxx_MemoryMappedMode(); 	 // 配置QSPI为内存映射模式

	SCB_DisableICache();		// 关闭ICache
	SCB_DisableDCache();		// 关闭Dcache

	SysTick->CTRL = 0;		// 关闭SysTick
	SysTick->LOAD = 0;		// 清零重载值
	SysTick->VAL = 0;			// 清零计数值

	JumpToApplication = (pFunction) (*(__IO uint32_t*) (W25Qxx_MemoryMapped_Addr + 4));	// 设置起始地址
	__set_MSP(*(__IO uint32_t*) W25Qxx_MemoryMapped_Addr);	// 设置主堆栈指针
	JumpToApplication();			// 执行跳转

	while (1)
	{	

	}
}

/****************************************************************************************************/
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 480000000 (CPU Clock)
  *            HCLK(Hz)                       = 240000000 (AXI and AHBs Clock)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 192
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
  
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable 
  */
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);
//  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the CPU, AHB and APB busses clocks 
  */
	  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }	  
//	  
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//  RCC_OscInitStruct.PLL.PLLM = 5;		
//  RCC_OscInitStruct.PLL.PLLN = 192;
//  RCC_OscInitStruct.PLL.PLLP = 2;
//  RCC_OscInitStruct.PLL.PLLQ = 4;
//  RCC_OscInitStruct.PLL.PLLR = 2;
//  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
//  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
//  RCC_OscInitStruct.PLL.PLLFRACN = 0;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
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
  
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_QSPI; // 选择外设时钟
  
/*************************************** QSPI内核时钟设置 **************************************************/
/*** FANKE ***
>>>>> 配置说明：
	*
	*	1.	W25Q64JV 所允许的最高驱动频率为133MHz(必须是后缀JV的器件才能达到133M)
	*	2. 750的QSPI最高驱动频率也是133MHz(参考STM32H750数据手册 第 7.3.18小节： Quad-SPI interface characteristics)
	*	3. QSPI内核时钟可以是 rcc_hclk3 / pll1_q_ck / pll2_r_ck / per_ck 其中一个，注意这里的内核时钟不是实际通信的驱动时钟！
	*	4. 其中 per_ck 的时钟源可以是 hse_ck / hsi_ker_ck / csi_ker_ck ，这三个时钟都只有几十兆，无法满足QSPI的高速通信需求
	*	5. rcc_hclk3 由系统时钟2分频之后产生，当主频为480M时，rcc_hclk3 = 240M
	*	6. pll1_q_ck 由 PLLN倍频之后（960M）再分频产生，最大可以设置为480M
	*	7. 因为QSPI驱动时钟只能由内核时钟进行整数分频，所以当选择 rcc_hclk3 和 pll1_q_ck作为内核时钟时，无法得到133M的最高速度
	*	8. 使用 pll2_r_ck 作为QSPI内核时钟,经过设置得到133M,然后QSPI将分频设置1,最终可以得到133M的驱动时钟
	*	9. QSPI内核时钟最高只允许到250M，详情可以查阅英文版的750参考手册 第8.5.8小节  Kernel clock selection（版本：RM0433 Rev 7 ，February 2020 ）
	*
>>>>> 重要说明：
	*
	*	1.W25QXX的擦除时间是限定的!!! 手册给出的典型参考值为: 4K-45ms, 32K-150ms ,64K-150ms,整片擦除20S
	*
	*	2.W25QXX的写入时间是限定的!!! 手册给出的典型参考值为: 256字节-0.4ms，也就是 1M字节/s （实测大概在600K字节/s左右）
	*
	*	3.如果使用库函数直接读取，那么是否使用DMA、是否开启Cache、编译器的优化等级以及数据存储区的位置(内部 TCM SRAM 或者 AXI SRAM)都会影响读取的速度
	*
	*	4.如果使用内存映射模式，则读取性能只与QSPI的驱动时钟有关
	*
	*	5.使用库函数进行直接读取，keil版本5.30，编译器AC6.14，编译等级Oz image size，读取速度为 7M字节/S ，数据放在TCM SRAM 或者 AXI SRAM  
	*    都是差不多的结果，因为CPU直接访问外设寄存器的效率很低，直接使用HAL库进行读取的话，速度很慢
	*
	*	6.如果使用MDMA进行读取，可以达到 58M字节/S，使用内存映射模式的话，几乎可以达到驱动时钟的全速，62.14M/s
	*
	*  7.W25Q64JV 所允许的最高驱动频率为133MHz，750的QSPI最高驱动频率也是133MHz ，但是对于HAL库函数直接读取而言，驱动时钟超过15M已经不会对性能有提升
	*
	*	8.对于内存映射模式直接读取而言，驱动时钟超过127.5M已经不会对性能有提升，因为QSPI内核时钟最高限定为250M，所以建议实际QSPI驱动时钟不要超过125M，
	*	  具体的时钟配置请参考 SystemClock_Config 函数
	*
	*	9.实际使用中，当数据比较大时，建议使用64K或者32K擦除，擦除时间比4K擦除块	
	*
*** FANKE ***/
  
  
//  // 以下代码由cubeMX生成
//	PeriphClkInitStruct.PLL2.PLL2M = 25;   // 晶振分频系数25(注：FK750M1-VBT6的晶振为25M)
//	PeriphClkInitStruct.PLL2.PLL2N = 500;	// 将经过预分频后的晶振时钟进行500倍倍频，得到500M时钟
//	PeriphClkInitStruct.PLL2.PLL2P = 2;		//	这个时钟无关QSPI，用户可自由配置和使用
//	PeriphClkInitStruct.PLL2.PLL2Q = 2;		// 这个时钟无关QSPI，用户可自由配置和使用
//	PeriphClkInitStruct.PLL2.PLL2R = 2;		// 进行2分频，得到 250M 的 pll2_r_ck 时钟
//	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
//	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
//	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
//  
//	// 将 pll2_r_ck 设置为QSPI的内核时钟，按照上面的PLL2设置，此时 pll2_r_ck 为250MHz
// 	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;		
//  
 	// 将 rcc_hclk3 设置为QSPI的内核时钟，当750主频设置为480M时， rcc_hclk3 为240MHz 
	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;  // QSPI初始化的时候，设置为2分频即可得到120M驱动时钟

/********************************************************************************************************/   
	PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;  
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
	 Error_Handler();
	}  
	
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
/*************************************************************************************************************************************************************************FANKE*****/

