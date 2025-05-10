/**
 *******************************************************************************
 * @file        usb_device.c
 * @author      liqingxin0092
 * @version     V1.1
 * @date        2025-5-10
 * @brief       usb�豸��
 * ���¼�¼:    ��
 *****************************************************************************************
 */
 
#include "usb_device.h"
#include "stm32f10x.h"
#include "delay.h"

void usb_init(void )
{
    RCC->APB1ENR |=0x800000;   //��USB�Ĵ�������ʱ��
    
    RCC->APB1RSTR|=0x800000;   //USB�豸��λ
    delay_ms(1);
    RCC->APB1RSTR&=~0x800000;   //USBֹͣ�豸��λ
    
    delay_us(1);
    USB->CNTR&=~1; //FRES  : USBֹͣ��λ  , Ĭ���ж��ǲ�����
    
    USB->ISTR=0;   //��reset��־
    
    USB->DADDR|=0x80;  //usbģ��ʹ��
    /*�����ַ��Ĭ��Ϊ0*/
    
    USB->EP0R |=0x200; //���ö˵�0Ϊ���ƶ˵�
}                          









