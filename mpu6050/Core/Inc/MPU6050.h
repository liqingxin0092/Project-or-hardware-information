#ifndef __MPU6050_H
#define __MPU6050_H

#include "main.h"
#include "i2c.h"

#define MPU6050_ADDRESS 0xd0
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75   //������Ĭ��0x68����0x98

typedef struct
{
    double AccX;
    double AccY;
    double AccZ;
    double GyroX;
    double GyroY;
    double GyroZ;
    double  Temperature;
}SixAxisData;
typedef struct
{
    float roll;
    float pitch;
    float yaw;
}Angle;
extern uint8_t Test;
void TestMPU6050(void);
void HardWare_TestMPU6050(void);
void MPU6050Init(void);
void GetData(SixAxisData * data);
void GetAngle(Angle * angle,SixAxisData * data);
#endif