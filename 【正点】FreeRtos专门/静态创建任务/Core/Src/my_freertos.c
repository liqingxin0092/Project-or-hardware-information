#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"

/*这两个文件注意起名字,头文件不区分大小写*/

/*静态创建内存分配*/ /*空闲任务配置*/
StaticTask_t idle_task_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                        StackType_t ** ppxIdleTaskStackBuffer,
                                        uint32_t * pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer=&idle_task_tcb;
    *ppxIdleTaskStackBuffer=idle_task_stack;
    *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}
/* 软件定时器任务配置*/
StaticTask_t timer_task_tcb;
StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, 
                                     StackType_t ** ppxTimerTaskStackBuffer, 
                                     uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer=&timer_task_tcb;
    *ppxTimerTaskStackBuffer=timer_task_stack;
    *pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}
/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
StackType_t Start_task_stack[START_STK_SIZE]; /* 任务堆栈 */
StaticTask_t start_task_tcb;  /* 任务控制块 */
void start_task(void *pvParameters); /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO 2            /* 任务优先级 */
#define TASK1_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler; /* 任务句柄 */
StackType_t Task1_stack[TASK1_STK_SIZE]; /* 任务堆栈 */
StaticTask_t Task1_tcb;  /* 任务控制块 */
void task1(void *pvParameters); /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO 3            /* 任务优先级 */
#define TASK2_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
StackType_t Task2_stack[TASK2_STK_SIZE]; /* 任务堆栈 */
StaticTask_t Task2_tcb;  /* 任务控制块 */
void task2(void *pvParameters); /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO 4            /* 任务优先级 */
#define TASK3_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task3Task_Handler; /* 任务句柄 */
StackType_t Task3_stack[TASK3_STK_SIZE]; /* 任务堆栈 */
StaticTask_t Task3_tcb;  /* 任务控制块 */
void task3(void *pvParameters); /* 任务函数 */

void start_task(void *pvParameters)
{
  /* 进入临界区 */
  taskENTER_CRITICAL(); // 关闭中断调度任务
 
   Task1Task_Handler=xTaskCreateStatic(  task1,
                                         "task1",
                                         TASK1_STK_SIZE,
                                         NULL,
                                         TASK1_PRIO,
                                         Task1_stack,
                                         &Task1_tcb );
   Task2Task_Handler=xTaskCreateStatic(  task2,
                                         "task2",
                                         TASK2_STK_SIZE,
                                         NULL,
                                         TASK2_PRIO,
                                         Task2_stack,
                                         &Task2_tcb );
   Task3Task_Handler=xTaskCreateStatic(  task3,
                                         "task3",
                                         TASK3_STK_SIZE,
                                         NULL,
                                         TASK3_PRIO,
                                         Task3_stack,
                                         &Task3_tcb );
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
            if (Task1Task_Handler != NULL)
            {
                vTaskDelete(Task1Task_Handler);
                Task1Task_Handler = NULL;
                printf("任务一删除\r\n");
            }
       }
       else if(!(((GPIOB->IDR)>>9)&1))
       {
            if (Task2Task_Handler != NULL)
            {
                vTaskDelete(Task2Task_Handler);
                Task2Task_Handler = NULL;
                printf("任务二删除\r\n");
            }
       }
  }
}
void freertos_demo(void)
{
   StartTask_Handler=xTaskCreateStatic(  start_task,
                                         "start_task",
                                         START_STK_SIZE,
                                         NULL,
                                         START_TASK_PRIO,
                                         Start_task_stack,
                                         &start_task_tcb );
  vTaskStartScheduler();
}
