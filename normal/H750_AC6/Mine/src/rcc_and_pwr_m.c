/**
 *******************************************************************************
 * @file        rcc_and_pwr_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       ̽��һ��rcc,˳�����PWR�Ķ���.
 *              
 *****************************************************************************************
 */
 
#include "rcc_and_pwr_m.h"
#include "stm32h750xx.h"

/*��2ȡ����*/
static uint8_t ln2(uint8_t val)
{
    int a = 0;
    while (val != 1)
    {
        val /= 2;
        a++;
    }
    return a;
}

/*D1CPRE��HPRE�ķ�Ƶ������*/
const uint16_t div_para_table[9][2]={
     {1   ,  0x0},
     {2   ,  0x8},
     {4   ,  0x9},
     {8   ,  0xA},
     {16  ,  0xB},
     {64  ,  0xC},  
     {128 ,  0xD},
     {256 ,  0xE},
     {512 ,  0xF},   
};
    
/*��¼����ֵʱ��ֵ*/
//������HSE��PLL1���ֳ���������Ч.
//����Ҫ�ȵ�ʱ�ӳ�ʼ������ִ�����˺�鿴.
//ͨ����������.
static uint32_t system_clk;

/*��Ƭ����������*/
//����Ĭ��ѡ���ڲ�LDO���ö�����LDO��·
//(1).POR�ϵ縴λ,���������VOS3(1V)��Vcore����.
//(2).��Ƭ�����ڸ�λ״ֱ̬��Vcore�ĵ�ѹ��ȷ(VOSready,ָʾ��ѹ��ƽ���ڻ���� VOS ��ѡ��ƽ)
//(3).���븴λ״̬,��������HSI
//(4).HSI�ȶ���,CPU����flash��ѡ���ֽ�������,������ģʽ(run*)����.
//(5).�ȴ�ACTVOSRDY����(ָʾ��ѹˮƽ����VOSѡ��ĵ�ѹ),cpu������ȫ������ģʽ

//ע:���ACTVOSRDYָʾ�ĵ�ѹˮƽ��Ч,��cpuһֱ����run*ģʽ,����д��ram,VOS���ܸ�.

/*��ʼhse��pll1*/
//ע��PLL1�������ϵͳʱ�ӵ�.
//�����ļ���ִ�е��Ǹ�system_init��λ��һЩʱ�ӼĴ���,��������ټ�
//DIVM1:��PLL������(HSE)���з�Ƶ.0~63,0,������DIVM1.1,1��Ƶ.63,63��Ƶ.
//DIVN1:����DIVM1��ı�Ƶ,4~512,�����ڼ�1.
//DIVP1:���汶Ƶ���ٷ�Ƶ.���ֵֻ����ż��, 0,����Ƶ 2,4,...,128��Ƶ
//DIVR1:0xff,�ر����ʱ��Դ, 1~128��Ƶ,�����ڼ�1.
//DIVQ1:0xff,�ر����ʱ��Դ, 1~128��Ƶ,�����ڼ�1.
void rcc_hse_pll1_init(uint8_t DIVM1,uint16_t DIVN1,uint8_t DIVP1,uint8_t DIVR1,uint8_t DIVQ1)
{
    /*�û򷢻�����*/
    RCC->CFGR      = 0;			
	RCC->D1CFGR    = 0;			
	RCC->D2CFGR    = 0;			
	RCC->D3CFGR    = 0;
    RCC->PLL1DIVR  = 0;    
	RCC->PLLCKSELR = 0;	 	
    if(DIVP1&1)  
        while(1); //DIVP1�������ż��
    float ref1_ck=0.0f;//����Ǿ���DIVM1���ʱ��
    /*����Ĭ��ѡ��HSE�����������·*/
    /*����VOS0����*/   //����������ߵ�Vcore��ѹ,����V�汾
    PWR->CR3&=~4;	//SCUEN,��λ��0,����������VOS1
    PWR->D3CR|=0xc000;//����VOS1
    while((PWR->D3CR&0x2000)==0);//VOS1û���ú�������ȵ�.
    while((PWR->CSR1&0xC000)==0);//�����ǰ���õĲ���VOS1,��������ȵ�
    RCC->APB4ENR|=2;//ʹ��SYSCFG peripheralʱ��.
    SYSCFG->PWRCR=1;//ʹ��ODENλ
    while((PWR->D3CR&0x2000)==0);//VOS0û���ú�������ȵ�.
    /*���ϵͳƵ��*/
    RCC->CR|=0x10000;//HSEON
    while((RCC->CR&0x20000)==0);//HSEû׼����,������ȵ�
    /*����Ĭ��PLL1��Ƶ��������ģʽ*/
    RCC->PLLCKSELR|=2;//PLLѡ��HSE��Ϊʱ��Դ
    RCC->PLLCKSELR|=DIVM1<<4;//����DIVM1
    ref1_ck=HSE_CLOCK/DIVM1;
    /*�˴�ʡ��ref1_ck��1~2��Χʱ������*/
    if(2<=ref1_ck&&ref1_ck<4)      //���� PLL1RGE
    {
         RCC->PLLCFGR|=0x4;   
    }
    else if(4<=ref1_ck&&ref1_ck<8)
    {
         RCC->PLLCFGR|=0x8; 
    }  
    else if(8<=ref1_ck&&ref1_ck<=16)
    {
         RCC->PLLCFGR|=0xC;
    }
    else    
        while(1);//���ɶ��Χ��������,��������.
    RCC->PLL1DIVR|=DIVN1-1; //����DIVN1
    ref1_ck*=DIVN1;
#if USE_OVERCLOCK==0   
    if((ref1_ck<150)||(ref1_ck>960))
        while(1);//ʱ�����õ��м�ֵ�е�̫������
#endif    
    if(ref1_ck<=420)
        RCC->PLLCFGR|=2;   //����PLL1VCOSEL
    RCC->PLL1DIVR|=(DIVP1-1)<<9; //����DIVP1
    if(DIVR1==0xff)
        RCC->PLLCFGR&=~0x40000;//�ر�R1ʱ��Դ
    else
        RCC->PLL1DIVR|=(DIVR1-1)<<24; //����DIVR1
    if(DIVQ1==0xff)
        RCC->PLLCFGR&=~0x20000;//�ر�Q1ʱ��Դ
    else
        RCC->PLL1DIVR|=(DIVQ1-1)<<16; //����DIVQ1
    RCC->CR|=0x1000000;//����PLL1
    while((RCC->CR&2000000)==0);//���PLL1û����,�����￨��.    
    system_clk=ref1_ck/DIVP1*1000000;   //����ʱ��,���ﻹ����CPUʱ��.
}

/*��ʼ��cpu clk�����֧ʱ������*/
//D1CPRE:SYSCLK���һ�η�Ƶ.������������
//HPRE:�������Ǹ�����һ��,��Ƶ1,2,4,8,16,64,128,256,512
//D1PPRE:����HPRE��Ƶ��,������η�Ƶ,ͨ��APB3,��Ƶ1,2,4,8,16.
//D2PPRE1:����HPRE��Ƶ��,������η�Ƶ,ͨ��APB1,��Ƶ1,2,4,8,16.
//D2PPRE2:����HPRE��Ƶ��,������η�Ƶ,ͨ��APB2,��Ƶ1,2,4,8,16.
//D3PPRE:����HPRE��Ƶ��,������η�Ƶ,ͨ��APB4,��Ƶ1,2,4,8,16.
//TIMPRE:0,D2PPREx�ķ�Ƶ���=1,2,  ��ôTIMƵ�ʾ�=rcc_hclk1,����ʱ��Ƶ��=rcc_pclkx*2
//       1,D2PPREx�ķ�Ƶ���=1,2,4,��ôTIMƵ�ʾ�=rcc_hclk1,����ʱ��Ƶ��=rcc_pclkx*4
void rcc_cpu_periph_clk_init(uint16_t D1CPRE,uint16_t HPRE,uint8_t D1PPRE,uint8_t D2PPRE1,uint8_t D2PPRE2,uint8_t D3PPRE,uint8_t TIMPRE)
{
    uint8_t i;
    uint32_t clk_temp=0;
    uint32_t after_hpre_clk=0;
    for(i=0;i<9;i++)
        if(div_para_table[i][0]==D1CPRE)
        {
            RCC->D1CFGR|=div_para_table[i][1]<<8;//����D1CPRE
            break;
        }      
    system_clk/=D1CPRE;//CPU_clk
#if USE_OVERCLOCK==0        
    if(system_clk>480000000)
        while(1);//��Ƶ��
#endif
    for(i=0;i<9;i++)
        if(div_para_table[i][0]==HPRE)
        {
            RCC->D1CFGR|=div_para_table[i][1];//����HPRE
            break;
        }
    after_hpre_clk= system_clk/HPRE; 
#if USE_OVERCLOCK==0         
    if(after_hpre_clk>240000000)
        while(1);//��Ƶ�� 
#endif    
    FLASH->ACR=0x24;  //���õȴ����ں��ӳ�
    while(FLASH->ACR!=0x24);//û���úþ�������ȵ�.
    
    RCC->CFGR|=3;//ѡ��PLL1��ΪsysclkԴ
    while((RCC->CFGR&0x18)!=0x18);//���û��PLL1��Ϊʱ��Դ,������ȵ�.
    if (D1PPRE!=1)        
        RCC->D1CFGR|= ((ln2(D1PPRE)-1)|4)<<4; //APB3ʱ�� 
    clk_temp=after_hpre_clk/D1PPRE;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000)
        while(1);  //APB3ʱ�ӳ�Ƶ��
#endif
    
    if(D2PPRE1!=1)
        RCC->D2CFGR|= ((ln2(D2PPRE1)-1)|4)<<4;//APB1ʱ��
    clk_temp=after_hpre_clk/D2PPRE1;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000 )
        while(1);  //APB1ʱ�ӳ�Ƶ��
#endif
    if(D2PPRE2!=1)
        RCC->D2CFGR|= ((ln2(D2PPRE2)-1)|4)<<8;//APB2ʱ��
    clk_temp=after_hpre_clk/D2PPRE2;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000 )
        while(1);  //APB2ʱ�ӳ�Ƶ��
#endif
    if(D3PPRE!=1)
        RCC->D3CFGR|= ((ln2(D3PPRE)-1)|4)<<4;//APB4ʱ��
    clk_temp=after_hpre_clk/D3PPRE;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000 )
        while(1);  //APB2ʱ�ӳ�Ƶ��
#endif   
    RCC->CFGR|=TIMPRE<<15;
}

/*��ȡʱ��*/
uint32_t rcc_get_system_clk(void)
{
     return system_clk;
}