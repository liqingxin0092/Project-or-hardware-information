#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "stdio.h"
#include "stm32f10x.h" 
#include "inv_mpu.h"
#include "oled.h"
#include "timers.h"
#include "max30102.h"
#include "queue.h"
#include "motor.h"
#include "pid_ctrl.h"
#include "sg90.h"
#include "buzzer.h"
#include "semphr.h"

uint32_t aun_ir_buffer[500]__attribute__((section("RW_IRAM1"))); 	 
int32_t n_sp02; //SPO2值
int8_t ch_spo2_valid;   //用于显示SP02计算是否有效的指示符
int32_t n_heart_rate;   //心率值
int8_t  ch_hr_valid;    //用于显示心率计算是否有效的指示符
u8 temp[6];
u8 dis_hr=0;
u8 last_hr=0;


volatile uint16_t speed;

typedef struct
{
    uint8_t thre_state:1;
    uint8_t set_sp:1;
    uint8_t set_hr:1;
    uint8_t set_sg:1;
}flag_t;
int8_t speed_thre=0; //3
int16_t hr_thre=85;    //2
int16_t sg_thre=0; 
flag_t set_flag;

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
void start_task(void *pvParameters); /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO 2            /* 任务优先级 */
#define TASK1_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler; /* 任务句柄 */
void task1(void *pvParameters); /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO 2            /* 任务优先级 */
#define TASK2_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
void task2(void *pvParameters); /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO 3            /* 任务优先级 */
#define TASK3_STK_SIZE 192      /* 任务堆栈大小 */
volatile TaskHandle_t Task3Task_Handler; /* 任务句柄 */
void task3(void *pvParameters); /* 任务函数 */

/*获得速度*/
TimerHandle_t timer1_handle;
void get_speed(TimerHandle_t xTimer);

/*按键扫描*/
TimerHandle_t timer2_handle;
void get_key(TimerHandle_t xTimer);

/*按键队列*/
QueueHandle_t key_buffer_handle;

volatile uint8_t timer_lock;

/*起始任务*/
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL(); // 进入临界区
  /* 创建任务1 */
  if(xTaskCreate((TaskFunction_t)task1,
              (const char *)"task1",
              (uint16_t)TASK1_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK1_PRIO,
              (TaskHandle_t *)&Task1Task_Handler)!=pdPASS)
              while(1);
  /* 创建任务2 */
  if(xTaskCreate((TaskFunction_t)task2,
              (const char *)"task2",
              (uint16_t)TASK2_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK2_PRIO,
              (TaskHandle_t *)&Task2Task_Handler)!=pdPASS)
              while(1);
  /* 创建任务3 */
  if(xTaskCreate((TaskFunction_t)task3,
              (const char *)"task3",
              (uint16_t)TASK3_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK3_PRIO,
              (TaskHandle_t *)&Task3Task_Handler)!=pdPASS)
              while(1);
 /*得到速度*/              
  timer1_handle=xTimerCreate( "get_encoder", 
                100,  //100ms
                pdTRUE,
                (void *)1,
                 get_speed);
  xTimerStart( timer1_handle, 0);   //立刻启动
                
 /*扫描按键*/              
  timer2_handle=xTimerCreate( "get_encoder", 
                20,  //20ms
                pdTRUE,
                (void *)2,
                 get_key);
  xTimerStart( timer2_handle, 0);   //立刻启动
                
  key_buffer_handle=xQueueCreate( 10, 1);  //按键队列
   
                
  vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL();            // 退出临界区
}

/*任务1*/
float pitch,roll,yaw;
void task1(void *pvParameters)
{
  while (1)
  {     
     if(!(GPIOB->IDR&0x10))  //获取mpu6050数据
     { 
          mpu_dmp_get_data(&pitch,&roll,&yaw); 
     }
     
     if(dis_hr>hr_thre)   //如果当前心率大于心率阈值
     {
         if((dis_hr!=0)&&(last_hr>hr_thre)&&(last_hr!=dis_hr))
         {
           motor_pid_struct.tar_val=speed_thre=0; //电机停止
           motor_pid_struct.err_sum=0;
           sg_thre=0;
           MOTOR_SET_CCR(0);
           sg90_set_angle(sg_thre);               //舵机复位
           BUZZER_ALARM(100);                     //警报
         }
         last_hr=dis_hr;
     }      
     else if(pitch>40) // > 40
     {
           motor_pid_struct.tar_val=speed_thre=0; //电机停止
           motor_pid_struct.err_sum=0;
           sg_thre=0;
           MOTOR_SET_CCR(0);
           sg90_set_angle(sg_thre);               //舵机复位
           BUZZER_ALARM(100);                     //警报
     }    
     else
     {
           MOTOR_SET_CCR((uint32_t)pid_control(&motor_pid_struct,speed)); 
     }       
        
     vTaskDelay(30);
  }
  
}

/*任务二具体实现*/
void task2(void *pvParameters)
{
    uint8_t key_val=0;
    OLED_ShowString(0, 0, "pit:",OLED_6X8);
    OLED_ShowString(60, 0,"ccr:",OLED_6X8);
    OLED_ShowString(0, 8, "hr:",OLED_6X8);
    OLED_ShowString(0, 16,"sp:",OLED_6X8);
    OLED_ShowString(0, 24,"key:",OLED_6X8);
    OLED_ShowString(0, 32,"hr_t:",OLED_6X8);
    OLED_ShowString(48, 32,"sg_t:",OLED_6X8);
    OLED_ShowString(0, 40,"sp_t:",OLED_6X8);
    OLED_ShowString(0, 48,"hr_s:",OLED_6X8);
    OLED_ShowString(36, 48,"sg_s:",OLED_6X8);
    OLED_ShowString(0, 56,"sp_s:",OLED_6X8);
    while (1)
    {
        if(xQueueReceive( key_buffer_handle,&key_val,0 )==errQUEUE_EMPTY)
           key_val=0;
        
        /*设置心率逻辑*/
        if(key_val==1&&set_flag.set_hr==0)
        {   
            set_flag.set_hr=1;//进入设置hr模式
        }
        else if(set_flag.set_hr) //设置hr模式
        {
            if(key_val==1)
            {     
                set_flag.set_hr=0;//退出设置hr模式
            } 
            else if(key_val==3)
            {
                hr_thre+=2;
                if(hr_thre>=100)
                    hr_thre=100;
            }
            else if(key_val==4)
            {
                hr_thre-=2;
                if(hr_thre<=65)
                    hr_thre=65;
            }
        }
        
        /*设置速度逻辑*/
        if((key_val==2)&&(set_flag.set_sp==0))
        {    
            set_flag.set_sp=1;//进入设置sp模式
        }
        else if(set_flag.set_sp) //设置sp模式
        {
            if(key_val==2)
            {     
                set_flag.set_sp=0;//退出设置sp模式
            } 
            else if(key_val==3)
            {
                speed_thre+=10;
                if(speed_thre>=90)
                    speed_thre=90;
            }
            else if(key_val==4)
            {
                speed_thre-=10;
                if(speed_thre<=0)
                    speed_thre=0;
            }
        }
        
        /*设置舵机逻辑*/
        if((key_val==5)&&(set_flag.set_sg==0))
        {    
            set_flag.set_sg=1;//进入设置sg模式
        }
        else if(set_flag.set_sg) //设置sg模式
        {
            if(key_val==5)
            {     
                set_flag.set_sg=0;//退出设置sg模式
            } 
            else if(key_val==3)
            {
                sg_thre+=5;
                if(sg_thre>=90)
                    sg_thre=90;
                
            }
            else if(key_val==4)
            {
                sg_thre-=5;
                if(sg_thre<=0)
                    sg_thre=0;
            }
        }
        
        sg90_set_angle(sg_thre);             //舵机设置
        motor_pid_struct.tar_val=speed_thre; //目标速度赋值
        
        OLED_ShowFloatNum(24, 0,  (double)pitch, 2, 2, OLED_6X8);
        OLED_ShowNum(84,  0, TIM2->CCR1, 2,OLED_6X8);
        OLED_ShowNum(18, 8, dis_hr, 3,OLED_6X8);
        OLED_ShowNum(18, 16, speed, 2,OLED_6X8);
        OLED_ShowNum(24, 24, key_val, 1,OLED_6X8);
        OLED_ShowNum(30, 32, hr_thre, 3,OLED_6X8);
        OLED_ShowNum(78, 32, sg_thre, 3,OLED_6X8);
        OLED_ShowNum(30, 40, speed_thre, 2,OLED_6X8);
        OLED_ShowNum(30, 48, set_flag.set_hr, 1,OLED_6X8);
        OLED_ShowNum(66, 48, set_flag.set_sg, 1,OLED_6X8);
        OLED_ShowNum(30, 56, set_flag.set_sp, 1,OLED_6X8);
        
        OLED_Update();
        vTaskDelay(100);
    }
}

/*任务3具体实现*/
void task3(void *pvParameters)
{    
    MAX30102_Init();
    uint16_t i=0;
	for(i=0;i<500;i++)
	{
		max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   	 // 将值合并得到实际数字
	}
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, 500, NULL, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 

	while(1)
	{
		//舍去前100组样本，并将后400组样本移到顶部，将100~500缓存数据移位到0~400
			for(i=100;i<500;i++)
			{
                aun_ir_buffer[i-100]=aun_ir_buffer[i];		//将100-500缓存数据移位到0-400
			}
			//在计算心率前取100组样本，取的数据放在400-500缓存数组中
			for(i=400;i<500;i++)
			{
                max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);		//读取传感器数据，赋值到temp中
                ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
                aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   	//将值合并得到实际数字，数组400-500为新读取数据
                if(ch_hr_valid == 1 && n_heart_rate<120)
                {
                    dis_hr = n_heart_rate;
                }
                else
                {
                    dis_hr = 0;
                }
            }
            maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, 500, NULL, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
	}
}

/*创建开始任务并调度*/
void freertos_demo(void)
{
  xTaskCreate(start_task,          /* 任务函数 */
              "start_task",        /* 任务名称 */
              START_STK_SIZE,      /* 任务堆栈大小 */
              NULL,                /* 传入给任务函数的参数 */
              START_TASK_PRIO,     /* 任务优先级 */
              &StartTask_Handler); /* 任务句柄 */
  vTaskStartScheduler();
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI->PR&0x20)     // pin5触发
    { 
        EXTI->PR|=1<<5;   //清标志位
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(Task3Task_Handler,&xHigherPriorityTaskWoken);  
        if(xHigherPriorityTaskWoken)
        {
             portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }            
}

void get_speed( TimerHandle_t xTimer )
{
//    GPIOC->ODR^=0x2000;
    speed=TIM3->CNT*5;  //   *10 /2    
    TIM3->CNT=0; 
}

void get_key( TimerHandle_t xTimer )
{
     uint8_t val=0;
     if(!(GPIOB->IDR&0x4000))
        val=1;
     else if(!(GPIOB->IDR&0x8000))
        val=2;
     else if(!(GPIOA->IDR&0x100))
        val=3;
     else if(!(GPIOA->IDR&0x200))
        val=4;
     else if(!(GPIOA->IDR&0x400))
        val=5;
     xQueueSend(key_buffer_handle,&val,0);
}


