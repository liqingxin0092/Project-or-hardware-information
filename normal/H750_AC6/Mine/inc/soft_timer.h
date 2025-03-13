#ifndef __SOFT_TIMER_H
#define __SOFT_TIMER_H

#include "sys.h"

/*************************ע��**********************************/

// ע�������val��reload��systick��reload��val����һ�������
// ��λͳһΪms
// ע��˺��������˵δ�ʱ��,�����������˵δ�ʱ���ĺ���������
// 0�Ŷ�ʱ���̶�����delay
/*****************************************************************/

/**************************�û�����******************************/

/*�����ʱ������*/
#define SOFT_TIMER_NUMBER 4

/*����CPU��Ƶ��systicʱ����Ϊ1ms*/
#define SOFT_TIMER_HZ 479999

/*runtime*/
#define SOFT_TIMER_RUNTIME_1US 480

/*�Ƿ�����΢�붨ʱ��*/
#define SOFT_TIMER_USE_US 1

/*****************************************************************/

/*soft timer��һЩ��־λ*/
// is_using:�Ƿ�����,1:��
// time_up:�Ƿ�ʱ����,1:��
// mode:һ����ģʽorѭ��װ��ģʽ,1:circle,0:once.
typedef struct
{
    volatile uint8_t is_using : 1;
    volatile uint8_t time_up : 1;
    volatile uint8_t mode : 1;
} timer_flag_t;

/*ģʽȡֵ*/
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

/*��Ϣ��*/
typedef enum
{
    TMR_OK = 0,
    TMR_IS_USING_ERR = 1,     // �����ʱ������ʹ�õ���
    TMR_IS_NOT_USING_ERR = 2, // �����ʱ��û�б�����
    TMR_INVALID_ID_ERR = 3,   // id��Ч

    TMR_TIME_UP = 4,
    TMR_NOT_TIME_UP = 5,
} TMR_INFO_CODE;

void soft_timer_init(void); // ��ʼ�������ʱ��
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
uint8_t soft_timer_start_once(uint8_t id, uint32_t time);   // ����һ��onceģʽ��ʱ��
uint8_t soft_timer_start_circle(uint8_t id, uint32_t time); // ����һ��circleģʽ��ʱ��
void soft_timer_dec(void);                                  // �ݼ�����ֵ,��systick�ж�
uint8_t soft_timer_check_time_up(uint8_t id);               // ����Ƿ�ʱ
void soft_timer_show_info(void);                            // �鿴������Ϣ
void soft_timer_start_measure(void);
uint32_t soft_timer_get_time(void);

#if SOFT_TIMER_USE_US == 1
void hard_timer_set_point(uint32_t nus); // Ӳ����ʱ��΢���ʱ
uint8_t hard_timer_get_result(void);     // Ӳ����ʱ��΢����
#endif

#endif
