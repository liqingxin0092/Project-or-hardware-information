#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "delay.h"
#include "LTDC.h"
#include "malloc.h"

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
#define TASK2_PRIO 2            /* 任务优先级 */
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

/*任务1具体实现*/
void task1(void *pvParameters)
{
  while (1)
  {
    LTDC_turn_area_color(450, 450, 500, 500, 0xF81F, RED);
    vTaskDelay(500);
  }
}
char task_buffer[300];
/*任务二:各种API函数的使用*/
void task2(void *pvParameters)
{
  UBaseType_t priority = 0;
  UBaseType_t task_num = 0;
  UBaseType_t task_num2 = 0;
  TaskStatus_t *status_array = 0;
  TaskStatus_t *single_task_status = 0;
  TaskHandle_t task_hander = 0;
  UBaseType_t task_stack_min = 0;
  eTaskState task_state = 0;
  
  // char p[30] = {0};

  // /*设置任务优先级*/
  // vTaskPrioritySet(Task2Task_Handler, 30);

  // /*读取任务优先级*/
  // priority = uxTaskPriorityGet(NULL);

  // /*得到任务数量*/
  // task_num = uxTaskGetNumberOfTasks();

  // /*申请内存并获取全部任务状态*/
  // status_array = mymalloc(SRAMIN, sizeof(TaskStatus_t) * task_num);
  // task_num2 = uxTaskGetSystemState(status_array, task_num, NULL);

  // /*申请内存并获取单个任务状态*/
  // single_task_status = mymalloc(SRAMIN, sizeof(TaskStatus_t));
  // vTaskGetInfo(Task1Task_Handler, single_task_status, pdTRUE, eInvalid);

  // /*信息显示*/
  // sprintf(p, "task2 priority:%ld", priority);
  // LTDC_ShowString(0, 0, 1024, 24, 24, (uint8_t *)p, YELLOW, 0);
  // sprintf(p, "task num:%ld", task_num);
  // LTDC_ShowString(0, 24, 1024, 24, 24, (uint8_t *)p, PURPLE, 0);
  // sprintf(p, "task num2:%ld", task_num2);
  // LTDC_ShowString(0, 48, 1024, 24, 24, (uint8_t *)p, PURPLE, 0);

  // printf("任务名\t\t任务优先级\t任务编号\r\n");
  // for (uint8_t i = 0; i < task_num2; i++)
  // {
  //   printf("%s\t\t%ld\t%ld\r\n", status_array[i].pcTaskName, status_array[i].uxCurrentPriority, status_array[i].xTaskNumber);
  // }
  // printf("任务名字:%s,任务优先级:%ld,任务编号:%ld,任务状态:%d", single_task_status->pcTaskName,
  //        single_task_status->uxCurrentPriority,
  //        single_task_status->xTaskNumber,
  //        single_task_status->eCurrentState);

  // /*通过任务名获取句柄*/
  // task_hander = xTaskGetHandle("task1");
  // printf("任务1地址:%#x\r\n", (uint32_t)task_hander);
  // printf("任务1句柄地址:%#x\r\n", (uint32_t)Task1Task_Handler);

  // task_stack_min=uxTaskGetStackHighWaterMark(Task2Task_Handler);
  // printf("剩余堆栈:%ld\r\n",task_stack_min);

//  task_state = eTaskGetState(Task1Task_Handler);
//  printf("%d", task_state);
  vTaskList(task_buffer);
  printf("%s\r\n",task_buffer);
  while (1)
  {
    vTaskDelay(500);
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
