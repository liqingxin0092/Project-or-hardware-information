/**
 *******************************************************************************
 * @file        H7_CORE.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-14
 * @brief       CMSIS�㼶����
 *              ������h7ϵͳ���Ĵ���
 *****************************************************************************************
 */

#include "H7_CORE.h"
#include "stdio.h"

/*��ȡipsr��Ϣ*/
void show_ipsr_info(void)
{
    uint32_t ulCurrentInterrupt;//                 д              ��
    __asm volatile ( "mrs %0, ipsr" : "=r" ( ulCurrentInterrupt ):    :"memory" );
    printf("\r\n�쳣���(0��ʾ�߳�ģʽ):%d\r\n",ulCurrentInterrupt);
}

/*����CONTROL�Ĵ���*/
//����ģʽ�²�������ջָ��,�̶��õ�MSP
//00:msp,��Ȩ.  01:msp ,�û�. 10:psp,��Ȩ. 11:psp,�û�
void set_control(uint32_t value)
{
    __asm volatile( "msr control, %0" : : "r"(value)   :"memory" );
    __asm volatile("isb");
}

/*չʾCONTROL�Ĵ�����Ϣ*/
void show_CONTROL_info(void)
{
    uint32_t val;
    __asm volatile ( "mrs %0, CONTROL" : "=r" ( val ):    :"memory" );
    printf("\r\n������CONTROL�Ĵ�����Ϣ:\r\n");
    if((val>>2)&1)
        printf("���㵥Ԫ�����Ĵ���\r\n");
    else
        printf("���㵥Ԫ�����Ĳ�����\r\n");
    if((val>>1)&1)
        printf("��ǰ�õ�PSP\r\n");
    else
        printf("��ǰ�õ�MSP\r\n");
    if(val&1)
        printf("��ǰ����ȼ�Ϊ����Ȩ\r\n");
    else
        printf("��ǰ����ȼ�Ϊ��Ȩ\r\n");
}
    

/*��ʾCPUID��Ϣ*/
void SCB_CPUID_show_info(void)
{
    printf("\r\n����(ʵ����):%#x\r\n", SCB_CPUID->Implementer);
    if (SCB_CPUID->Implementer == 0x41)
    {
        printf("����ARM\r\n");
    }
    printf("��������:%#x\r\n", SCB_CPUID->PartNo);
    if (SCB_CPUID->PartNo == 0xC27)
    {
        printf("����Cortex-M7\r\n");
    }
    printf("�汾��:%d\r\n", SCB_CPUID->Variant);
    printf("������ :%d\r\n", SCB_CPUID->Revision);
    printf("r%dp%d\r\n", SCB_CPUID->Variant, SCB_CPUID->Revision);
}

/*չʾSCB_ICSR�ṩ����Ϣ*/
// �ṩ����Ϣ���ֲ�׼ȷ����RETTOBASE,����״̬��־
void SCB_ICSR_show_info(void)
{
    volatile scb_icsr_t temp = *SCB_ICSR;
    if (temp.VECTACTIVE)
    {
        printf("\r\n��ִ�е��쳣���:%d\r\n", temp.VECTACTIVE);
    }
    else
    {
        printf("����û���ж�ִ��\r\n");
    }
    if (temp.RETTOBASE)
    {
        printf("û�з����ж���ռ��Ϊ\r\n");
    }
    else
    {
        printf("�������ж���ռ��Ϊ\r\n");
    }
    if (temp.VECTPENDING)
    {
        printf("��������쳣���:%d\r\n", temp.VECTPENDING);
    }
    else
    {
        printf("û�д�������ж�\r\n");
    }
    if (temp.ISRPENDING)
    {
        printf("���жϹ���,(������NMI,FAULT)\r\n");
    }
    else
    {
        printf("û���жϹ���,(������NMI,FAULT)\r\n");
    }
    if (temp.PENDSTSET)
    {
        printf("systic�жϹ�����\r\n");
    }
    else
    {
        printf("systic�ж�û�й���\r\n");
    }
    if (temp.PENDSVSET)
    {
        printf("PendSV�жϹ�����\r\n");
    }
    else
    {
        printf("PendSVû�й���\r\n");
    }
    if (temp.NMIPENDSET)
    {
        printf("NMI�жϹ�����\r\n");
    }
    else
    {
        printf("NMI�ж�û�й���\r\n");
    }
}

/*չʾ�쳣��Ϣ*/
void SCB_show_fault_info(void)
{
    printf("\r\nSCB_SHCSR����ֵ:%#x\r\n", *(uint32_t *)SCB_SHCSR);
    if (SCB_SHCSR->MEMFAULTACT)
        printf("MemManage exception����ִ��\r\n");
    if (SCB_SHCSR->BUSFAULTACT)
        printf("BusFault exception����ִ��\r\n");
    if (SCB_SHCSR->USGFAULTACT)
        printf("UsageFault exception����ִ��\r\n");
    if (SCB_SHCSR->SVCALLACT)
        printf("SVCall����ִ��\r\n");
    if (SCB_SHCSR->MONITORACT)
        printf("Debug monitor����ִ��\r\n");
    if (SCB_SHCSR->PENDSVACT)
        printf("pendSV����ִ��\r\n");
    if (SCB_SHCSR->SYSTICKACT)
        printf("SysTick handle����ִ��\r\n");
    if (SCB_SHCSR->USGFAULTPENDED)
        printf("UsageFault exception������\r\n");
    if (SCB_SHCSR->MEMFAULTPENDED)
        printf("MemManage exception������\r\n");
    if (SCB_SHCSR->BUSFAULTPENDED)
        printf("BusFault exception������\r\n");
    if (SCB_SHCSR->SVCALLPENDED)
        printf("SVCall������\r\n");
}

/*չʾ��΢��ϸ�����Ϣ*/
// ����쳣�ֲ�
void SCB_show_detailed_info(void)
{
    uint8_t flag = 0;
    uint32_t mm_add = *SCB_MMAR;
    uint32_t bus_add = *SCB_BFAR;
    volatile scb_cfsr_u * temp = SCB_CFSR;
    if (temp->mini_struct.MMFSR)
    {
        flag = 1;
        printf("\r\n�������ڴ������Ĵ���\r\n");
        printf("������ϸ���ݼ��쳣�ֲ�\r\n");
        if (temp->bit_struct.IACCVIOL)
            printf("���������ԴӲ�����ִ�е�λ�û�ȡָ��\r\n");
        if (temp->bit_struct.DACCVIOL)
            printf("�����������ڲ����������λ�ý��м��ػ�洢\r\n");
        if (temp->bit_struct.MUNSTKERR)
            printf("�쳣����ʱ�ĳ�ջ���·��ʳ�ͻ\r\n");
        if (temp->bit_struct.MSTKERR)
            printf("�쳣����ʱ����ջ���·��ʳ�ͻ\r\n");
        if (temp->bit_struct.MLSPERR)
            printf("�������ѹջ�ڼ�����ڴ�������\r\n");
        if (temp->bit_struct.MMARVALID)
            printf("MMAR�Ĵ����д�����Ч�����ַ,ֵΪ:%#x\r\n", mm_add);
    }
    if (temp->mini_struct.BFSR)
    {
        flag = 1;
        printf("\r\n���������ߴ���\r\n");
        printf("������ϸ���ݼ��쳣�ֲ�\r\n");
        if (temp->bit_struct.IBUSERR)
        {
            printf("ָ�����ߴ���\r\n");
            printf("�����Ǵ���ĺ���ָ����ת\r\n");
            printf("������Ч�ķ��أ���Ϊջ����\r\n");
        }
        if (temp->bit_struct.PRECISERR)
            printf("��ȷ���������ߴ���\r\n");
        if (temp->bit_struct.IMPRECISERR)
            printf("����ȷ���������ߴ���\r\n");
        if (temp->bit_struct.UNSTKERR)
            printf("�쳣����ʱ�ĳ�ջ������һ���������ߴ���\r\n");
        if (temp->bit_struct.STKERR)
            printf("�쳣����ʱ��ѹջ������һ���������ߴ���\r\n");
        if (temp->bit_struct.LSPERR)
            printf("һ�����ߴ������ڸ������ѹջ�ڼ�\r\n");
        if (temp->bit_struct.BFARVALID)
            printf("BFAR�д�����Ч�Ĵ����ַ:%#x\r\n", bus_add);
    }
    if (temp->mini_struct.UFSR)
    {
        flag = 1;
        printf("\r\n�������÷�����\r\n");
        printf("������ϸ���ݼ��쳣�ֲ�\r\n");
        if (temp->bit_struct.UNDEFINSTR)
            printf("����������ִ��δ�����ָ��\r\n");
        if (temp->bit_struct.INVSTATE)
        {
            printf("����������ִ�зǷ�ʹ��EPSR��ָ��\r\n");
            printf("���߲���thumb״̬,ָ��ص�ַlsb==0��\r\n");
        }
        if (temp->bit_struct.INVPC)
            printf("�쳣��ջʱ,EXC_RETURNֵ��Ч\r\n");
        if (temp->bit_struct.NOCP)
            printf("���������Է���Э������\r\n");
        if (temp->bit_struct.UNALIGNED)
            printf("������������δ������ڴ����\r\n");
        if (temp->bit_struct.DIVBYZERO)
            printf("�����˳�0����\r\n");
    }
    if (SCB_HFSR->VECTTBL)
    {
        flag = 1;
        printf("\r\n������Ӳ������\r\n");
        printf("�쳣����ʱ�������������ߴ���\r\n");
    }
    if (SCB_HFSR->FORCED)
    {
        flag = 1;
        printf("\r\n������Ӳ������\r\n");
        printf("�����������������������\r\n");
    }

    /*չʾ����ABFSR����Ϣ*/
    switch (AC_ABFSR->AXIMTYPE)
    {
    case 0:
        printf("AC_ABFSRָʾ��Ϣ:\r\n");
        printf("\r\n���ܴ��ڵط�Ƿ��\r\n");
        break;
    case 1:
        printf("AC_ABFSRָʾ��Ϣ:\r\n");
        printf("��ռ���ʳɹ�\r\n");
        break;
    case 2:
        printf("AC_ABFSRָʾ��Ϣ:\r\n");
        printf("�ӻ�����\r\n");
        flag = 1;
        break;
    case 3:
        printf("AC_ABFSRָʾ��Ϣ:\r\n");
        printf("�������\r\n");
        flag = 1;
        break;
    }
    if (AC_ABFSR->ITCM)
        printf("���ߴ��������ITCM�ӿڴ�\r\n");
    if (AC_ABFSR->DTCM)
        printf("���ߴ��������DTCM�ӿڴ�\r\n");
    if (AC_ABFSR->AHBP)
        printf("���ߴ��������AHBP�ӿڴ�\r\n");
    if (AC_ABFSR->AXIM)
        printf("���ߴ��������AXIM�ӿڴ�\r\n");
    if (AC_ABFSR->EPPB)
        printf("���ߴ��������EPPB�ӿڴ�\r\n");
    if (!flag)
        printf("\r\nû�д�����\r\n");
}

/*չʾFPCCR����Ϣ*/
void FPU_CCR_show_info(void)
{
    printf("\r\nչʾFPCCR����Ϣ:\r\n");
    if(FPU_FPCCR->LSPACT)    
    {
        printf("�������ѹջ���ڻ�Ծ״̬,����ջ֡�Ѿ�������\r\n");
        printf("�����ջ��δ��両��Ĵ����ĵ�ַ%#x\r\n",__GET_FPU_STACK_ADDRESS());
    }
    else
        printf("�������ѹջû�н���\r\n");
    printf("������ջ֡������ʱ:");
    if(FPU_FPCCR-> USER)
        printf("��������û���\r\n");
    else
        printf("���������Ȩ��\r\n");
    if(FPU_FPCCR-> THREAD)
        printf("��������߳�ģʽ\r\n");
    else
        printf("������ڴ���ģʽ\r\n");
    if(FPU_FPCCR-> HFRDY)
        printf("Ӳ�������������\r\n");
    else
        printf("Ӳ�������������\r\n");
    if(FPU_FPCCR->MMRDY)
        printf("�ڴ�����쳣�������\r\n");
    else
        printf("�ڴ�����쳣���������\r\n");
    if(FPU_FPCCR->BFRDY)
        printf("���ߴ����쳣�������\r\n");
    else
        printf("���ߴ����쳣���������\r\n");
    if(FPU_FPCCR->MONRDY)
        printf("debug�����쳣�������\r\n");
    else
        printf("debug�����쳣���������\r\n");
}
    

// �����ж��ڱ��õ�ʱ��ע��
void NMI_Handler(void)
{
    printf("������NMI�ж�\r\n");
}

/*���õ���������ڴ�����*/
//region_code:0~15,�����
//sub_dis:8λ,д1���õ���1/8���������,��λ��Ӧ��λ,
//        ��λ��Ӧ��λ,�Ӷ���¶����֮�ص����������
//        256 bytes��С�����ϳߴ�������ô�λ.
static void mpu_set_region(uint8_t region_code,uint32_t base_addr,mpu_attribute_t* mpu_para,uint8_t sub_dis,uint32_t size)
{
//    MPU_RASR->ENABLE=0;//ʧ������λ
    *MPU_RNR=region_code;//���������
    *(uint32_t*)MPU_RBAR=base_addr;//���û���ַ
    MPU_RASR->SRD =sub_dis;//���ý���������
    MPU_RASR->SIZE=size;//���óߴ�
    MPU_RASR->XN=mpu_para->XN;
    MPU_RASR->AP=mpu_para->AP;
    MPU_RASR->TEX=mpu_para->TEX;
    MPU_RASR->C=mpu_para->C;
    MPU_RASR->B=mpu_para->B;
    MPU_RASR->S=mpu_para->S;
    MPU_RASR->ENABLE=1;//ʹ��������������
}

/*��������MPU*/
//Ӧ�ü��ĺ���
//��������8~15�ĺ���Ҳ������������,����16Ҳ��������,ע�����.
void mpu_set(void)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    mpu_attribute_t mpu_struct={0};
    DISABLE_MPU();//ʧ��MPU
    mpu_struct.AP=3;//ȫ����
    mpu_struct.XN=0;//����ָ�����,1���ǽ�ֹ
    mpu_struct.TEX=0;//1����
    mpu_struct.B=1;//������
    mpu_struct.C=1;//������
    mpu_struct.S=0;//��ֹ����
    mpu_set_region(0,0x20000000,&mpu_struct,0,MPU_REGION_SIZE_64_KBYTES);//��������DTCM,��64K�ֽ�
    mpu_set_region(1,0x20010000,&mpu_struct,0,MPU_REGION_SIZE_256_KBYTES);//��������SRAM1/SRAM2,��256K�ֽ�
    mpu_set_region(2,0x00000000,&mpu_struct,0,MPU_REGION_SIZE_16_KBYTES);//��������ITCM,��16K�ֽ�
    
    ENABLE_DEFAULT_MEMORY_MAP();//��������ʹ��
    ENABLE_MPU();//ʹ��MPU
    __asm volatile("dsb");
    __asm volatile("isb");
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
}

/*Ӧ�ü��������û���*/
void cache_set(void)
{
    enable_I_Cache();
    enable_D_Cache();
}

//void PendSV_Handler(void)
//{
//    printf("������PendSV�ж�\r\n");
//    SCB_ICSR_show_info();  // չʾSCB_ICSR�ṩ����Ϣ
//    SCB_show_fault_info(); // չʾSCB_SHCSR��Ϣ
//    CLEAR_PENDSV_STATE();  // �����־λ
//}

void BusFault_Handler(void)
{
    SCB_show_detailed_info();
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    SCB_show_detailed_info();
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

void MemManage_Handler(void)
{ 
    SCB_show_detailed_info();
	printf("Mem Access Error!!\r\n"); 	//���������Ϣ
	while(1);
}
