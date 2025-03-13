#include "bsp.h"
#include "h7_core.h"

extern FATFS SD_FatFs;
                   
void bsp_init(void)
{
 
    cache_set();
    mpu_set(); // 内存配置
      
    detailed_kernel_Settings(); // 详细内核设置

#if USE_EVENTRECORD == 1
    EventRecorderInitialize(EventRecordAll, 1U); // 初始化event recorder
    EventRecorderStart();
#endif

    set_interrupt_grouping(GROUPING_16p_1s); // 设置中断分组,默认没有子优先级
    Stm32_Clock_Init(192, 5, 2, 4);          // 寄存器时钟初始化,reset程序中会复位时钟配置
    
    RCC_GPIO_Init();            // 使能GPIO时钟,配置一些IO,注意一定要在带IO的设备前使用.
    QSPI_W25Qxx_Init();
    QSPI_W25Qxx_MemoryMappedMode();
    
    MX_USART1_UART_Init();                   // 串口初始化

#if USE_MODBUS_M == 1
    modbus_hardware_init();
#endif
    soft_timer_init();
    
    //    soft_timer_start_measure();

    //    b=soft_timer_get_time();

#if USE_BUFFER_KEY == 1
    buffer_key_init();
#endif

#if USE_LCD == 1
    LCD_Init(); // LCD初始化
#endif

#if USE_USMART == 1
    usmart_init(240); // 初始化USMART,这里是定时器频率
#endif

#if USE_FATFS == 1
#if USE_FATTESTER == 1
    mf_init();
#endif
    f_mount(&SD_FatFs, "0:", 1); // 挂载SD卡
#endif

#if UES_RAM_ECC==1
    ram_ecc_enable();
#endif
    //    test_led_init();
}