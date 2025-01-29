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
#include "malloc.h"
#include "stmflash.h"
#include "btim.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "EventRecorder.h"

extern const unsigned char gImage_2[9800] ;
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
  SDRAM_Init(&__SDRAM_handle, 2000); // ��ʼ��SDRAM
  my_mem_init(SRAMIN);            //�ڴ�س�ʼ��
  my_mem_init(SRAMCCM);           //�ڲ�CCM��ʼ��
  MX_LTDC_Init(0);                  // ��ʼ��LTDC
  Touch_init();                     // ��ʼ������IC
  W25QXX_Init();                    // spi flash��ʼ��
  exfuns_init();                    // Ϊfatfs��ر��������ڴ�
  f_mount(fs[0], "0:", 1);          // ����SD��
  f_mount(fs[1], "1:", 1);          // ����NAND FLASH
  fonts_init();                     // �ֿ��ʼ��

  EventRecorderInitialize(EventRecordAll, 1U);//���Գ�ʼ��
  EventRecorderStart();

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

/**
 * @brief       ִ��: WFIָ��(ִ�����ָ�����͹���״̬, �ȴ��жϻ���)
 * @param       ��
 * @retval      ��
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

void sys_standby(void)
{
    RCC->APB1ENR |= 1 << 28;    /* ʹ�ܵ�Դʱ�� */
    PWR->CSR |= 1 << 8;         /* ����WKUP���ڻ��� */
    PWR->CR |= 1 << 2;          /* ���WKUP ��־ */
    PWR->CR |= 1 << 1;          /* PDDS = 1, ����������˯��ģʽ(PDDS) */
    SCB->SCR |= 1 << 2;         /* ʹ��SLEEPDEEPλ (SYS->CTRL) */
    sys_wfi_set();              /* ִ��WFIָ��, �������ģʽ */
}
/**
 * @brief       �ر������ж�(���ǲ�����fault��NMI�ж�)
 * @param       ��
 * @retval      ��
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       ���������ж�
 * @param       ��
 * @retval      ��
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}
/**
 * @brief       ����CPU˯��ģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_sleep(void)
{
    EXTI->PR = 0;   /* ���WKUP�ϵ��жϱ�־λ */
    sys_wfi_set();      /* ִ��WFIָ��, �������ģʽ */
}

/**
 * @brief       ����ֹͣģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_stop(void)
{
    RCC->APB1ENR |= 1 << 28;        /* ʹ�ܵ�Դʱ�� */
    EXTI->PR = 0;   /* ���WKUP�ϵ��жϱ�־λ */

    PWR->CR |= 1 << 0;              /* LPDS=1, ֹͣģʽ�µ�ѹ���������ڵ͹���ģʽ */
    PWR->CR &= ~(1 << 1);           /* PDDS=0, CPU���˯��ʱ����ֹͣģʽ */
    SCB->SCR |= 1 << 2;             /* ʹ��SLEEPDEEPλ */

    sys_wfi_set();                  /* ִ��WFIָ��, �������ģʽ */
    
    SCB->SCR &= ~(1 << 2);          /* �ر�SLEEPDEEPλ */
}

/**
 * @brief       �������ģʽ
 * @param       ��
 * @retval      ��
 */
void pwr_enter_standby(void)
{
    uint32_t tempreg;   /* ��ʱ�洢�Ĵ���ֵ�� */

    EXTI->PR = 0;   /* ���WKUP�ϵ��жϱ�־λ */
    
    /* STM32F4/F7/H7,��������RTC����жϺ�,�����ȹر�RTC�ж�,�����жϱ�־λ,Ȼ����������
     * RTC�ж�,�ٽ������ģʽ�ſ�����������,�����������.
     */
    PWR->CR |= 1 << 8;          /* ������дʹ�� */
    /* �ر�RTC�Ĵ���д���� */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    tempreg = RTC->CR & (0X0F << 12);   /* ��¼ԭ����RTC�ж����� */
    RTC->CR &= ~(0XF << 12);    /* �ر�RTC�����ж� */
    RTC->ISR &= ~(0X3F << 8);   /* �������RTC�жϱ�־. */
    RTC->CR |= tempreg;         /* ��������RTC�ж� */
    RTC->WPR = 0xFF;            /* ʹ��RTC�Ĵ���д���� */
    
    sys_standby();              /* �������ģʽ */
}


/**
 * @brief       ϵͳ��λ
 * @param       ��
 * @retval      ��
 */
void sys_soft_reset(void)
{
    SCB->AIRCR = 0X05FA0000 | (uint32_t)0x04;
}

/*���Ժ���*/
//uint32_t EventRecord2(uint32_t id,uint32_t val1,uint32_t val2 );
//uint32_t EventRecord4(uint32_t id,uint32_t val1,uint32_t val2,uint32_t val3,uint32_t val4);
//uint32_t EventRecordData(uint32_t id,const void *data,uint32_t len);
