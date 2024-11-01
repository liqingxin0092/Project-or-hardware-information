#ifndef CONTROL_H__
#define CONTROL_H__

#include "mpu6050.h"
#include "inv_mpu.h"
int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int Encoder_left,int Encoder_right);
int Turn(int gyro_Z);

	

#endif

