#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "delay.h"
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
#define TASK1_PRIO 3            /* 任务优先级 */
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

  vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL();            // 退出临界区
}

void task1(void *pvParameters)
{
  uint32_t num = 0;
  while (1)
  {
    taskENTER_CRITICAL(); // 进入临界区
    printf("任务1运行次数:%d\r\n", ++num);
    delay_ms(10);
    taskEXIT_CRITICAL(); // 退出临界区
  }
}

void task2(void *pvParameters)
{
  uint32_t num = 0;
  while (1)
  {
    taskENTER_CRITICAL(); // 进入临界区
    printf("任务2运行次数:%d\r\n", ++num);
    delay_ms(10);
    taskEXIT_CRITICAL(); // 退出临界区
  }
}

/*创建起始任务并开始调度*/
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
