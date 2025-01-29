#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
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
              
  vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
  taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}
void task1(void *pvParameters)
{
   uint8_t num=0;
  while (1)
  {
    if( ++num == 5)
    {
        portDISABLE_INTERRUPTS();
        printf("关闭中断\r\n");
        delay_ms(5000);
        //vTaskDelay(5000);
        portENABLE_INTERRUPTS();
        printf("开中断\r\n");
        num=0;
    }
    vTaskDelay(1000);
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
