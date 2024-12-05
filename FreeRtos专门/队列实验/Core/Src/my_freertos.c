#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "GBK_LibDrive.h"
#include "key.h"
#include "queue.h"
#include "touch.h"
/*�������ļ�ע��������,ͷ�ļ������ִ�Сд*/

char buff[100]="����һ��������,��������,1212131415,AAScss";
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

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO 2            /* �������ȼ� */
#define TASK3_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task3Task_Handler; /* ������ */
void task3(void *pvParameters); /* ������ */

/*���ж���*/
QueueHandle_t key_queue;        //С����
QueueHandle_t big_data_queue;   //������

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
  /* ��������3 */
  xTaskCreate((TaskFunction_t)task3,
              (const char *)"task3",
              (uint16_t)TASK3_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK3_PRIO,
              (TaskHandle_t *)&Task3Task_Handler);
  vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
  taskEXIT_CRITICAL();            // �˳��ٽ���
}

/*����1,ʵ�����*/
void task1(void *pvParameters)
{
     uint8_t key_num=0;
    char * buff_ptr=buff;//����������������ø��βν�������.
  while (1)
  {
      key_num=KEY_Scan(0);
      if (key_num==1||key_num==2)//��ֵ���.
      {
          if(xQueueSend(key_queue,&key_num,portMAX_DELAY)!=pdTRUE)
              printf("���Ͷ���key_queueʧ��!\r\n");
      }
      else if (key_num==4)
      {
          if(xQueueSend(big_data_queue,&buff_ptr,portMAX_DELAY)!=pdTRUE)
              printf("���Ͷ���big_data_queueʧ��!\r\n");
      }  
      vTaskDelay(50);
  }
}

/*�����,С���ݳ���*/
void task2(void *pvParameters)
{  
   uint8_t key=0;
  while (1)
  {
      if(xQueueReceive(key_queue,&key,portMAX_DELAY)!=pdTRUE)
          printf("key_queue��ȡʧ��!!!\r\n");
      else
          printf("key_queue��ȡ�ɹ�!!!ֵΪ:%d\r\n",key);
      vTaskDelay(50);
  }
}

/*����3,�����ݳ���*/
void task3(void *pvParameters)
{
    char * buff_ptr;
  while (1)
  {
      if(xQueueReceive(big_data_queue,&buff_ptr,portMAX_DELAY)!=pdTRUE)
          printf("big_data_queue��ȡʧ��!!!\r\n");
      else
          printf("big_data_queue��ȡ�ɹ�!!!ֵΪ:%s\r\n",buff_ptr);
      vTaskDelay(50);
  }
}

/*������ʼ���񲢵���*/
void freertos_demo(void)
{
    /*��������*/
    key_queue=xQueueCreate(2,sizeof(uint8_t)); //���г���,ÿ��Ԫ�ش�С
    if(key_queue!=NULL)
        printf("key_queue���д����ɹ�!!\r\n");
    else 
        printf("key_queue���д���ʧ��!!\r\n");
    
    big_data_queue=xQueueCreate(1,sizeof(char*)); //���г���,ÿ��Ԫ�ش�С
    if(big_data_queue!=NULL)
        printf("big_data_queue���д����ɹ�!!\r\n");
    else 
        printf("big_data_queue���д���ʧ��!!\r\n");
    
    /*������ʼ����*/
  xTaskCreate(start_task,          /* ������ */
              "start_task",        /* �������� */
              START_STK_SIZE,      /* �����ջ��С */
              NULL,                /* ������������Ĳ��� */
              START_TASK_PRIO,     /* �������ȼ� */
              &StartTask_Handler); /* ������ */
  vTaskStartScheduler();
}
