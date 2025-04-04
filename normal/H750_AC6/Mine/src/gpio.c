/**
 *******************************************************************************
 * @file        gpio.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-20
 * @brief       ����,����GPIO
 * ���¼�¼:    2025-3-22 :�����˶�IO������֧��.
 * 
 *****************************************************************************************
 */
#include "gpio.h"
#include "stm32h750xx.h"

/*����һ������*/
//mode:00,����ģʽ
//     01,���ģʽ
//     10,����ģʽ
//     11,ģ��ģʽ
//out_type:0,�������
//         1,��©���
//speed:00,low
//      01,medium
//      10,high
//      11,very high
//up_or_down:00,����
//           01,����
//           10,����
//alterfun:0000~1111 --> AF0~AF15
void gpio_init_pin(gpio_t* GPIOx,uint8_t pin,uint8_t mode,uint8_t out_type,uint8_t speed,uint8_t up_or_down,uint8_t alterfun)
{
    GPIOx->MODER&=~(3<<(pin<<1));//ģʽ��Ӧλ�����λֵ
    GPIOx->MODER|=mode<<(pin<<1);//ģʽ����
    GPIOx->OTYPER|=out_type<<pin;//�����������
    GPIOx->OSPEEDR&=~(3<<(pin<<1));//�ٶȶ�Ӧλ�����λֵ
    GPIOx->OSPEEDR|=speed<<(pin<<1);//�ٶ�����
    GPIOx->PUPDR&=~(3<<(pin<<1));//��������Ӧλ�����λֵ
    GPIOx->PUPDR|=up_or_down<<(pin<<1);//����������
    (0<=pin&&pin<=7)?(GPIOx->AFRL|=alterfun<<(pin<<2)):(GPIOx->AFRH|=alterfun<<((pin-8)<<2));
}
 
/*������������(������Ӧ���ƺ͹��ܼĴ���)*/
//pin_group: msb ~~ lsb , 1111 1111 1111 1111 ,���ĸ�,��Ӧλ��1
void gpio_lock_pin(gpio_t* GPIOx,uint16_t pin_group)
{
    uint32_t key=0x10000|(uint32_t)pin_group;
    GPIOx->LCKR=key;
    GPIOx->LCKR=(uint32_t)pin_group;
    GPIOx->LCKR=key;
    (void)GPIOx->LCKR;
    while(!GPIOx->LCKR);
}

/*io�˿�ʹ��*/
//ע���Ӧѡ���ֽڵ�check
//ע��һ�¹ٷ��ľ���:
//This bit is active only if IO_HSLV user option bit is set.
//It must be used only if the product supply voltage is below 2.7 V.
//Setting this bit when VDD is higher than 2.7 V might be destructive.
static inline void en_io_compensation(void)
{
    if((RCC->CR&0x100)==0)//���CSIû��
    {
        RCC->CR|=0x80;//��CSI
        while((RCC->CR&0x100)==0);//���CSI��û��������ȵ�.
    }
    SYSCFG->CCCSR|=0x10000;//16bit,�ٶ��Ż�
    /*����ѡ��Ĭ�ϵĲ���code*/
    while((SYSCFG->CCCSR&0x100)==0)//���io������Ԫû׼����,������ȵ�.
    SYSCFG->CCCSR|=1;//en io ������Ԫ.      
}
    
/*Ӧ�ò�*/
//use_io_comp: 1,ʹ��io������Ԫ. 0,��ʹ��io������Ԫ. 
void RCC_GPIO_Init(uint8_t use_io_comp)
{
    RCC->AHB4ENR|=0x1f;//��GPIOABCDEʱ��.   
    while((RCC->AHB4ENR&0x1f)!=0x1f); //����ļ�����
    
    if(use_io_comp==1)        //����io������Ԫ
        en_io_compensation();
    gpio_init_pin(GPIO_C,13,1,0,2,0,0);//gpioc 13 �������,���Ե�
}



