#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "GBK_LibDrive.h"
#include "key.h"
#include "touch.h"
#include "TF-card.h"

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
#define TASK2_PRIO 2            /* �������ȼ� */
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

uint8_t buff[512];
/*����1��ʱ�����ʱ����*/
void task1(void *pvParameters)
{
    pen_color=PURPLE;
  while (1)
  {
        tp_dev.scan(0);
        TOUCH_Line( pen_color);		//��ͼ	
        vTaskDelay(15);
  }
}

/*���������ʵ��*/
void task2(void *pvParameters)
{  u8 flag=0;
    u8 key_num;
  while (1)
  {
      key_num=KEY_Scan(0);
      if(key_num==2)//����
          LCD_Clear(background_color);
      else if(key_num==1) //У׼
      {
          taskENTER_CRITICAL(); // �����ٽ���
          TP_Adjust();
          taskEXIT_CRITICAL();            // �˳��ٽ���
      }
      else if(key_num==4)//����ɫ
      {
          if(pen_color==PURPLE)
                pen_color=CYAN;
          else if(pen_color==CYAN)
              pen_color=PURPLE;
      }
      LCD_Show_Picture(0,0,70,70,gImage_99);
      LCD_test_runing(310,230, 319, 239, BLUE,YELLOW,&flag);//��������(����)
      vTaskDelay(500);
  }
}

/*������ʼ���񲢵���*/
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
