#include "buffer_key.h"
#include "gpio.h"


/*检测按键是否按下*/
key_info_t key_mem[BUFFER_KEY_NUM]={
    { GPIO_A ,11 ,0},
    { GPIO_A ,12 ,1},
};

/*按键环形队列*/
 key_buffer_t keyBuffer;

/*按键检测时的变量*/
static key_test_t Key_test[BUFFER_KEY_TEST_NUM];


/*检测是否按下*/
//按下了返回1
static uint8_t is_key_acticve(uint8_t id)
{
    uint8_t trigger_count=0;
    /*单键*/
    if(id<BUFFER_KEY_NUM) 
    {
        if(is_active(id))
            trigger_count++;
        if(trigger_count==1)
            return 1;
        else 
            return 0;
    }
    /*组合键*/
    else
    {
        if(is_active(0)&is_active(1)) //a11,a12组合
            return 1;
        else
            return 0;
    }        
}    

/*写队列*/
static void write_buffer(key_buffer_t* circle_buffer, uint8_t val)
{
    uint8_t temp=circle_buffer->write_ptr+1;
    (temp==KEY_BUFFER_NUM+1)?(temp=0):(temp=temp);
    if(circle_buffer->read_ptr!=temp)
    {
        circle_buffer->buffer[circle_buffer->write_ptr]=val;
    }
    else return ;//队列满
    if(++circle_buffer->write_ptr==KEY_BUFFER_NUM+1)
        circle_buffer->write_ptr=0;
}

/*读队列*/
uint8_t read_buffer(key_buffer_t* circle_buffer)
{
    uint8_t return_val; 
    if(circle_buffer->read_ptr!=circle_buffer->write_ptr)
    {
        return_val=circle_buffer->buffer[circle_buffer->read_ptr];
    }
    else return 0;//队列空
    
    if(++circle_buffer->read_ptr==KEY_BUFFER_NUM+1)
        circle_buffer->read_ptr=0;
    return return_val;
}

/*初始化buffer_key*/
void buffer_key_init(void)
{
    /*两个按键测试*/
    gpio_init_pin(GPIO_A,11,0,0,0,1,0);//低电平触发
    gpio_init_pin(GPIO_A,12,0,0,0,2,0);//高电平触发
}

/*获取键值*/
uint8_t buffer_key_get_code(void)
{
    return read_buffer(&keyBuffer);
}

/*10ms检测,放中断*/
void key_detect_10ms(void)
{
    uint8_t i;
    for(i=0;i<BUFFER_KEY_TEST_NUM;i++)//包括单键,组合键
    {
        if(is_key_acticve(i))//有按键按下
        {
            /*进入滤波*/
            if(Key_test[i].Filter_counter<KEY_FILTERING_TIME)//如果未达到滤波时间
            {
                Key_test[i].Filter_counter++;
            }
            else  //达到滤波时间
            {
                if(Key_test[i].state==0) //记录首次按下
                {
                    Key_test[i].state=1;//纪录为按下状态
                    write_buffer(&keyBuffer, 3*i+1);//按下的状态码
                }
                if(Key_test[i].state==1)//如果状态为按下,才能记录长摁
                {
                    if (Key_test[i].long_time < KEY_LONG_TIME) //如果没达到长摁时间,才记录长摁下
                    {
                        /* 发送按钮持续按下的消息 */
                        if (++Key_test[i].long_time  == KEY_LONG_TIME)
                        {
                            /* 键值放入按键 FIFO */
                            write_buffer(&keyBuffer,3*i+2);
                        }
                    }
                    #if USE_CONTINUOUS_TRANSMISSION==1
                    else   //长摁超时了,进行长摁连发
                    {
                        if (++Key_test[i].long_time_repeat_time >= KEY_REPEAT_TIME)
                        {
                            Key_test[i].long_time_repeat_time= 0;
                            /* 常按键后，每隔 KEY_REPEAT_TIME * 10ms 发送 1 个按键 */
                            write_buffer(&keyBuffer, 3*i+2 );
                        }
                    }
                   #endif
                }
            }
        }
        else//无按键按下
        {
            if(Key_test[i].Filter_counter > KEY_FILTERING_TIME)
            {
                Key_test[i].Filter_counter= KEY_FILTERING_TIME;
            }
            else if(Key_test[i].Filter_counter!= 0)
            {
                Key_test[i].Filter_counter--;
            }
            else
            {
                if (Key_test[i].state == 1)
                {
                    Key_test[i].state = 0;//纪录为按下状态
                    write_buffer(&keyBuffer, 3*i+3);//谈起的状态码
                }
            }
            Key_test[i].long_time=0;   //清空长摁
            Key_test[i].Filter_counter=0; //清空滤波
        }
    }
}


