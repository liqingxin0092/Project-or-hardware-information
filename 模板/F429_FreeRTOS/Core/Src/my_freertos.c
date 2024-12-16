#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "stdio.h"
#include "list.h"
#include "delay.h"
#include "touch.h"
#include "gpio.h"
#include "piclib.h"
#include "string.h"
#include "text.h"
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
#define TASK1_STK_SIZE 68      /* �����ջ��С */
TaskHandle_t Task1Task_Handler; /* ������ */
void task1(void *pvParameters); /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO 2            /* �������ȼ� */
#define TASK2_STK_SIZE 88      /* �����ջ��С */
TaskHandle_t Task2Task_Handler; /* ������ */
void task2(void *pvParameters); /* ������ */

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO 2            /* �������ȼ� */
#define TASK3_STK_SIZE 210      /* �����ջ��С */
TaskHandle_t Task3Task_Handler; /* ������ */
void task3(void *pvParameters); /* ������ */
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
  /* ��������3 */
  xTaskCreate((TaskFunction_t)task3,
              (const char *)"task3",
              (uint16_t)TASK3_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK3_PRIO,
              (TaskHandle_t *)&Task3Task_Handler);        
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
char task_buffer[300];
/*���������ʵ��*/
void task2(void *pvParameters)
{
  uint8_t flag1=0;
  while (1)
  {
//    vTaskList(task_buffer);
//    vTaskGetRunTimeStats(task_buffer);
//    printf("%s\r\n",task_buffer);
    LTDC_turn_area_color(1005, 586, 1023, 599, YELLOW, BLUE,&flag1);
      
    taskENTER_CRITICAL();  //�����ٽ���
    if(1005<tp_dev.x[0]&&tp_dev.x[0]<1023&&586<tp_dev.y[0]&&tp_dev.y[0]<599)
    {
        LTDC_CLear(background_color);
        tp_dev.x[0]=1;
    }
    
    taskEXIT_CRITICAL();  //�˳��ٽ���
    
    vTaskDelay(200);
   }
}

/*����3*/
void task3(void *pvParameters)
{
  DIR picdir;	 		 //ͼƬĿ¼
  u16 totpicnum; 		 //ͼƬ�ļ����� 
  FILINFO *picfileinfo;  //�ļ���Ϣ
  u8 *pname;			 //��·�����ļ���
  u32 *picoffsettbl;	 //ͼƬ�ļ�offset������ 
  u8 res;                //ִ�н��
  u16 curindex;		     //ͼƬ��ǰ����
  u16 temp;
  u8 keynum;
  char p[20];
  vTaskDelay(30);
  if(f_opendir(&picdir,"0:/PICTURE")!=0)//��ͼƬ�ļ���
  {
      printf("ͼƬĿ¼��ʧ��\r\n");
      while(1);
  }
  totpicnum=pic_get_tnum("0:/PICTURE");
  if(totpicnum==0)////�õ�����Ч�ļ���,ͼƬ�ļ�Ϊ0
  {
      printf("ͼƬ��ĿΪ0\r\n");
      while(1);
  }
  vTaskDelay(30);
  printf("ͼƬ��ĿΪ%d\r\n",totpicnum);
  picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//���ļ���Ϣ�����ڴ�
  pname=mymalloc(SRAMIN,FF_MAX_LFN*2+1);					//Ϊ��·�����ļ��������ڴ�
  picoffsettbl=mymalloc(SRAMIN,2*totpicnum);				//����4*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
  if(!picfileinfo||!pname||!picoffsettbl)
  {
      printf("�ڴ����ʧ��,picfileinfo:%#x,pname:%#x,picoffsettbl:%#x",(u32)picfileinfo,(u32)pname,(u32)picoffsettbl);
  }
   f_closedir (&picdir);  
   res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.dptr;								//��¼��ǰdptrƫ��
	        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�	 	 
			res=exfuns_file_type(picfileinfo->fname);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picoffsettbl[curindex]=temp;//��¼����
				curindex++;
                printf("picture fats name:%s\r\n",picfileinfo->fname);
			}	    
		} 
	}   
  	piclib_init();										//��ʼ����ͼ	   	   
	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
    while(res==FR_OK)//�򿪳ɹ�
	{	
        
		dir_sdi(&picdir,picoffsettbl[curindex]);	    //�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�
		strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)picfileinfo->fname);//���ļ������ں���
 		ai_load_picfile(pname,0,0,1024,600,1);//��ʾͼƬ 
		while(1) 
		{
            vTaskDelay(30);
			keynum=KEY_Scan(0);		//ɨ�谴��
            sprintf(p,"key����:%d",keynum);
            text_show_string(0, 584, 1024, 600, p, 16, 0, PURPLE);
            if(keynum)//��һ��
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				break;
			}			    
		    res=0;  
	    } 
        vTaskDelay(30);
    } 
    myfree(SRAMIN,picfileinfo);			//�ͷ��ڴ�						   		    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picoffsettbl);			//�ͷ��ڴ�
}
/*������ʼ���񲢵���*/
void freertos_start(void)
{
  xTaskCreate(start_task,          /* ������ */
              "start_task",          /* �������� */
              START_STK_SIZE,            /* �����ջ��С */
              NULL,                        /* ������������Ĳ��� */
              START_TASK_PRIO,        /* �������ȼ� */
              &StartTask_Handler); /* ������ */
  vTaskStartScheduler();
}
