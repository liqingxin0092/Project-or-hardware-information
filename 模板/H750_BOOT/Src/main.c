#include "main.h"
#include "led.h"  
#include "usart.h"
#include "qspi_w25q64.h"
#include <string.h>


typedef  void (*pFunction)(void);
pFunction JumpToApplication;//����һ������ָ��

void SystemClock_Config(void);	// ϵͳʱ�ӳ�ʼ��


int main(void)
{
	SCB_EnableICache();
	SCB_EnableDCache();

	HAL_Init();             // ��ʼ��HAL��
	SystemClock_Config();   // ����ϵͳʱ�ӣ���Ƶ480MH

	QSPI_W25Qxx_Init();	  				 // ��ʼ��W25Q64
	QSPI_W25Qxx_MemoryMappedMode(); 	 // ����QSPIΪ�ڴ�ӳ��ģʽ

	SCB_DisableICache();		// �ر�ICache
	SCB_DisableDCache();		// �ر�Dcache

	SysTick->CTRL = 0;		// �ر�SysTick
	SysTick->LOAD = 0;		// ��������ֵ
	SysTick->VAL = 0;			// �������ֵ

	JumpToApplication = (pFunction) (*(__IO uint32_t*) (W25Qxx_MemoryMapped_Addr + 4));	// ������ʼ��ַ
	__set_MSP(*(__IO uint32_t*) W25Qxx_MemoryMapped_Addr);	// ��������ջָ��
	JumpToApplication();			// ִ����ת

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
  
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_QSPI; // ѡ������ʱ��
  
/*************************************** QSPI�ں�ʱ������ **************************************************/
/*** FANKE ***
>>>>> ����˵����
	*
	*	1.	W25Q64JV ��������������Ƶ��Ϊ133MHz(�����Ǻ�׺JV���������ܴﵽ133M)
	*	2. 750��QSPI�������Ƶ��Ҳ��133MHz(�ο�STM32H750�����ֲ� �� 7.3.18С�ڣ� Quad-SPI interface characteristics)
	*	3. QSPI�ں�ʱ�ӿ����� rcc_hclk3 / pll1_q_ck / pll2_r_ck / per_ck ����һ����ע��������ں�ʱ�Ӳ���ʵ��ͨ�ŵ�����ʱ�ӣ�
	*	4. ���� per_ck ��ʱ��Դ������ hse_ck / hsi_ker_ck / csi_ker_ck ��������ʱ�Ӷ�ֻ�м�ʮ�ף��޷�����QSPI�ĸ���ͨ������
	*	5. rcc_hclk3 ��ϵͳʱ��2��Ƶ֮�����������ƵΪ480Mʱ��rcc_hclk3 = 240M
	*	6. pll1_q_ck �� PLLN��Ƶ֮��960M���ٷ�Ƶ����������������Ϊ480M
	*	7. ��ΪQSPI����ʱ��ֻ�����ں�ʱ�ӽ���������Ƶ�����Ե�ѡ�� rcc_hclk3 �� pll1_q_ck��Ϊ�ں�ʱ��ʱ���޷��õ�133M������ٶ�
	*	8. ʹ�� pll2_r_ck ��ΪQSPI�ں�ʱ��,�������õõ�133M,Ȼ��QSPI����Ƶ����1,���տ��Եõ�133M������ʱ��
	*	9. QSPI�ں�ʱ�����ֻ����250M��������Բ���Ӣ�İ��750�ο��ֲ� ��8.5.8С��  Kernel clock selection���汾��RM0433 Rev 7 ��February 2020 ��
	*
>>>>> ��Ҫ˵����
	*
	*	1.W25QXX�Ĳ���ʱ�����޶���!!! �ֲ�����ĵ��Ͳο�ֵΪ: 4K-45ms, 32K-150ms ,64K-150ms,��Ƭ����20S
	*
	*	2.W25QXX��д��ʱ�����޶���!!! �ֲ�����ĵ��Ͳο�ֵΪ: 256�ֽ�-0.4ms��Ҳ���� 1M�ֽ�/s ��ʵ������600K�ֽ�/s���ң�
	*
	*	3.���ʹ�ÿ⺯��ֱ�Ӷ�ȡ����ô�Ƿ�ʹ��DMA���Ƿ���Cache�����������Ż��ȼ��Լ����ݴ洢����λ��(�ڲ� TCM SRAM ���� AXI SRAM)����Ӱ���ȡ���ٶ�
	*
	*	4.���ʹ���ڴ�ӳ��ģʽ�����ȡ����ֻ��QSPI������ʱ���й�
	*
	*	5.ʹ�ÿ⺯������ֱ�Ӷ�ȡ��keil�汾5.30��������AC6.14������ȼ�Oz image size����ȡ�ٶ�Ϊ 7M�ֽ�/S �����ݷ���TCM SRAM ���� AXI SRAM  
	*    ���ǲ��Ľ������ΪCPUֱ�ӷ�������Ĵ�����Ч�ʺܵͣ�ֱ��ʹ��HAL����ж�ȡ�Ļ����ٶȺ���
	*
	*	6.���ʹ��MDMA���ж�ȡ�����Դﵽ 58M�ֽ�/S��ʹ���ڴ�ӳ��ģʽ�Ļ����������Դﵽ����ʱ�ӵ�ȫ�٣�62.14M/s
	*
	*  7.W25Q64JV ��������������Ƶ��Ϊ133MHz��750��QSPI�������Ƶ��Ҳ��133MHz �����Ƕ���HAL�⺯��ֱ�Ӷ�ȡ���ԣ�����ʱ�ӳ���15M�Ѿ����������������
	*
	*	8.�����ڴ�ӳ��ģʽֱ�Ӷ�ȡ���ԣ�����ʱ�ӳ���127.5M�Ѿ��������������������ΪQSPI�ں�ʱ������޶�Ϊ250M�����Խ���ʵ��QSPI����ʱ�Ӳ�Ҫ����125M��
	*	  �����ʱ��������ο� SystemClock_Config ����
	*
	*	9.ʵ��ʹ���У������ݱȽϴ�ʱ������ʹ��64K����32K����������ʱ���4K������	
	*
*** FANKE ***/
  
  
//  // ���´�����cubeMX����
//	PeriphClkInitStruct.PLL2.PLL2M = 25;   // �����Ƶϵ��25(ע��FK750M1-VBT6�ľ���Ϊ25M)
//	PeriphClkInitStruct.PLL2.PLL2N = 500;	// ������Ԥ��Ƶ��ľ���ʱ�ӽ���500����Ƶ���õ�500Mʱ��
//	PeriphClkInitStruct.PLL2.PLL2P = 2;		//	���ʱ���޹�QSPI���û����������ú�ʹ��
//	PeriphClkInitStruct.PLL2.PLL2Q = 2;		// ���ʱ���޹�QSPI���û����������ú�ʹ��
//	PeriphClkInitStruct.PLL2.PLL2R = 2;		// ����2��Ƶ���õ� 250M �� pll2_r_ck ʱ��
//	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
//	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
//	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
//  
//	// �� pll2_r_ck ����ΪQSPI���ں�ʱ�ӣ����������PLL2���ã���ʱ pll2_r_ck Ϊ250MHz
// 	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;		
//  
 	// �� rcc_hclk3 ����ΪQSPI���ں�ʱ�ӣ���750��Ƶ����Ϊ480Mʱ�� rcc_hclk3 Ϊ240MHz 
	PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;  // QSPI��ʼ����ʱ������Ϊ2��Ƶ���ɵõ�120M����ʱ��

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

