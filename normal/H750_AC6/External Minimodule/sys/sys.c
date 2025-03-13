#include "sys.h"
#include "h7_core.h"
#include  "string.h"

//ʱ�����ú���
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCOƵ��
//Fsys:ϵͳʱ��Ƶ��,Ҳ��PLL1��p��Ƶ���ʱ��Ƶ��
//Fq:PLL1��q��Ƶ���ʱ��Ƶ��
//Fs:PLL����ʱ��Ƶ��,������HSI,CSI,HSE��. 

//plln:PLL1��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:4~512.
//pllm:PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��,ȡֵ��Χ:2~128.(�ұ�����2�ı���)
//pllq:PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:1~128.

//CPUƵ��(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//pll2_p_ck=(25/25)*440/2)=220Mhz
//pll2_r_ck=FMCʱ��Ƶ��=((25/25)*440/2)=110Mhz

//�ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=160,pllm=5,pllp=2,pllq=4.
//�õ�:Fvco=25*(160/5)=800Mhz
//     Fsys=pll1_p_ck=800/2=400Mhz
//     Fq=pll1_q_ck=800/4=200Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
static u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{ 
	u16 retry=0;
	u8 status=0;
	
	PWR->CR3&=~(1<<2);				//SCUEN=0,����LDOEN��BYPASSλ������
	PWR->D3CR|=3<<14;				//VOS=3,Scale1,1.15~1.26V�ں˵�ѹ,FLASH���ʿ��Եõ��������
	while((PWR->D3CR&(1<<13))==0);	//�ȴ���ѹ�ȶ� 
	RCC->CR|=1<<16;					//HSEON=1,����HSE
	while(((RCC->CR&(1<<17))==0)&&(retry<0X7FFF))retry++;//�ȴ�HSE RDY
	if(retry==0X7FFF)status=1;		//HSE�޷�����
	else   
	{
		RCC->PLLCKSELR|=2<<0;		//PLLSRC[1:0]=2,ѡ��HSE��ΪPLL������ʱ��Դ
		RCC->PLLCKSELR|=pllm<<4;	//DIVM1[5:0]=pllm,����PLL1��Ԥ��Ƶϵ��
		RCC->PLL1DIVR|=(plln-1)<<0;	//DIVN1[8:0]=plln-1,����PLL1�ı�Ƶϵ��,����ֵ���1
		RCC->PLL1DIVR|=(pllp-1)<<9;	//DIVP1[6:0]=pllp-1,����PLL1��p��Ƶϵ��,����ֵ���1
		RCC->PLL1DIVR|=(pllq-1)<<16;//DIVQ1[6:0]=pllq-1,����PLL1��q��Ƶϵ��,����ֵ���1
		RCC->PLL1DIVR|=1<<24;		//DIVR1[6:0]=pllr-1,����PLL1��r��Ƶϵ��,����ֵ���1,r��Ƶ������ʱ��û�õ�
		RCC->PLLCFGR|=2<<2;			//PLL1RGE[1:0]=2,PLL1����ʱ��Ƶ����4~8Mhz֮��(25/5=5Mhz),���޸�pllm,��ȷ�ϴ˲���
		RCC->PLLCFGR|=0<<1;			//PLL1VCOSEL=0,PLL1���VCO��Χ,192~836Mhz
		RCC->PLLCFGR|=3<<16;		//DIVP1EN=1,DIVQ1EN=1,ʹ��pll1_p_ck��pll1_q_ck
		RCC->CR|=1<<24;				//PLL1ON=1,ʹ��PLL1
		while((RCC->CR&(1<<25))==0);//PLL1RDY=1?,�ȴ�PLL1׼����  
	
		//����PLL2��R��Ƶ���,Ϊ220Mhz,������SDRAMʱ��,�ɵõ�110M��SDRAMʱ��Ƶ��
		RCC->PLLCKSELR|=25<<12;		//DIVM2[5:0]=25,����PLL2��Ԥ��Ƶϵ��
		RCC->PLL2DIVR|=(440-1)<<0;	//DIVN2[8:0]=440-1,����PLL2�ı�Ƶϵ��,����ֵ���1
		RCC->PLL2DIVR|=(2-1)<<9;	//DIVP2[6:0]=2-1,����PLL2��p��Ƶϵ��,����ֵ���1
		RCC->PLL2DIVR|=(2-1)<<24;	//DIVR2[6:0]=2-1,����PLL2��r��Ƶϵ��,����ֵ���1
		RCC->PLLCFGR|=0<<6;			//PLL2RGE[1:0]=0,PLL2����ʱ��Ƶ����1~2Mhz֮��(25/25=1Mhz)
		RCC->PLLCFGR|=0<<5;			//PLL2VCOSEL=0,PLL2���VCO��Χ,192~836Mhz
		RCC->PLLCFGR|=1<<19;		//DIVP2EN=1,ʹ��pll2_p_ck
		RCC->PLLCFGR|=1<<21;		//DIVR2EN=1,ʹ��pll2_r_ck
		RCC->D1CCIPR&=~(3<<0);		//���FMCSEL[1:0]ԭ��������
		RCC->D1CCIPR|=2<<0;			//FMCSEL[1:0]=2,FMCʱ��������pll2_r_ck		
		RCC->CR|=1<<26;				//PLL2ON=1,ʹ��PLL2
		while((RCC->CR&(1<<27))==0);//PLL2RDY=1?,�ȴ�PLL2׼����  
	
		RCC->D1CFGR|=8<<0;			//HREF[3:0]=8,rcc_hclk1/2/3/4=sys_d1cpre_ck/2=400/2=200Mhz,��AHB1/2/3/4=200Mhz
		RCC->D1CFGR|=0<<8;			//D1CPRE[2:0]=0,sys_d1cpre_ck=sys_clk/1=400/1=400Mhz,��CPUʱ��=400Mhz
		RCC->CFGR|=3<<0;			//SW[2:0]=3,ϵͳʱ��(sys_clk)ѡ������pll1_p_ck,��400Mhz
		while(1)
		{
			retry=(RCC->CFGR&(7<<3))>>3;	//��ȡSWS[2:0]��״̬,�ж��Ƿ��л��ɹ�
			if(retry==3)break;		//�ɹ���ϵͳʱ��Դ�л�Ϊpll1_p_ck
		}
			
		FLASH->ACR|=2<<0;			//LATENCY[2:0]=2,2��CPU�ȴ�����(@VOS1 Level,maxclock=210Mhz)
		FLASH->ACR|=2<<4;			//WRHIGHFREQ[1:0]=2,flash����Ƶ��<285Mhz
		
		RCC->D1CFGR|=4<<4;			//D1PPRE[2:0]=4,rcc_pclk3=rcc_hclk3/2=100Mhz,��APB3=100Mhz
		RCC->D2CFGR|=4<<4;			//D2PPRE1[2:0]=4,rcc_pclk1=rcc_hclk1/2=100Mhz,��APB1=100Mhz
		RCC->D2CFGR|=4<<8;			//D2PPRE2[2:0]=4,rcc_pclk2=rcc_hclk1/2=100Mhz,��APB2=100Mhz
		RCC->D3CFGR|=4<<4;			//D3PPRE[2:0]=4,rcc_pclk4=rcc_hclk4/2=100Mhz,��APB4=100Mhz
 
		RCC->CR|=1<<7;				//CSION=1,ʹ��CSI,ΪIO������Ԫ�ṩʱ��
		RCC->APB4ENR|=1<<1;			//SYSCFGEN=1,ʹ��SYSCFGʱ��
		SYSCFG->CCCSR|=1<<0;		//EN=1,ʹ��IO������Ԫ 
	} 
	return status;
}  
 
//ϵͳʱ�ӳ�ʼ������
//plln:PLL1��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:4~512.
//pllm:PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��,ȡֵ��Χ:2~128.(�ұ�����2�ı���)
//pllq:PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~128.
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{  
	
	RCC->CR=0x00000001;				//����HISON,�����ڲ�����RC�񵴣�����λȫ����
	RCC->CFGR=0x00000000;			//CFGR���� 
	RCC->D1CFGR=0x00000000;			//D1CFGR���� 
	RCC->D2CFGR=0x00000000;			//D2CFGR���� 
	RCC->D3CFGR=0x00000000;			//D3CFGR���� 
	RCC->PLLCKSELR=0x00000000;		//PLLCKSELR���� 
	RCC->PLLCFGR=0x00000000;		//PLLCFGR���� 
	RCC->CIER=0x00000000;			//CIER����,��ֹ����RCC����ж�  
	//AXI_TARG7_FN_MOD�Ĵ���,����û����stm32h750xx.h���涨��,����,ֻ����ֱ��
	//������ַ�ķ�ʽ,���޸�,�üĴ�����<<STM32H750�ο��ֲ�>>��113ҳ,AXI_TARGx_FN_MOD
	*((vu32*)0x51008108)=0x00000001;//����AXI SRAM�ľ����ȡ����Ϊ1 
	Sys_Clock_Set(plln,pllm,pllp,pllq);//����ʱ��  
}


// �ж�I_Cache�Ƿ��
// ����ֵ:0 �رգ�1 ��
u8 Get_ICahceSta(void)
{
	u8 sta;
	sta = ((SCB->CCR) >> 17) & 0X01;
	return sta;
}

// �ж�I_Dache�Ƿ��
// ����ֵ:0 �رգ�1 ��
u8 Get_DCahceSta(void)
{
	u8 sta;
	sta = ((SCB->CCR) >> 16) & 0X01;
	return sta;
}

// ϵͳ��λ
void Sys_Soft_Reset(void)
{
	SCB->AIRCR = 0X05FA0000 | (u32)0x04;
}

/**
 * @brief MCO1�Ĳ���
 *
 */
void MCO_test(void)
{
	RCC->CFGR |= 2 << 22; // ѡ��hseΪʱ��Դ
						  // ���ﲻ��Ƶ
}
/*����ѡ�����MCO1*/
// PA8��AF0
void MCO_test_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_Struct = {0};
	GPIO_Struct.Mode = GPIO_MODE_AF_PP; // ����ģʽ
	GPIO_Struct.Pin = GPIO_PIN_8;
	GPIO_Struct.Pull = GPIO_NOPULL;
	GPIO_Struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // ����
	GPIO_Struct.Alternate = GPIO_AF0_MCO;
	HAL_GPIO_Init(GPIOA, &GPIO_Struct);
}

/*��ת��DFU���ز���*/
void JumpToBootloader(void)
{
     uint32_t i=0;
     void (*SysMemBootJump)(void); /* ����һ������ָ�� */
     __IO uint32_t BootAddr = 0x1FF09800; /* STM32H7 ��ϵͳ BootLoader ��ַ */

     /* �ر�ȫ���ж� */
     DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();

     /* �رյδ�ʱ������λ��Ĭ��ֵ */
     SysTick->CTRL = 0;
     SysTick->LOAD = 0;
     SysTick->VAL = 0;

     /* ��������ʱ�ӵ�Ĭ��״̬��ʹ�� HSI ʱ�� */
     HAL_RCC_DeInit();

     /* �ر������жϣ���������жϹ����־ */
     for (i = 0; i < 8; i++)
     {
     NVIC->ICER[i]=0xFFFFFFFF;
     NVIC->ICPR[i]=0xFFFFFFFF;
     }

     /* ʹ��ȫ���ж� */
     ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
     
     /* ��ת��ϵͳ BootLoader���׵�ַ�� MSP����ַ+4 �Ǹ�λ�жϷ�������ַ */
     SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

     /* ��������ջָ�� */
     __set_MSP(*(uint32_t *)BootAddr);

     /* �� RTOS ���̣�����������Ҫ������Ϊ��Ȩ��ģʽ��ʹ�� MSP ָ�� */
     __set_CONTROL(0);

     /* ��ת��ϵͳ BootLoader */
     SysMemBootJump(); 
     /* ��ת�ɹ��Ļ�������ִ�е�����û�������������Ӵ��� */
     while (1);
}

// ����ջ����ַ
// addr:ջ����ַ
void MSR_MSP(u32 addr)
{
	__asm("MSR MSP, r0"); // set Main Stack value
	__asm("BX r14");
}

/*�����ж�������λ��*/
//�����ַ��Ҳ�����ж�������
void set_votor(uint32_t addr)
{
    uint32_t * src_addr=(uint32_t*)0x08000000;
    uint32_t * dest_addr=(uint32_t*)addr;
    memcpy(dest_addr,src_addr,0x400);
    SET_VTOR_ADD(addr);
}