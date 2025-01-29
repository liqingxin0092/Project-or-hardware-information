#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"

/*这两个文件注意起名字,头文件不区分大小写*/

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
#define TASK2_PRIO 3            /* 任务优先级 */
#define TASK2_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
void task2(void *pvParameters); /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO 4            /* 任务优先级 */
#define TASK3_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task3Task_Handler; /* 任务句柄 */
void task3(void *pvParameters); /* 任务函数 */

void start_task(void *pvParameters)
{
  /* 进入临界区 */
  taskENTER_CRITICAL(); // 关闭中断调度任务
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
  taskEXIT_CRITICAL();            /* 退出临界区 */
}
void task1(void *pvParameters)
{

  while (1)
  {
    GPIOF->ODR ^= 1 << 10;
    printf("任务一正在执行\r\n");
    vTaskDelay(200);
  }
}
void task2(void *pvParameters)
{

  while (1)
  {
    GPIOF->ODR ^= 1 << 9;
    vTaskDelay(1000); /* 延时1000ticks */
    printf("任务二正在执行\r\n");
  }
}
void task3(void *pvParameters)
{

  while (1)
  {
       vTaskDelay(20);
       if(!(((GPIOB->IDR)>>8)&1))
       {

                vTaskSuspend( Task1Task_Handler );
           printf("在任务中:任务1挂起\r\n");
       }
       else if(!(((GPIOB->IDR)>>9)&1))
       {
            vTaskResume( Task1Task_Handler );
           printf("在任务中:任务1恢复\r\n");
       }
  }
}
void freertos_demo(void)
{
  xTaskCreate((TaskFunction_t)start_task,          /* 任务函数 */
              (const char *)"start_task",          /* 任务名称 */
              (uint16_t)START_STK_SIZE,            /* 任务堆栈大小 */
              (void *)NULL,                        /* 传入给任务函数的参数 */
              (UBaseType_t)START_TASK_PRIO,        /* 任务优先级 */
              (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */
  vTaskStartScheduler();
}
