#include "stm32f10x.h"                  // Device header
#include "timer.h"
#include "math.h"
#include "delay.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "driver_wireless.h"

#define WIRELESS_SEND_TIME 	3000	//上报数据时间间隔（单位：毫秒）
#define SENSOR_READ_TIME	3000    //获取传感器数据时间间隔（单位：毫秒）
#define KEY_SCAN_TIME		20    	//按键扫描（单位：毫秒）

uint8_t wireless_send_flag = 0;     //上报数据标志位
uint8_t sensor_read_flag = 0;		//读取传感器数据标志位

int main(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC->APB2ENR|=1;         
    AFIO->MAPR|= 0x2000000;	    //解除jtag
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Config(72000);//1ms
      
    USART1_Config(115200); 
    wireless_init();
    timer2_init(100, 720);          //定时器二定时中断初始化，用来定时执行任务
    
	while(1)
	{
		
		if(wireless_send_flag == 1) 		//数据上报标志位
		{
			wireless_send_flag = 0;
			wireless_system_handler();		//执行无线模块相关事件
			wireless_onenet_data_handler();	//处理有关onenet的数据
			if((WirelessStatus.error_code & ERROR_MQTT_CONNECT) == 0)  wireless_publish_data(); 	//发布数据测试	
		}		
		if(wireless_get_receive_flag() == W_OK)		//无线模块接收到数据
		{
			wireless_receive_data_handler();		//接收数据处理函数
		}		
	}
}


/**  
  * @简要  串口2接收中断服务函数
  * @参数  	无
  * @注意	将无线模块接收函数放在此中断的接收中
  * @返回值 无  
  */
void USART2_IRQHandler(void)
{
	uint8_t data = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)		
	{
		data = USART_ReceiveData(USART2);	
		//Serial_SendByte(data);
		wireless_receive_callback(data);	/* 将无线模块接收数据函数放在串口接收中断中，接收数据 */		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);																	
	}
}

/**  
  * @简要  定时器2中断服务函数
  * @参数  	1ms
  * @注意	定时设置标志位 ,
  * @返回值 无  
  */
void TIM2_IRQHandler(void)
{
	static uint16_t wireless_send_timer = 0;
	static uint16_t sensor_read_timer = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		if(wireless_send_timer++ == WIRELESS_SEND_TIME) {wireless_send_timer = 0;wireless_send_flag = 1;}
		if(sensor_read_timer++ == SENSOR_READ_TIME) {sensor_read_timer = 0;sensor_read_flag = 1;}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
