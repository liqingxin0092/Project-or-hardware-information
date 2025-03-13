#include "soft_timer.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "h7_core.h"

/*΢�뼶Ӳ����ʱ��*/
static TIM_HandleTypeDef htim2;
uint32_t hard_timer_point;
uint8_t is_use_hard_timer;

/*freeRTOSʱ��*/
extern void xPortSysTickHandler(void);

/*������Ӧ�����������ʱ��*/
static volatile timer_t soft_timer[SOFT_TIMER_NUMBER];

/*����MS��ʱ*/
static volatile uint8_t software_ms_delay_flag;
static volatile uint32_t software_ms_delay_val;

/*��������ʱ��*/
volatile uint8_t software_runtime_flag;
volatile uint32_t software_runtime_overflow_time;
volatile uint32_t software_runtime_start_val;

/*��ʼ�������ʱ��*/
void soft_timer_init(void)
{
    uint8_t i;
    /*�����ʱ����λ*/
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
    /*΢�뼶��ʱ����ʼ��*/
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 240 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP; // ���ϼ�����
    htim2.Init.Period = 0xffffffff;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&htim2);

#endif
}

/*����һ��onceģʽ��ʱ��*/
// id:���,ȡֵ��0~SOFT_TIMER_NUMBER-1
// time:ʱ��
// ����1ms�����
uint8_t soft_timer_start_once(uint8_t id, uint32_t time)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.mode = TMR_ONCE_MODE; // onceģʽ
    soft_timer[id].val = time;                      // ����װ��ֵ
    soft_timer[id].reload = time;
    soft_timer[id].timer_flag.is_using = 1; // ��Ǵ˶�ʱ�����ڱ�ʹ��
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    return TMR_OK;
}

/*����һ��circleģʽ��ʱ��*/
// id:���,ȡֵ��0~SOFT_TIMER_NUMBER-1
// time:ʱ��
// �ú����ڵ�һ��ѭ��ʱ��1ms�ڵ����,������׼
uint8_t soft_timer_start_circle(uint8_t id, uint32_t time)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    if (id >= SOFT_TIMER_NUMBER)
        return TMR_INVALID_ID_ERR;
    if (soft_timer[id].timer_flag.is_using == 1)
        return TMR_IS_USING_ERR;
    soft_timer[id].timer_flag.mode = TMR_CIRCLE_MODE; // onceģʽ
    soft_timer[id].val = time;                        // ����װ��ֵ
    soft_timer[id].reload = time;
    soft_timer[id].timer_flag.is_using = 1; // ��Ǵ˶�ʱ�����ڱ�ʹ��
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    return TMR_OK;
}

/*�ݼ�����ֵ*/
// �ŵ��ж����õ�
void soft_timer_dec(void)
{
    uint8_t i;
    for (i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        if (soft_timer[i].timer_flag.is_using && soft_timer[i].val > 0) // ��������ʱ�������ò���val����0
        {
            if (--soft_timer[i].val == 0) // �������0��
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
    if (soft_timer[id].timer_flag.time_up == 1) // ���������ʱ��
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

/*΢����ʱ*/
// ��׼
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD; /* LOAD��ֵ */

    ticks = nus * 480;   /* ��Ҫ�Ľ����� */
    told = SysTick->VAL; /* �ս���ʱ�ļ�����ֵ */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)                  // �����ͬ1��reload����
                tcnt += told - tnow;          // ����ѭ����ȥ��ʱ��
            else                              // �������ͬ1��reload����
                tcnt += reload - tnow + told; // ����ѭ����ȥ��ʱ��
            told = tnow;                      // ����told��ֵ
            if (tcnt >= ticks)                // ��ʱ����
                break;                        /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
        }
    }
}

/*������ʱ*/
// �ǳ���׼
void delay_ms(uint32_t nms)
{
    uint32_t systic_val = SYSTIC_CVR->CURRENT; // ��¼systic��ǰֵ
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    software_ms_delay_flag = 0;
    software_ms_delay_val = nms;
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    while (1) // ����ʱ��
    {
        if (software_ms_delay_flag == 1)
            break;
    }
    while (SYSTIC_CVR->CURRENT >= systic_val) // ΢��ʱ��
        ;
}
/*�鿴������Ϣ*/
void soft_timer_show_info(void)
{
    uint8_t temp = 0;
    uint8_t i;
    printf("\r\n��ǰ�궨�������ʱ������:%d\r\n", SOFT_TIMER_NUMBER);
    for (i = 0; i < SOFT_TIMER_NUMBER; i++)
    {
        if (soft_timer[i].timer_flag.is_using == 1)
        {
            ++temp;
            printf("���Ϊ%d�������ʱ������ʹ��\r\n", i);
            if (soft_timer[i].timer_flag.mode == 1)
            {
                printf("��ѭ��ģʽ\r\n");
            }
            else
            {
                printf("�ǵ���ģʽģʽ\r\n");
            }
            printf("װ��ֵΪ%d\r\n", soft_timer[i].reload);
            printf("��ǰֵΪ%d\r\n", soft_timer[i].val);
        }
    }
    printf("��ǰʹ���е������ʱ������:%d\r\n", temp);
}

/*��ʼ����ʱ��*/
void soft_timer_start_measure(void)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    software_runtime_flag = 1;                        // ����runtime����
    software_runtime_overflow_time = 0;               // ����������
    software_runtime_start_val = SYSTIC_CVR->CURRENT; // ��¼��ǰsystickֵ
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
}

/*�õ��������ֵ���˶���us*/
// ��������ʱ���õ�
// ��λus,�ǳ���׼
uint32_t soft_timer_get_time(void)
{
    uint32_t return_val;
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    software_runtime_flag = 0; // �ر�runtime����
    uint32_t curren_val = SYSTIC_CVR->CURRENT;
    if (software_runtime_overflow_time == 0) // ���û���
        return_val = (software_runtime_start_val - curren_val) / SOFT_TIMER_RUNTIME_1US;
    else // ��������
        return_val = (software_runtime_start_val + SYSTIC_RVR->RELOAD - curren_val) / SOFT_TIMER_RUNTIME_1US + (software_runtime_overflow_time - 1) * 1000;
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
    return return_val;
}

#if SOFT_TIMER_USE_US == 1
/*����΢���ʱ��*/
void hard_timer_set_point(uint32_t nus)
{
    TIM2->CNT = 0;
    is_use_hard_timer=1;
    TIM2->CR1 |= 1; // ��ʼ��ʱ
    hard_timer_point = nus;
}

/*����Ƿ�ʱ*/
// 1:��ʱ    0:δ��ʱ
uint8_t hard_timer_get_result(void)
{
    if ((is_use_hard_timer==1)&&(TIM2->CNT >= hard_timer_point)) // �����ʱ
    {
        TIM2->CR1 &= ~1; // ������ʱ
        is_use_hard_timer=0;
        return 1;
    }
    else
    {
        return 0; // ���δ��ʱ
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
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) /* OS��ʼ����,��ִ�������ĵ��ȴ��� */
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
