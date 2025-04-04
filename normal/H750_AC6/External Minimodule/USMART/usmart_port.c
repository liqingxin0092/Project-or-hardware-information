#include "usmart.h"
#include "usmart_port.h"
#include "usart_m.h"
#include "stdio.h"
#include "h7_core.h"

uint8_t usmart_rcv_pack;
static TIM_HandleTypeDef g_timx_usmart_handle;      /* ��ʱ����� */

#define BUFFER_MAX 100
uint8_t usamart_buffer[BUFFER_MAX];

/**
 * @brief       ��ȡ����������(�ַ���)
 *   @note      USMARTͨ�������ú������ص��ַ����Ի�ȡ����������������Ϣ
 * @param       ��
 * @retval
 *   @arg       0,  û�н��յ�����
 *   @arg       ����,�������׵�ַ(������0)
 */
char *usmart_get_input_string(void)
{
    char *pbuf = 0;
    uint32_t i;
    if(usmart_rcv_pack==1)//���յ��˽�����'\n'
    {
        for(i=0;i<BUFFER_MAX;i++)
        {
            usart_receive_char(&USART_APP, usamart_buffer+i);
            if(usamart_buffer[i]=='\r')
            {
               usamart_buffer[i]='\0'; 
               return (char*)usamart_buffer;
            }
        } 
    }
    else
        return 0;  //û�յ�����0
}

/* ���ʹ���˶�ʱ��ɨ��, ����Ҫ�������º��� */
#if USMART_ENTIMX_SCAN == 1

/**
 * ��ֲע��:��������stm32Ϊ��,���Ҫ��ֲ������mcu,������Ӧ�޸�.
 * usmart_reset_runtime,�����������ʱ��,��ͬ��ʱ���ļ����Ĵ����Լ���־λһ������.��������װ��ֵΪ���,������޶ȵ��ӳ���ʱʱ��.
 * usmart_get_runtime,��ȡ��������ʱ��,ͨ����ȡCNTֵ��ȡ,����usmart��ͨ���жϵ��õĺ���,���Զ�ʱ���жϲ�����Ч,��ʱ����޶�
 * ֻ��ͳ��2��CNT��ֵ,Ҳ���������+���һ��,���������2��,û������,���������ʱ,������:2*������CNT*0.1ms.��STM32��˵,��:13.1s����
 * ������:USMART_TIMX_IRQHandler��Timer4_Init,��Ҫ����MCU�ص������޸�.ȷ������������Ƶ��Ϊ:10Khz����.����,��ʱ����Ҫ�����Զ���װ�ع���!!
 */

/**
 * @brief       ��λruntime
 *   @note      ��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
 * @param       ��
 * @retval      ��
 */
void usmart_timx_reset_time(void)
{
    __HAL_TIM_CLEAR_FLAG(&g_timx_usmart_handle, TIM_FLAG_UPDATE); /* ����жϱ�־λ */
    __HAL_TIM_SET_AUTORELOAD(&g_timx_usmart_handle, 0XFFFF);      /* ����װ��ֵ���õ���� */
    __HAL_TIM_SET_COUNTER(&g_timx_usmart_handle, 0);              /* ��ն�ʱ����CNT */
    usmart_dev.runtime = 0;
}

/**
 * @brief       ���runtimeʱ��
 *   @note      ��Ҫ��������ֲ����MCU�Ķ�ʱ�����������޸�
 * @param       ��
 * @retval      ִ��ʱ��,��λ:0.1ms,�����ʱʱ��Ϊ��ʱ��CNTֵ��2��*0.1ms
 */
uint32_t usmart_timx_get_time(void)
{
    if (__HAL_TIM_GET_FLAG(&g_timx_usmart_handle, TIM_FLAG_UPDATE) == SET)  /* �������ڼ�,�����˶�ʱ����� */
    {
        usmart_dev.runtime += 0XFFFF;
    }
    usmart_dev.runtime += __HAL_TIM_GET_COUNTER(&g_timx_usmart_handle);
    return usmart_dev.runtime;                                 /* ���ؼ���ֵ */
}

/**
 * @brief       ��ʱ����ʼ������
 * @param       arr:�Զ���װ��ֵ
 *              psc:��ʱ����Ƶϵ��
 * @retval      ��
 */ 
void usmart_timx_init(uint16_t arr, uint16_t psc)
{
    USMART_TIMX_CLK_ENABLE();
    g_timx_usmart_handle.Instance = USMART_TIMX;                 /* ͨ�ö�ʱ��4 */
    g_timx_usmart_handle.Init.Prescaler = psc;                   /* ��Ƶϵ�� */
    g_timx_usmart_handle.Init.CounterMode = TIM_COUNTERMODE_UP;  /* ���ϼ����� */
    g_timx_usmart_handle.Init.Period = arr;                      /* �Զ�װ��ֵ */
    g_timx_usmart_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&g_timx_usmart_handle);
    HAL_TIM_Base_Start_IT(&g_timx_usmart_handle);                /* ʹ�ܶ�ʱ���Ͷ�ʱ���ж� */
    __set_priority(USMART_TIMX_IRQn,4,0);                  /* �����ж����ȼ�����ռ���ȼ�3�������ȼ�0 */
    __enable_IRQ(USMART_TIMX_IRQn);                        /* ����ITM�ж� */ 
}

/**
 * @brief       USMART��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void USMART_TIMX_IRQHandler(void)
{
    if(__HAL_TIM_GET_IT_SOURCE(&g_timx_usmart_handle,TIM_IT_UPDATE)==SET)/* ����ж� */
    {
        usmart_dev.scan();                                   /* ִ��usmartɨ�� */
        __HAL_TIM_SET_COUNTER(&g_timx_usmart_handle, 0);    /* ��ն�ʱ����CNT */
        __HAL_TIM_SET_AUTORELOAD(&g_timx_usmart_handle, 100);/* �ָ�ԭ�������� */
    }
    
    __HAL_TIM_CLEAR_IT(&g_timx_usmart_handle, TIM_IT_UPDATE);/* ����жϱ�־λ */
}

#endif
















