#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "delay.h"
#include "LTDC.h"
#include "malloc.h"

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

/*����1����ʵ��*/
void task1(void *pvParameters)
{
  while (1)
  {
    LTDC_turn_area_color(450, 450, 500, 500, 0xF81F, RED);
    vTaskDelay(500);
  }
}
char task_buffer[300];
/*�����:����API������ʹ��*/
void task2(void *pvParameters)
{
  UBaseType_t priority = 0;
  UBaseType_t task_num = 0;
  UBaseType_t task_num2 = 0;
  TaskStatus_t *status_array = 0;
  TaskStatus_t *single_task_status = 0;
  TaskHandle_t task_hander = 0;
  UBaseType_t task_stack_min = 0;
  eTaskState task_state = 0;
  
  // char p[30] = {0};

  // /*�����������ȼ�*/
  // vTaskPrioritySet(Task2Task_Handler, 30);

  // /*��ȡ�������ȼ�*/
  // priority = uxTaskPriorityGet(NULL);

  // /*�õ���������*/
  // task_num = uxTaskGetNumberOfTasks();

  // /*�����ڴ沢��ȡȫ������״̬*/
  // status_array = mymalloc(SRAMIN, sizeof(TaskStatus_t) * task_num);
  // task_num2 = uxTaskGetSystemState(status_array, task_num, NULL);

  // /*�����ڴ沢��ȡ��������״̬*/
  // single_task_status = mymalloc(SRAMIN, sizeof(TaskStatus_t));
  // vTaskGetInfo(Task1Task_Handler, single_task_status, pdTRUE, eInvalid);

  // /*��Ϣ��ʾ*/
  // sprintf(p, "task2 priority:%ld", priority);
  // LTDC_ShowString(0, 0, 1024, 24, 24, (uint8_t *)p, YELLOW, 0);
  // sprintf(p, "task num:%ld", task_num);
  // LTDC_ShowString(0, 24, 1024, 24, 24, (uint8_t *)p, PURPLE, 0);
  // sprintf(p, "task num2:%ld", task_num2);
  // LTDC_ShowString(0, 48, 1024, 24, 24, (uint8_t *)p, PURPLE, 0);

  // printf("������\t\t�������ȼ�\t������\r\n");
  // for (uint8_t i = 0; i < task_num2; i++)
  // {
  //   printf("%s\t\t%ld\t%ld\r\n", status_array[i].pcTaskName, status_array[i].uxCurrentPriority, status_array[i].xTaskNumber);
  // }
  // printf("��������:%s,�������ȼ�:%ld,������:%ld,����״̬:%d", single_task_status->pcTaskName,
  //        single_task_status->uxCurrentPriority,
  //        single_task_status->xTaskNumber,
  //        single_task_status->eCurrentState);

  // /*ͨ����������ȡ���*/
  // task_hander = xTaskGetHandle("task1");
  // printf("����1��ַ:%#x\r\n", (uint32_t)task_hander);
  // printf("����1�����ַ:%#x\r\n", (uint32_t)Task1Task_Handler);

  // task_stack_min=uxTaskGetStackHighWaterMark(Task2Task_Handler);
  // printf("ʣ���ջ:%ld\r\n",task_stack_min);

//  task_state = eTaskGetState(Task1Task_Handler);
//  printf("%d", task_state);
  vTaskList(task_buffer);
  printf("%s\r\n",task_buffer);
  while (1)
  {
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
