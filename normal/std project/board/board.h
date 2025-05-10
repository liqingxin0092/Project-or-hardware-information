/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-20     BruceOu      first implementation
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include "gd32f4xx.h"
#include "stdint.h"
#include "stdio.h"
#include "gd32f4xx_libopt.h"
#include "gd32f4xx_exti.h"
#include "usart.h"
#include "spi_flash.h"
#include "bsp_uart.h"
#include "rtc.h"
#include "bsp_adc.h"
#include "soft_timer.h"
#include "buzzer.h"
#include "bsp_led.h"
#include "ad.h"

void board_init(void);
uint32_t get_system_tick(void);
void delay_us(uint32_t _us);
void delay_ms(uint32_t _ms);

#endif

