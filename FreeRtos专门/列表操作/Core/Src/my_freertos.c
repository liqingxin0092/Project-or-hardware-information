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
              
  vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL();            /* 退出临界区 */
}
void task1(void *pvParameters)
{
  while (1)
  {
    GPIOF->ODR^=1<<9;
//    printf("任务一正在运行..\r\n");
    vTaskDelay(500);
  }
}
List_t TestList;
ListItem_t item1;
ListItem_t item2;
ListItem_t item3;
/*列表项的插入和删除*/
void task2(void *pvParameters)
{
    vListInitialise(&TestList);
    vListInitialiseItem(&item1);
    vListInitialiseItem(&item2);
    vListInitialiseItem(&item3);
    item1.xItemValue=40;
    item2.xItemValue=60;
    item3.xItemValue=50;
    
    printf("/**************第二步：打印列表和列表项的地址**************/\r\n");
    printf("项目\t\t\t地址\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &item1);
    printf("ListItem2\t\t0x%p\t\r\n", &item2);
    printf("ListItem3\t\t0x%p\t\r\n", &item3);
    printf("/**************************结束***************************/\r\n");
    
    printf("/*****************第三步：列表项1插入列表******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&item1);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("/**************************结束***************************/\r\n");
    
    printf("/*****************第四步：列表项2插入列表******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&item2);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (item2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (item2.pxPrevious));
    printf("/**************************结束***************************/\r\n");
    
    printf("/*****************第五步：列表项3插入列表******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&item3);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (item2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (item3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (item2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (item3.pxPrevious));
    printf("/**************************结束***************************/\r\n");
    
    printf("/*******************第六步：移除列表项2********************/\r\n");
    uxListRemove((ListItem_t*   )&item2);   /* 移除列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (item3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (item3.pxPrevious));
    printf("/**************************结束***************************/\r\n");
    
    printf("/****************第七步：列表末尾添加列表项2****************/\r\n");
    TestList.pxIndex=&item1;
    vListInsertEnd((List_t*     )&TestList,     /* 列表 */
                   (ListItem_t* )&item2);   /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (item2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (item3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (item2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (item3.pxPrevious));
    printf("/************************实验结束***************************/\r\n");
    vTaskDelay(100);
  while (1)
  {
      vTaskDelay(100);
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
