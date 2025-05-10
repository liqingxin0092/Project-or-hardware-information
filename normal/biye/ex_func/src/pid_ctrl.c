/**
 *******************************************************************************
 * @file        pid_ctrl.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-4-7
 * @brief       pid����
 * ���¼�¼:    ��
 * 
 *****************************************************************************************
 */
 
#include "pid_ctrl.h"
#include "math.h"

/*pid ԭ��*/
//�������ṩһ������,���Լ���ϵͳ��Ӧ.
//ֻ�б�����,�����Ϊ0ʱ,���������Ϊ0,����������.
//����Ȼ�����µ����Ǹ���(Ħ����,�¶���ɢ)�͵�ǰ����Ӧ�����������ʱ,���γ�����̬���.

//�������һ��ȱ����ϵͳ���ͺ���,���Ŀ��ֵǰ�����ܴ�,Ҫ����ʱ��������ĺ�.

//΢����,�������ڲ��ϼ�С,�������ȥ�ϴ������һ����ֵ,���΢������һ������.
//΢�����ܻ��pi���Ÿ�,ǰ��������Խ��,����Խǿ.

//���ٻ��ֺͻ��ַ����ǲ��е�

//΢������:ɾ�������ֵͻ��ʱ�ĺܴ��������.
//΢������˲�:΢���������յ�������Ӱ��.

/*pid������*/
//λ��ʽpid
//�����޷�,����޷�,���ַ���,΢���˲����ܲ��ø�0
//act_val:ʵ��ֵ
//����ֵ:�����������������
float pid_control(pid_t* pid_struct,float act_val)
{
    float out_val=0;
    pid_struct->err         =   pid_struct->tar_val- act_val;
    
    /*��¼��һ��ʵ��ֵ�����ʵ��ֵ*/
    pid_struct->act_last    =   pid_struct->act;
    pid_struct->act         =   act_val;
    
    /*�������*/
    if(fabs(pid_struct->err)<pid_struct->out_threshold)
    {
        
        out_val=0;
        goto exit;
    }
    
    /*�������(���ַ���)*/
    if(pid_struct->inc_use_threshold!=0)
    {
        if(fabs(pid_struct->err) <= pid_struct->inc_use_threshold)
        {
            pid_struct->err_sum  +=   pid_struct->err;
        }
        else
        {
            pid_struct->err_sum   = 0;
        }
    }
    
    /*�����޷�*/
    if(pid_struct->inc_max!=0)
    {
         if(pid_struct->err_sum>pid_struct->inc_max)
            pid_struct->err_sum= pid_struct->inc_max;
    }
    if(pid_struct->inc_min!=0)
    {
         if(pid_struct->err_sum<pid_struct->inc_min)
            pid_struct->err_sum= pid_struct->inc_min;
    }
    
    /*΢������˲�*/
    pid_struct->dout = ( 1 - pid_struct->dpart) * -pid_struct->kd * (pid_struct->act - pid_struct->act_last) + pid_struct->dpart * pid_struct->dout;
    
    /*���ֵ����*/
    out_val =   pid_struct->kp *   pid_struct->err
            +   pid_struct->ki *   pid_struct->err_sum
            +   pid_struct->dout;
    
    /*����޷�*/
    if(pid_struct->out_max!=0)
    {
         if(out_val>pid_struct->out_max)
            out_val= pid_struct->out_max;
    }
    if(pid_struct->out_min!=0)
    {
         if(out_val<pid_struct->out_min)
            out_val= pid_struct->out_min;
    }
    
    /*���ƫ��*/
    if(out_val>0)
        out_val+=pid_struct->out_offset;
    else if(out_val<0)
        out_val-=pid_struct->out_offset;
    
    exit:    
    return  out_val;
}
    





