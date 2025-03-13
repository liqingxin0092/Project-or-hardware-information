#ifndef __BUFFER_KEY_H
#define __BUFFER_KEY_H

#include "stdint.h"
#include "gpio.h"
#include "bsp.h"

/*按键数量*/
#define BUFFER_KEY_NUM 2
#define BUFFER_KEY_TEST_NUM BUFFER_KEY_NUM+1 //额外一种组合键

/*滤波时间 * 10ms */
#define KEY_FILTERING_TIME 3  

/*长摁认定时间 * 10ms */
#define KEY_LONG_TIME 100  //1000ms

/*缓冲区大小*/
#define KEY_BUFFER_NUM 10

/*是否支持长摁连发*/
#define USE_CONTINUOUS_TRANSMISSION 0

/*长摁连发时间 * 10ms */
#define KEY_REPEAT_TIME 10

/*缓冲区结构*/
typedef struct
{
    uint8_t write_ptr;
    uint8_t read_ptr;
    uint8_t buffer[KEY_BUFFER_NUM+1];
}key_buffer_t;

/*按键信息*/
typedef struct
{
    gpio_t * GPIO_X;
    uint8_t pin;
    uint8_t active_state;
}key_info_t;

/*检测信息*/
typedef struct
{
    uint8_t Filter_counter;         //滤波计数
    uint8_t state;                  //状态
    uint8_t long_time;              //长摁计数
    uint8_t long_time_repeat_time;  //长摁连发计数
}key_test_t;

/*判断是否按下*/
#define is_active(id) (uint8_t)(GPIO_IN(key_mem[id].GPIO_X,key_mem[id].pin)==key_mem[id].active_state)

/*返回值表*/
typedef enum
{
    no_trigger=0, //没有摁下
    
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
void key_detect_10ms(void);//中断

#endif

