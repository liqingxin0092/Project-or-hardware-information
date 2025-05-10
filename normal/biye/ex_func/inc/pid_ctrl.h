 /**
 *******************************************************************************
 * @file        pid_ctrl.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-4-7
 * @brief       pid����
 * ���¼�¼:    ��
 * 
 *****************************************************************************************
 */
 
#ifndef __PID_CTRL_H
#define __PID_CTRL_H
 
#include "stdint.h" 

/*pid�������ṹ��*/
//һ��Ҫ�����ø��Եı���
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
    
    float dpart;  //��ȥ��Ȩ��
    
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
}pid_t;

float pid_control(pid_t* pid_struct,float act_val);

#endif



