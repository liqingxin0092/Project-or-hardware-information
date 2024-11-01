#ifndef MOTOR_H__
#define MOTOR_H__

#include "main.h"
#include "pid.h"
#include "cmsis_os.h"

typedef struct
{
    float speed;
    pid_t pid_struct;
    int CCR;
    void (*SetCCR)(int);
    void (*SetTar)(float);
} wheel_t;

void MIX_PID_Init(void);
void Wheels_Init(void);

void _1(int CCR);
void _2(int CCR);
void _3(int CCR);
void _4(int CCR);

void __1(float target);
void __2(float target);
void __3(float target);
void __4(float target);

void SetCCR_By_TargetSpeed(char mode[]);

void KeyJumpTo(uint8_t value, uint8_t flag_to_jump, osStatus (*delay)(uint32_t));
void Auto__DisToSpeed(float distance,uint8_t flag_to_jump);
void Const_DisToSpeed(float distance, float speed2, uint8_t flag_to_jump); //  one round 19.5cm
void rotate(float angle, uint8_t flag_to_jump);
void Move_By_Distance_And_Angle(float distance, float angle, float speed, uint8_t flag_to_jump);
#endif
