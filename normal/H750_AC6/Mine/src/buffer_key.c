#include "buffer_key.h"
#include "gpio.h"
#include "data_structure_m.h"

/*�������ζ���*/
circle_buffer_t keyBuffer;

/*����������*/
uint8_t key_buffer[KEY_BUFFER_SIZE+1];

/*��ⰴ���Ƿ���*/
key_info_t key_mem[BUFFER_KEY_NUM]={
    { GPIO_A ,11 ,0},
    { GPIO_A ,12 ,1},
};

/*�������ʱ�ı���*/
static key_test_t Key_test[BUFFER_KEY_TEST_NUM];

/*����Ƿ���*/
//�����˷���1
static uint8_t is_key_acticve(uint8_t id)
{
    uint8_t trigger_count=0;
    /*����*/
    if(id<BUFFER_KEY_NUM) 
    {
        if(is_active(id))
            trigger_count++;
        if(trigger_count==1)
            return 1;
        else 
            return 0;
    }
    /*��ϼ�*/
    else
    {
        if(is_active(0)&is_active(1)) //a11,a12���
            return 1;
        else
            return 0;
    }        
}    

/*��ʼ��buffer_key*/
void buffer_key_init(void)
{
    /*������������*/
    gpio_init_pin(GPIO_A,11,0,0,0,1,0);//�͵�ƽ����
    gpio_init_pin(GPIO_A,12,0,0,0,2,0);//�ߵ�ƽ����
    
    /*���ݽṹ��ʼ��*/
    keyBuffer.buffer=key_buffer;
    keyBuffer.size=KEY_BUFFER_SIZE;
}

/*��ȡ��ֵ*/
uint8_t buffer_key_get_code(void)
{
    return read_buffer(&keyBuffer);
}

/*10ms���,���ж�*/
void key_detect_10ms(void)
{
    uint8_t i;
    for(i=0;i<BUFFER_KEY_TEST_NUM;i++)//��������,��ϼ�
    {
        if(is_key_acticve(i))//�а�������
        {
            /*�����˲�*/
            if(Key_test[i].Filter_counter<KEY_FILTERING_TIME)//���δ�ﵽ�˲�ʱ��
            {
                Key_test[i].Filter_counter++;
            }
            else  //�ﵽ�˲�ʱ��
            {
                if(Key_test[i].state==0) //��¼�״ΰ���
                {
                    Key_test[i].state=1;//��¼Ϊ����״̬
                    write_buffer(&keyBuffer, 3*i+1);//���µ�״̬��
                }
                if(Key_test[i].state==1)//���״̬Ϊ����,���ܼ�¼����
                {
                    if (Key_test[i].long_time < KEY_LONG_TIME) //���û�ﵽ����ʱ��,�ż�¼������
                    {
                        /* ���Ͱ�ť�������µ���Ϣ */
                        if (++Key_test[i].long_time  == KEY_LONG_TIME)
                        {
                            /* ��ֵ���밴�� FIFO */
                            write_buffer(&keyBuffer,3*i+2);
                        }
                    }
                    #if USE_CONTINUOUS_TRANSMISSION==1
                    else   //������ʱ��,���г�������
                    {
                        if (++Key_test[i].long_time_repeat_time >= KEY_REPEAT_TIME)
                        {
                            Key_test[i].long_time_repeat_time= 0;
                            /* ��������ÿ�� KEY_REPEAT_TIME * 10ms ���� 1 ������ */
                            write_buffer(&keyBuffer, 3*i+2 );
                        }
                    }
                   #endif
                }
            }
        }
        else//�ް�������
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
                    Key_test[i].state = 0;//��¼Ϊδ����״̬
                    write_buffer(&keyBuffer, 3*i+3);//�����״̬��
                }
            }
            Key_test[i].long_time=0;   //��ճ���
            Key_test[i].Filter_counter=0; //����˲�
        }
    }
}


