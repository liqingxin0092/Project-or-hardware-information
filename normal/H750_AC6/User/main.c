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
#include "malloc.h"
#include "piclib.h"
#include "string.h"
#include "soft_timer.h"

FATFS SD_FatFs;
extern const unsigned char gImage_weixin[115200];

/*���õĹ���ע��ע��*/
int main()
{
    //  SCB->VTOR = 0x90000000; // �����ж��������ַ,���ӳ����ص�0x08...
    MPU_Memory_Protection(); // boot��,�ڴ汣��
    Cache_Enable();          // boot��,ʹ��cache

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); // boot��,�жϷ���,ԭhal_init()
    Stm32_Clock_Init(192, 5, 2, 4);                 // �Ĵ���ʱ�ӳ�ʼ��,reset�����лḴλʱ������
    MX_USART1_UART_Init();                          // ���ڳ�ʼ��
    QSPI_W25Qxx_Init();
    QSPI_W25Qxx_MemoryMappedMode();
    RCC_GPIO_Init();                             // ʹ��GPIOʱ��,����һЩIO
    delay_init(480);                             // ��ʼ����ʱ
    usmart_init(240);                            // ��ʼ��USMART,�����Ƕ�ʱ��Ƶ��
    LCD_Init();                                  // LCD��ʼ��
    EventRecorderInitialize(EventRecordAll, 1U); // ��ʼ��event recorder
    EventRecorderStart();

    exfuns_init();               // Ϊfatfs��ر��������ڴ�
    f_mount(&SD_FatFs, "0:", 1); // ����SD��

     picture_scan_and_display(0, 0, 240, 240, 2000);

    //    freertos_demo();
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
