#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "delay.h"
/*�������ļ�ע��������,ͷ�ļ������ִ�Сд*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1            /* �������ȼ� */
#define START_STK_SIZE 128           /* �����ջ��С */
TaskHandle_t StartTask_Handler;      /* ������ */
void start_task(void *pvParameters); /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO 2            /* �������ȼ� */
#define TASK1_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task1Task_Handler; /* ������ */
void task1(void *pvParameters); /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO 3            /* �������ȼ� */
#define TASK2_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task2Task_Handler; /* ������ */
void task2(void *pvParameters); /* ������ */

void start_task(void *pvParameters)
{
  /* �����ٽ��� */
  taskENTER_CRITICAL(); // �ر��жϵ�������
  /* ��������1 */
  xTaskCreate((TaskFunction_t)task1,
              (const char *)"task1",
              (uint16_t)TASK1_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK1_PRIO,
              (TaskHandle_t *)&Task1Task_Handler);
  /* ��������2 */
  xTaskCreate((TaskFunction_t)task2,
              (const char *)"task2",
              (uint16_t)TASK2_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK2_PRIO,
              (TaskHandle_t *)&Task2Task_Handler);
              
  vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
  taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}
void task1(void *pvParameters)
{
  while (1)
  {
    GPIOF->ODR^=1<<9;
//    printf("����һ��������..\r\n");
    vTaskDelay(500);
  }
}
List_t TestList;
ListItem_t item1;
ListItem_t item2;
ListItem_t item3;
/*�б���Ĳ����ɾ��*/
void task2(void *pvParameters)
{
    vListInitialise(&TestList);
    vListInitialiseItem(&item1);
    vListInitialiseItem(&item2);
    vListInitialiseItem(&item3);
    item1.xItemValue=40;
    item2.xItemValue=60;
    item3.xItemValue=50;
    
    printf("/**************�ڶ�������ӡ�б���б���ĵ�ַ**************/\r\n");
    printf("��Ŀ\t\t\t��ַ\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &item1);
    printf("ListItem2\t\t0x%p\t\r\n", &item2);
    printf("ListItem3\t\t0x%p\t\r\n", &item3);
    printf("/**************************����***************************/\r\n");
    
    printf("/*****************���������б���1�����б�******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* �б� */
                (ListItem_t*)&item1);       /* �б��� */
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("/**************************����***************************/\r\n");
    
    printf("/*****************���Ĳ����б���2�����б�******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* �б� */
                (ListItem_t*)&item2);       /* �б��� */
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (item2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (item2.pxPrevious));
    printf("/**************************����***************************/\r\n");
    
    printf("/*****************���岽���б���3�����б�******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* �б� */
                (ListItem_t*)&item3);       /* �б��� */
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (item2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (item3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (item2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (item3.pxPrevious));
    printf("/**************************����***************************/\r\n");
    
    printf("/*******************���������Ƴ��б���2********************/\r\n");
    uxListRemove((ListItem_t*   )&item2);   /* �Ƴ��б��� */
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (item3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (item3.pxPrevious));
    printf("/**************************����***************************/\r\n");
    
    printf("/****************���߲����б�ĩβ����б���2****************/\r\n");
    TestList.pxIndex=&item1;
    vListInsertEnd((List_t*     )&TestList,     /* �б� */
                   (ListItem_t* )&item2);   /* �б��� */
    printf("��Ŀ\t\t\t\t��ַ\r\n");
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (item1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (item2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (item3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (item1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (item2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (item3.pxPrevious));
    printf("/************************ʵ�����***************************/\r\n");
    vTaskDelay(100);
  while (1)
  {
      vTaskDelay(100);
  }
}
void freertos_demo(void)
{
  xTaskCreate((TaskFunction_t)start_task,          /* ������ */
              (const char *)"start_task",          /* �������� */
              (uint16_t)START_STK_SIZE,            /* �����ջ��С */
              (void *)NULL,                        /* ������������Ĳ��� */
              (UBaseType_t)START_TASK_PRIO,        /* �������ȼ� */
              (TaskHandle_t *)&StartTask_Handler); /* ������ */
  vTaskStartScheduler();
}
