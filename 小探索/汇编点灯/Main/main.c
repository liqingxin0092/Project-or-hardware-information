
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
	
	/* 1. ʹ��GPIOB��ʱ��
	 * GPIO�ǹ�����APB2�ϵģ���������Ҫʹ��APB2������ʱ��ʹ��λ
	 * оƬ�ֲ�113ҳ RCC_APB2ENR�Ĵ������������ǿ��Կ���APB2ENR[8:2]��GPIO[G:A]ʱ�ӵĿ���λ������GPIOʱ����ʧ�ܻ���ʹ��
	 * �������ϵ�LED��GPIO����ʹ�õ�GPIOB����Ҫʹ��APB2ENR[3]λ��Ϊ�丳ֵ��1�����ɴ�GPIOB��ʱ�� */
	*pRccApb2Enr |= 1<<4;

	/* 2. ����GPIOΪ�������:
	 * MODE_0=1,PB0���ģʽ������������10MHz 
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

