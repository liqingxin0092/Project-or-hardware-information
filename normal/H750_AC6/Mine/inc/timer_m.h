/**
 *******************************************************************************
 * @file        timer_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-1
 * @brief       各种定时,计时需求
 * 更新记录:    无
 * 
 *****************************************************************************************
 */
#ifndef __TIMER_M_H
#define __TIMER_M_H

#include "bsp.h"

/*************************注意**********************************/

// 注意这里的val和reload和systick的reload与val不是一个层面的
// 单位统一为ms
// 注意此函数设置了滴答定时器,与其他设置了滴答定时器的函数不兼容
// 0号定时器固定用于delay
/*****************************************************************/

/**************************用户配置******************************/

/*软件定时器数量*/
#define SOFT_TIMER_NUMBER 4

#if CPU_CLOCK ==480
#define SOFT_TIMER_HZ 479999
#define SOFT_TIMER_RUNTIME_1US 480
#elif CPU_CLOCK ==600
#define SOFT_TIMER_HZ 599999
#define SOFT_TIMER_RUNTIME_1US 600
#endif

/*是否启用微秒定时器*/
#define SOFT_TIMER_USE_US 1

/*****************************************************************/

/*soft timer的一些标志位*/
// is_using:是否被启用,1:是
// time_up:是否到时间了,1:是
// mode:一次性模式or循环装载模式,1:circle,0:once.
typedef struct
{
    volatile uint8_t is_using : 1;
    volatile uint8_t time_up : 1;
    volatile uint8_t mode : 1;
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

void timer_init(void); // 初始化定时器.

/*延时*/
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);

/*软件定时器(ms定时)*/
uint8_t soft_timer_start_once(uint8_t id, uint32_t time);   // 启用一个once模式定时器
uint8_t soft_timer_start_circle(uint8_t id, uint32_t time); // 启用一个circle模式定时器
void soft_timer_dec(void);                                  // 递减计数值,放systick中断
uint8_t soft_timer_check_time_up(uint8_t id);               // 检查是否到时
void soft_timer_show_info(void);                            // 查看调试信息

/*us测量*/
void soft_timer_start_measure(void);
uint32_t soft_timer_get_time(void);

/*us定时*/
#if SOFT_TIMER_USE_US == 1
void hard_timer_set_point(uint32_t nus); // 硬件定时器微秒计时
uint8_t hard_timer_get_result(void);     // 硬件定时器微秒检查
#endif

#endif
