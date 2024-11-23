#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "delay.h"
#include "touch.h"
#include "gpio.h"
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
#define TASK2_PRIO 2            /* �������ȼ� */
#define TASK2_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task2Task_Handler; /* ������ */
void task2(void *pvParameters); /* ������ */

/*��ʼ����*/
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL();  //�����ٽ��� 
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
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

/*����1��ʱ�����ʱ����*/
void task1(void *pvParameters)
{
  while (1)
  {
    gt9xxx_scan(0);
    TOUCH_Line(2);
    vTaskDelay(20);
  }
}

/*���������ʵ��*/
void task2(void *pvParameters)
{
    uint8_t flag1=0;
  while (1)
  {
    LTDC_turn_area_color(1005, 586, 1023, 599, YELLOW, BLUE,&flag1);
    taskENTER_CRITICAL();  //�����ٽ���
    if(KEY_Scan(0))
        LTDC_CLear(background_color);
    if(1005<tp_dev.x[0]&&tp_dev.x[0]<1023&&586<tp_dev.y[0]&&tp_dev.y[0]<599)
    {
        LTDC_CLear(background_color);
        tp_dev.x[0]=1;
    }
      taskEXIT_CRITICAL();            //�˳��ٽ���
    vTaskDelay(200);
   }
}

/*������ʼ���񲢵���*/
void freertos_demo(void)
{
  xTaskCreate(start_task,          /* ������ */
              "start_task",          /* �������� */
              START_STK_SIZE,            /* �����ջ��С */
              NULL,                        /* ������������Ĳ��� */
              START_TASK_PRIO,        /* �������ȼ� */
              &StartTask_Handler); /* ������ */
  vTaskStartScheduler();
}
