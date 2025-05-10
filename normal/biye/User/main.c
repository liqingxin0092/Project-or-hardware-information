#include "stm32f10x.h"                  // Device header
#include "key.h"
#include "oled.h"
#include "buzzer.h"
#include "sg90.h"
#include "Delay.h"
#include "motor.h"
#include "max30102.h"
#include "my_freertos.h"
#include "mpu6050.h"
#include "inv_mpu.h"

int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
    RCC->APB2ENR|=1;      //AFIO时钟  
    AFIO->MAPR|= 0x2000000;  //解除jtag
    SysTick_Config(72000);//1ms
    
	key_init();       //按键初始化
    OLED_Init();      //oled初始化
    sg90_init();    //舵机初始化
    buzzer_init();  //蜂鸣器初始化
    motor_init();   //电机初始化
    encoder_init();   //红外对射
    
    MPU_IIC_Init();   //初始化IIC总线
    mpu_dmp_init();
    mpu_set_int_level(1);
    mpu_set_int_latched(1);

    freertos_demo();

}



