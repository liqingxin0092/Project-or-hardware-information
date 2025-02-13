#include "mpu.h" 
#include "usart.h" 
#include "delay.h" 
#include "stdio.h"
//��ֹMPU����
void MPU_Disable(void)
{  
	SCB->SHCSR&=~(1<<16);	//��ֹMemManage 
	MPU->CTRL&=~(1<<0);		//��ֹMPU
}

//����MPU����
void MPU_Enable(void)
{ 
	MPU->CTRL=7;//ʹ��PRIVDEFENA,ʹ��MPU,HFNMIENA:Enables the operation of MPU during hard fault, NMI, and FAULTMASK handlers. 
	SCB->SHCSR|=1<<16;		//ʹ��MemManage
}

//��nbytesת��Ϊ2Ϊ�׵�ָ��.
//NumberOfBytes:�ֽ���.
//����ֵ:��2Ϊ�׵�ָ��ֵ
static u8 MPU_Convert_Bytes_To_POT(u32 nbytes)
{
	u8 count=0;
	while(nbytes!=1)
	{
		nbytes>>=1;
		count++;
	}
	return count;
} 

//����ĳ�������MPU����
//baseaddr:MPU��������Ļ�ַ(�׵�ַ)
//size:MPU��������Ĵ�С(������32�ı���,��λΪ�ֽ�)
//rnum:MPU���������,��Χ:0~7,���֧��8����������
//de:��ָֹ�����;0,����ָ�����;1,��ָֹ�����
//ap:����Ȩ��,���ʹ�ϵ����:
//0,�޷��ʣ���Ȩ&�û������ɷ��ʣ�
//1,��֧����Ȩ��д����
//2,��ֹ�û�д���ʣ���Ȩ�ɶ�д���ʣ�
//3,ȫ���ʣ���Ȩ&�û����ɷ��ʣ�
//4,�޷�Ԥ��(��ֹ����Ϊ4!!!)
//5,��֧����Ȩ������
//6,ֻ������Ȩ&�û���������д��
//���:STM32H7����ֲ�.pdf,4.6.6��,Table 91.
//sen:�Ƿ�������;0,������;1,����
//cen:�Ƿ�����cache;0,������;1,����
//ben:�Ƿ�������;0,������;1,����
//����ֵ;0,�ɹ�.
//    ����,����.
u8 MPU_Set_Protection(u32 baseaddr,u32 size,u32 rnum,u8 de,u8 ap,u8 sen,u8 cen,u8 ben)
{
	u32 tempreg=0;
	u8 rnr=0;
	if((size%32)||size==0)return 1;	//��С����32�ı���,����sizeΪ0,˵����������
	rnr=MPU_Convert_Bytes_To_POT(size)-1;//ת��Ϊ2Ϊ�׵�ָ��ֵ
	MPU_Disable();					//����֮ǰ,�Ƚ�ֹMPU����
	MPU->RBAR=baseaddr|0x10|rnum;				//���û�ַ, VALID bit set to 1,���
	tempreg|=((u32)de)<<28;			//��ֹ/����ָ�����(��ֹ/�����ȡָ��)
	tempreg|=((u32)ap)<<24;			//���÷���Ȩ��,
//	tempreg|=0<<19;					//����������չ��Ϊlevel0
	tempreg|=((u32)sen)<<18;		//�Ƿ�������
	tempreg|=((u32)cen)<<17;		//�Ƿ�����cache
	tempreg|=((u32)ben)<<16;		//�Ƿ�������
//	tempreg|=0<<8;					//��ֹ������
	tempreg|=rnr<<1;				//���ñ��������С
	tempreg|=1;					//ʹ�ܸñ������� 
    MPU->RASR=tempreg;				//����RASR�Ĵ���
	MPU_Enable();					//�������,ʹ��MPU����
	return 0;
}

//������Ҫ�����Ĵ洢��
//����Բ��ִ洢�������MPU����,������ܵ��³��������쳣
//����MCU������ʾ,����ͷ�ɼ����ݳ���ȵ�����...
void MPU_Memory_Protection(void)
{
	MPU_Set_Protection(0x20000000,64*1024,1,0,MPU_REGION_FULL_ACCESS,0,1,1);		//��������DTCM,��64K�ֽ�,����ָ�����,��ֹ����,����cache,������
	MPU_Set_Protection(0x20010000,256*1024,2,0,MPU_REGION_FULL_ACCESS,0,1,1);		//��������SRAM1/SRAM2,��256K�ֽ�,����ָ�����,��ֹ����,����cache,������
	MPU_Set_Protection(0x00000000,16*1024,3,0,MPU_REGION_FULL_ACCESS,0,1,1);		//��������ITCM,��16K�ֽ�,����ָ�����,��ֹ����,����cache,������ 

	MPU_Set_Protection(0x60000000,64*1024*1024,4,0,MPU_REGION_FULL_ACCESS,0,0,0);	//����MCU LCD�����ڵ�FMC����,,��64M�ֽ�,����ָ�����,��ֹ����,��ֹcache,��ֹ����
	MPU_Set_Protection(0XC0000000,64*1024*1024,5,0,MPU_REGION_FULL_ACCESS,0,1,1);	//����SDRAM����,��64M�ֽ�,����ָ�����,��ֹ����,����cache,������
	MPU_Set_Protection(0X80000000,256*1024*1024,6,1,MPU_REGION_FULL_ACCESS,0,0,0);	//��������NAND FLASH����,��256M�ֽ�,��ָֹ�����,��ֹ����,��ֹcache,��ֹ����
}
 
//MemManage�������ж�
//������ж��Ժ�,���޷��ָ���������!!
void MemManage_Handler(void)
{ 
	printf("Mem Access Error!!\r\n"); 	//���������Ϣ
	delay_ms(1000);	
	printf("Soft Reseting...\r\n");		//��ʾ�������
	delay_ms(1000);	
//	Sys_Soft_Reset();					//��λ
}














