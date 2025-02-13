#include "soft_timer.h"

/*������Ӧ�����������ʱ��*/
timer_t soft_timer[SOFT_TIMER_NUMBER];

/*��ʼ�������ʱ��*/
// ע��˺��������˵δ�ʱ��,�����������˵δ�ʱ���ĺ���������
void soft_timer_init(void)
{
    /*�����ʱ����λ*/
    for (uint8_t i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        soft_timer[i].reload = 0;
        soft_timer[i].val = 0;
        soft_timer[i].timer_flag.is_using = 0;
        soft_timer[i].timer_flag.time_up = 0;
        soft_timer[i].timer_flag.mode = TMR_ONCE_MODE;
    }

    /*����systickʱ��Ϊ1ms*/
    // 2     1: Processor clock.
    // 1     1: Counting down to zero asserts the SysTick exception request
    // 0     1: Counter enabled
    while (SysTick->CTRL != 7)
        SysTick->CTRL = 7;
    SysTick->LOAD = 479999; // 480 000 -1
}

/*����һ��onceģʽ��ʱ��*/
// id:���,ȡֵ��0~SOFT_TIMER_NUMBER-1
// time:ʱ��
uint8_t soft_timer_start_once(uint8_t id, uint32_t time)
{
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.is_using = 1;         // ��Ǵ˶�ʱ�����ڱ�ʹ��
    soft_timer[id].timer_flag.mode = TMR_ONCE_MODE; // onceģʽ
    soft_timer[id].val = time;                      // ����װ��ֵ
    return TMR_OK;
}

/*����һ��circleģʽ��ʱ��*/
// id:���,ȡֵ��0~SOFT_TIMER_NUMBER-1
// time:ʱ��
uint8_t soft_timer_start_circle(uint8_t id, uint32_t time)
{
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.is_using = 1;           // ��Ǵ˶�ʱ�����ڱ�ʹ��
    soft_timer[id].timer_flag.mode = TMR_CIRCLE_MODE; // onceģʽ
    soft_timer[id].val = time;                        // ����װ��ֵ
    soft_timer[id].reload = time;
    return TMR_OK;
}

/*�ݼ�����ֵ*/
// �ŵ��ж����õ�
void soft_timer_dec(void)
{
    for (uint8_t i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        if (soft_timer[i].timer_flag.is_using && soft_timer[i].val > 0) // ��������ʱ�������ò���val����0
        {
            soft_timer[i].val--;
            if (soft_timer[i].val == 0) // �������0��
            {
                soft_timer[i].timer_flag.time_up = 1; // �õ�ʱ��־
                if (soft_timer[i].timer_flag.mode == TMR_CIRCLE_MODE)
                    soft_timer[i].val = soft_timer[i].reload; // �����ѭ��ģʽ����װ
            }
        }
    }
}

/*����Ƿ�ʱ*/
uint8_t soft_timer_check_time_up(uint8_t id)
{
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 0)
        return TMR_IS_NOT_USING_ERR;
    if (soft_timer[id].timer_flag.time_up) // ���������ʱ��
    {
        if (soft_timer[id].timer_flag.mode == TMR_ONCE_MODE)
        {
            soft_timer[id].timer_flag.time_up = 0;  // ��յ�ʱ��־λ
            soft_timer[id].timer_flag.is_using = 0; // ����ʹ��
            return TMR_TIME_UP;
        }
        else if (soft_timer[id].timer_flag.mode == TMR_CIRCLE_MODE)
        {
            soft_timer[id].timer_flag.time_up = 0; // ��յ�ʱ��־λ
            return TMR_TIME_UP;
        }
    }
    else
        return TMR_NOT_TIME_UP;
}
