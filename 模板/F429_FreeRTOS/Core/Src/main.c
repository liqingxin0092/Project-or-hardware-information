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
  HAL_Init();                       // HAL库初始化
  SystemClock_Config();             // 时钟配置
  MX_USART1_UART_Init();            // 串口初始化
  MX_GPIO_Init();                   // SDRAM GPIO时钟配置,LCD背光IO配置
  delay_init(180);                  // 初始化延时
  usmart_dev.init(90);              // 初始化USMART,这里是定时器频率
  MX_FMC_Init();                    // SDRAM和NAND FLASH初始化
  SDRAM_Init(&__SDRAM_handle, 2000); // 初始化SDRAM
  my_mem_init(SRAMIN);            //内存池初始化
  my_mem_init(SRAMCCM);           //内部CCM初始化
  MX_LTDC_Init(0);                  // 初始化LTDC
  Touch_init();                     // 初始化触摸IC
  W25QXX_Init();                    // spi flash初始化
  exfuns_init();                    // 为fatfs相关变量申请内存
  f_mount(fs[0], "0:", 1);          // 挂载SD卡
  f_mount(fs[1], "1:", 1);          // 挂载NAND FLASH
  fonts_init();                     // 字库初始化

  EventRecorderInitialize(EventRecordAll, 1U);//调试初始化
  EventRecorderStart();

  freertos_start();                 // FreeRTOS开始调度
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
 * @brief       执行: WFI指令(执行完该指令进入低功耗状态, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

void sys_standby(void)
{
    RCC->APB1ENR |= 1 << 28;    /* 使能电源时钟 */
    PWR->CSR |= 1 << 8;         /* 设置WKUP用于唤醒 */
    PWR->CR |= 1 << 2;          /* 清除WKUP 标志 */
    PWR->CR |= 1 << 1;          /* PDDS = 1, 允许进入深度睡眠模式(PDDS) */
    SCB->SCR |= 1 << 2;         /* 使能SLEEPDEEP位 (SYS->CTRL) */
    sys_wfi_set();              /* 执行WFI指令, 进入待机模式 */
}
/**
 * @brief       关闭所有中断(但是不包括fault和NMI中断)
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       开启所有中断
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}
/**
 * @brief       进入CPU睡眠模式
 * @param       无
 * @retval      无
 */
void pwr_enter_sleep(void)
{
    EXTI->PR = 0;   /* 清除WKUP上的中断标志位 */
    sys_wfi_set();      /* 执行WFI指令, 进入待机模式 */
}

/**
 * @brief       进入停止模式
 * @param       无
 * @retval      无
 */
void pwr_enter_stop(void)
{
    RCC->APB1ENR |= 1 << 28;        /* 使能电源时钟 */
    EXTI->PR = 0;   /* 清除WKUP上的中断标志位 */

    PWR->CR |= 1 << 0;              /* LPDS=1, 停止模式下电压调节器处于低功耗模式 */
    PWR->CR &= ~(1 << 1);           /* PDDS=0, CPU深度睡眠时进入停止模式 */
    SCB->SCR |= 1 << 2;             /* 使能SLEEPDEEP位 */

    sys_wfi_set();                  /* 执行WFI指令, 进入待机模式 */
    
    SCB->SCR &= ~(1 << 2);          /* 关闭SLEEPDEEP位 */
}

/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void pwr_enter_standby(void)
{
    uint32_t tempreg;   /* 零时存储寄存器值用 */

    EXTI->PR = 0;   /* 清除WKUP上的中断标志位 */
    
    /* STM32F4/F7/H7,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置
     * RTC中断,再进入待机模式才可以正常唤醒,否则会有问题.
     */
    PWR->CR |= 1 << 8;          /* 后备区域写使能 */
    /* 关闭RTC寄存器写保护 */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    tempreg = RTC->CR & (0X0F << 12);   /* 记录原来的RTC中断设置 */
    RTC->CR &= ~(0XF << 12);    /* 关闭RTC所有中断 */
    RTC->ISR &= ~(0X3F << 8);   /* 清除所有RTC中断标志. */
    RTC->CR |= tempreg;         /* 重新设置RTC中断 */
    RTC->WPR = 0xFF;            /* 使能RTC寄存器写保护 */
    
    sys_standby();              /* 进入待机模式 */
}


/**
 * @brief       系统软复位
 * @param       无
 * @retval      无
 */
void sys_soft_reset(void)
{
    SCB->AIRCR = 0X05FA0000 | (uint32_t)0x04;
}

/*调试函数*/
//uint32_t EventRecord2(uint32_t id,uint32_t val1,uint32_t val2 );
//uint32_t EventRecord4(uint32_t id,uint32_t val1,uint32_t val2,uint32_t val3,uint32_t val4);
//uint32_t EventRecordData(uint32_t id,const void *data,uint32_t len);
