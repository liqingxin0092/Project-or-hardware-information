
#define RCC_APB2ENR  (0x40021000 + 0x18)
#define GPIOC_BASE   (0x40011000)
#define GPIOC_CRL    (GPIOC_BASE + 4)
#define GPIOC_ODR    (GPIOC_BASE + 0X0c)

void delay(volatile int i)
{
	while (i--);
}

int main(void)
{
	volatile unsigned int *pRccApb2Enr;
	volatile unsigned int *pGpiocCrl;
	volatile unsigned int *pGpiocOdr;
	
	pRccApb2Enr = (volatile unsigned int *)RCC_APB2ENR;
	pGpiocCrl   = (volatile unsigned int *)GPIOC_CRL;
	pGpiocOdr   = (volatile unsigned int *)GPIOC_ODR;
	
	/* 1. 使能GPIOB的时钟
	 * GPIO是挂载在APB2上的，所以我们要使能APB2的外设时钟使能位
	 * 芯片手册113页 RCC_APB2ENR寄存器描述中我们可以看到APB2ENR[8:2]是GPIO[G:A]时钟的控制位，控制GPIO时钟是失能或者使能
	 * 开发板上的LED的GPIO组是使用的GPIOB，即要使能APB2ENR[3]位，为其赋值“1”即可打开GPIOB的时钟 */
	*pRccApb2Enr |= 1<<4;

	/* 2. 设置GPIO为输出功能:
	 * MODE_0=1,PB0输出模式，输出速率最大10MHz 
	 */
  *pGpiocCrl |= 1<<20;
	
	while (1)
	{
    *pGpiocOdr |= (1<<13);
		delay(100000);
    *pGpiocOdr &= ~(1<<13);
		delay(100000);
	}
}

