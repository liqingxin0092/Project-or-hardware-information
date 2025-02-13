#ifndef __SOFT_TIMER_H
#define __SOFT_TIMER_H

#include "sys.h"

// 注意这里的val和reload和systick的reload与val不是一个层面的
// 单位统一为ms

/*软件定时器数量*/
#define SOFT_TIMER_NUMBER 3

/*soft timer的一些标志位*/
// is_using:是否被启用,1:是
// time_up:是否到时间了,1:是
// mode:一次性模式or循环装载模式,1:circle,0:once.
typedef struct
{
    uint8_t is_using : 1;
    uint8_t time_up : 1;
    uint8_t mode : 1;
} timer_flag_t;

/*模式取值*/
typedef enum
{
    TMR_ONCE_MODE = 0,  // once
    TMR_CIRCLE_MODE = 1 // circle
} TMR_MODE_E;

typedef struct
{
    volatile uint32_t reload;
    volatile uint32_t val;
    volatile timer_flag_t timer_flag;
} timer_t;

/*信息码*/
typedef enum
{
    TMR_OK = 0,
    TMR_IS_USING_ERR = 1,     // 这个定时器还在使用当中
    TMR_IS_NOT_USING_ERR = 2, // 这个定时器没有被启用
    TMR_INVALID_ID_ERR = 3,   // id无效

    TMR_TIME_UP = 4,
    TMR_NOT_TIME_UP = 5,
} TMR_INFO_CODE;

void soft_timer_init(void);                                 // 初始化软件定时器
uint8_t soft_timer_start_once(uint8_t id, uint32_t time);   // 启用一个once模式定时器
uint8_t soft_timer_start_circle(uint8_t id, uint32_t time); // 启用一个circle模式定时器
void soft_timer_dec(void);                                  // 递减计数值,中断用
uint8_t soft_timer_check_time_up(uint8_t id);               // 检查是否到时

#endif
