/**
 *******************************************************************************
 * @file        H7_core.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-14
 * @brief       CMSIS�㼶����
 *              ������h7ϵͳ���Ĵ���
 *****************************************************************************************
 */
#ifndef __M7_SCB_H
#define __M7_SCB_H

#include "stdint.h"
#include "parameter.h"
#include "stdio.h"

/*���ڼĴ�����ʵ��*/
//****��Щ�򵥵ļĴ�����û�б����,����FPU�����üĴ���.
//(1)����û�б��Processor features�е�Cache level ID register
// ���ṩ����Ϣ:ָ������ݲ����ʵ����һ��һ������.
//(2)Cache type registerû�б��
// ���ṩ����Ϣ:Smallest cache line of all the instruction caches :8 words
//             Smallest cache line of all the data and unified caches:8words
// ����ͬ��ԭ��:The local monitor within the processor does not hold any physical
//             address. It treats any STREX instruction access as matching the address of
//             the previous LDREX instruction.
// Cache Writeback Granule:8words, ArmV7�Ĵ�����ʽ.
//(3)Cache size ID register�� Cache size selection register�Ĵ��������˱��!!���Ҫ��
// һ���ṩ��Ϣ:���ֻ���ָ������ݶ�֧��,͸д,��д,д����,������.
// data cache set��:128,way��:4,line size:8words
// Instruction cache set��:256,way��:2,line size:8words
//(4)�δ�ʱ����У׼ָʾ�Ĵ���δ���.
//(5)MPU type registerδ���.
//   ָʾ��Ϣ: 16 MPU regions.
//     The Cortex-M7 MPU memory map is unified. This means instruction accesses and data 
//     accesses have same region settings
//(6)AHBPCR ,��Ĵ��������д�Ĳ���,������ָʾAHBP sizeΪ256MBytes
//(7)MPU�м��������Ĵ���,��֪�������û���.

/* ICSR bit assignments */
// �����жϺܺ�ʹ
typedef struct
{
    uint32_t VECTACTIVE : 9; // Contains the active exception number
    uint32_t reverse3 : 2;
    uint32_t RETTOBASE : 1;   // Indicates whether there are preempted active exceptions
    uint32_t VECTPENDING : 9; // Indicates the exception number of the highest priority pending enabled exception
    uint32_t reverse2 : 1;
    uint32_t ISRPENDING : 1; // Interrupt pending flag, excluding NMI and Faults
    uint32_t reverse1 : 2;
    uint32_t PENDSTCLR : 1; // SysTick exception clear-pending bit.1: Removes the pending state
    uint32_t PENDSTSET : 1; // SysTick exception set-pending bit,Ҳ���Կ�״̬
    uint32_t PENDSVCLR : 1; // PendSV clear-pending bit.1: Removes the pending state
    uint32_t PENDSVSET : 1; // PendSV set-pending bit,Ҳ���Կ�״̬
    uint32_t reverse0 : 2;
    uint32_t NMIPENDSET : 1; // NMI set-pending bit,Ҳ���Կ�״̬
} scb_icsr_t;

/*SCB �������ƼĴ���*/
// ����32λ��ֻʵ����ǰ13λ��Чλ
// �˼Ĵ�����صĹ���Ĭ�Ͽ���,д1�ر�
typedef struct
{
    uint32_t reserve0 : 2;
    uint32_t DISFOLD : 1; // Disables dual-issue
    uint32_t reserve1 : 7;
    uint32_t FPEXCODIS : 1;      // Disables FPU exception outputs
    uint32_t DISRAMODE : 1;      // Disables dynamic read allocate mode for Write-Back Write-Allocate memory regions
    uint32_t DISITMATBFLUSH : 1; // Disables ITM and DWT ATB flush
} scb_actlr_t;

/*SCB CPU ID�Ĵ���*/
// ����չʾ�汾��Ϣ
typedef struct
{
    uint32_t Revision : 4; // Patch
    uint32_t PartNo : 12;  // ������:Cortex-M7
    uint32_t reserve0 : 4;
    uint32_t Variant : 4;     // Revision
    uint32_t Implementer : 8; // Implementer code: Arm
} scb_cpuid_t;

/*Ӧ���жϺ͸�λ������*/
typedef struct
{
    uint32_t reserve0 : 2;
    uint32_t SYSRESETREQ : 1; // System reset request�������λ
    uint32_t reserve1 : 5;
    uint32_t PRIGROUP : 3; // Interrupt priority grouping field,�ж����ȼ�����
    uint32_t reserve2 : 4;
    uint32_t ENDIANNESS : 1; // ָʾС��,ֻ��
    uint32_t VECTKEY : 16;   // KEY
} scb_aircr_t;

/*ϵͳ���ƼĴ���*/
typedef struct
{
    uint32_t reverse0 : 1;
    uint32_t SLEEPONEXIT : 1; // ���жϷ��ص��߳�ģʽʱ�Ƿ�˯��
    uint32_t SLEEPDEEP : 1;   // ѡ����ͨ˯��OR deep sleep
    uint32_t reverse1 : 1;
    uint32_t SEVONPEND : 1; // ���õ��ж�Ҳ���Ի��Ѵ�����
} scb_scr_t;

/*���úͿ��ƼĴ���*/
typedef struct
{
    uint32_t NONBASETHRDENA : 1; // ��˵���жϽ�����ǿ�ƽ����߳�ģʽ
    uint32_t USERSETMPEND : 1;   // �������Ȩ������� STIR
    uint32_t reverse0 : 1;
    uint32_t UNALIGN_TRP : 1; // ����Ƕ�����ʴ���
    uint32_t DIV_0_TRP : 1;   // �����0����
    uint32_t reverse1 : 3;
    uint32_t BFHFNMIGN : 1; // Enables handlers with priority -1 or -2 to ignore dat
                            // a BusFaults caused by load and store instructions
    uint32_t STKALIGN : 1;  // ֻ��,��Ӳ��ʹ��,���쳣����ʱ�ָ�8�ֽڶ���
    uint32_t reverse2 : 6;
    uint32_t DC : 1; // ʹ������cache
    uint32_t IC : 1; // ʹ��ָ��cache
    uint32_t BP : 1; // ֻ��,ָʾ���÷�֧Ԥ��
    uint32_t reverse3 : 13;
} scb_ccr_t;

/*ϵͳ�ж����ȼ��Ĵ���1*/
typedef struct
{
    uint32_t PRI_4 : 8; // MemManage�ж����ȼ�
    uint32_t PRI_5 : 8; // BusFault�ж����ȼ�
    uint32_t PRI_6 : 8; // UsageFault�ж����ȼ�
} scb_shpr1_t;

/*ϵͳ�ж����ȼ��Ĵ���2*/
typedef struct
{
    uint32_t reverse : 24;
    uint32_t PRI_11 : 8; // SVCall�ж����ȼ�
} scb_shpr2_t;

/*ϵͳ�ж����ȼ��Ĵ���3*/
typedef struct
{
    uint32_t reverse : 16;
    uint32_t PRI_14 : 8; // PendSV�ж����ȼ�
    uint32_t PRI_15 : 8; // Systic�ж����ȼ�
} scb_shpr3_t;

/*ϵͳ�жϿ��ƺ�״̬�Ĵ���*/
typedef struct
{
    uint32_t MEMFAULTACT : 1; // MemManage exception active bit
    uint32_t BUSFAULTACT : 1; // BusFault exception active bit
    uint32_t reserve0 : 1;
    uint32_t USGFAULTACT : 1; // UsageFault exception active bit
    uint32_t reserve1 : 3;
    uint32_t SVCALLACT : 1;  // SVCall active bit
    uint32_t MONITORACT : 1; // Debug monitor active bit
    uint32_t reserve2 : 1;
    uint32_t PENDSVACT : 1;      // PendSV exception active bit
    uint32_t SYSTICKACT : 1;     // SysTick exception active bit
    uint32_t USGFAULTPENDED : 1; // UsageFault exception pending bit
    uint32_t MEMFAULTPENDED : 1; // MemManage exception pending bit,
    uint32_t BUSFAULTPENDED : 1; // BusFault exception pending bit
    uint32_t SVCALLPENDED : 1;   // SVCall pending bit,
    uint32_t MEMFAULTENA : 1;    // MemManage enable bit
    uint32_t BUSFAULTENA : 1;    // BusFault enable bit
    uint32_t USGFAULTENA : 1;    // UsageFault enable bit
} scb_shcsr_t;

/*�����õĹ���״̬�Ĵ���*/
typedef struct
{
    /*MemManage fault status register*/
    uint32_t IACCVIOL : 1;
    uint32_t DACCVIOL : 1;
    uint32_t reserve0 : 1;
    uint32_t MUNSTKERR : 1;
    uint32_t MSTKERR : 1;
    uint32_t MLSPERR : 1;
    uint32_t reserve1 : 1;
    uint32_t MMARVALID : 1;

    /*BusFault status register*/
    uint32_t IBUSERR : 1;
    uint32_t PRECISERR : 1;
    uint32_t IMPRECISERR : 1;
    uint32_t UNSTKERR : 1;
    uint32_t STKERR : 1;
    uint32_t LSPERR : 1;
    uint32_t reserve2 : 1;
    uint32_t BFARVALID : 1;

    /*UsageFault status register*/
    uint32_t UNDEFINSTR : 1;
    uint32_t INVSTATE : 1;
    uint32_t INVPC : 1;
    uint32_t NOCP : 1;
    uint32_t reserve3 : 4;
    uint32_t UNALIGNED : 1;
    uint32_t DIVBYZERO : 1;
    uint32_t reserve4 : 6;
} scb_cfsr_t;

/*С�Ϳ����õĹ���״̬�Ĵ����Ľṹ*/
typedef struct
{
    uint8_t MMFSR;
    uint8_t BFSR;
    uint16_t UFSR;
} scb_mini_cfsr_t;

/*���õĹ���״̬�Ĵ���--����ռ�*/
typedef union 
{
    scb_cfsr_t   bit_struct  ;
    scb_mini_cfsr_t  mini_struct;
}scb_cfsr_u;    
    
/*Ӳ������״̬�Ĵ���*/
typedef struct
{
    uint32_t reverse0 : 1;
    uint32_t VECTTBL : 1;
    uint32_t reverse1 : 28;
    uint32_t FORCED : 1;
    uint32_t DEBUGEVT : 1;
} scb_hfsr_t;

/*ָ������ݽ�������ڴ���ƼĴ���*/
typedef struct
{
    uint32_t EN : 1;
    uint32_t RMW : 1;
    uint32_t RETEN : 1;
    uint32_t SZ : 4;
} ac_iodtcmr_t;

/*AHBP���ƼĴ���*/
typedef struct
{
    uint32_t EN : 1;
    uint32_t SZ : 1;
} ac_ahbpcr_t;

/*����������ƼĴ���*/
//ȫʵ��
typedef struct
{
    uint32_t SIWT : 1;
    uint32_t ECCEN : 1;
    uint32_t FORCEWT : 1;
} ac_cacr_t;

/*AHB�ӿ��ƼĴ���*/
// û������Ĵ����Ǹ�ɶ��
// ������Ե�ټ�
typedef struct
{
    uint32_t CTL : 2;
    uint32_t TPRI : 9;
    uint32_t INITCOUNT : 5;
} ac_ahbscr_t;

/*�������ߴ���״̬�Ĵ���*/
//��AXI���߹����ܴ�
typedef struct
{
    uint32_t ITCM : 1;
    uint32_t DTCM : 1;
    uint32_t AHBP : 1;
    uint32_t AXIM : 1;
    uint32_t EPPB : 1;
    uint32_t reverse0 : 3;
    uint32_t AXIMTYPE : 2;
} ac_abfsr_t;

/*systick ���ƺ�״̬�Ĵ���*/
typedef struct
{
    uint32_t ENABLE : 1;    // ʹ�ܼ���
    uint32_t TICKINT : 1;   // ʹ���쳣����
    uint32_t CLKSOURCE : 1; // ѡ��ʱ��Դͷ
    uint32_t reverse0 : 13;
    uint32_t COUNTFLAG : 1; // ��0����1
} systic_csr_t;

/*systick��װ��ֵ�Ĵ���*/
typedef struct
{
    uint32_t RELOAD : 24;
} systic_rvr_t;

/* systick��ǰֵ�Ĵ���*/
typedef struct
{
    uint32_t CURRENT : 24;
} systic_cvr_t;

/*���������Ŀ��ƼĴ���*/
typedef struct
{
    uint32_t LSPACT:1;//ָʾ����ѹջ״̬�����ڼ���״̬
    uint32_t USER:1;  //ָʾ�����ջ����ʱ�ĳ���ȼ�,���û�������Ȩ
    uint32_t reverse0:1;
    uint32_t THREAD:1; //ָʾ�����ջ����ʱ�Ĵ���״̬,���̻߳���handle
    uint32_t HFRDY:1; 
    uint32_t MMRDY:1;
    uint32_t BFRDY:1;
    uint32_t reverse1:1;
    uint32_t MONRDY:1;
    uint32_t reverse2:21;
    uint32_t LSPEN:1;
    uint32_t ASPEN:1;
}fpu_fpccr_t;

/*����״̬���ƼĴ���*/
//����Ĵ���û���ڴ�ӳ��,ֻ��ͨ��VMSR,VMRS����,�ݲ�������
typedef struct
{
    uint32_t IOC:1;//��Ч�����쳣
    uint32_t DZC:1;//�����쳣
    uint32_t OFC:1;//����쳣
    uint32_t UFC:1;//�����쳣
    uint32_t IXC:1;//����ȷ�쳣
    uint32_t reverse0:2;
    uint32_t IDC:1;//�����������쳣,�ܵ�
    uint32_t reverse1:14;
    uint32_t RMode:2;//����ģʽ����
    uint32_t FZ:1;//����ģʽ
    uint32_t DN:1;//Ĭ�ϷǷ���
    uint32_t AHP:1;
    uint32_t reverse2:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
}fpu_fpscr_t;

/*����״̬Ĭ��ֵ�Ĵ���*/
//�ݲ����
//��һ������FPU,�´��ټ�
typedef struct
{
    uint32_t reverse0:22;
    uint32_t RMode:2;
    uint32_t FZ:1;
    uint32_t DN:1;
    uint32_t AHP:1;
    uint32_t reverse1:5;
}fpu_fpdscr_t;

/*MPU���ƼĴ���*/
typedef struct
{
    uint32_t ENABLE:1;
    uint32_t HFNMIENA:1;
    uint32_t PRIVDEFENA:1;
}mpu_ctrl_t;

/*MPU��ַ�Ĵ���*/
typedef struct
{
    uint32_t REGION:4;
    uint32_t VALID:1;
    uint32_t ADDR:27;  //ʵ���ϲ�������������,��Ӳ���Զ��������
}mpu_rbar_t;

/*MPU�������Ժͳߴ�Ĵ���*/
typedef struct
{
    uint32_t ENABLE:1;
    uint32_t SIZE:5;
    uint32_t reserve0:2;
    uint32_t SRD:8;
    uint32_t B:1;
    uint32_t C:1;
    uint32_t S:1;
    uint32_t TEX:3;
    uint32_t reserve1:2;
    uint32_t AP:3;
    uint32_t reserve2:1;
    uint32_t XN:1;
    uint32_t reserve3:3;
}mpu_rasr_t;

/*�����С�Ĵ���*/
typedef struct
{
    uint32_t LineSize:3;
    uint32_t Associativity:10;
    uint32_t NumSets:15;
    uint32_t WA:1;
    uint32_t RA:1;
    uint32_t WB:1;
    uint32_t WT:1;
}cache_ccsidr;

/*������Ϣѡ��Ĵ���*/
typedef struct
{
    uint32_t InD:1;
    uint32_t Level:3;
    uint32_t reverse0:28;
}cache_ccselr;

/*��������ͨ��set,way*/
typedef struct
{
    uint32_t reserve0:5;
    uint32_t Set:9;
    uint32_t reserve1:16;
    uint32_t Way:2;
}cache_opera_sw_t;  
    
/*��������ͨ����ַ*/
typedef uint32_t cache_opera_addr_t;

#define SCB_CPUID ((volatile scb_cpuid_t *)0xE000ED00)   // ����,�汾��Ϣ
#define SCB_ACTLR ((volatile scb_actlr_t *)0xE000E008)   // �������ƼĴ���,�쳣�����
#define SCB_VTOR ((volatile uint32_t *)0xE000ED08)       // ������ƫ�ƼĴ���
#define SCB_AIRCR ((volatile scb_aircr_t *)0xE000ED0C)   // Ӧ���жϺ͸�λ������
#define SCB_SCR ((volatile scb_scr_t *)0xE000ED10)       // �͹��ķ������
#define SCB_CCR ((volatile scb_ccr_t *)0xE000ED14)       // ���úͿ��ƼĴ���
#define SCB_ICSR ((volatile scb_icsr_t *)0xE000ED04)     // �жϿ��ƺ�״̬�Ĵ���
#define SCB_SHPR1 ((volatile uint8_t *)0xE000ED18)       // ϵͳ�ж����ȼ��Ĵ���1,8λ������12��
#define SCB_SHCSR ((volatile scb_shcsr_t *)0xE000ED24)   // ϵͳ�жϿ��ƺ�״̬�Ĵ���
#define SCB_CFSR ((volatile scb_cfsr_u *)0xE000ED28)     // �����õĹ���״̬�Ĵ���
#define SCB_MMAR ((volatile uint32_t *)0xE000ED34)       // �ڴ��������ַ�Ĵ���
#define SCB_BFAR ((volatile uint32_t *)0xE000ED38)       // ���ߴ�������ַ�Ĵ���
#define SCB_HFSR ((volatile scb_hfsr_t *)0xE000ED2C)     // Ӳ������״̬�Ĵ���
#define AC_ITCMR ((volatile ac_iodtcmr_t *)0xE000EF90)   // ָ���������ڴ���ƼĴ���
#define AC_DTCMR ((volatile ac_iodtcmr_t *)0xE000EF94)   // ���ݽ�������ڴ���ƼĴ���
#define AC_AHBPCR ((volatile ac_ahbpcr_t *)0xE000EF98)   // AHBP���ƼĴ���
#define AC_CACR ((volatile ac_cacr_t *)0xE000EF9C)       // ����������ƼĴ���
#define AC_AHBSCR ((volatile ac_ahbscr_t *)0xE000EFA0)   // AHB�ӿ��ƼĴ���
#define AC_ABFSR ((volatile ac_abfsr_t *)0xE000EFA8)     // �������ߴ���״̬�Ĵ���
#define SYSTIC_CSR ((volatile systic_csr_t *)0xE000E010) // systick ���ƺ�״̬�Ĵ���
#define SYSTIC_RVR ((volatile systic_rvr_t *)0xE000E014) // systick��װ��ֵ�Ĵ���
#define SYSTIC_CVR ((volatile systic_cvr_t *)0xE000E018) // systick��ǰֵ�Ĵ���
#define NVIC_ISER  ((volatile uint32_t*)0xE000E100)      //�ж�ʹ�ܼĴ���,��8��
#define NVIC_ICER  ((volatile uint32_t*)0XE000E180)      //�ж����ʹ�ܼĴ���,��8��
#define NVIC_ISPR  ((volatile uint32_t*)0XE000E200)      //�ж����ù���Ĵ���,��8��
#define NVIC_ICPR  ((volatile uint32_t*)0XE000E280)      //�ж��������Ĵ���,��8��
#define NVIC_IABR  ((volatile uint32_t*)0xE000E300)      //�жϻ�Ծ�Ĵ���,��8��
#define NVIC_IPR   ((volatile uint8_t*)0xE000E400)       //�ж����ȼ��Ĵ���,��60��,���ֽڷ�����240��
#define NVIC_STIR  ((volatile uint32_t*)0xE000EF00)      //��������жϼĴ���
#define FPU_FPCCR  ((volatile fpu_fpccr_t*)0xE000EF34)   //���������Ŀ��ƼĴ���
#define FPU_FPCAR  ((volatile uint32_t*)0xE000EF38)      //�����ջ֡��ַ�Ĵ���
#define FPU_FPDSCR ((volatile fpu_fpdscr_t*)0xE000EF3C)  //����״̬Ĭ��ֵ�Ĵ���
#define MPU_CTRL   ((volatile mpu_ctrl_t*)0xE000ED94)    //MPU���ƼĴ���
#define MPU_RNR    ((volatile uint32_t*)0xE000ED98)      //MPU����żĴ���
#define MPU_RBAR   ((volatile mpu_rbar_t*)0xE000ED9C)    //MPU��ַ�Ĵ���
#define MPU_RASR   ((volatile mpu_rasr_t*)0xE000EDA0)    //MPU�������Ժͳߴ�Ĵ���
    
#define CACHE_CCSIDR    ((volatile cache_ccsidr*)0xE000ED80) //������Ϣ�Ĵ���    
#define CACHE_CCSELR    ((volatile cache_ccselr*)0xE000ED84) //������Ϣѡ��Ĵ���     
    
#define CACHE_ICIALLU   ((volatile uint32_t*)0xE000EF50)         //ICACHE��Ч���Ĵ���  
#define CACHE_DCISW     ((volatile uint32_t*)0xE000EF60) //���ݻ�����Ч���Ĵ��� 
#define CACHE_DCCSW     ((volatile uint32_t*)0xE000EF6C) //���ݻ�������Ĵ��� 
#define CACHE_DCCISW    ((volatile uint32_t*)0xE000EF74) //���ݻ�����Ч��,����Ĵ��� 
    
#define CACHE_ICIMVAU   ((volatile cache_opera_addr_t*)0xE000EF58) //ָ�����Ч��ͨ�����õ�ַ,�������ڴ�--ָ��cache,�Ը�д����
#define CACHE_DCIMVAC   ((volatile cache_opera_addr_t*)0xE000EF5C) //���ݻ�����Ч��ͨ�����õ�ַ,�������ڴ�--����cache,
#define CACHE_DCCMVAU   ((volatile cache_opera_addr_t*)0xE000EF64) //���ݻ������  ͨ�����õ�ַ,�������ڴ�--ָ��cache,�Ը�д����
#define CACHE_DCCMVAC   ((volatile cache_opera_addr_t*)0xE000EF68) //���ݻ������  ͨ�����õ�ַ,�������ڴ�--����cache
#define CACHE_DCCIMVAC  ((volatile cache_opera_addr_t*)0xE000EF70) //���ݻ��������Ч�� ͨ�����õ�ַ,�������ڴ�--����cache
    
/*ʧ��MPU*/
#define DISABLE_MPU() MPU_CTRL->ENABLE=0

/*ʹ��MPU*/
#define ENABLE_MPU() MPU_CTRL->ENABLE=1

/*ʹ��MPU�Ĳ�����ʹ�ڴ����쳣���������*/
#define ENABLE_MPU_IN_EXCEPTION() MPU_CTRL->HFNMIENA=1

/*���ñ�����*/
#define ENABLE_DEFAULT_MEMORY_MAP() MPU_CTRL->PRIVDEFENA=1


/*��ø����ջ��δ��両��Ĵ����ĵ�ַ*/
#define __GET_FPU_STACK_ADDRESS() *FPU_FPCAR

/*����CONTROL��FPCAλ*/
#define ENABLE_CONTROL_FPCA() FPU_FPCCR->ASPEN=1

/*���ø������ѹջ*/
#define ENABLE_FPU_LAZY_STACK() FPU_FPCCR->LSPEN=1

/*32λ�Ĵ���(ԭ��)��ռ����,������,��Ĳ����ٷ���*/
#define EXC_XOR_SET(REG,BIT) while (__STREXW (__LDREXW ((uint32_t *)&(REG))^(BIT), (uint32_t *)&(REG)))
    
/*����NMI�ж�*/
// NMI�����־���Զ����,�����ֶ������־λ,������ⲻ��
// ���Ӳ�����Ϊ״̬��־λ������
#define JUMP_TO_NMI() SCB_ICSR->NMIPENDSET = 1

/*��PendSV�ж�*/
#define JUMP_TO_PENDSV() SCB_ICSR->PENDSVSET = 1

/*��ȡPendSV����״̬*/
// �����ȡ״̬�ĺ겻��ʹ
#define GET_PENDSV_STATE() (volatile uint8_t)SCB_ICSR->PENDSVSET

/*���PendSV����״̬*/
#define CLEAR_PENDSV_STATE() SCB_ICSR->PENDSVCLR = 1

/*����systick�ж�*/
#define JUMP_TO_SYSTICK_HANDLE() SCB_ICSR->PENDSTSET = 1

/*�鿴systick�ж�״̬*/
// �����ȡ״̬�ĺ겻��ʹ
#define GET_SYSTIC_HANDLE_STATE() (volatile uint8_t)SCB_ICSR->PENDSTSET

/*���systick�жϱ�־*/
#define CLEAR_SYSTIC_HANDLE_STATE() SCB_ICSR->PENDSTCLR = 1

/*����dual-issue*/
#define DISABLE_DUAL_ISSUE() SCB_ACTLR->DISFOLD = 1

/*����FPU�쳣���*/
#define DISABLE_FPU_EXCEPTION_OUT() SCB_ACTLR->FPEXCODIS = 1

/*���ö�����*/
// ����Write-Back Write-Allocate������
#define DISABLE_READ_ALLOCATE() SCB_ACTLR->DISRAMODE = 1

/*����ITM and DWT ATB flush*/
#define DISBALE_ITM_AND_DWT_ATB_FLUSH SCB_ACTLR->DISITMATBFLUSH = 1

/*�����ж��������ƫ��*/
#define SET_VTOR_ADD(add) *SCB_VTOR = (add)

/*�������Ȩ�������STIR(��������ж�)*/
#define ALLOW_UNPRIVILEGED_STIR() SCB_CCR->USERSETMPEND=1

/*�����0����*/
// �������Ż���ԭ��,����ʹ,��λ���˿�����Ч��
#define DIV_BY_0_TRAP_ENABLE() SCB_CCR->DIV_0_TRP = 1

/*����Ƕ�����ʴ���*/
#define UNALIGN_ACCESS_TRAP_ENABLE() SCB_CCR->UNALIGN_TRP = 1

/*��ȡ��ǰ�жϷ���*/
#define __GET_INTERRUPT_GROUPING() SCB_AIRCR->PRIGROUP

/*ʹ��use fault�쳣*/
#define ENABLE_USE_FAULT_HANDLE() SCB_SHCSR->USGFAULTENA = 1

/*ʹ��bus fault�쳣*/
#define ENABLE_BUS_FAULT_HANDLE() SCB_SHCSR->BUSFAULTENA = 1

/*ʹ��MemManage fault�쳣*/
#define ENABLE_MEM_MANAGE_FAULT_HANDLE() SCB_SHCSR->MEMFAULTENA = 1

/*ʹ��I-Cache*/
#define ENABLE_I_Cache() SCB_CCR->IC = 1

/*ʧ��I-Cache*/
#define DISABLE_I_Cache() SCB_CCR->IC = 0

/*ʹ��D-Cache*/
#define ENABLE_D_Cache() SCB_CCR->DC = 1

/*ʧ��D-Cache*/
#define DISABLE_D_Cache() SCB_CCR->DC = 0

/*ʹ��ITCM*/
#define ENBABLE_ITCM() AC_ITCMR->EN = 1

/*ʹ��DTCM*/
#define ENBABLE_DTCM() AC_DTCMR->EN = 1

/*ʹ��ITCM�Ķ���д*/
#define ENBABLE_ITCM_RMW() AC_ITCMR->RMW = 1

/*ʹ��DTCM�Ķ���д*/
#define ENBABLE_DTCM_RMW() AC_DTCMR->RMW = 1

/*ʹ��ITCM���������*/
#define ENBABLE_ITCM_RET() AC_ITCMR->RETEN = 1

/*ʹ��DTCM���������*/
#define ENBABLE_DTCM_RET() AC_DTCMR->RETEN = 1

/*ʹ��AHBP*/
#define ENABKE_AHBP() AC_AHBPCR->EN = 1

/*����һ���Բ���ʹ��1�����ַ�ʽ*/
// ��λ��Ҫʱ���ֲ��
#define CACHE_COHERENCY_1() AC_CACR->SIWT = 1

/*���û�����ECC*/
//��λ�ڻ���رյ�ʱ������,����������Ҫ��Ч������.
#define DISABLE_ECC_IN_CACHE() AC_CACR->ECCEN = 1

/*����ǿ��͸д*/
#define ENABLE_FORCE_WRITE_THROUGH() AC_CACR->FORCEWT = 1

/*�������п��������ȼ��ж�*/
#define DISNABLE_ALL_CONFIGURABLE_INTERRUPTS() __asm volatile("CPSID i")

/*�������п��������ȼ��ж�*/
#define ENABLE_ALL_CONFIGURABLE_INTERRUPTS() __asm volatile("CPSIE i")

/*�������г�NMI���ж�*/
#define DISNABLE_INTERRUPTS_EXCEPT_NMI() __asm volatile("CPSID f")
    
/*�������г�NMI���ж�*/
#define ENABLE_INTERRUPTS_EXCEPT_NMI() __asm volatile("CPSIE f")

/*�õ���ǰ���ȼ�����*/
#define __GET_INTERRUPT_GROUPING() SCB_AIRCR->PRIGROUP

/*ʹ���ж�*/
//ע��ֵ�����Ǹ���
#define __enable_IRQ(irq_num) NVIC_ISER[(irq_num)>>5]=1<<((irq_num)&0x1f)

/*�ж�ʹ��ûʹ��*/
#define __is_enable_IRQ(irq_num) (uint8_t)((NVIC_ISER[(irq_num)>>5]>>((irq_num)&0x1f))&1)

/*�������IRQ*/
#define __trigger_IRQ(irq_num) *NVIC_STIR=irq_num

/*�ر��ж�*/
//ע��ֵ�����Ǹ���
static inline void __disable_IRQ(uint8_t irq_num)
{   
    NVIC_ICER[irq_num>>5]=1<<(irq_num&0x1f);//�൱��/32 , %32
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*�����ж�*/
#define __set_pending_IRQ(irq_num) NVIC_ISPR[(irq_num)>>5]=1<<((irq_num)&0x1f)

/*���������ж�*/
#define __clear_pending_IRQ(irq_num) NVIC_ICPR[(irq_num)>>5]=1<<((irq_num)&0x1f)

/*��ȡ�ж�active*/
#define __get_IRQ_active(irq_num) ((NVIC_IABR[(irq_num)>>5]>>((irq_num)&0x1f))&1)

/*�������ȼ�*/
static inline uint8_t __trans_p_s_to_u8(uint8_t GroupPriority,uint8_t SubPriority)
{
    if(__GET_INTERRUPT_GROUPING()==GROUPING_16p_1s)
    {
        return GroupPriority<<4;
    }
    else if(__GET_INTERRUPT_GROUPING()==GROUPING_1p_16s)
    {
        return SubPriority<<4;
    }    
    return ((GroupPriority<<(7-__GET_INTERRUPT_GROUPING()))|SubPriority)<<4;
}

/*�رմ����ȼ����������ж�*/
static inline void __disable_specific_interrupts(uint8_t GroupPriority,uint8_t SubPriority)
{
    __asm volatile("MSR basepri, %0" : : "r"(__trans_p_s_to_u8(GroupPriority,SubPriority)) : "memory");
}

/*�����ж�,�漰basepri�Ĵ���*/
#define __enable_all_interrupts_basepri() __asm("MSR basepri, %0"::"r"(0):"memory");


/*�����ж����ȼ�*/
//���������ں��ж�
static inline void __set_priority(IRQn_e irq_num,uint8_t GroupPriority,uint8_t SubPriority)
{
    if(irq_num>=0)
        NVIC_IPR[irq_num]=__trans_p_s_to_u8(GroupPriority,SubPriority);
    else 
        SCB_SHPR1[(((uint32_t)irq_num)&0xf)-4]=__trans_p_s_to_u8(GroupPriority,SubPriority);
    //����Ѹ����ڴ��еĲ���ȡ�˳������� -1 -> 0xffff,-2 -> 0xfffe
}

/*�����жϷ���*/
// PRIGROUP��stmֻ��ȡ��8λ�е�ǰ��λ
// ������ interrupt_grouping_e
static inline void set_interrupt_grouping(uint8_t num)
{
    uint32_t temp = *(uint32_t *)SCB_AIRCR;
    ((scb_aircr_t *)&temp)->VECTKEY = 0x5FA; // д����
    ((scb_aircr_t *)&temp)->PRIGROUP = num;  // д������
    *(uint32_t *)SCB_AIRCR = temp;           // ��ֵ
    while (SCB_AIRCR->PRIGROUP != num)
        ; // �ؿ�
}

/*�����λ*/
static __inline void __software_reset(void)
{
    __asm("dsb");
    uint32_t temp = *(uint32_t *)SCB_AIRCR;
    ((scb_aircr_t *)&temp)->VECTKEY = 0x5FA; // д����
    ((scb_aircr_t *)&temp)->SYSRESETREQ = 1; //
    *(uint32_t *)SCB_AIRCR = temp;           // ��ֵ
    __asm("dsb");
    while (1)
        ;
}

/*��ϸ���ں�����*/
static inline void detailed_kernel_Settings(void)
{
    ENABLE_USE_FAULT_HANDLE();        // �����÷����������
    ENABLE_BUS_FAULT_HANDLE();        // �������ߴ��������
    ENABLE_MEM_MANAGE_FAULT_HANDLE(); // �����ڴ�����쳣�������
    DIV_BY_0_TRAP_ENABLE();           // �����0����
    ALLOW_UNPRIVILEGED_STIR();        // �������Ȩ�������STIR
    //  UNALIGN_ACCESS_TRAP_ENABLE(); //����Ƕ�����ʴ���
    ENABLE_CONTROL_FPCA();            //����control.fpcaλ
    ENABLE_FPU_LAZY_STACK();          //���ø������ѹջ
//    ENBABLE_ITCM();//ʹ��ITCM,reset���ĺ������˹���
//    ENBABLE_DTCM();//ʹ��DTCM,reset���ĺ������˹���
//    ENBABLE_ITCM_RMW();//ʹ��ITCM�Ķ���д
//    ENBABLE_DTCM_RMW();//ʹ��DTCM�Ķ���д
//    ENBABLE_ITCM_RET();//ʹ��ITCM���������
//    ENBABLE_DTCM_RET();//ʹ��DTCM���������
} 

/*��ʼ���δ�ʱ��*/
// use_exception:1,�����쳣
// reload:��װ��ֵ,��ֵΪ��Ҫ��ʱ��������-1;
// clock_source,ʱ��Դ:1,������ʱ��;0,�ⲿʱ��
static inline void __init_systic(uint32_t reload, uint8_t clock_source, uint8_t use_exception)
{
    if (use_exception == 1)
        SYSTIC_CSR->TICKINT = 1;
    if (clock_source == 1)
        SYSTIC_CSR->CLKSOURCE = 1;
    else
        SYSTIC_CSR->CLKSOURCE = 0;
    SYSTIC_RVR->RELOAD = reload;
    SYSTIC_CVR->CURRENT = 0;
    SYSTIC_CSR->ENABLE = 1;
}

/*ʹ��FPU*/
//ȫ����,��Ȩ�ͷ���Ȩ�������ʹ��,����Ĵ���δ���.
static inline void enable_fpu(void)
{
     __asm volatile("LDR R0,=0xE000ED88"); 
     __asm volatile("MOV R1,#0xF00000");   
     __asm volatile("STR R1,[R0]");  
     __asm volatile("DSB"); 
     __asm volatile("ISB");    
}

/*ʹ��I-Cache*/
static inline void enable_I_Cache(void)
{
    __asm volatile("dsb");
    __asm volatile("isb");
    *CACHE_ICIALLU=0;   //��Ч��I-Cache
    __asm volatile("dsb");
    __asm volatile("isb");
    ENABLE_I_Cache();   //ʹ��I_Cache
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*ʧ��I-Cache*/
static inline void disable_I_Cache(void)
{
    __asm volatile("dsb");
    __asm volatile("isb");
    DISABLE_I_Cache();   //ʧ��ICache
    *CACHE_ICIALLU=0;   //��Ч��I-Cache
    __asm volatile("dsb");
    __asm volatile("isb");
}
//#include "core_cm7.h"
/*ʹ��D cache*/
// ���Ż� __attribute__((optnone))
static inline void enable_D_Cache(void) 
{    
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    CACHE_CCSELR->InD = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
   __asm("dsb");

                                            /* invalidate D-Cache */
    sets = (uint32_t)(CACHE_CCSIDR->NumSets);
    do {
      ways = (uint32_t)(CACHE_CCSIDR->Associativity);
      do {
        *CACHE_DCISW= (((sets << 5) ) |
                      ((ways << 30) )  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);
 __asm("dsb");

    ENABLE_D_Cache();
    __asm("dsb");
    __asm("isb");
}



/*ʧ��D_Cache*/
static inline void disable_D_Cache(void)
{
    register uint32_t ccsidr;
    register uint32_t sets;
    register uint32_t ways;

    CACHE_CCSELR->InD = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
    __asm("dsb");

    DISABLE_D_Cache();
     __asm("dsb");

                                            /* clean & invalidate D-Cache */
    sets = (uint32_t)(CACHE_CCSIDR->NumSets);
    do {
      ways = (uint32_t)(CACHE_CCSIDR->Associativity);
      do {
        *CACHE_DCCISW = ((sets << 5)  |
                    (ways << 30)   );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __asm("dsb");
    __asm("isb");
    

}    

/*��Ч��D_Cache*/
//�ڴ��cache
static inline void invalidate_D_Cache(void)
{
    register uint32_t ccsidr;
    register uint32_t sets;
    register uint32_t ways;
    CACHE_CCSELR->InD = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
    __asm("dsb");
    sets = (uint32_t)(CACHE_CCSIDR->NumSets);
    do {
      ways = (uint32_t)(CACHE_CCSIDR->Associativity);
      do {
        *CACHE_DCISW = ((sets << 5)  |(ways << 30)   );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __asm("dsb");
    __asm("isb");
}

/*clean D_Cache*/
//cache���ڴ�
static inline void clean_D_Cache(void)
{
    CACHE_CCSELR->InD=0;//ѡ��鿴D_Cache����Ϣ
    __asm volatile("dsb");
    uint32_t way;
    uint32_t sets=CACHE_CCSIDR->NumSets;
    do{
        way=CACHE_CCSIDR->Associativity;
        do{ 
            *CACHE_DCCSW = ((sets << 5)  |(way << 30)   );
          }while(way--!=0);
      }while(sets--!=0);
    
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*clean ������Ч�� D_Cache*/
//˫��ͬ��
static inline void clean_and_invalidaye_D_Cache(void)
{
    CACHE_CCSELR->InD=0;//ѡ��鿴D_Cache����Ϣ
    __asm volatile("dsb");
    uint32_t sets=CACHE_CCSIDR->NumSets;
    uint32_t way;
    do{
        way=CACHE_CCSIDR->Associativity;
        do
        { 
            *CACHE_DCCISW = ((sets << 5)  |(way << 30)   );
          }while(way--!=0);
      }while(sets--!=0);
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*��Ч�����ݻ���,����ַ*/
static inline void invalidate_D_Cache_by_Addr(uint32_t *addr, uint32_t dsize)
{
    __asm volatile("dsb");
    uint32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
    while (op_size > 0) 
    {
      *CACHE_DCIMVAC=op_addr;
      op_addr += 32;
      op_size -= 32;
    }
    __asm volatile("dsb");
    __asm volatile("isb");
}
    
/*������ݻ���,����ַ*/
static inline void clean_D_Cache_by_Addr(uint32_t *addr, uint32_t dsize)
{
    __asm volatile("dsb");
    uint32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
    while (op_size > 0) 
    {
      *CACHE_DCCMVAC=op_addr;
      op_addr += 32;
      op_size -= 32;
    }
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*���,��Ч�����ݻ���,����ַ*/
static inline void invalidate_clean_D_Cache_by_Addr(uint32_t *addr, uint32_t dsize)
{
    __asm volatile("dsb");
    uint32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
    while (op_size > 0) 
    {
      *CACHE_DCCIMVAC=op_addr;
      op_addr += 32;
      op_size -= 32;
    }
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*��Ч��ָ���,����ַ*/
static inline void invalidate_I_Cache_by_Addr(uint32_t *addr, uint32_t dsize)
{
    __asm volatile("dsb");
    __asm volatile("isb");
    uint32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
    while (op_size > 0) 
    {
      *CACHE_ICIMVAU=op_addr;
      op_addr += 32;
      op_size -= 32;
    }
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*�����������ȼ�*/
void SCB_CPUID_show_info(void);    // ��ʾCPUID��Ϣ
void SCB_ICSR_show_info(void);     // չʾSCB_ICSR�ṩ����Ϣ
void SCB_show_fault_info(void);    // չʾSCB_SHCSR��Ϣ
void SCB_show_detailed_info(void); // չʾ��ϸ���쳣��Ϣ
void show_ipsr_info(void);         // չʾIPSR�쳣
void show_CONTROL_info(void);      // չʾCONTROL�Ĵ�����Ϣ
void FPU_CCR_show_info(void);      // չʾ����FPUִ�й����е�һЩ��Ϣ(���Ǽ���״̬�Ĵ���,�Ǹ�û���)

void set_control(uint32_t value); // ���ÿ��ƼĴ���
void mpu_set(void);               // ����mpu
void cache_set(void);             // ���û���
void set_votor(uint32_t addr);    //

#endif
