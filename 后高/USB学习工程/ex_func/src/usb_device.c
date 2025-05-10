/**
 *******************************************************************************
 * @file        usb_device.c
 * @author      liqingxin0092
 * @version     V1.1
 * @date        2025-5-10
 * @brief       usb设备库
 * 更新记录:    无
 *****************************************************************************************
 */
 
#include "usb_device.h"
#include "stm32f10x.h"
#include "delay.h"

void usb_init(void )
{
    RCC->APB1ENR |=0x800000;   //开USB寄存器部分时钟
    
    RCC->APB1RSTR|=0x800000;   //USB设备复位
    delay_ms(1);
    RCC->APB1RSTR&=~0x800000;   //USB停止设备复位
    
    delay_us(1);
    USB->CNTR&=~1; //FRES  : USB停止复位  , 默认中断是不开的
    
    USB->ISTR=0;   //清reset标志
    
    USB->DADDR|=0x80;  //usb模块使能
    /*这里地址表默认为0*/
    
    USB->EP0R |=0x200; //设置端点0为控制端点
}                          









