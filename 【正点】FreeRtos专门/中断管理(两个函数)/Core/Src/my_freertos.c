#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
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
#define TASK1_PRIO 2            /* 任务优先级 */
#define TASK1_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler; /* 任务句柄 */
void task1(void *pvParameters); /* 任务函数 */


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
              
  vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL();            /* 退出临界区 */
}
void task1(void *pvParameters)
{
   uint8_t num=0;
  while (1)
  {
    if( ++num == 5)
    {
        portDISABLE_INTERRUPTS();
        printf("鍏抽棴涓柇\r\n");
        delay_ms(5000);
        //vTaskDelay(5000);
        portENABLE_INTERRUPTS();
        printf("寮�涓柇\r\n");
        num=0;
    }
    vTaskDelay(1000);
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
