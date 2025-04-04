/**
 *******************************************************************************
 * @file        dma_c.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-22
 * @brief       dma���ù���
 *              
 *****************************************************************************************
 */
#include "dma_m.h"
#include "stm32h750xx.h"
#include "h7_core.h"

//�ȿ����ֲ�.
//MDMA,�����ϳ���AHB2�ϵĶ������ܷ���,����ȫ�ܷ���.
//����ɢɢ��,û��sequence.
//���հ�������˳������,��ʱ���Ҹо�����ֲ᲻�ܰ�˳��.

//DMAMUX&BDMA

//ע��BDMA��8��ͨ��,keil debug����ʾ1~8,ʵ�����ֲ�����0~7,һһ��Ӧ.
//BDMA���Կ���GPIO

typedef union
{
    struct  bit
    {    
        volatile uint8_t ch0_flag:1;
        volatile uint8_t ch1_flag:1;
        volatile uint8_t ch2_flag:1;
        volatile uint8_t ch3_flag:1;
        volatile uint8_t ch4_flag:1;
        volatile uint8_t ch5_flag:1;
        volatile uint8_t ch6_flag:1;
        volatile uint8_t ch7_flag:1;
    }bit;
    volatile uint8_t whole;
}bdma_lock_t; 

/*bdmaͨ������־*/
bdma_lock_t bdma_lock;

/*BDMA����*/
//lenth:����
//size:��С
//switch_state:[1:0]: the channel priority. 00: low,01: medium,10: high,11: very high
//             [3:2]: src size: 00: 8 bits,01: 16 bits,10: 32 bits
//             [5:4]: des size: 00: 8 bits,01: 16 bits,10: 32 bits
//               [6]: mem2mem mode:1��,0��.
//               [7]: 1,p_addr����
//               [8]: 1,m_addr����
//               [9]: 1,m to p. 0,p to m.
//              [10]: 1,circle mode on.
//���������ǰ�ȴ�.
void bdam_trans(BDMA_Channel_TypeDef* bdma_chx,uint32_t p_addr,uint32_t m_addr,uint16_t lenth,uint16_t switch_state)
{
    uint8_t bdma_num=((uint32_t)bdma_chx - (uint32_t)BDMA_Channel0)/0x14;//0~7
    uint8_t tc_offset=(bdma_num<<2)+1;
    
    if(!((RCC->AHB4ENR>>21)&1))
        RCC->AHB4ENR |=1<<21;    
    while((bdma_lock.whole>>bdma_num)&1);//��ס��������ȵ�       
    bdma_chx->CCR&=~1;//��en
    bdma_chx->CCR=0;
    bdma_chx->CPAR =p_addr;              
    bdma_chx->CM0AR=m_addr;
    bdma_chx->CNDTR=lenth;
    bdma_chx->CCR|=(((switch_state>>6)&1)<<14)       //mem2mem mode
                 | ((switch_state&3)<<12) //���ȼ�����
                 | ((1&(switch_state>>7))<<6) //p_addr����
                 | ((1&(switch_state>>8))<<7) //m_addr����
                 | (((switch_state>>2)&3)<<8)//psize
                 | (((switch_state>>4)&3)<<10)//msize
                 | (((switch_state>>9)&1)<<4)//dir   
                 | (((switch_state>>10)&1)<<5)//circle mode 
                 | 2; //tc�ж�
    if(!__is_enable_IRQ(BDMA_Channel0_IRQn+bdma_num))//���ûʹ�ܾ�ʹ��.
    {
        __set_priority(BDMA_Channel0_IRQn+bdma_num, 3, 0);
        __enable_IRQ(BDMA_Channel0_IRQn+bdma_num);
    }
    bdma_chx->CCR|=1;//en 
    __asm volatile("cpsid i");
    bdma_lock.whole|=1<<bdma_num;//����
    __asm volatile("cpsie i");
}

void BDMA_Channel0_IRQHandler(void)
{
   if(BDMA->ISR&2)//tc��־
   {
       bdma_lock.bit.ch0_flag=0;//����
      BDMA->IFCR|=2;//��tc
   }              
}

void BDMA_Channel1_IRQHandler(void)
{
   if(BDMA->ISR&0x20)//tc��־
   {
       bdma_lock.bit.ch1_flag=0;//����
      BDMA->IFCR|=0x20;//��tc
   } 
}

void BDMA_Channel2_IRQHandler(void)
{
   if(BDMA->ISR&0x200)//tc��־
   {
       bdma_lock.bit.ch2_flag=0;//����
      BDMA->IFCR|=0x200;//��tc
   } 
}