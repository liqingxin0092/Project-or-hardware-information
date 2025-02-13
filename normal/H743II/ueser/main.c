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
	Cache_Enable();								 // ��L1-Cache
	HAL_Init();									 // ��ʼ��HAL��
	Stm32_Clock_Init(192, 5, 2, 4);				 // ����ʱ��,480Mhz
	delay_init(480);							 // ��ʱ��ʼ��
	uart_init(115200);							 // ���ڳ�ʼ��
	LED_Init();									 // ��ʼ��LED
    bsp_InitKey();//��ʼ��bsp_key
    
	EventRecorderInitialize(EventRecordAll, 1U); // ��������
	EventRecorderStart();
    
	while (1)
	{a=bsp_GetKey();
        if(a!=0)
            printf("%d\r\n",a);
        delay_ms(1);
		
	}
}
