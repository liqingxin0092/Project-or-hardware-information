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
/*这两个文件注意起名字,头文件不区分大小写*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
void start_task(void *pvParameters); /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO 2            /* 任务优先级 */
#define TASK1_STK_SIZE 68      /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler; /* 任务句柄 */
void task1(void *pvParameters); /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO 2            /* 任务优先级 */
#define TASK2_STK_SIZE 88      /* 任务堆栈大小 */
TaskHandle_t Task2Task_Handler; /* 任务句柄 */
void task2(void *pvParameters); /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO 2            /* 任务优先级 */
#define TASK3_STK_SIZE 210      /* 任务堆栈大小 */
TaskHandle_t Task3Task_Handler; /* 任务句柄 */
void task3(void *pvParameters); /* 任务函数 */
/*起始任务*/
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL();  //进入临界区 
  /* 创建任务1 */
  xTaskCreate((TaskFunction_t)task1,
              (const char *)"task1",
              (uint16_t)TASK1_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK1_PRIO,
              (TaskHandle_t *)&Task1Task_Handler);
  /* 创建任务2 */
  xTaskCreate((TaskFunction_t)task2,
              (const char *)"task2",
              (uint16_t)TASK2_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK2_PRIO,
              (TaskHandle_t *)&Task2Task_Handler);
  /* 创建任务3 */
  xTaskCreate((TaskFunction_t)task3,
              (const char *)"task3",
              (uint16_t)TASK3_STK_SIZE,
              (void *)NULL,
              (UBaseType_t)TASK3_PRIO,
              (TaskHandle_t *)&Task3Task_Handler);        
  vTaskDelete(StartTask_Handler); /* 删除开始任务 */
  taskEXIT_CRITICAL();            //退出临界区
}

/*任务1延时相对延时函数*/
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
/*任务二具体实现*/
void task2(void *pvParameters)
{
  uint8_t flag1=0;
  while (1)
  {
//    vTaskList(task_buffer);
//    vTaskGetRunTimeStats(task_buffer);
//    printf("%s\r\n",task_buffer);
    LTDC_turn_area_color(1005, 586, 1023, 599, YELLOW, BLUE,&flag1);
      
    taskENTER_CRITICAL();  //进入临界区
    if(1005<tp_dev.x[0]&&tp_dev.x[0]<1023&&586<tp_dev.y[0]&&tp_dev.y[0]<599)
    {
        LTDC_CLear(background_color);
        tp_dev.x[0]=1;
    }
    
    taskEXIT_CRITICAL();  //退出临界区
    
    vTaskDelay(200);
   }
}

/*任务3*/
void task3(void *pvParameters)
{
  DIR picdir;	 		 //图片目录
  u16 totpicnum; 		 //图片文件总数 
  FILINFO *picfileinfo;  //文件信息
  u8 *pname;			 //带路径的文件名
  u32 *picoffsettbl;	 //图片文件offset索引表 
  u8 res;                //执行结果
  u16 curindex;		     //图片当前索引
  u16 temp;
  u8 keynum;
  char p[20];
  vTaskDelay(30);
  if(f_opendir(&picdir,"0:/PICTURE")!=0)//打开图片文件夹
  {
      printf("图片目录打开失败\r\n");
      while(1);
  }
  totpicnum=pic_get_tnum("0:/PICTURE");
  if(totpicnum==0)////得到总有效文件数,图片文件为0
  {
      printf("图片数目为0\r\n");
      while(1);
  }
  vTaskDelay(30);
  printf("图片数目为%d\r\n",totpicnum);
  picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//给文件信息申请内存
  pname=mymalloc(SRAMIN,FF_MAX_LFN*2+1);					//为带路径的文件名分配内存
  picoffsettbl=mymalloc(SRAMIN,2*totpicnum);				//申请4*totpicnum个字节的内存,用于存放图片索引
  if(!picfileinfo||!pname||!picoffsettbl)
  {
      printf("内存分配失败,picfileinfo:%#x,pname:%#x,picoffsettbl:%#x",(u32)picfileinfo,(u32)pname,(u32)picoffsettbl);
  }
   f_closedir (&picdir);  
   res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.dptr;								//记录当前dptr偏移
	        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出	 	 
			res=exfuns_file_type(picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picoffsettbl[curindex]=temp;//记录索引
				curindex++;
                printf("picture fats name:%s\r\n",picfileinfo->fname);
			}	    
		} 
	}   
  	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
    while(res==FR_OK)//打开成功
	{	
        
		dir_sdi(&picdir,picoffsettbl[curindex]);	    //改变当前目录索引	   
        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)picfileinfo->fname);//将文件名接在后面
 		ai_load_picfile(pname,0,0,1024,600,1);//显示图片 
		while(1) 
		{
            vTaskDelay(30);
			keynum=KEY_Scan(0);		//扫描按键
            sprintf(p,"key键码:%d",keynum);
            text_show_string(0, 584, 1024, 600, p, 16, 0, PURPLE);
            if(keynum)//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}			    
		    res=0;  
	    } 
        vTaskDelay(30);
    } 
    myfree(SRAMIN,picfileinfo);			//释放内存						   		    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picoffsettbl);			//释放内存
}
/*创建开始任务并调度*/
void freertos_start(void)
{
  xTaskCreate(start_task,          /* 任务函数 */
              "start_task",          /* 任务名称 */
              START_STK_SIZE,            /* 任务堆栈大小 */
              NULL,                        /* 传入给任务函数的参数 */
              START_TASK_PRIO,        /* 任务优先级 */
              &StartTask_Handler); /* 任务句柄 */
  vTaskStartScheduler();
}
