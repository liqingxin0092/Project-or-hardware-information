#ifndef __FUNC_H
#define __FUNC_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usart.h"
#include "tim.h"
#include "OLED.h"
extern char String[30];
extern float AutoAngle;
extern double angle_AutoSet;
extern float AutoDisatance;
void USART_Printf( UART_HandleTypeDef * chuankou,char *format, ...) ;
void scan_mood(void);
void AutoCarryOut(void);
void set_Angle1_Up(double Angle); 
void set_Angle1_Down(double Angle); 

double Dis_To_Ang1(double dis); //最初
double Dis_To_Ang2(double dis); //点线
double DTA_3(double angle);    //三次

uint16_t InpuD_To_ModiD(uint16_t dis);
double DTA_3(double angle);
//手动充能
#define Manual_charge()  do{ \
						                 	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET );\
                            }while(0)

														
//手动结束充能
#define Stop_charge()     do{ \
						                 	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET ); \
                            }while(0)

														
														
//发射
#define shoot()         do{\
															HAL_Delay(1000);\
															HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET );\
															HAL_Delay(250);\
															HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET );\
													}while(0)

													
													
//自动充能 
#define Auto_charge()   do{\
														HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET );\
														HAL_Delay(Time);\
														HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET );\
													}while(0)	

//1000ms充能 
#define lOOOms_charge()   do{\
														HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET );\
														HAL_Delay(1000);\
														HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET );\
													}while(0)	
													
													
//清除标志位
#define ClearFlag()			do{		flag=0;  }while(0)



//设置上面那个舵机角度
//#define set_Angle1_Up(Angle)  do{\
//                               __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,(int)((Angle)/45*500+1500));\
//																}while(0)

//																
////设置底下那个舵机角度
//#define set_Angle1_Down(Angle)  do{\
//                               __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,(int)((Angle)/45*500+1500));\
//																}while(0)

					
																
																
#endif
