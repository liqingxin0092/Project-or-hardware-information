#include "soft_timer.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "h7_core.h"

/*微秒级硬件定时器*/
static TIM_HandleTypeDef htim2;
uint32_t hard_timer_point;
uint8_t is_use_hard_timer;

/*freeRTOS时基*/
extern void xPortSysTickHandler(void);

/*创建对应数量的软件定时器*/
static volatile timer_t soft_timer[SOFT_TIMER_NUMBER];

/*用于MS延时*/
static volatile uint8_t software_ms_delay_flag;
static volatile uint32_t software_ms_delay_val;

/*测量运行时间*/
volatile uint8_t software_runtime_flag;
volatile uint32_t software_runtime_overflow_time;
volatile uint32_t software_runtime_start_val;

/*初始化软件定时器*/
void soft_timer_init(void)
{
    uint8_t i;
    /*软件定时器复位*/
    for (i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        soft_timer[i].reload = 0;
        soft_timer[i].val = 0;
        soft_timer[i].timer_flag.is_using = 0;
        soft_timer[i].timer_flag.time_up = 0;
        soft_timer[i].timer_flag.mode = TMR_ONCE_MODE;
    }
    __init_systic(SOFT_TIMER_HZ, 1, 1);

#if SOFT_TIMER_USE_US == 1
    /*微秒级定时器初始化*/
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 240 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数器
    htim2.Init.Period = 0xffffffff;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim2);

#endif
}

/*启用一个once模式定时器*/
// id:序号,取值：0~SOFT_TIMER_NUMBER-1
// time:时间
// 存在1ms内误差
uint8_t soft_timer_start_once(uint8_t id, uint32_t time)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.mode = TMR_ONCE_MODE; // once模式
    soft_timer[id].val = time;                      // 设置装载值
    soft_timer[id].reload = time;
    soft_timer[id].timer_flag.is_using = 1; // 标记此定时器正在被使用
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    return TMR_OK;
}

/*启用一个circle模式定时器*/
// id:序号,取值：0~SOFT_TIMER_NUMBER-1
// time:时间
// 该函数在第一次循环时有1ms内的误差,后续精准
uint8_t soft_timer_start_circle(uint8_t id, uint32_t time)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.mode = TMR_CIRCLE_MODE; // once模式
    soft_timer[id].val = time;                        // 设置装载值
    soft_timer[id].reload = time;
    soft_timer[id].timer_flag.is_using = 1; // 标记此定时器正在被使用
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    return TMR_OK;
}

/*递减计数值*/
// 放到中断里用的
void soft_timer_dec(void)
{
    uint8_t i;
    for (i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        if (soft_timer[i].timer_flag.is_using && soft_timer[i].val > 0) // 如果这个定时器被启用并且val大于0
        {
            if (--soft_timer[i].val == 0) // 如果减到0了
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
    if (soft_timer[id].timer_flag.time_up == 1) // 如果计数到时了
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

/*微秒延时*/
// 精准
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD; /* LOAD的值 */

    ticks = nus * 480;   /* 需要的节拍数 */
    told = SysTick->VAL; /* 刚进入时的计数器值 */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)                  // 如果在同1个reload周期
                tcnt += told - tnow;          // 本次循环过去的时间
            else                              // 如果不在同1个reload周期
                tcnt += reload - tnow + told; // 本次循环过去的时间
            told = tnow;                      // 更新told的值
            if (tcnt >= ticks)                // 到时间了
                break;                        /* 时间超过/等于要延迟的时间,则退出 */
        }
    }
}

/*毫秒延时*/
// 非常精准
void delay_ms(uint32_t nms)
{
    uint32_t systic_val = SYSTIC_CVR->CURRENT; // 记录systic当前值
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    software_ms_delay_flag = 0;
    software_ms_delay_val = nms;
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    while (1) // 毫秒时间
    {
        if (software_ms_delay_flag == 1)
            break;
    }
    while (SYSTIC_CVR->CURRENT >= systic_val) // 微秒时间
        ;
}
/*查看调试信息*/
void soft_timer_show_info(void)
{
    uint8_t temp = 0;
    uint8_t i;
    printf("\r\n当前宏定义软件定时器个数:%d\r\n", SOFT_TIMER_NUMBER);
    for (i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        if (soft_timer[i].timer_flag.is_using == 1)
        {
            ++temp;
            printf("标号为%d的软件定时器正在使用\r\n", i);
            if (soft_timer[i].timer_flag.mode == 1)
            {
                printf("是循环模式\r\n");
            }
            else
            {
                printf("是单次模式模式\r\n");
            }
            printf("装载值为%d\r\n", soft_timer[i].reload);
            printf("当前值为%d\r\n", soft_timer[i].val);
        }
    }
    printf("当前使用中的软件定时器个数:%d\r\n", temp);
}

/*开始测量时间*/
void soft_timer_start_measure(void)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    software_runtime_flag = 1;                        // 开启runtime功能
    software_runtime_overflow_time = 0;               // 清空溢出次数
    software_runtime_start_val = SYSTIC_CVR->CURRENT; // 记录当前systick值
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
}

/*得到距离参数值过了多少us*/
// 测量运行时间用的
// 单位us,非常精准
uint32_t soft_timer_get_time(void)
{
    uint32_t return_val;
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    software_runtime_flag = 0; // 关闭runtime功能
    uint32_t curren_val = SYSTIC_CVR->CURRENT;
    if (software_runtime_overflow_time == 0) // 如果没溢出
        return_val = (software_runtime_start_val - curren_val) / SOFT_TIMER_RUNTIME_1US;
    else // 如果溢出了
        return_val = (software_runtime_start_val + SYSTIC_RVR->RELOAD - curren_val) / SOFT_TIMER_RUNTIME_1US + (software_runtime_overflow_time - 1) * 1000;
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    return return_val;
}

#if SOFT_TIMER_USE_US == 1
/*设置微秒计时点*/
void hard_timer_set_point(uint32_t nus)
{
    TIM2->CNT = 0;
    is_use_hard_timer=1;
    TIM2->CR1 |= 1; // 开始计时
    hard_timer_point = nus;
}

/*检查是否到时*/
// 1:到时    0:未到时
uint8_t hard_timer_get_result(void)
{
    if ((is_use_hard_timer==1)&&(TIM2->CNT >= hard_timer_point)) // 如果到时
    {
        TIM2->CR1 &= ~1; // 结束计时
        is_use_hard_timer=0;
        return 1;
    }
    else
    {
        return 0; // 如果未到时
    }
}
#endif

void SysTick_Handler(void)
{
    /*delay_ms*/
    if (--software_ms_delay_val == 0)
        software_ms_delay_flag = 1;

    /*soft_timer*/
    soft_timer_dec();

    /*runtime*/
    if (software_runtime_flag == 1)
        software_runtime_overflow_time++;

    /*freeRTOS*/
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) /* OS开始跑了,才执行正常的调度处理 */
    {
        xPortSysTickHandler();
    }

#if USE_BUFFER_KEY == 1
    /*buffer_key_10ms*/
    static uint8_t time_10 = 10;
    if (--time_10 == 0)
    {
        key_detect_10ms();
        time_10 = 10;
    }
#endif
}
