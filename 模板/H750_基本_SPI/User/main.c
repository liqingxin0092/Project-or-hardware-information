#include "sdmmc_sdcard.h"
#include "usmart.h"
#include "delay.h"
#include "my_freertos.h"
#include "qspi_w25q64.h"
#include "lcd.h"
#include "sys.h"
#include "mpu.h"
#include "exfuns.h"
#include "EventRecorder.h"

FATFS 	SD_FatFs;

/*不用的功能注意注释*/
int main()
{
//  SCB->VTOR = 0x90000000; // 设置中断向量表地址,不加程序会回到0x08... 
    
  MPU_Memory_Protection(); //boot无,内存保护
  Cache_Enable();          //boot无,使能cache
    
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);//boot有,中断分组,原hal_init()
  Stm32_Clock_Init(192, 5, 2, 4); //寄存器时钟初始化,reset程序中会复位时钟配置
    
  QSPI_W25Qxx_Init();
  QSPI_W25Qxx_MemoryMappedMode();
     
  RCC_GPIO_Init();       // 使能GPIO时钟,配置一些IO
  delay_init(480);       // 初始化延时
  MX_USART1_UART_Init(); // 串口初始化
  usmart_init(240);      //初始化USMART,这里是定时器频率
  LCD_Init();            // LCD初始化
  LCD_FillRect(25, 25, 50,50);
  EventRecorderInitialize(EventRecordAll, 1U);//初始化event recorder
  EventRecorderStart();
//  exfuns_init();                      // 为fatfs相关变量申请内存
  f_mount(&SD_FatFs, "0:", 1);          // 挂载SD卡
    
  freertos_demo();
}

void RCC_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
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
