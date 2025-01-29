#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "GBK_LibDrive.h"
#include "key.h"
#include "queue.h"
#include "touch.h"
/*这两个文件注意起名字,头文件不区分大小写*/

char buff[100]="我是一个大数组,大大的数组,1212131415,AAScss";
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
#define TASK3_PRIO 2            /* 任务优先级 */
#define TASK3_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task3Task_Handler; /* 任务句柄 */
void task3(void *pvParameters); /* 任务函数 */

/*队列定义*/
QueueHandle_t key_queue;        //小数据
QueueHandle_t big_data_queue;   //大数据

/*起始任务*/
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL(); // 进入临界区
  /* 创建任务1 */
  xTaskCreate((TaskFunction_t)task1,
              (const char *)"task1",
              (uint16_t)TASK1_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK1_PRIO,
              (TaskHandle_t *)&Task1Task_Handler);
  /* 创建任务2 */
  xTaskCreate((TaskFunction_t)task2,
              (const char *)"task2",
              (uint16_t)TASK2_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK2_PRIO,
              (TaskHandle_t *)&Task2Task_Handler);
  /* 创建任务3 */
  xTaskCreate((TaskFunction_t)task3,
              (const char *)"task3",
              (uint16_t)TASK3_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK3_PRIO,
              (TaskHandle_t *)&Task3Task_Handler);
  vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL();            // 退出临界区
}

/*任务1,实现入队*/
void task1(void *pvParameters)
{
     uint8_t key_num=0;
    char * buff_ptr=buff;//这里必须在任务里拿个形参接收以下.
  while (1)
  {
      key_num=KEY_Scan(0);
      if (key_num==1||key_num==2)//键值入队.
      {
          if(xQueueSend(key_queue,&key_num,portMAX_DELAY)!=pdTRUE)
              printf("发送队列key_queue失败!\r\n");
      }
      else if (key_num==4)
      {
          if(xQueueSend(big_data_queue,&buff_ptr,portMAX_DELAY)!=pdTRUE)
              printf("发送队列big_data_queue失败!\r\n");
      }  
      vTaskDelay(50);
  }
}

/*任务二,小数据出队*/
void task2(void *pvParameters)
{  
   uint8_t key=0;
  while (1)
  {
      if(xQueueReceive(key_queue,&key,portMAX_DELAY)!=pdTRUE)
          printf("key_queue读取失败!!!\r\n");
      else
          printf("key_queue读取成功!!!值为:%d\r\n",key);
      vTaskDelay(50);
  }
}

/*任务3,大数据出队*/
void task3(void *pvParameters)
{
    char * buff_ptr;
  while (1)
  {
      if(xQueueReceive(big_data_queue,&buff_ptr,portMAX_DELAY)!=pdTRUE)
          printf("big_data_queue读取失败!!!\r\n");
      else
          printf("big_data_queue读取成功!!!值为:%s\r\n",buff_ptr);
      vTaskDelay(50);
  }
}

/*创建开始任务并调度*/
void freertos_demo(void)
{
    /*创建队列*/
    key_queue=xQueueCreate(2,sizeof(uint8_t)); //队列长度,每个元素大小
    if(key_queue!=NULL)
        printf("key_queue队列创建成功!!\r\n");
    else 
        printf("key_queue队列创建失败!!\r\n");
    
    big_data_queue=xQueueCreate(1,sizeof(char*)); //队列长度,每个元素大小
    if(big_data_queue!=NULL)
        printf("big_data_queue队列创建成功!!\r\n");
    else 
        printf("big_data_queue队列创建失败!!\r\n");
    
    /*创建起始任务*/
  xTaskCreate(start_task,          /* 任务函数 */
              "start_task",        /* 任务名称 */
              START_STK_SIZE,      /* 任务堆栈大小 */
              NULL,                /* 传入给任务函数的参数 */
              START_TASK_PRIO,     /* 任务优先级 */
              &StartTask_Handler); /* 任务句柄 */
  vTaskStartScheduler();
}
