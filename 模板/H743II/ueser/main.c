#include "delay.h"
#include "usart.h"
#include "led.h"
#include "bsp_key.h"
#include "mpu.h" 
// D:\keil5\ARM\Pack\Keil\ARM_Compiler\1.4.0\Include
#include "EventRecorder.h"

uint8_t a;
int main(void)
{
    MPU_Memory_Protection();
	Cache_Enable();								 // 打开L1-Cache
	HAL_Init();									 // 初始化HAL库
	Stm32_Clock_Init(192, 5, 2, 4);				 // 设置时钟,480Mhz
	delay_init(480);							 // 延时初始化
	uart_init(115200);							 // 串口初始化
	LED_Init();									 // 初始化LED
    bsp_InitKey();//初始化bsp_key
    
	EventRecorderInitialize(EventRecordAll, 1U); // 调试启用
	EventRecorderStart();
    
	while (1)
	{a=bsp_GetKey();
        if(a!=0)
            printf("%d\r\n",a);
        delay_ms(1);
		
	}
}
