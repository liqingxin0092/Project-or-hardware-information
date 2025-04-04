#include "bsp.h"
#include "h7_core.h"
#include "data_structure_m.h"

extern FATFS SD_FatFs;
uint8_t a[16];
__attribute__((section(".ARM.__at_0x38000000"))) uint8_t bufferr[20]={0,1,2,3,4,5,6,7,8,9};
__attribute__((section(".ARM.__at_0x38000030"))) uint8_t bu[20]={0,1,2,3,4,5,6,7,8,9};

void bsp_init(void)
{
    cache_set();
    mpu_set(); // 内存配置
      
    detailed_kernel_Settings(); // 详细内核设置

#if USE_EVENTRECORD == 1
    EventRecorderInitialize(EventRecordAll, 1U); // 初始化event recorder
    EventRecorderStart();
#endif
    
#if CPU_CLOCK==600
    rcc_hse_pll1_init(5,240,2,0xff,0xff);   //时钟初始化
    rcc_cpu_periph_clk_init(1,4,2,2,2,2,0);
#elif CPU_CLOCK==480
    rcc_hse_pll1_init(5,192,2,0xff,0xff);   //时钟初始化
    rcc_cpu_periph_clk_init(1,2,2,2,2,2,0);
#endif
    
//    a= rcc_get_system_clk();    //看当前时钟频率
    set_interrupt_grouping(GROUPING_16p_1s); // 设置中断分组,默认没有子优先级
    
    RCC_GPIO_Init(1);            // 使能GPIO时钟,配置一些IO,注意一定要在带IO的设备前使用.
    QSPI_W25Qxx_Init();
    QSPI_W25Qxx_MemoryMappedMode();
    
    timer_init();
    
    usart_init_app(); //串口初始化
    
    bdam_trans(BDMA_Channel2,(uint32_t)bufferr,(uint32_t)&bufferr[10],5,0x1D7);
    bdam_trans(BDMA_Channel2,(uint32_t)bu,(uint32_t)&bu[10],5,0x1D7);
    
    
#if USE_MODBUS_M == 1
    modbus_hardware_init();
#endif
    
    
//       soft_timer_start_measure();
//       delay_us(666);
//       b=soft_timer_get_time();

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

}

/*跳转到DFU下载部分*/
void JumpToBootloader(void)
{
     uint32_t i=0;
     void (*SysMemBootJump)(void); /* 声明一个函数指针 */
     __IO uint32_t BootAddr = 0x1FF09800; /* STM32H7 的系统 BootLoader 地址 */

     /* 关闭全局中断 */
     DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();

     /* 关闭滴答定时器，复位到默认值 */
     SysTick->CTRL = 0;
     SysTick->LOAD = 0;
     SysTick->VAL = 0;

     /* 设置所有时钟到默认状态，使用 HSI 时钟 */
     HAL_RCC_DeInit();

     /* 关闭所有中断，清除所有中断挂起标志 */
     for (i = 0; i < 8; i++)
     {
     NVIC->ICER[i]=0xFFFFFFFF;
     NVIC->ICPR[i]=0xFFFFFFFF;
     }

     /* 使能全局中断 */
     ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
     
     /* 跳转到系统 BootLoader，首地址是 MSP，地址+4 是复位中断服务程序地址 */
     SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

     /* 设置主堆栈指针 */
     __set_MSP(*(uint32_t *)BootAddr);

     /* 在 RTOS 工程，这条语句很重要，设置为特权级模式，使用 MSP 指针 */
     __set_CONTROL(0);

     /* 跳转到系统 BootLoader */
     SysMemBootJump(); 
     /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
     while (1);
}
