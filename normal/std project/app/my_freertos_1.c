#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "stdio.h"
#include "gd32f4xx.h"
#include "board.h"
#include "oled.h"
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
#define TASK2_PRIO 1            /* 任务优先级 */
#define TASK2_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
void task2(void *pvParameters); /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO 1            /* 任务优先级 */
#define TASK3_STK_SIZE 128      /* 任务堆栈大小 */
TaskHandle_t Task3Task_Handler; /* 任务句柄 */
void task3(void *pvParameters); /* 任务函数 */

/*起始任务*/
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL(); // 进入临界区
  xTaskCreate(task1, "task1", TASK1_STK_SIZE, NULL, TASK1_PRIO, &Task1Task_Handler);
  xTaskCreate(task2, "task2", TASK2_STK_SIZE, NULL, TASK2_PRIO, &Task2Task_Handler);
  xTaskCreate(task3, "task3", TASK3_STK_SIZE, NULL, TASK3_PRIO, &Task3Task_Handler);
  vTaskDelete(StartTask_Handler); // 删除开始任务
  taskEXIT_CRITICAL();            // 退出临界区
}

/*任务1延时相对延时函数*/
void task1(void *pvParameters)
{
  uint8_t key_state = 0;
  while (1)
  {
    RtcShowTime();      // rtc测
    if (key_state == 0) // 没按下
    {
      if ((GPIO_ISTAT(GPIOA) & 1))
      {
        key_state = 1; // 现在按下了
      }
    }
    else // 按下
    {
      if (!(GPIO_ISTAT(GPIOA) & 1))
      {
        xTaskNotify(Task2Task_Handler, 1, eSetValueWithOverwrite); // 通知任务
        key_state = 0;                                             // 现在松开了
      }
    }
    vTaskDelay(10);
  }
}

/*任务二具体实现*/
void task2(void *pvParameters)
{
  uint32_t val = 0;
  while (1)
  {
    xTaskNotifyWait(~0, ~0, &val, 0);
    if (val == 1)
      GPIO_TG(GPIOB) = 0x4; // 切换灯
    vTaskDelay(100);
  }
}
extern uint16_t adc_value1;
void task3(void *pvParameters)
{
  while (1)
  {
    adc_value1 = ad_soft_filter(ADC1, 10, 7, 100, 500);
      
    taskENTER_CRITICAL();   
    OLED_ShowNum(0, 0, adc_value1, 4, 16, 1);
    OLED_Refresh();
    taskEXIT_CRITICAL();
      
    vTaskDelay(100);
  }
}

/*创建开始任务并调度*/
void freertos_demo(void)
{
  xTaskCreate(start_task, "start_task", START_STK_SIZE, NULL, START_TASK_PRIO, &StartTask_Handler);
  vTaskStartScheduler();
}
