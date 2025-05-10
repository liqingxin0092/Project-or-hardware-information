#include "board.h"
#include "string.h"
#include "OLED.h"
#include "ff.h"
#include "my_freertos.h"

static uint8_t b[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // ex_flash
uint16_t id;                                           // ex_flash
unsigned char buff[20];                                // ex_flash

uint16_t adc_value1;   //滤
uint16_t adc_value2;   //没滤

FATFS sd_f;
FIL file;
DIR File1Dir; // 目录信息结构体
UINT bw;
UINT br;
FRESULT f_res;

char f_read_buffer[30];

int main(void)
{
        NVIC_SetPriorityGrouping(3);
        board_init();
        bsp_led_init();
        usart_gpio_config(115200U);

#if USB_USART_DMA           // 使用DMA
        usart_dma_config(); // DMA配置
#endif

#if 1
        bsp_spi_init();          // ex_flash
        bsp_rtc_init();          // rtc
        ad_init(ADC1, 0, 1, 10); // my ad
        key_config();
        OLED_Init();
#endif

#if 0 /*rtc晶振测量*/ 
    if((RCU_AHB1EN&1)!=1) //开时钟
        RCU_AHB1EN|=1;  
    RCU_CFG0|=1<<21;//选择外部低速晶体振荡器,作为输出,不分频               
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_8); //PA8,CK out0   
    gpio_mode_set(GPIOA, GPIO_MODE_AF,GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA,GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_8);
#endif

#if 1 /*ex_flash test*/
        id = GD25Q32_readID();
        printf("ID = %X\r\n", id);  // usart tx测
        GD25Q32_read(buff, 20, 10); // ex_flash测

#if 0 /*ex_flahs write test*/
		GD25Q32_write(b, 20, 10);
        GD25Q32_read(buff, 20, 10);//ex_flash测
#endif
#endif

#if 1 /*tf & fatfs test*/
        f_res = f_mount(&sd_f, "1:", 1);
        f_res = f_open(&file, "1:liqingxins.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
        f_res = f_write(&file, "123456", strlen("123456"), &bw);
        f_res = f_lseek(&file, 0);
        f_res = f_read(&file, f_read_buffer, strlen("123456"), &br);
        f_res = f_close(&file);
        printf("%s\r\n", f_read_buffer);
#endif

#if 0 /*ad filter test*/
        while (1)
        {
                adc_value1 = ad_soft_filter(ADC1, 10, 7, 100, 500);
                adc_value2 = ad_start_once(ADC1, 10, 7);
                printf("%d,%d\r\n", adc_value1, adc_value2);
        }
#endif
        freertos_demo();
}
