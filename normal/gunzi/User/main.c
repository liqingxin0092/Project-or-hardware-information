#include "stm32f10x.h"                  // Device header
#include "Motor.h"
#include "math.h"
#include "delay.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"
#include "mpu6050.h"
#include "inv_mpu.h"

uint8_t rcv_tar;               //树莓派发送的目标值
float cur_angle;               //当前角度
extern float angular_speed;    //当前角速度
extern float set;            //目标偏差

float roll;
short roll_speed;
float kd2;
short acc;

int main(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC->APB2ENR|=1;         
    AFIO->MAPR|= 0x2000000;	    //解除jtag
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Config(72000);//1ms
    
    Motor_Init();
    USART1_Config();       //蓝牙树莓派  tx
    USART2_Config();       //debug
//    USART3_Config();       //蓝牙树莓派  rx  
    Timer_Init();
    
    MPU_IIC_Init();   //初始化IIC总线
    mpu_dmp_init();
    mpu_set_int_level(1);
    mpu_set_int_latched(1);
    
    MPU_Write_Byte(MPU_INT_EN_REG,0X01);	//开数据准备好中断
//    float angle_temp=0;

    printf("\r\nreset\r\n\r\n");
    delay_ms(1000);
    
	while (1)
	{
//         if(uart3_receive)    //蓝牙树莓派
//         {
//             uart3_buffer[uart3_current_length-1]='\0';
//             sscanf((char*)uart3_buffer,"%d[%f]",(int*)&rcv_tar,&angle_temp);
//             if(angle_temp!=-135)
//                 cur_angle=angle_temp;
//             
//             UART3_TAKE();
//         }
         if(!(GPIOB->IDR&0x20))  //获取mpu6050数据
         { 
                 mpu_dmp_get_data(&roll,&roll_speed,&acc); 
                 printf("%f,%f,%d,%f,%d\r\n",roll,motor_pid_struct.tar_val,roll_speed,kd2,acc);
         }
         
         if(uart2_receive)     //debug  ,T
         {
             uart2_buffer[uart2_current_length-1]='\0';
             sscanf((char*)uart2_buffer,"kp=%f,ki=%f,kd=%f,kd2=%f,tar=%f,out_offset=%f",&motor_pid_struct.kp,&motor_pid_struct.ki,&motor_pid_struct.kd,&kd2,&motor_pid_struct.tar_val,&motor_pid_struct.out_offset);              
             UART2_TAKE();
         }

//         
         
         
	}
}
