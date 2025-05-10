/**
 *******************************************************************************
 * @file        ad.c
 * @author      liqingxin0092
 * @version     V1.1
 * @date        2025-3-29
 * @brief       ��ϸ����adת��
 * ���¼�¼:    3.30 ������Ӳ����������ؿ���
 *              4.1  ������dma����ɨ���·ADC����
 *****************************************************************************************
 */
#include "ad.h"
#include "stdint.h"
#include "gd32f4xx_adc.h"
#include "gd32f4xx_gpio.h"
#include <board.h>
#include "gd32f4xx_dma.h"

//ad������������:
//Ӳ����������������������Ľ��,���ʶ���ȡƽ��.
//��������������ʹ���ź�Խ��Խƽ��,���ǳ����ʻή��.

/*ͨ����IO��ӳ���*/
const static chx_io_t chx_to_io_table[16] = {
    {GPIOA, GPIO_PIN_0},
    {GPIOA, GPIO_PIN_1},
    {GPIOA, GPIO_PIN_2},
    {GPIOA, GPIO_PIN_3},
    {GPIOA, GPIO_PIN_4},
    {GPIOA, GPIO_PIN_5},
    {GPIOA, GPIO_PIN_6},
    {GPIOA, GPIO_PIN_7},
    {GPIOB, GPIO_PIN_0},
    {GPIOB, GPIO_PIN_1},
    {GPIOC, GPIO_PIN_0},
    {GPIOC, GPIO_PIN_1},
    {GPIOC, GPIO_PIN_2},
    {GPIOC, GPIO_PIN_3},
    {GPIOC, GPIO_PIN_4},
    {GPIOC, GPIO_PIN_5},
};

/*У׼*/
// periph_num  :ADC0,ADC1,ADC2
static void ad_calibration(uint32_t periph_num)
{
    if ((ADC_CTL1(periph_num) & 1) != 1) // ADCû���������￨��.
        while (1)
            ;
    ADC_CTL1(periph_num) |= 0x8; // ��λУ׼
    delay_us(10);
    ADC_CTL1(periph_num) &= ~0x8; // ������λУ׼
    ADC_CTL1(periph_num) |= 0x4;  // ��ʼУ׼
    while (ADC_CTL1(periph_num) & 0x4)
        ; // ���ûУ׼��������ȵ�.
}

/*io��ʼ��*/
// chx:      ͨ�����,0~15
static void ad_io_init(uint8_t chx)
{
    uint8_t GPIOx_num = 0;
    GPIOx_num = (GPIO_PORT_chx(chx) - GPIOA) >> 10;
    if ((RCU_AHB1EN & (1 << GPIOx_num)) == 0) // ʱ���ſ�
        RCU_AHB1EN |= 1 << GPIOx_num;
    gpio_mode_set(GPIO_PORT_chx(chx), GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_chx(chx));
}

/*����ɨ��ģʽ��DMA*/
//�������ɨ��ģʽ,�ⲿ�������
//����̶�һ��
//ѡ��DMA1. 000,ͨ��0,ADC0
//          001,ͨ��2,ADC1
//          010,ͨ��0,ADC2
void ad_dma_init(uint32_t periph_num)
{
    if((RCU_AHB1EN&0x400000)!=1)
    {
        RCU_AHB1EN|= 0x400000;
    }
    /*ADC0��ADC2�����*/
    if(periph_num!=ADC1)
    {
        DMA_CH0CTL(DMA1)=0; //��Ĵ�����λ
        /*����Ĭ�����赽�洢��*/
        /*����Ĭ��ѡ��m0�Ĵ���*/
        /*�����ADC0�Ļ�������ʲô����,DMA1ͨ��0Ĭ��ѡ��ADC0*/
        if(periph_num==ADC2)
        {
            DMA_CH0CTL(DMA1)|=0x4000000; //DMA1ͨ��0ѡ��ADC2
        }
        DMA_CH0PADDR(DMA1)=(uint32_t)&ADC_RDATA(periph_num);
        /*���ȼ�Ĭ�ϵ�*/
        DMA_CH0CTL(DMA1)|=0x2800   //����˫��16λ����
                        |0x400     //�ڴ��ַ����
                        |0x20;     //�������DMA 
    }
    /*ADC1�����*/
    else 
    {
        DMA_CH2CTL(DMA1)=0; //��Ĵ�����λ
        /*����Ĭ�����赽�洢��*/
        /*����Ĭ��ѡ��m0�Ĵ���*/
        DMA_CH2CTL(DMA1)|=0x2000000; //DMA1ͨ��2ѡ��ADC1
        DMA_CH2PADDR(DMA1)=(uint32_t)&ADC_RDATA(periph_num);
        /*���ȼ�Ĭ�ϵ�*/
        DMA_CH2CTL(DMA1)|=0x2800   //����˫��16λ����
                        |0x400    //�ڴ��ַ����
                        |0x20;     //�������DMA
    }
}

/*ad��ʼ��*/
// ʱ������:
// ��Ƶ168Mhz
// ������HCLK=168Mhz.   PCLK=HCLK/2=84Mhz
// ADCʱ��ѡ��,ADC_SYNCCTL��ADCCK,��ѡ����Դ,�������˷�Ƶ.
// ���40Mhz
// periph_num  :ADC0,ADC1,ADC2
// src_div :000��PCLK2 2��Ƶ
//          001��PCLK2 4��Ƶ
//          010��PCLK2 6��Ƶ
//          011��PCLK2 8��Ƶ
//          100��HCLK 5 ��Ƶ
//          101��HCLK 6 ��Ƶ
//          110��HCLK 10 ��Ƶ
//          111��HCLK 20 ��Ƶ
// ���000,����41Mhz.
// chx_lenth:   ��ʼ��ͨ��������
// chx_sets:    ͨ����ŵļ���  [3:0]: 0~15 ,��0��λ��
//                              [7:4]: 0~15 ,��1��λ��
//                             [11:8]: 0~15 ,��2��λ��
//                            [15:12]: 0~15 ,��3��λ��
//                            [19:16]: 0~15 ,��4��λ��
//                            [23:20]: 0~15 ,��5��λ��
//                            [27:24]: 0~15 ,��6��λ��
//                            [31:28]: 0~15 ,��7��λ��
//                            [35:32]: 0~15 ,��8��λ��
//                            [39:36]: 0~15 ,��9��λ��
//                            [43:40]: 0~15 ,��10��λ��
//                            [47:44]: 0~15 ,��11��λ��
//                            [51:48]: 0~15 ,��12��λ��
//                            [55:52]: 0~15 ,��13��λ��
//                            [59:56]: 0~15 ,��14��λ��
//                            [63:60]: 0~15 ,��15��λ��
// hw_ovsmpl:Ӳ��������         [3:0]:0001����1λ //�����൱�ڳ���
//                                   0010����2λ
//                                   0011����3λ
//                                   0100����4λ
//                                   0101����5λ
//                                   0110����6λ
//                                   0111����7λ
//                                   1000����8λ     ->  /256
//                             [6:4]:000��2x  //�����ǹ�������
//                                   001��4x
//                                   010��8x
//                                   011��16x
//                                   100��32x
//                                   101��64x
//                                   110��128x
//                                   111��256x
void ad_init(uint32_t periph_num, uint8_t src_div, uint8_t chx_lenth, uint64_t chx_sets, uint8_t hw_ovsmpl)
{
    uint8_t temp = 0;
    for (uint8_t i = 0; i < chx_lenth; i++)
    {
        temp = chx_sets & 0xf;
        ad_io_init(temp);
        chx_sets >>= 4;
    }
    
    uint8_t adc_num = (periph_num - ADC_BASE) >> 8; // ֵ0,1,2
    if ((RCU_APB2EN & (1 << (8 + adc_num))) == 0)   // ʱ���ſ�ʹ��
        RCU_APB2EN |= 1 << (8 + adc_num);
    ADC_SYNCCTL |= (src_div << 16)  // Դ,div����
                |   0x800000;       // ʹ���ڲ�ͨ��
    /*����Ĭ�������Ҷ���*/
    /*����Ĭ��ADCΪ����ģʽ*/
    /*����Ĭ��12λ�ֱ���*/
    /*����Ĭ��һ��ת���������й�����*/
    ADC_OVSAMPCTL(periph_num) |= ((hw_ovsmpl & 0xf) << 5)         // ����
                              |  (((hw_ovsmpl >> 4) & 7) << 2);   // �˷�
    ADC_OVSAMPCTL(periph_num) |= 1;                               // ������ʹ��,ע��ʹ��λ��ò�Ҫ��������
    ADC_CTL1(periph_num) |= 1;                                    // adc on
    delay_us(1);
    ad_calibration(periph_num); // У׼
}

/*����ģʽ��ʼ��*/
// �������
// periph_num  :ADC0,ADC1,ADC2
// chx:            ͨ�����,0~18.
// smp_time:   000:ͨ������ʱ��Ϊ 3������
//             001:ͨ������ʱ��Ϊ 15������
//             010:ͨ������ʱ��Ϊ 28������
//             011:ͨ������ʱ��Ϊ 56������
//             100:ͨ������ʱ��Ϊ 84������
//             101:ͨ������ʱ��Ϊ 112������
//             110:ͨ������ʱ��Ϊ 144������
//             111:ͨ������ʱ��Ϊ 480������
// ����ֵ:��ε�ADֵ
void ad_single_init(uint32_t periph_num, uint8_t chx, uint8_t smp_time)
{
    ADC_CTL0(periph_num) = 0;  // �Ĵ�����λ
    ADC_CTL1(periph_num) &= 1; // ���˵�һλ����
    ADC_SAMPT0(periph_num) = ADC_SAMPT1(periph_num) = 0;
    ADC_RSQ2(periph_num) |= chx; // RL����,Ĭ��һ��ͨ��.
    (chx > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (chx - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * chx));
}
 
/*adc����һ������*/
uint16_t ad_start_once(uint32_t periph_num)
{
    uint16_t rel = 0;
    ADC_CTL1(periph_num) |= 0x40000000; // ��ʼ
    while ((ADC_STAT(periph_num) & 2) == 0)
        ; // ûת����������ȵ�
    rel = ADC_RDATA(periph_num);
    return rel;
}


/*ɨ��ģʽ��ʼ��*/
void ad_scan_init(uint32_t periph_num, chx_sets_t * p_struct,uint8_t smp_time)
{
     ADC_CTL0(periph_num)|=0x100; //SMɨ��
     ADC_CTL1(periph_num)|=0x100  //DMA����
                         |0x200;  //DDM
     ADC_RSQ0(periph_num)|= (p_struct->chx_length-1)<<20;//��������
     for(uint8_t i=0;i<p_struct->chx_length;i++)     //��ADC_RSQ2
     {                                               //sample time Ҳһ�����.
        if(i>5)         //6�Ͳ���,��RSQ2��
            break;      //��0��5,��������ǰ6��
        (p_struct->chx_buffer[i] > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (p_struct->chx_buffer[i] - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * p_struct->chx_buffer[i]));
        ADC_RSQ2(periph_num)|= (p_struct->chx_buffer[i])<<(5*i);        
     }
     if(p_struct->chx_length>6)     //����7
     {
         for(uint8_t i=6;i<p_struct->chx_length;i++)    //��ADC_RSQ1
         {
             (p_struct->chx_buffer[i] > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (p_struct->chx_buffer[i] - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * p_struct->chx_buffer[i]));
             ADC_RSQ1(periph_num)|= (p_struct->chx_buffer[i])<<(5*(i-6));
         }
     }
     if(p_struct->chx_length>12)//ǰʮ��������RSQ2��RSQ1������
     {
         for(uint8_t i=12;i<p_struct->chx_length;i++)    //��ADC_RSQ0
         {
             (p_struct->chx_buffer[i] > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (p_struct->chx_buffer[i] - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * p_struct->chx_buffer[i]));
             ADC_RSQ0(periph_num)|= (p_struct->chx_buffer[i])<<(5*(i-12));
         }
     }
}   

/*ɨ��һ��*/
void ad_scan_once(uint32_t periph_num,uint16_t * m_addr)
{
    uint32_t i=0;
    /*ADC0,ADC2�����*/
    if(periph_num!=ADC1)
    {
        DMA_CH0CTL(DMA1)&=~1; //dmaʧ��
        while((DMA_CH0CTL(DMA1)&1)!=0);//ûʧ�ܳɹ�
        DMA_INTC0(DMA1)|=1<<5;//�������ɱ�־
        DMA_CH0M0ADDR(DMA1)=(uint32_t)m_addr;//���õ�ַ
        DMA_CH0CTL(DMA1)|=1;    //dmaʹ��
    }
    /*ADC1�����*/
    else 
    {
        DMA_CH2CTL(DMA1)&=~1; //dmaʧ��
        while((DMA_CH2CTL(DMA1)&1)!=0);//ûʧ�ܳɹ�
        DMA_INTC0(DMA1)|=1<<21;//�������ɱ�־
        DMA_CH2M0ADDR(DMA1)=(uint32_t)m_addr;//���õ�ַ
        DMA_CH2CTL(DMA1)|=1;    //dmaʹ��
    }
    ADC_CTL1(periph_num) |= 0x40000000; // ��ʼ
    while((ADC_STAT(periph_num)&2)==0) // ûת����������ȵ�
    { 
        if(++i==0x50000)
            ADC_CTL1(periph_num)|= 0x40000000; // ��ʼ
    }
}


/*�ڲ����������*/
// inter_fre:�ڲ�������� ,>=3
static uint16_t ad_soft_filter_inter(uint32_t periph_num, uint16_t inter_fre)
{
    uint16_t max_val = 0;
    uint16_t min_val = 0;
    uint16_t ad_temp = 0;
    uint32_t inter_add = 0;
    max_val = min_val = ad_start_once(periph_num);
    for (uint16_t i = 0; i < inter_fre; i++)
    {
        ad_temp = ad_start_once(periph_num);
        (max_val > ad_temp) ? (0) : (max_val = ad_temp);
        (min_val < ad_temp) ? (0) : (min_val = ad_temp);
        inter_add += ad_temp;
    }
    return (inter_add - max_val - min_val) / (inter_fre - 2);
}

/*���������*/
// inter_fre:�ڲ�������� ,>=3
// ex_fre:����������,>=3
uint16_t ad_soft_filter(uint32_t periph_num, uint16_t inter_fre, uint16_t ex_fre)
{
    uint16_t max_val = 0;
    uint16_t min_val = 0;
    uint16_t ad_temp = 0;
    uint32_t add_val = 0;
    max_val = min_val = ad_soft_filter_inter(periph_num,  inter_fre);
    for (uint16_t i = 0; i < ex_fre; i++)
    {
        ad_temp = ad_soft_filter_inter(periph_num,  inter_fre);
        (max_val > ad_temp) ? (0) : (max_val = ad_temp);
        (min_val < ad_temp) ? (0) : (min_val = ad_temp);
        add_val += ad_temp;
    }
    return (add_val - max_val - min_val) / (ex_fre - 2);
}

