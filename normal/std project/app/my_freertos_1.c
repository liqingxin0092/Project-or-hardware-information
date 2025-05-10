#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "stdio.h"
#include "gd32f4xx.h"
#include "board.h"
#include "oled.h"
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
#define TASK2_PRIO 1            /* �������ȼ� */
#define TASK2_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task2Task_Handler; /* ������ */
void task2(void *pvParameters); /* ������ */

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO 1            /* �������ȼ� */
#define TASK3_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task3Task_Handler; /* ������ */
void task3(void *pvParameters); /* ������ */

/*��ʼ����*/
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL(); // �����ٽ���
  xTaskCreate(task1, "task1", TASK1_STK_SIZE, NULL, TASK1_PRIO, &Task1Task_Handler);
  xTaskCreate(task2, "task2", TASK2_STK_SIZE, NULL, TASK2_PRIO, &Task2Task_Handler);
  xTaskCreate(task3, "task3", TASK3_STK_SIZE, NULL, TASK3_PRIO, &Task3Task_Handler);
  vTaskDelete(StartTask_Handler); // ɾ����ʼ����
  taskEXIT_CRITICAL();            // �˳��ٽ���
}

/*����1��ʱ�����ʱ����*/
void task1(void *pvParameters)
{
  uint8_t key_state = 0;
  while (1)
  {
    RtcShowTime();      // rtc��
    if (key_state == 0) // û����
    {
      if ((GPIO_ISTAT(GPIOA) & 1))
      {
        key_state = 1; // ���ڰ�����
      }
    }
    else // ����
    {
      if (!(GPIO_ISTAT(GPIOA) & 1))
      {
        xTaskNotify(Task2Task_Handler, 1, eSetValueWithOverwrite); // ֪ͨ����
        key_state = 0;                                             // �����ɿ���
      }
    }
    vTaskDelay(10);
  }
}

/*���������ʵ��*/
void task2(void *pvParameters)
{
  uint32_t val = 0;
  while (1)
  {
    xTaskNotifyWait(~0, ~0, &val, 0);
    if (val == 1)
      GPIO_TG(GPIOB) = 0x4; // �л���
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

/*������ʼ���񲢵���*/
void freertos_demo(void)
{
  xTaskCreate(start_task, "start_task", START_STK_SIZE, NULL, START_TASK_PRIO, &StartTask_Handler);
  vTaskStartScheduler();
}
