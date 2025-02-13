#include "btim.h"
#include "FreeRTOS.h"
#include "lvgl.h"

TIM_HandleTypeDef g_tim3_handle;        /* 定时器3句柄 */

uint32_t FreeRTOSRunTimeTicks;          /* FreeRTOS时间统计所用的节拍计数器 */

void ConfigureTimeForRunTimeStats(void)
{
    FreeRTOSRunTimeTicks = 0;           /* 节拍计数器初始化为0 */
    btim_tim3_int_init(10-1, 900-1);     // FreeRTOS函数
}

//定时器给freeRTOS函数的配置:btim_tim3_int_init(10-1, 900-1);

//给LVGL的配置: btim_tim3_int_init(1000-1, 90-1);
/**
 * @brief       基本定时器TIM3中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIM3_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim3_handle);  /*定时器回调函数*/
}
/**
 * @brief       回调函数，定时器中断服务函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_tim3_handle))
    {
//        lv_tick_inc(1);
//        FreeRTOSRunTimeTicks++; //FreeRTOS函数
        
    }
}

/**
 * @brief       基本定时器TIM3定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    g_tim3_handle.Instance = BTIM_TIM3_INT;                      /*通用定时器3*/
    g_tim3_handle.Init.Prescaler = psc;                          /* 设置预分频器  */
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    g_tim3_handle.Init.Period = arr;                             /*自动装载值*/
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /*时钟分频因子*/
    HAL_TIM_Base_Init(&g_tim3_handle);

    HAL_TIM_Base_Start_IT(&g_tim3_handle);                       /*使能通用定时器3和及其更新中断：TIM_IT_UPDATE*/
}

/**
 * @brief       定时器底册驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIM3_INT)
    {
        BTIM_TIM3_INT_CLK_ENABLE();                     /*使能TIM时钟*/
        HAL_NVIC_SetPriority(BTIM_TIM3_INT_IRQn, 4, 0); /* 抢占4，子优先级0，组4 */
        HAL_NVIC_EnableIRQ(BTIM_TIM3_INT_IRQn);         /*开启ITM3中断*/
    }
}
