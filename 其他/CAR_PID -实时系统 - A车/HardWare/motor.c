#include "motor.h"
#include "pid.h"
#include "cmsis_os.h"
#include <math.h>
#include <string.h>

wheel_t LeftUp;
wheel_t LeftDown;
wheel_t RightUp;
wheel_t RightDown;

float offset;                    // Angular offset compensates for velocity
extern double ActDistance;       // Current travel distance
extern uint8_t ActionFlag;       // Planning action
extern float tmpZ;               // direction
uint8_t Need_To_Record_Distance; // Whether to record the distance

//Pos:{"p":75,"i":0.6,"d":0.0,"a":0}
//Inc:{"p":0,"i":0.0,"d":0.0,"a":0}
#define kp 75.0f
#define ki 0.6f
#define kd 0.0f
#define t 1.0f


pid_t Direction_Pid;
pid_t DTS_Pid;
void MIX_PID_Init()
{
    //{"p":0.0123,"i":0.0,"d":0.000005}
    Direction_Pid.p = 0.012;
    Direction_Pid.i = 0.000;
    Direction_Pid.d = 0.000005;

    DTS_Pid.p = 0.0;
    DTS_Pid.i = 0.0;
    DTS_Pid.d = 0.0;
}

void Wheels_Init()
{
    LeftUp.SetCCR = _1;
    LeftDown.SetCCR = _2;
    RightUp.SetCCR = _3;
    RightDown.SetCCR = _4;

    RightDown.SetTar = __1;
    RightUp.SetTar = __2;
    LeftDown.SetTar = __3;
    LeftUp.SetTar = __4;

    RightDown.pid_struct.p = RightUp.pid_struct.p = LeftDown.pid_struct.p = LeftUp.pid_struct.p = kp;
    RightDown.pid_struct.i = RightUp.pid_struct.i = LeftDown.pid_struct.i = LeftUp.pid_struct.i = ki;
    RightDown.pid_struct.d = RightUp.pid_struct.d = LeftDown.pid_struct.d = LeftUp.pid_struct.d = kd;
    RightDown.pid_struct.tar = RightUp.pid_struct.tar = LeftDown.pid_struct.tar = LeftUp.pid_struct.tar = t;
}

void _1(int CCR)
{
    if (CCR > 0)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, CCR);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -CCR);
    }
}
void _2(int CCR)
{
    if (CCR > 0)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, CCR);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, -CCR);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
    }
}
void _3(int CCR)
{
    if (CCR > 0)
    {
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, CCR);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, -CCR);
        __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, 0);
    }
}
void _4(int CCR)
{
    if (CCR > 0)
    {
        __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, CCR);
        __HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, 0);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, -CCR);
    }
}

void __1(float target)
{
    RightDown.pid_struct.tar = target;
}
void __2(float target)
{
    RightUp.pid_struct.tar = target;
}
void __3(float target)
{
    LeftDown.pid_struct.tar = target;
}
void __4(float target)
{
    LeftUp.pid_struct.tar = target;
}

void SetCCR_By_TargetSpeed(char mode[])
{
    if(strcmp(mode,"Inc")==0)
    {
        LeftDown.CCR  += (int)IncPID_Trans(&LeftDown.pid_struct, LeftDown.speed);
        RightDown.CCR += (int)IncPID_Trans(&RightDown.pid_struct, RightDown.speed);
        RightUp.CCR   += (int)IncPID_Trans(&RightUp.pid_struct, RightUp.speed);
        LeftUp.CCR    += (int)IncPID_Trans(&LeftUp.pid_struct, LeftUp.speed);
        LeftDown.SetCCR(LeftDown.CCR);
        RightDown.SetCCR(RightDown.CCR);
        RightUp.SetCCR(RightUp.CCR);
        LeftUp.SetCCR(LeftUp.CCR);
    }
    else if(strcmp(mode,"Pos")==0)
    {
        LeftDown.SetCCR((int)PosPID_Trans(&LeftDown.pid_struct, LeftDown.speed));
        RightDown.SetCCR((int)PosPID_Trans(&RightDown.pid_struct, RightDown.speed));
        RightUp.SetCCR((int)PosPID_Trans(&RightUp.pid_struct, RightUp.speed));
        LeftUp.SetCCR((int)PosPID_Trans(&LeftUp.pid_struct, LeftUp.speed));
    }
}
void Auto__DisToSpeed(float distance,uint8_t flag_to_jump)
{
    DTS_Pid.tar = distance;
    Need_To_Record_Distance = 1;

    if (fabs(distance) - fabs(ActDistance) > 0.4)
    {
        RightDown.SetTar(PosPID_Trans(&DTS_Pid, ActDistance));
        LeftUp.SetTar(PosPID_Trans(&DTS_Pid, ActDistance));
        LeftDown.SetTar(PosPID_Trans(&DTS_Pid, ActDistance));
        RightUp.SetTar(PosPID_Trans(&DTS_Pid, ActDistance));
    }
    else
    {
        ActionFlag = flag_to_jump; // Skip to next
        Need_To_Record_Distance = 0;
        ActDistance = 0;

        RightDown.SetTar(0);
        LeftUp.SetTar(0);
        LeftDown.SetTar(0);
        RightUp.SetTar(0);
    }
}
void Const_DisToSpeed(float distance, float speed2, uint8_t flag_to_jump)
{
    Need_To_Record_Distance = 1;
    if (fabs(distance) >= fabs(ActDistance))
    {
        RightDown.SetTar(speed2);
        LeftUp.SetTar(speed2);
        LeftDown.SetTar(speed2);
        RightUp.SetTar(speed2);
    }
    else
    {
        ActionFlag = flag_to_jump; // Skip to next
        Need_To_Record_Distance = 0;
        ActDistance = 0;

        RightDown.SetTar(0);
        LeftUp.SetTar(0);
        LeftDown.SetTar(0);
        RightUp.SetTar(0);
    }
}
void rotate(float angle, uint8_t flag_to_jump)
{
    Direction_Pid.tar = angle; // Set the target value to be rotated

    if (fabs(Direction_Pid.tar - tmpZ) > 1.0)
    {
        offset = PosPID_Trans(&Direction_Pid, tmpZ);
        LeftDown.SetTar(-offset);
        LeftUp.SetTar(-offset);
        RightDown.SetTar(+offset);
        RightUp.SetTar(+offset);
    }
    else
    {
        ActionFlag = flag_to_jump; // Skip to next
        LeftDown.SetTar(0);
        LeftUp.SetTar(0);
        RightDown.SetTar(0);
        RightUp.SetTar(0);
    }
}

void KeyJumpTo(uint8_t value, uint8_t flag_to_jump, osStatus (*delay)(uint32_t))
{
    if (((GPIOC->IDR >> 9) & 1) == value)
    {
        delay(20);
        if (((GPIOC->IDR >> 9) & 1) == value)
        {
            while (((GPIOC->IDR >> 9) & 1) == value)
                ;
            ActionFlag = flag_to_jump; // Skip to next
            delay(20);
        }
    }
}
void Move_By_Distance_And_Angle(float distance, float angle, float speed, uint8_t flag_to_jump)
{
    Direction_Pid.tar = angle; // Set the target value to be rotated
    Need_To_Record_Distance = 1;
    if (fabs(distance) >= fabs(ActDistance))
    {
        offset = PosPID_Trans(&Direction_Pid, tmpZ);
        LeftDown.SetTar(speed - 2 * offset);
        LeftUp.SetTar(speed - 2 * offset);
        RightDown.SetTar(speed + 2 * offset);
        RightUp.SetTar(speed + 2 * offset);
    }
    else
    {
        ActionFlag = flag_to_jump; // Skip to next
        Need_To_Record_Distance = 0;
        ActDistance = 0;

        RightDown.SetTar(0);
        LeftUp.SetTar(0);
        LeftDown.SetTar(0);
        RightUp.SetTar(0);
    }
}
