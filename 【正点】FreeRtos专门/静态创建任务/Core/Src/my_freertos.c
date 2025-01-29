#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"

/*�������ļ�ע��������,ͷ�ļ������ִ�Сд*/

/*��̬�����ڴ����*/ /*������������*/
StaticTask_t idle_task_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                        StackType_t ** ppxIdleTaskStackBuffer,
                                        uint32_t * pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer=&idle_task_tcb;
    *ppxIdleTaskStackBuffer=idle_task_stack;
    *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}
/* �����ʱ����������*/
StaticTask_t timer_task_tcb;
StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, 
                                     StackType_t ** ppxTimerTaskStackBuffer, 
                                     uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer=&timer_task_tcb;
    *ppxTimerTaskStackBuffer=timer_task_stack;
    *pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}
/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1            /* �������ȼ� */
#define START_STK_SIZE 128           /* �����ջ��С */
TaskHandle_t StartTask_Handler;      /* ������ */
StackType_t Start_task_stack[START_STK_SIZE]; /* �����ջ */
StaticTask_t start_task_tcb;  /* ������ƿ� */
void start_task(void *pvParameters); /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO 2            /* �������ȼ� */
#define TASK1_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task1Task_Handler; /* ������ */
StackType_t Task1_stack[TASK1_STK_SIZE]; /* �����ջ */
StaticTask_t Task1_tcb;  /* ������ƿ� */
void task1(void *pvParameters); /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO 3            /* �������ȼ� */
#define TASK2_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task2Task_Handler; /* ������ */
StackType_t Task2_stack[TASK2_STK_SIZE]; /* �����ջ */
StaticTask_t Task2_tcb;  /* ������ƿ� */
void task2(void *pvParameters); /* ������ */

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO 4            /* �������ȼ� */
#define TASK3_STK_SIZE 128      /* �����ջ��С */
TaskHandle_t Task3Task_Handler; /* ������ */
StackType_t Task3_stack[TASK3_STK_SIZE]; /* �����ջ */
StaticTask_t Task3_tcb;  /* ������ƿ� */
void task3(void *pvParameters); /* ������ */

void start_task(void *pvParameters)
{
  /* �����ٽ��� */
  taskENTER_CRITICAL(); // �ر��жϵ�������
 
   Task1Task_Handler=xTaskCreateStatic(  task1,
                                         "task1",
                                         TASK1_STK_SIZE,
                                         NULL,
                                         TASK1_PRIO,
                                         Task1_stack,
                                         &Task1_tcb );
   Task2Task_Handler=xTaskCreateStatic(  task2,
                                         "task2",
                                         TASK2_STK_SIZE,
                                         NULL,
                                         TASK2_PRIO,
                                         Task2_stack,
                                         &Task2_tcb );
   Task3Task_Handler=xTaskCreateStatic(  task3,
                                         "task3",
                                         TASK3_STK_SIZE,
                                         NULL,
                                         TASK3_PRIO,
                                         Task3_stack,
                                         &Task3_tcb );
  vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
  taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}
void task1(void *pvParameters)
{

  while (1)
  {
    GPIOF->ODR ^= 1 << 10;
    printf("����һ����ִ��\r\n");
    vTaskDelay(200);
  }
}
void task2(void *pvParameters)
{

  while (1)
  {
    GPIOF->ODR ^= 1 << 9;
    vTaskDelay(1000); /* ��ʱ1000ticks */
    printf("���������ִ��\r\n");
  }
}
void task3(void *pvParameters)
{

  while (1)
  {
       vTaskDelay(20);
       if(!(((GPIOB->IDR)>>8)&1))
       {
            if (Task1Task_Handler != NULL)
            {
                vTaskDelete(Task1Task_Handler);
                Task1Task_Handler = NULL;
                printf("����һɾ��\r\n");
            }
       }
       else if(!(((GPIOB->IDR)>>9)&1))
       {
            if (Task2Task_Handler != NULL)
            {
                vTaskDelete(Task2Task_Handler);
                Task2Task_Handler = NULL;
                printf("�����ɾ��\r\n");
            }
       }
  }
}
void freertos_demo(void)
{
   StartTask_Handler=xTaskCreateStatic(  start_task,
                                         "start_task",
                                         START_STK_SIZE,
                                         NULL,
                                         START_TASK_PRIO,
                                         Start_task_stack,
                                         &start_task_tcb );
  vTaskStartScheduler();
}
