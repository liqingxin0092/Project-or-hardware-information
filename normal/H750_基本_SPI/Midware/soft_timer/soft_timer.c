#include "soft_timer.h"

/*创建对应数量的软件定时器*/
timer_t soft_timer[SOFT_TIMER_NUMBER];

/*初始化软件定时器*/
// 注意此函数设置了滴答定时器,与其他设置了滴答定时器的函数不兼容
void soft_timer_init(void)
{
    /*软件定时器复位*/
    for (uint8_t i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        soft_timer[i].reload = 0;
        soft_timer[i].val = 0;
        soft_timer[i].timer_flag.is_using = 0;
        soft_timer[i].timer_flag.time_up = 0;
        soft_timer[i].timer_flag.mode = TMR_ONCE_MODE;
    }

    /*设置systick时基为1ms*/
    // 2     1: Processor clock.
    // 1     1: Counting down to zero asserts the SysTick exception request
    // 0     1: Counter enabled
    while (SysTick->CTRL != 7)
        SysTick->CTRL = 7;
    SysTick->LOAD = 479999; // 480 000 -1
}

/*启用一个once模式定时器*/
// id:序号,取值：0~SOFT_TIMER_NUMBER-1
// time:时间
uint8_t soft_timer_start_once(uint8_t id, uint32_t time)
{
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.is_using = 1;         // 标记此定时器正在被使用
    soft_timer[id].timer_flag.mode = TMR_ONCE_MODE; // once模式
    soft_timer[id].val = time;                      // 设置装载值
    return TMR_OK;
}

/*启用一个circle模式定时器*/
// id:序号,取值：0~SOFT_TIMER_NUMBER-1
// time:时间
uint8_t soft_timer_start_circle(uint8_t id, uint32_t time)
{
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.is_using = 1;           // 标记此定时器正在被使用
    soft_timer[id].timer_flag.mode = TMR_CIRCLE_MODE; // once模式
    soft_timer[id].val = time;                        // 设置装载值
    soft_timer[id].reload = time;
    return TMR_OK;
}

/*递减计数值*/
// 放到中断里用的
void soft_timer_dec(void)
{
    for (uint8_t i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        if (soft_timer[i].timer_flag.is_using && soft_timer[i].val > 0) // 如果这个定时器被启用并且val大于0
        {
            soft_timer[i].val--;
            if (soft_timer[i].val == 0) // 如果减到0了
            {
                soft_timer[i].timer_flag.time_up = 1; // 置到时标志
                if (soft_timer[i].timer_flag.mode == TMR_CIRCLE_MODE)
                    soft_timer[i].val = soft_timer[i].reload; // 如果是循环模式就重装
            }
        }
    }
}

/*检查是否到时*/
uint8_t soft_timer_check_time_up(uint8_t id)
{
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 0)
        return TMR_IS_NOT_USING_ERR;
    if (soft_timer[id].timer_flag.time_up) // 如果计数到时了
    {
        if (soft_timer[id].timer_flag.mode == TMR_ONCE_MODE)
        {
            soft_timer[id].timer_flag.time_up = 0;  // 清空到时标志位
            soft_timer[id].timer_flag.is_using = 0; // 结束使用
            return TMR_TIME_UP;
        }
        else if (soft_timer[id].timer_flag.mode == TMR_CIRCLE_MODE)
        {
            soft_timer[id].timer_flag.time_up = 0; // 清空到时标志位
            return TMR_TIME_UP;
        }
    }
    else
        return TMR_NOT_TIME_UP;
}
