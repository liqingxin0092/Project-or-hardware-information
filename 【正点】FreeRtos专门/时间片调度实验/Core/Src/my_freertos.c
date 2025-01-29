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
#define TASK1_PRIO 3            /* �������ȼ� */
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

/*��ʼ����*/
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL(); // �����ٽ���
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
  taskEXIT_CRITICAL();            // �˳��ٽ���
}

void task1(void *pvParameters)
{
  uint32_t num = 0;
  while (1)
  {
    taskENTER_CRITICAL(); // �����ٽ���
    printf("����1���д���:%d\r\n", ++num);
    delay_ms(10);
    taskEXIT_CRITICAL(); // �˳��ٽ���
  }
}

void task2(void *pvParameters)
{
  uint32_t num = 0;
  while (1)
  {
    taskENTER_CRITICAL(); // �����ٽ���
    printf("����2���д���:%d\r\n", ++num);
    delay_ms(10);
    taskEXIT_CRITICAL(); // �˳��ٽ���
  }
}

/*������ʼ���񲢿�ʼ����*/
void freertos_demo(void)
{
  xTaskCreate(start_task,          /* ������ */
              "start_task",        /* �������� */
              START_STK_SIZE,      /* �����ջ��С */
              NULL,                /* ������������Ĳ��� */
              START_TASK_PRIO,     /* �������ȼ� */
              &StartTask_Handler); /* ������ */
  vTaskStartScheduler();
}
