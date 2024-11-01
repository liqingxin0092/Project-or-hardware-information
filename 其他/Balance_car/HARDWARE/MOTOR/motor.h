#ifndef MOTOR_H__
#define MOTOR_H__

#include "main.h"
#include "tim.h"

#define AIN1_RESET HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,GPIO_PIN_RESET)
#define BIN1_RESET HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,GPIO_PIN_RESET)

#define AIN1_SET HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,GPIO_PIN_SET)
#define BIN1_SET HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,GPIO_PIN_SET)


void Motor_Set (int motor1, int motor2);

#endif

