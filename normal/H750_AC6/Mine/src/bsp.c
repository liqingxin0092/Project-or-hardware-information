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
    mpu_set(); // �ڴ�����
      
    detailed_kernel_Settings(); // ��ϸ�ں�����

#if USE_EVENTRECORD == 1
    EventRecorderInitialize(EventRecordAll, 1U); // ��ʼ��event recorder
    EventRecorderStart();
#endif
    
#if CPU_CLOCK==600
    rcc_hse_pll1_init(5,240,2,0xff,0xff);   //ʱ�ӳ�ʼ��
    rcc_cpu_periph_clk_init(1,4,2,2,2,2,0);
#elif CPU_CLOCK==480
    rcc_hse_pll1_init(5,192,2,0xff,0xff);   //ʱ�ӳ�ʼ��
    rcc_cpu_periph_clk_init(1,2,2,2,2,2,0);
#endif
    
//    a= rcc_get_system_clk();    //����ǰʱ��Ƶ��
    set_interrupt_grouping(GROUPING_16p_1s); // �����жϷ���,Ĭ��û�������ȼ�
    
    RCC_GPIO_Init(1);            // ʹ��GPIOʱ��,����һЩIO,ע��һ��Ҫ�ڴ�IO���豸ǰʹ��.
    QSPI_W25Qxx_Init();
    QSPI_W25Qxx_MemoryMappedMode();
    
    timer_init();
    
    usart_init_app(); //���ڳ�ʼ��
    
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

}

/*��ת��DFU���ز���*/
void JumpToBootloader(void)
{
     uint32_t i=0;
     void (*SysMemBootJump)(void); /* ����һ������ָ�� */
     __IO uint32_t BootAddr = 0x1FF09800; /* STM32H7 ��ϵͳ BootLoader ��ַ */

     /* �ر�ȫ���ж� */
     DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();

     /* �رյδ�ʱ������λ��Ĭ��ֵ */
     SysTick->CTRL = 0;
     SysTick->LOAD = 0;
     SysTick->VAL = 0;

     /* ��������ʱ�ӵ�Ĭ��״̬��ʹ�� HSI ʱ�� */
     HAL_RCC_DeInit();

     /* �ر������жϣ���������жϹ����־ */
     for (i = 0; i < 8; i++)
     {
     NVIC->ICER[i]=0xFFFFFFFF;
     NVIC->ICPR[i]=0xFFFFFFFF;
     }

     /* ʹ��ȫ���ж� */
     ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
     
     /* ��ת��ϵͳ BootLoader���׵�ַ�� MSP����ַ+4 �Ǹ�λ�жϷ�������ַ */
     SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

     /* ��������ջָ�� */
     __set_MSP(*(uint32_t *)BootAddr);

     /* �� RTOS ���̣�����������Ҫ������Ϊ��Ȩ��ģʽ��ʹ�� MSP ָ�� */
     __set_CONTROL(0);

     /* ��ת��ϵͳ BootLoader */
     SysMemBootJump(); 
     /* ��ת�ɹ��Ļ�������ִ�е�����û�������������Ӵ��� */
     while (1);
}
