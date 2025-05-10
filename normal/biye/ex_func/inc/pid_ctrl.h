 /**
 *******************************************************************************
 * @file        pid_ctrl.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-4-7
 * @brief       pid控制
 * 更新记录:    无
 * 
 *****************************************************************************************
 */
 
#ifndef __PID_CTRL_H
#define __PID_CTRL_H
 
#include "stdint.h" 

/*pid控制器结构体*/
//一定要各自用各自的变量
typedef struct
{
    float kp;
    float ki;
    float kd;
    float tar_val;
    
    /*积分限幅*/
    float inc_max;
    float inc_min;
    
    /*积分加入的阈值*/
    float inc_use_threshold;

    /*输出限幅*/
    float out_max;
    float out_min;
    
    float dpart;  //过去的权重
    
    /*输出偏移*/
    float out_offset;
    
    /*输出死区*/
    float out_threshold;
    
    /*中间变量*/
    float err;
    float err_sum;
    
    /*微分先行中间变量*/
    float act;
    float act_last;
    
    /*微分滤波中间变量*/
    float dout;
}pid_t;

float pid_control(pid_t* pid_struct,float act_val);

#endif



