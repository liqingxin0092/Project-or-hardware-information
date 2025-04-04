/**
 *******************************************************************************
 * @file        rcc_and_pwr_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       探究一下rcc,顺便带点PWR的东西.
 *              
 *****************************************************************************************
 */
 
#include "rcc_and_pwr_m.h"
#include "stm32h750xx.h"

/*对2取对数*/
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

/*D1CPRE和HPRE的分频参数表*/
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
    
/*记录理论值时钟值*/
//仅对于HSE过PLL1这种常规做法有效.
//而且要等到时钟初始化函数执行完了后查看.
//通过函数来看.
static uint32_t system_clk;

/*单片机启动流程*/
//这里默认选择内部LDO启用而不是LDO旁路
//(1).POR上电复位,结束后采用VOS3(1V)给Vcore供电.
//(2).单片机处于复位状态直到Vcore的电压正确(VOSready,指示电压电平等于或高于 VOS 所选电平)
//(3).脱离复位状态,并且启用HSI
//(4).HSI稳定后,CPU加载flash和选项字节中内容,以限制模式(run*)运行.
//(5).等待ACTVOSRDY就绪(指示电压水平等于VOS选择的电压),cpu处于完全的运行模式

//注:如果ACTVOSRDY指示的电压水平无效,则cpu一直处于run*模式,不能写入ram,VOS不能改.

/*初始hse和pll1*/
//注意PLL1是输出到系统时钟的.
//启动文件里执行的那个system_init复位了一些时钟寄存器,如果不够再加
//DIVM1:对PLL的输入(HSE)进行分频.0~63,0,不启用DIVM1.1,1分频.63,63分频.
//DIVN1:经过DIVM1后的倍频,4~512,函数内减1.
//DIVP1:上面倍频后再分频.这个值只能是偶数, 0,不分频 2,4,...,128分频
//DIVR1:0xff,关闭这个时钟源, 1~128分频,函数内减1.
//DIVQ1:0xff,关闭这个时钟源, 1~128分频,函数内减1.
void rcc_hse_pll1_init(uint8_t DIVM1,uint16_t DIVN1,uint8_t DIVP1,uint8_t DIVR1,uint8_t DIVQ1)
{
    /*让或发挥作用*/
    RCC->CFGR      = 0;			
	RCC->D1CFGR    = 0;			
	RCC->D2CFGR    = 0;			
	RCC->D3CFGR    = 0;
    RCC->PLL1DIVR  = 0;    
	RCC->PLLCKSELR = 0;	 	
    if(DIVP1&1)  
        while(1); //DIVP1必须得是偶数
    float ref1_ck=0.0f;//这个是经过DIVM1后的时钟
    /*这里默认选择HSE晶振而不是旁路*/
    /*启用VOS0序列*/   //这是性能最高的Vcore电压,仅限V版本
    PWR->CR3&=~4;	//SCUEN,这位清0,才能设置上VOS1
    PWR->D3CR|=0xc000;//设置VOS1
    while((PWR->D3CR&0x2000)==0);//VOS1没设置好在这里等的.
    while((PWR->CSR1&0xC000)==0);//如果当前设置的不是VOS1,就在这里等的
    RCC->APB4ENR|=2;//使能SYSCFG peripheral时钟.
    SYSCFG->PWRCR=1;//使能ODEN位
    while((PWR->D3CR&0x2000)==0);//VOS0没设置好在这里等的.
    /*提高系统频率*/
    RCC->CR|=0x10000;//HSEON
    while((RCC->CR&0x20000)==0);//HSE没准备好,在这里等的
    /*这里默认PLL1分频采用整数模式*/
    RCC->PLLCKSELR|=2;//PLL选择HSE作为时钟源
    RCC->PLLCKSELR|=DIVM1<<4;//设置DIVM1
    ref1_ck=HSE_CLOCK/DIVM1;
    /*此处省略ref1_ck在1~2范围时的情形*/
    if(2<=ref1_ck&&ref1_ck<4)      //设置 PLL1RGE
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
        while(1);//如果啥范围都不属于,卡到这里.
    RCC->PLL1DIVR|=DIVN1-1; //设置DIVN1
    ref1_ck*=DIVN1;
#if USE_OVERCLOCK==0   
    if((ref1_ck<150)||(ref1_ck>960))
        while(1);//时钟设置的中间值有点太极端了
#endif    
    if(ref1_ck<=420)
        RCC->PLLCFGR|=2;   //设置PLL1VCOSEL
    RCC->PLL1DIVR|=(DIVP1-1)<<9; //设置DIVP1
    if(DIVR1==0xff)
        RCC->PLLCFGR&=~0x40000;//关闭R1时钟源
    else
        RCC->PLL1DIVR|=(DIVR1-1)<<24; //设置DIVR1
    if(DIVQ1==0xff)
        RCC->PLLCFGR&=~0x20000;//关闭Q1时钟源
    else
        RCC->PLL1DIVR|=(DIVQ1-1)<<16; //设置DIVQ1
    RCC->CR|=0x1000000;//开启PLL1
    while((RCC->CR&2000000)==0);//如果PLL1没开启,在这里卡的.    
    system_clk=ref1_ck/DIVP1*1000000;   //计算时钟,这里还不是CPU时钟.
}

/*初始化cpu clk及其分支时钟配置*/
//D1CPRE:SYSCLK完的一次分频.参数搜他类型
//HPRE:和上面那个参数一样,分频1,2,4,8,16,64,128,256,512
//D1PPRE:经过HPRE分频后,来到这次分频,通往APB3,分频1,2,4,8,16.
//D2PPRE1:经过HPRE分频后,来到这次分频,通往APB1,分频1,2,4,8,16.
//D2PPRE2:经过HPRE分频后,来到这次分频,通往APB2,分频1,2,4,8,16.
//D3PPRE:经过HPRE分频后,来到这次分频,通往APB4,分频1,2,4,8,16.
//TIMPRE:0,D2PPREx的分频如果=1,2,  那么TIM频率就=rcc_hclk1,否则定时器频率=rcc_pclkx*2
//       1,D2PPREx的分频如果=1,2,4,那么TIM频率就=rcc_hclk1,否则定时器频率=rcc_pclkx*4
void rcc_cpu_periph_clk_init(uint16_t D1CPRE,uint16_t HPRE,uint8_t D1PPRE,uint8_t D2PPRE1,uint8_t D2PPRE2,uint8_t D3PPRE,uint8_t TIMPRE)
{
    uint8_t i;
    uint32_t clk_temp=0;
    uint32_t after_hpre_clk=0;
    for(i=0;i<9;i++)
        if(div_para_table[i][0]==D1CPRE)
        {
            RCC->D1CFGR|=div_para_table[i][1]<<8;//设置D1CPRE
            break;
        }      
    system_clk/=D1CPRE;//CPU_clk
#if USE_OVERCLOCK==0        
    if(system_clk>480000000)
        while(1);//超频了
#endif
    for(i=0;i<9;i++)
        if(div_para_table[i][0]==HPRE)
        {
            RCC->D1CFGR|=div_para_table[i][1];//设置HPRE
            break;
        }
    after_hpre_clk= system_clk/HPRE; 
#if USE_OVERCLOCK==0         
    if(after_hpre_clk>240000000)
        while(1);//超频了 
#endif    
    FLASH->ACR=0x24;  //设置等待周期和延迟
    while(FLASH->ACR!=0x24);//没设置好就在这里等的.
    
    RCC->CFGR|=3;//选择PLL1作为sysclk源
    while((RCC->CFGR&0x18)!=0x18);//如果没把PLL1作为时钟源,在这里等的.
    if (D1PPRE!=1)        
        RCC->D1CFGR|= ((ln2(D1PPRE)-1)|4)<<4; //APB3时钟 
    clk_temp=after_hpre_clk/D1PPRE;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000)
        while(1);  //APB3时钟超频了
#endif
    
    if(D2PPRE1!=1)
        RCC->D2CFGR|= ((ln2(D2PPRE1)-1)|4)<<4;//APB1时钟
    clk_temp=after_hpre_clk/D2PPRE1;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000 )
        while(1);  //APB1时钟超频了
#endif
    if(D2PPRE2!=1)
        RCC->D2CFGR|= ((ln2(D2PPRE2)-1)|4)<<8;//APB2时钟
    clk_temp=after_hpre_clk/D2PPRE2;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000 )
        while(1);  //APB2时钟超频了
#endif
    if(D3PPRE!=1)
        RCC->D3CFGR|= ((ln2(D3PPRE)-1)|4)<<4;//APB4时钟
    clk_temp=after_hpre_clk/D3PPRE;
#if USE_OVERCLOCK==0 
    if(clk_temp>120000000 )
        while(1);  //APB2时钟超频了
#endif   
    RCC->CFGR|=TIMPRE<<15;
}

/*获取时钟*/
uint32_t rcc_get_system_clk(void)
{
     return system_clk;
}