/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "pid.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define XJ_1_Pin GPIO_PIN_2
#define XJ_1_GPIO_Port GPIOA
#define XJ_2_Pin GPIO_PIN_3
#define XJ_2_GPIO_Port GPIOA
#define XJ_3_Pin GPIO_PIN_6
#define XJ_3_GPIO_Port GPIOA
#define XJ_4_Pin GPIO_PIN_7
#define XJ_4_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_11
#define OLED_DC_GPIO_Port GPIOD
#define OLED_RST_Pin GPIO_PIN_12
#define OLED_RST_GPIO_Port GPIOD
#define OLED_SDA_Pin GPIO_PIN_13
#define OLED_SDA_GPIO_Port GPIOD
#define OLED_SCL_Pin GPIO_PIN_14
#define OLED_SCL_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define MOTOR_B_TIMER htim9
#define MOTOR_B_CHANNEL_1 TIM_CHANNEL_1
#define MOTOR_B_CHANNEL_2 TIM_CHANNEL_2

#define MOTOR_C_TIMER htim1
#define MOTOR_C_CHANNEL_1 TIM_CHANNEL_1
#define MOTOR_C_CHANNEL_2 TIM_CHANNEL_2

#define SENSOR htim12
#define SENSOR_CAHNNEL_1 TIM_CHANNEL_1

  extern uint8_t RX_dat;
  extern uint8_t TimerCount;
  extern int32_t encoder_count_motor_a;
  extern int32_t encoder_count_motor_b;
  extern int32_t encoder_count_motor_c;
  extern int32_t encoder_count_motor_d;
  extern uint8_t Direction;
  extern float Speed_a;
  extern float Speed_b;
  extern float Speed_c;
  extern float Speed_d;
  extern __IO float output_motor_a;
  extern __IO float output_motor_b;
  extern __IO float output_motor_c;
  extern __IO float output_motor_d;
  extern float Pitch, Roll, Yaw;
  extern short aacx, aacy, aacz;
  extern short gyrox, gyroy, gyroz;
  extern uint8_t FLAG;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
