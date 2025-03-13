/**
 *******************************************************************************
 * @file        h750_flash_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       ��h750/743д��flash�����
 *         3.13 : �õ�ʱ���ټӰ�.
 *              
 *****************************************************************************************
 */
#include "flash_m.h"
#include "stm32h750xx.h"

/*flash��������*/
static void flash_unlock(void)
{
    FLASH->KEYR1= 0x45670123;
    FLASH->KEYR1= 0xCDEF89AB;
    while((FLASH->CR1&1)==1);//���û���������￨��
}

/*ѡ���ֽڽ�������*/
static void option_bytes_unlock(void)
{
    FLASH->OPTKEYR=0x08192A3B;
    FLASH->OPTKEYR= 0x4C5D6E7F;
    while((FLASH->OPTCR&1)==1);//��������ľ������    
}

/*��������ֱ��ָ�����*/

/*flashд����*/
//h750��Χ�����ַ��Χ:0x0800 0000 ~ 0x0802 0000
//lenth:�ֽ���
//һ����32���ֽ�
void flash_single_write(uint32_t addr,uint32_t data_source,uint16_t lenth)
{
    uint16_t fifo_div=lenth>>5; //��װ���ٻ���
    uint16_t fifo_rem=lenth&0x1f;//����ʣ��
    if((FLASH->CR1&1)==1)//���û����
    {
         flash_unlock();       //flash��������
     }
     FLASH->CR1|=2;//PGλ��1
     FLASH->CR1&=~(3<<4);//���ԭ������
     FLASH->CR1|=3<<4;//64λ���ж�
     for(uint32_t i=0;i<fifo_div;i++)//�����������
     {
         while((FLASH->SR1&1)==1);//���æ,�����￨��
         for(uint32_t j=0;j<4;j++)
         {
             *(uint64_t*)addr=*(uint64_t*)data_source;
             addr+=8;
             data_source+=8;
         }
         FLASH->CCR1|=10000;//��eop
     }
     while((FLASH->SR1&1)==1);//���æ,�����￨��
     while(fifo_rem>0)      //����һ������
     {
         if(fifo_rem>7)
         {
             FLASH->CR1&=~(3<<4);//���ԭ������
             FLASH->CR1|=3<<4;//64λ���ж�
             *(uint64_t*)addr=*(uint64_t*)data_source;
             addr+=8;
             data_source+=8;
             fifo_rem-=8;
         }
         else if(fifo_rem>3)
         {
             FLASH->CR1&=~(3<<4);//���ԭ������
             FLASH->CR1|=2<<4;//32λ���ж�
             *(uint32_t*)addr=*(uint32_t*)data_source;
             addr+=4;
             data_source+=4;
             fifo_rem-=4;
         }
         else if(fifo_rem>1)
         {
             FLASH->CR1&=~(3<<4);//���ԭ������
             FLASH->CR1|=1<<4;//16λ���ж�
             *(uint16_t*)addr=*(uint16_t*)data_source;
             addr+=2;
             data_source+=2;
             fifo_rem-=2;
         }
         else 
         {
             FLASH->CR1&=~(3<<4);//���ԭ������,0:8λ���ж�
             FLASH->CR1|=0<<4;//8λ���ж�
             *(uint8_t*)addr=*(uint8_t*)data_source;
             addr+=1;
             data_source+=1;
             fifo_rem-=1;
         }
         if(fifo_rem==0)
         {
            FLASH->CR1|=1<<6;//ǿ��д����
         }
     }
     FLASH->CR1&=~2;//PG��0
}

/*��������*/
//����Ȳ��ż�,h750һ��ȫû��
void flash_erase_sector(void)
{
    
    
}


/*ѡ���ֽ�--RDP�޸�*/
//val:0xaa                  :level 0,���û������������ʩ,flash�����в���������ִ�� 
//    0xff(��0xaa,0xcc����) :level 1,��ֹ��������ram�еĴ����flash���κη���(���ֲ���),�����ֲ������Զ�
//                           flash���κβ���.
//    0xcc                  :level 2,��������ȫ��,�����û���flash���в���,�������ram����,ѡ���ֽڲ����޸�.
//                           ��������ǳ�Σ��!!!
//ע��:level��1��0,����ȫ��
void option_bytes_modify_RDP(uint8_t val)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    while((FLASH->SR1&1)==1);//���æ,�����￨��,�����������ͨ���������ѡ���ֽ��޸Ĳ���
    FLASH->OPTSR_PRG&=~(0xff<<8);//���ԭ��ֵ
    FLASH->OPTSR_PRG|=val;//��ֵ
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ
}

/*�޸Ŀ��Ź����������*/
// stop_en_cnt     : 1,stopģʽ��,idog��Ȼ����.    0,stopģʽ��,idog������
// standby_en_cnt  : 1,standbyģʽ��,idog��Ȼ����. 0,standbyģʽ��,idog������
// hard_or_soft    : 1,�������idog.  0,Ӳ������idog(�ϵ�ʱ�Զ�ʹ��)
void option_bytes_modify_IDOG(uint8_t stop_en_cnt,uint8_t standby_en_cnt,uint8_t hard_or_soft)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    while((FLASH->SR1&1)==1);//���æ,�����￨��,�����������ͨ���������ѡ���ֽ��޸Ĳ���
    FLASH->OPTSR_PRG&=~(1<<4);//sw
    FLASH->OPTSR_PRG&=~(3<<17);
    FLASH->OPTSR_PRG|=stop_en_cnt    << 17;
    FLASH->OPTSR_PRG|=standby_en_cnt << 18;
    FLASH->OPTSR_PRG|=hard_or_soft   <<  4;
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ
}


/*��λ����*/
//bor: ���縴λˮƽ,��Ƭ����ѹ����һ���ĵ�ѹ�ͻ����һ�θ�λ,���°�����ֵ����.
//     0,Vbor0,��Ƭ����Ż��ڵ�ѹ����1.62~1.67ʱ������λ�ź�.
//     1,Vbor1,��Ƭ����Ż��ڵ�ѹ����2.00~2.10ʱ������λ�ź�.
//     2,Vbor2,��Ƭ����Ż��ڵ�ѹ����2.31~2.41ʱ������λ�ź�.
//     3,Vbor3,��Ƭ����Ż��ڵ�ѹ����2.61~2.70ʱ������λ�ź�.
//stop_d1:1,��d1 domain��stopģʽ����λ.0,��λ
//stby_d1:1,��d1 domain��standbyģʽ����λ.0,��λ
//����û���ҵ�d2 domain�������,Ҫ���õĻ���programmer��. 
void option_bytes_modify_reset(uint8_t bor ,uint8_t stop_d1,uint8_t stby_d1)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    while((FLASH->SR1&1)==1);//���æ,�����￨��,�����������ͨ���������ѡ���ֽ��޸Ĳ���
    FLASH->OPTSR_PRG&=~(3<<2);
    FLASH->OPTSR_PRG&=~(3<<6);
    FLASH->OPTSR_PRG|= bor<<2;   
    FLASH->OPTSR_PRG|= stop_d1 << 6;
    FLASH->OPTSR_PRG|= stby_d1 << 7;
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ
}

/*����bank1��2*/
//1,����. 0��������.
//h750�������
void option_bytes_modify_swap(uint8_t swap)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    while((FLASH->SR1&1)==1);//���æ,�����￨��,�����������ͨ���������ѡ���ֽ��޸Ĳ���
    FLASH->OPTSR_PRG&=~(1<<31);
    FLASH->OPTSR_PRG|= swap<<31;   
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ
}    

/*�Ƿ�����IO�Ż�*/
//1,����.     0,������.
void option_bytes_modify_IO_SPEED_OPTIMIZE(uint8_t is_optimize)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    while((FLASH->SR1&1)==1);//���æ,�����￨��,�����������ͨ���������ѡ���ֽ��޸Ĳ���
    FLASH->OPTSR_PRG&=~(1<<29);
    FLASH->OPTSR_PRG|= is_optimize<<29;   
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ 
}

/*����ר�д����������*/
//h750flash:0x0800 0000 ~ 0x0802 0000   ��128kb
//bank:ѡ������,1 or 2 , h750ֻ��1
//start:��ַ���뿪ʼ
//end:��ַ�������
//DMEP:1,�������õı�������������.0,���������
//��������: ���������С = [(END - START) + 1] x 256,��ַΪbank��ַ.
//eg: bank=1,start=0 ,end=0xf.  ���������0x0800 0000 �� 0x0800 0FFF ��16 * 256 = 4kbytes
//����DMEP����ÿ���,���������ȼ���1�����ˢ���������.
void option_bytes_modify_PCROP(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMEP)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    while((FLASH->SR1&1)==1);//���æ,�����￨��,�����������ͨ���������ѡ���ֽ��޸Ĳ���
    if(bank==1)
    {
        FLASH->PRAR_PRG1=0;//��ԭ��ֵ.
        FLASH->PRAR_PRG1|=start;
        FLASH->PRAR_PRG1|=end<<16;
        FLASH->PRAR_PRG1|=DMEP<<31;
    }
    else if(bank==2)
    {
        FLASH->PRAR_PRG2=0;//��ԭ��ֵ.
        FLASH->PRAR_PRG2|=start;
        FLASH->PRAR_PRG2|=end<<16;
        FLASH->PRAR_PRG2|=DMEP<<31;
    }
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ 
}

/*��ȫר����������*/
//��ַ�������һ��һ��  
//DMES��1,������,RDP��0���Ը�λ��Щ����
//���������࿪��secureλ.�������ò���.
//STM32H743 has no "secure area" (a.k.a. hide protection, HDP).
//This is the reason of "Error: Activating security Failure".
//H743��programer�ﶼû�����ѡ��(���ñ�������)
//H750����������ѡ������ò���.
//�ٷ�˵h753����.
//�����������ð�ȫ��һλ.
void option_bytes_modify_Secure_area(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMES)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    while((FLASH->SR1&1)==1);//���æ,�����￨��,�����������ͨ���������ѡ���ֽ��޸Ĳ���
    if((FLASH->OPTSR_CUR&0x200000)==0)    //�����ȫλû����
    {
        /*���ѡ��ÿֲ�,����programmer��stlink������*/
        FLASH->OPTSR_PRG|=1<<21;
        FLASH->OPTCR|=2;//��ʼ�޸�
        while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ
    }
    if(bank==1)
    {
        FLASH->SCAR_PRG1=0;//��ԭ��ֵ.
        FLASH->SCAR_PRG1|=start;
        FLASH->SCAR_PRG1|=end<<16;
        FLASH->SCAR_PRG1|=DMES<<31;
    }
    else if(bank==2)
    {
        FLASH->SCAR_PRG2=0;//��ԭ��ֵ.
        FLASH->SCAR_PRG2|=start;
        FLASH->SCAR_PRG2|=end<<16;
        FLASH->SCAR_PRG2|=DMES<<31;
    }
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ 
}

/*�����ȫѡ��*/
void option_bytes_clear_Secure(void)
{
    if((FLASH->OPTCR&1)==1)//���û����
    {
        option_bytes_unlock();//����
    }
    /*���ѡ��ÿֲ�,����programmer��stlink������*/
    FLASH->OPTSR_PRG&=~(1<<21);
    FLASH->OPTCR|=2;//��ʼ�޸�
    while((FLASH->OPTSR_CUR&1)==1);//�ȴ���æ 
}



