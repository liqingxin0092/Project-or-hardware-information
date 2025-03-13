#include "sys.h"
#include "h7_core.h"
#include  "string.h"

//时钟设置函数
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//Fsys:系统时钟频率,也是PLL1的p分频输出时钟频率
//Fq:PLL1的q分频输出时钟频率
//Fs:PLL输入时钟频率,可以是HSI,CSI,HSE等. 

//plln:PLL1倍频系数(PLL倍频),取值范围:4~512.
//pllm:PLL1预分频系数(进PLL之前的分频),取值范围:2~63.
//pllp:PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟,取值范围:2~128.(且必须是2的倍数)
//pllq:PLL1的q分频系数(PLL之后的分频),取值范围:1~128.

//CPU频率(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//pll2_p_ck=(25/25)*440/2)=220Mhz
//pll2_r_ck=FMC时钟频率=((25/25)*440/2)=110Mhz

//外部晶振为25M的时候,推荐值:plln=160,pllm=5,pllp=2,pllq=4.
//得到:Fvco=25*(160/5)=800Mhz
//     Fsys=pll1_p_ck=800/2=400Mhz
//     Fq=pll1_q_ck=800/4=200Mhz
//返回值:0,成功;1,失败。
static u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{ 
	u16 retry=0;
	u8 status=0;
	
	PWR->CR3&=~(1<<2);				//SCUEN=0,锁定LDOEN和BYPASS位的设置
	PWR->D3CR|=3<<14;				//VOS=3,Scale1,1.15~1.26V内核电压,FLASH访问可以得到最高性能
	while((PWR->D3CR&(1<<13))==0);	//等待电压稳定 
	RCC->CR|=1<<16;					//HSEON=1,开启HSE
	while(((RCC->CR&(1<<17))==0)&&(retry<0X7FFF))retry++;//等待HSE RDY
	if(retry==0X7FFF)status=1;		//HSE无法就绪
	else   
	{
		RCC->PLLCKSELR|=2<<0;		//PLLSRC[1:0]=2,选择HSE作为PLL的输入时钟源
		RCC->PLLCKSELR|=pllm<<4;	//DIVM1[5:0]=pllm,设置PLL1的预分频系数
		RCC->PLL1DIVR|=(plln-1)<<0;	//DIVN1[8:0]=plln-1,设置PLL1的倍频系数,设置值需减1
		RCC->PLL1DIVR|=(pllp-1)<<9;	//DIVP1[6:0]=pllp-1,设置PLL1的p分频系数,设置值需减1
		RCC->PLL1DIVR|=(pllq-1)<<16;//DIVQ1[6:0]=pllq-1,设置PLL1的q分频系数,设置值需减1
		RCC->PLL1DIVR|=1<<24;		//DIVR1[6:0]=pllr-1,设置PLL1的r分频系数,设置值需减1,r分频出来的时钟没用到
		RCC->PLLCFGR|=2<<2;			//PLL1RGE[1:0]=2,PLL1输入时钟频率在4~8Mhz之间(25/5=5Mhz),如修改pllm,请确认此参数
		RCC->PLLCFGR|=0<<1;			//PLL1VCOSEL=0,PLL1宽的VCO范围,192~836Mhz
		RCC->PLLCFGR|=3<<16;		//DIVP1EN=1,DIVQ1EN=1,使能pll1_p_ck和pll1_q_ck
		RCC->CR|=1<<24;				//PLL1ON=1,使能PLL1
		while((RCC->CR&(1<<25))==0);//PLL1RDY=1?,等待PLL1准备好  
	
		//设置PLL2的R分频输出,为220Mhz,后续做SDRAM时钟,可得到110M的SDRAM时钟频率
		RCC->PLLCKSELR|=25<<12;		//DIVM2[5:0]=25,设置PLL2的预分频系数
		RCC->PLL2DIVR|=(440-1)<<0;	//DIVN2[8:0]=440-1,设置PLL2的倍频系数,设置值需减1
		RCC->PLL2DIVR|=(2-1)<<9;	//DIVP2[6:0]=2-1,设置PLL2的p分频系数,设置值需减1
		RCC->PLL2DIVR|=(2-1)<<24;	//DIVR2[6:0]=2-1,设置PLL2的r分频系数,设置值需减1
		RCC->PLLCFGR|=0<<6;			//PLL2RGE[1:0]=0,PLL2输入时钟频率在1~2Mhz之间(25/25=1Mhz)
		RCC->PLLCFGR|=0<<5;			//PLL2VCOSEL=0,PLL2宽的VCO范围,192~836Mhz
		RCC->PLLCFGR|=1<<19;		//DIVP2EN=1,使能pll2_p_ck
		RCC->PLLCFGR|=1<<21;		//DIVR2EN=1,使能pll2_r_ck
		RCC->D1CCIPR&=~(3<<0);		//清除FMCSEL[1:0]原来的设置
		RCC->D1CCIPR|=2<<0;			//FMCSEL[1:0]=2,FMC时钟来自于pll2_r_ck		
		RCC->CR|=1<<26;				//PLL2ON=1,使能PLL2
		while((RCC->CR&(1<<27))==0);//PLL2RDY=1?,等待PLL2准备好  
	
		RCC->D1CFGR|=8<<0;			//HREF[3:0]=8,rcc_hclk1/2/3/4=sys_d1cpre_ck/2=400/2=200Mhz,即AHB1/2/3/4=200Mhz
		RCC->D1CFGR|=0<<8;			//D1CPRE[2:0]=0,sys_d1cpre_ck=sys_clk/1=400/1=400Mhz,即CPU时钟=400Mhz
		RCC->CFGR|=3<<0;			//SW[2:0]=3,系统时钟(sys_clk)选择来自pll1_p_ck,即400Mhz
		while(1)
		{
			retry=(RCC->CFGR&(7<<3))>>3;	//获取SWS[2:0]的状态,判断是否切换成功
			if(retry==3)break;		//成功将系统时钟源切换为pll1_p_ck
		}
			
		FLASH->ACR|=2<<0;			//LATENCY[2:0]=2,2个CPU等待周期(@VOS1 Level,maxclock=210Mhz)
		FLASH->ACR|=2<<4;			//WRHIGHFREQ[1:0]=2,flash访问频率<285Mhz
		
		RCC->D1CFGR|=4<<4;			//D1PPRE[2:0]=4,rcc_pclk3=rcc_hclk3/2=100Mhz,即APB3=100Mhz
		RCC->D2CFGR|=4<<4;			//D2PPRE1[2:0]=4,rcc_pclk1=rcc_hclk1/2=100Mhz,即APB1=100Mhz
		RCC->D2CFGR|=4<<8;			//D2PPRE2[2:0]=4,rcc_pclk2=rcc_hclk1/2=100Mhz,即APB2=100Mhz
		RCC->D3CFGR|=4<<4;			//D3PPRE[2:0]=4,rcc_pclk4=rcc_hclk4/2=100Mhz,即APB4=100Mhz
 
		RCC->CR|=1<<7;				//CSION=1,使能CSI,为IO补偿单元提供时钟
		RCC->APB4ENR|=1<<1;			//SYSCFGEN=1,使能SYSCFG时钟
		SYSCFG->CCCSR|=1<<0;		//EN=1,使能IO补偿单元 
	} 
	return status;
}  
 
//系统时钟初始化函数
//plln:PLL1倍频系数(PLL倍频),取值范围:4~512.
//pllm:PLL1预分频系数(进PLL之前的分频),取值范围:2~63.
//pllp:PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟,取值范围:2~128.(且必须是2的倍数)
//pllq:PLL1的q分频系数(PLL之后的分频),取值范围:2~128.
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{  
	
	RCC->CR=0x00000001;				//设置HISON,开启内部高速RC振荡，其他位全清零
	RCC->CFGR=0x00000000;			//CFGR清零 
	RCC->D1CFGR=0x00000000;			//D1CFGR清零 
	RCC->D2CFGR=0x00000000;			//D2CFGR清零 
	RCC->D3CFGR=0x00000000;			//D3CFGR清零 
	RCC->PLLCKSELR=0x00000000;		//PLLCKSELR清零 
	RCC->PLLCFGR=0x00000000;		//PLLCFGR清零 
	RCC->CIER=0x00000000;			//CIER清零,禁止所有RCC相关中断  
	//AXI_TARG7_FN_MOD寄存器,由于没有在stm32h750xx.h里面定义,所以,只能用直接
	//操作地址的方式,来修改,该寄存器在<<STM32H750参考手册>>第113页,AXI_TARGx_FN_MOD
	*((vu32*)0x51008108)=0x00000001;//设置AXI SRAM的矩阵读取能力为1 
	Sys_Clock_Set(plln,pllm,pllp,pllq);//设置时钟  
}


// 判断I_Cache是否打开
// 返回值:0 关闭，1 打开
u8 Get_ICahceSta(void)
{
	u8 sta;
	sta = ((SCB->CCR) >> 17) & 0X01;
	return sta;
}

// 判断I_Dache是否打开
// 返回值:0 关闭，1 打开
u8 Get_DCahceSta(void)
{
	u8 sta;
	sta = ((SCB->CCR) >> 16) & 0X01;
	return sta;
}

// 系统软复位
void Sys_Soft_Reset(void)
{
	SCB->AIRCR = 0X05FA0000 | (u32)0x04;
}

/**
 * @brief MCO1的测试
 *
 */
void MCO_test(void)
{
	RCC->CFGR |= 2 << 22; // 选择hse为时钟源
						  // 这里不分频
}
/*这里选择的是MCO1*/
// PA8的AF0
void MCO_test_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_Struct = {0};
	GPIO_Struct.Mode = GPIO_MODE_AF_PP; // 复用模式
	GPIO_Struct.Pin = GPIO_PIN_8;
	GPIO_Struct.Pull = GPIO_NOPULL;
	GPIO_Struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 高速
	GPIO_Struct.Alternate = GPIO_AF0_MCO;
	HAL_GPIO_Init(GPIOA, &GPIO_Struct);
}

/*调转到DFU下载部分*/
void JumpToBootloader(void)
{
     uint32_t i=0;
     void (*SysMemBootJump)(void); /* 声明一个函数指针 */
     __IO uint32_t BootAddr = 0x1FF09800; /* STM32H7 的系统 BootLoader 地址 */

     /* 关闭全局中断 */
     DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();

     /* 关闭滴答定时器，复位到默认值 */
     SysTick->CTRL = 0;
     SysTick->LOAD = 0;
     SysTick->VAL = 0;

     /* 设置所有时钟到默认状态，使用 HSI 时钟 */
     HAL_RCC_DeInit();

     /* 关闭所有中断，清除所有中断挂起标志 */
     for (i = 0; i < 8; i++)
     {
     NVIC->ICER[i]=0xFFFFFFFF;
     NVIC->ICPR[i]=0xFFFFFFFF;
     }

     /* 使能全局中断 */
     ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
     
     /* 跳转到系统 BootLoader，首地址是 MSP，地址+4 是复位中断服务程序地址 */
     SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

     /* 设置主堆栈指针 */
     __set_MSP(*(uint32_t *)BootAddr);

     /* 在 RTOS 工程，这条语句很重要，设置为特权级模式，使用 MSP 指针 */
     __set_CONTROL(0);

     /* 跳转到系统 BootLoader */
     SysMemBootJump(); 
     /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
     while (1);
}

// 设置栈顶地址
// addr:栈顶地址
void MSR_MSP(u32 addr)
{
	__asm("MSR MSP, r0"); // set Main Stack value
	__asm("BX r14");
}

/*设置中断向量表位置*/
//物理地址上也存在中断向量表
void set_votor(uint32_t addr)
{
    uint32_t * src_addr=(uint32_t*)0x08000000;
    uint32_t * dest_addr=(uint32_t*)addr;
    memcpy(dest_addr,src_addr,0x400);
    SET_VTOR_ADD(addr);
}