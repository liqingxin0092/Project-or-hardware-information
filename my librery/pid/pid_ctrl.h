/**
*******************************************************************************
* @file        pid_ctrl.h
* @author      liqingxin0092
* @version     V1.0
* @date        2025-4-7
* @brief       pid����
* ���¼�¼:    5.5   ����������ģ��,�޸���һЩ����
*
*****************************************************************************************
*/

/*����ģ��*/
///*�����pid�ṹ��*/
////ת��:0~100   ---->  CCR:0~100
// pid_t motor_pid_struct=
// {
//   /* kp */
//     .kp= 0,
//   /* ki */
//     .ki=0.00,
//   /* kd */
//     .kd=0.0,
//   /* tar_val */
//     .tar_val= 0,
//   /* ��������ۼ�ֵ����*/
//     .inc_max= 0,
//   /* ��������ۼ�ֵ����*/
//     .inc_min= 0,
//   /* ���ּ������ֵ*/
//     .inc_use_threshold= 0,
//   /* ���ֵ���� */
//     .out_max=999,
//   /* ���ֵ���� */
//     .out_min=-999,
//   /* ΢�����ȥȨ�� */
//     .dpart=0,
//   /* ���ƫ�� */
//     .out_offset= 65,
//   /* �������(�������) */
//     .out_threshold= 0,
// };

#ifndef __PID_CTRL_H
#define __PID_CTRL_H

#include "stdint.h"

/*pid�������ṹ��*/
// һ��Ҫ�����ø��Եı���
typedef struct
{
    float kp;
    float ki;
    float kd;
    float tar_val;

    /*�����޷�*/
    float inc_max;
    float inc_min;

    /*���ּ������ֵ*/
    float inc_use_threshold;

    /*����޷�*/
    float out_max;
    float out_min;

    float dpart; // ��ȥ��Ȩ��

    /*���ƫ��*/
    float out_offset;

    /*�������*/
    float out_threshold;

    /*�м����*/
    float err;
    float err_sum;

    /*΢�������м����*/
    float act;
    float act_last;

    /*΢���˲��м����*/
    float dout;
} pid_t;

float pid_control(pid_t *pid_struct, float act_val);
float pid_control_gunzi(pid_t *pid_struct, float act_val, short gyro);

#endif
