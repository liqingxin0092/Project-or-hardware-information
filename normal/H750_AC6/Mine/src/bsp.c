#include "bsp.h"
#include "h7_core.h"

extern FATFS SD_FatFs;
                   
void bsp_init(void)
{
 
    cache_set();
    mpu_set(); // �ڴ�����
      
    detailed_kernel_Settings(); // ��ϸ�ں�����

#if USE_EVENTRECORD == 1
    EventRecorderInitialize(EventRecordAll, 1U); // ��ʼ��event recorder
    EventRecorderStart();
#endif

    set_interrupt_grouping(GROUPING_16p_1s); // �����жϷ���,Ĭ��û�������ȼ�
    Stm32_Clock_Init(192, 5, 2, 4);          // �Ĵ���ʱ�ӳ�ʼ��,reset�����лḴλʱ������
    
    RCC_GPIO_Init();            // ʹ��GPIOʱ��,����һЩIO,ע��һ��Ҫ�ڴ�IO���豸ǰʹ��.
    QSPI_W25Qxx_Init();
    QSPI_W25Qxx_MemoryMappedMode();
    
    MX_USART1_UART_Init();                   // ���ڳ�ʼ��

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
    LCD_Init(); // LCD��ʼ��
#endif

#if USE_USMART == 1
    usmart_init(240); // ��ʼ��USMART,�����Ƕ�ʱ��Ƶ��
#endif

#if USE_FATFS == 1
#if USE_FATTESTER == 1
    mf_init();
#endif
    f_mount(&SD_FatFs, "0:", 1); // ����SD��
#endif

#if UES_RAM_ECC==1
    ram_ecc_enable();
#endif
    //    test_led_init();
}