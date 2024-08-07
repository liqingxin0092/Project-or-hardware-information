#include "kem.h"

void Kalman_Init(Kalman_pm_st* Kalman_pm)
{
/*注意：只需要调节Q矩阵[Q_angle,Q_gyro]和R矩阵[R_angle]即可*/
 
    Kalman_pm->P[0][0] = 1.0f;
    Kalman_pm->P[0][1] = 0.0f;
    Kalman_pm->P[1][0] = 0.0f;
    Kalman_pm->P[1][1] = 1.0f;

    Kalman_pm->dt = 0.01f;      //时间
    
    Kalman_pm->Q_angle = 0.002f;    //原来是 0.001  //本身值的波动大小
    Kalman_pm->Q_gyro = 0.0035f;    //原来是 0.003  //值导数的波动大小
    Kalman_pm->R_angle = 0.6f;      //原来是 0.5     //观测的波动大小
    
    Kalman_pm->q_bias = 0.0f;
    Kalman_pm->angle_f = 0.0f;
}
float Kalman_Filter(Kalman_pm_st *kalman_pm,float ValueForRepair,float D_Value)
{
    float angle_err;  //先验误差
    float angle_;     //先验估计
    float Pdot[2][2]; //先验误差协方差矩阵
 
    float K_0;
    float K_1; //卡尔曼增益
 
    /*赋值*/
    kalman_pm->angle_m= ValueForRepair;   //值
    kalman_pm->wb_m= D_Value;      //值的导数
    /*先验估计*/
    angle_ = kalman_pm->angle_f + (kalman_pm->wb_m - kalman_pm->q_bias) * kalman_pm->dt; //先验估计
    angle_err = kalman_pm->angle_m - angle_;                                             //先验误差
 
    /*先验误差协方差矩阵*/
    Pdot[0][0] = kalman_pm->P[0][0] + kalman_pm->Q_angle - (kalman_pm->P[0][1] + kalman_pm->P[1][0]) * kalman_pm->dt; // Q_angle->Q1
    Pdot[0][1] = kalman_pm->P[0][1] - (kalman_pm->P[1][1]) * kalman_pm->dt;
    Pdot[1][0] = kalman_pm->P[1][0] - (kalman_pm->P[1][1]) * kalman_pm->dt;
    Pdot[1][1] = kalman_pm->P[1][1] + kalman_pm->Q_gyro; // Q_gyro->Q2
 
    /*卡尔曼增益*/
    K_0 = Pdot[0][0] / (Pdot[0][0] + kalman_pm->R_angle);
    K_1 = Pdot[1][0] / (Pdot[0][0] + kalman_pm->R_angle);
 
    /*后验估计*/
    kalman_pm->angle_f = angle_ + K_0 * angle_err;         //最优值
    kalman_pm->q_bias += K_1 * angle_err;                  //最优导数偏差
    kalman_pm->wb_f = kalman_pm->wb_m - kalman_pm->q_bias; //最优导数
 
    /*更新误差协方差矩阵*/
    kalman_pm->P[0][0] = Pdot[0][0] - K_0 * Pdot[0][0];
    kalman_pm->P[0][1] = Pdot[0][1] - K_0 * Pdot[0][1];
    kalman_pm->P[1][0] = Pdot[1][0] - K_1 * Pdot[0][0];
    kalman_pm->P[1][1] = Pdot[1][1] - K_1 * Pdot[0][1];
    
    return kalman_pm->angle_f;
}

