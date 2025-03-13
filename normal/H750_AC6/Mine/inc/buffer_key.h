#ifndef __BUFFER_KEY_H
#define __BUFFER_KEY_H

#include "stdint.h"
#include "gpio.h"
#include "bsp.h"

/*��������*/
#define BUFFER_KEY_NUM 2
#define BUFFER_KEY_TEST_NUM BUFFER_KEY_NUM+1 //����һ����ϼ�

/*�˲�ʱ�� * 10ms */
#define KEY_FILTERING_TIME 3  

/*�����϶�ʱ�� * 10ms */
#define KEY_LONG_TIME 100  //1000ms

/*��������С*/
#define KEY_BUFFER_NUM 10

/*�Ƿ�֧�ֳ�������*/
#define USE_CONTINUOUS_TRANSMISSION 0

/*��������ʱ�� * 10ms */
#define KEY_REPEAT_TIME 10

/*�������ṹ*/
typedef struct
{
    uint8_t write_ptr;
    uint8_t read_ptr;
    uint8_t buffer[KEY_BUFFER_NUM+1];
}key_buffer_t;

/*������Ϣ*/
typedef struct
{
    gpio_t * GPIO_X;
    uint8_t pin;
    uint8_t active_state;
}key_info_t;

/*�����Ϣ*/
typedef struct
{
    uint8_t Filter_counter;         //�˲�����
    uint8_t state;                  //״̬
    uint8_t long_time;              //��������
    uint8_t long_time_repeat_time;  //������������
}key_test_t;

/*�ж��Ƿ���*/
#define is_active(id) (uint8_t)(GPIO_IN(key_mem[id].GPIO_X,key_mem[id].pin)==key_mem[id].active_state)

/*����ֵ��*/
typedef enum
{
    no_trigger=0, //û������
    
    a11_down=1,
    all_long=2,
    a11_up=3,
    
    a12_down=4,
    a12_long=5,
    a12_up=6,
    
    a11_a12_down=7,
    a11_a12_long=8,
    a11_a12_up=9,
    
}key_state_e;

extern key_info_t key_mem[BUFFER_KEY_NUM];

void buffer_key_init(void);
uint8_t buffer_key_get_code(void);
void key_detect_10ms(void);//�ж�

#endif

