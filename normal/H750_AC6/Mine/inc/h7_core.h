/**
 *******************************************************************************
 * @file        H7_core.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-14
 * @brief       CMSIS层级代码
 *              深入挖h7系统级寄存器
 *****************************************************************************************
 */
#ifndef __M7_SCB_H
#define __M7_SCB_H

#include "stdint.h"
#include "parameter.h"
#include "stdio.h"

/*关于寄存器的实现*/
//****有些简单的寄存器就没有编程了,比如FPU的启用寄存器.
//(1)这里没有编程Processor features中的Cache level ID register
// 它提供了信息:指令和数据层面各实现了一个一级缓存.
//(2)Cache type register没有编程
// 它提供了信息:Smallest cache line of all the instruction caches :8 words
//             Smallest cache line of all the data and unified caches:8words
// 关于同步原语:The local monitor within the processor does not hold any physical
//             address. It treats any STREX instruction access as matching the address of
//             the previous LDREX instruction.
// Cache Writeback Granule:8words, ArmV7寄存器格式.
//(3)Cache size ID register和 Cache size selection register寄存器进行了编程!!这个要用
// 一起提供信息:两种缓存指令和数据都支持,透写,回写,写分配,读分配.
// data cache set数:128,way数:4,line size:8words
// Instruction cache set数:256,way数:2,line size:8words
//(4)滴答定时器的校准指示寄存器未编程.
//(5)MPU type register未编程.
//   指示信息: 16 MPU regions.
//     The Cortex-M7 MPU memory map is unified. This means instruction accesses and data 
//     accesses have same region settings
//(6)AHBPCR ,这寄存器编程了写的部分,读部分指示AHBP size为256MBytes
//(7)MPU有几个别名寄存器,不知道干嘛的没编程.

/* ICSR bit assignments */
// 挂起中断很好使
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
    uint32_t PENDSTSET : 1; // SysTick exception set-pending bit,也可以看状态
    uint32_t PENDSVCLR : 1; // PendSV clear-pending bit.1: Removes the pending state
    uint32_t PENDSVSET : 1; // PendSV set-pending bit,也可以看状态
    uint32_t reverse0 : 2;
    uint32_t NMIPENDSET : 1; // NMI set-pending bit,也可以看状态
} scb_icsr_t;

/*SCB 辅助控制寄存器*/
// 本身32位但只实现了前13位有效位
// 此寄存器相关的功能默认开启,写1关闭
typedef struct
{
    uint32_t reserve0 : 2;
    uint32_t DISFOLD : 1; // Disables dual-issue
    uint32_t reserve1 : 7;
    uint32_t FPEXCODIS : 1;      // Disables FPU exception outputs
    uint32_t DISRAMODE : 1;      // Disables dynamic read allocate mode for Write-Back Write-Allocate memory regions
    uint32_t DISITMATBFLUSH : 1; // Disables ITM and DWT ATB flush
} scb_actlr_t;

/*SCB CPU ID寄存器*/
// 用于展示版本信息
typedef struct
{
    uint32_t Revision : 4; // Patch
    uint32_t PartNo : 12;  // 处理器:Cortex-M7
    uint32_t reserve0 : 4;
    uint32_t Variant : 4;     // Revision
    uint32_t Implementer : 8; // Implementer code: Arm
} scb_cpuid_t;

/*应用中断和复位控制器*/
typedef struct
{
    uint32_t reserve0 : 2;
    uint32_t SYSRESETREQ : 1; // System reset request，软件复位
    uint32_t reserve1 : 5;
    uint32_t PRIGROUP : 3; // Interrupt priority grouping field,中断优先级分组
    uint32_t reserve2 : 4;
    uint32_t ENDIANNESS : 1; // 指示小端,只读
    uint32_t VECTKEY : 16;   // KEY
} scb_aircr_t;

/*系统控制寄存器*/
typedef struct
{
    uint32_t reverse0 : 1;
    uint32_t SLEEPONEXIT : 1; // 从中断返回到线程模式时是否睡眠
    uint32_t SLEEPDEEP : 1;   // 选择普通睡眠OR deep sleep
    uint32_t reverse1 : 1;
    uint32_t SEVONPEND : 1; // 禁用的中断也可以唤醒处理器
} scb_scr_t;

/*配置和控制寄存器*/
typedef struct
{
    uint32_t NONBASETHRDENA : 1; // 据说是中断结束后强制进入线程模式
    uint32_t USERSETMPEND : 1;   // 允许非特权软件访问 STIR
    uint32_t reverse0 : 1;
    uint32_t UNALIGN_TRP : 1; // 捕获非对齐访问错误
    uint32_t DIV_0_TRP : 1;   // 捕获除0错误
    uint32_t reverse1 : 3;
    uint32_t BFHFNMIGN : 1; // Enables handlers with priority -1 or -2 to ignore dat
                            // a BusFaults caused by load and store instructions
    uint32_t STKALIGN : 1;  // 只读,供硬件使用,在异常返回时恢复8字节对齐
    uint32_t reverse2 : 6;
    uint32_t DC : 1; // 使能数据cache
    uint32_t IC : 1; // 使能指令cache
    uint32_t BP : 1; // 只读,指示启用分支预测
    uint32_t reverse3 : 13;
} scb_ccr_t;

/*系统中断优先级寄存器1*/
typedef struct
{
    uint32_t PRI_4 : 8; // MemManage中断优先级
    uint32_t PRI_5 : 8; // BusFault中断优先级
    uint32_t PRI_6 : 8; // UsageFault中断优先级
} scb_shpr1_t;

/*系统中断优先级寄存器2*/
typedef struct
{
    uint32_t reverse : 24;
    uint32_t PRI_11 : 8; // SVCall中断优先级
} scb_shpr2_t;

/*系统中断优先级寄存器3*/
typedef struct
{
    uint32_t reverse : 16;
    uint32_t PRI_14 : 8; // PendSV中断优先级
    uint32_t PRI_15 : 8; // Systic中断优先级
} scb_shpr3_t;

/*系统中断控制和状态寄存器*/
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

/*可配置的故障状态寄存器*/
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

/*小型可配置的故障状态寄存器的结构*/
typedef struct
{
    uint8_t MMFSR;
    uint8_t BFSR;
    uint16_t UFSR;
} scb_mini_cfsr_t;

/*配置的故障状态寄存器--共享空间*/
typedef union 
{
    scb_cfsr_t   bit_struct  ;
    scb_mini_cfsr_t  mini_struct;
}scb_cfsr_u;    
    
/*硬件错误状态寄存器*/
typedef struct
{
    uint32_t reverse0 : 1;
    uint32_t VECTTBL : 1;
    uint32_t reverse1 : 28;
    uint32_t FORCED : 1;
    uint32_t DEBUGEVT : 1;
} scb_hfsr_t;

/*指令或数据紧密耦合内存控制寄存器*/
typedef struct
{
    uint32_t EN : 1;
    uint32_t RMW : 1;
    uint32_t RETEN : 1;
    uint32_t SZ : 4;
} ac_iodtcmr_t;

/*AHBP控制寄存器*/
typedef struct
{
    uint32_t EN : 1;
    uint32_t SZ : 1;
} ac_ahbpcr_t;

/*辅助缓存控制寄存器*/
//全实现
typedef struct
{
    uint32_t SIWT : 1;
    uint32_t ECCEN : 1;
    uint32_t FORCEWT : 1;
} ac_cacr_t;

/*AHB从控制寄存器*/
// 没明白这寄存器是干啥的
// 这里有缘再见
typedef struct
{
    uint32_t CTL : 2;
    uint32_t TPRI : 9;
    uint32_t INITCOUNT : 5;
} ac_ahbscr_t;

/*辅助总线错误状态寄存器*/
//和AXI总线关联很大
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

/*systick 控制和状态寄存器*/
typedef struct
{
    uint32_t ENABLE : 1;    // 使能计数
    uint32_t TICKINT : 1;   // 使能异常请求
    uint32_t CLKSOURCE : 1; // 选择时钟源头
    uint32_t reverse0 : 13;
    uint32_t COUNTFLAG : 1; // 到0返回1
} systic_csr_t;

/*systick重装载值寄存器*/
typedef struct
{
    uint32_t RELOAD : 24;
} systic_rvr_t;

/* systick当前值寄存器*/
typedef struct
{
    uint32_t CURRENT : 24;
} systic_cvr_t;

/*浮点上下文控制寄存器*/
typedef struct
{
    uint32_t LSPACT:1;//指示惰性压栈状态正处于激活状态
    uint32_t USER:1;  //指示浮点堆栈分配时的程序等级,是用户还是特权
    uint32_t reverse0:1;
    uint32_t THREAD:1; //指示浮点堆栈分配时的处理状态,是线程还是handle
    uint32_t HFRDY:1; 
    uint32_t MMRDY:1;
    uint32_t BFRDY:1;
    uint32_t reverse1:1;
    uint32_t MONRDY:1;
    uint32_t reverse2:21;
    uint32_t LSPEN:1;
    uint32_t ASPEN:1;
}fpu_fpccr_t;

/*浮点状态控制寄存器*/
//这个寄存器没有内存映射,只能通过VMSR,VMRS访问,暂不做处理
typedef struct
{
    uint32_t IOC:1;//无效操作异常
    uint32_t DZC:1;//除零异常
    uint32_t OFC:1;//溢出异常
    uint32_t UFC:1;//下溢异常
    uint32_t IXC:1;//不精确异常
    uint32_t reverse0:2;
    uint32_t IDC:1;//非正规输入异常,总的
    uint32_t reverse1:14;
    uint32_t RMode:2;//舍入模式控制
    uint32_t FZ:1;//归零模式
    uint32_t DN:1;//默认非法数
    uint32_t AHP:1;
    uint32_t reverse2:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
}fpu_fpscr_t;

/*浮点状态默认值寄存器*/
//暂不作深究
//告一段落了FPU,下次再见
typedef struct
{
    uint32_t reverse0:22;
    uint32_t RMode:2;
    uint32_t FZ:1;
    uint32_t DN:1;
    uint32_t AHP:1;
    uint32_t reverse1:5;
}fpu_fpdscr_t;

/*MPU控制寄存器*/
typedef struct
{
    uint32_t ENABLE:1;
    uint32_t HFNMIENA:1;
    uint32_t PRIVDEFENA:1;
}mpu_ctrl_t;

/*MPU基址寄存器*/
typedef struct
{
    uint32_t REGION:4;
    uint32_t VALID:1;
    uint32_t ADDR:27;  //实际上并不是这样分配,有硬件自动对齐机制
}mpu_rbar_t;

/*MPU区域特性和尺寸寄存器*/
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

/*缓存大小寄存器*/
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

/*缓存信息选择寄存器*/
typedef struct
{
    uint32_t InD:1;
    uint32_t Level:3;
    uint32_t reverse0:28;
}cache_ccselr;

/*缓存设置通过set,way*/
typedef struct
{
    uint32_t reserve0:5;
    uint32_t Set:9;
    uint32_t reserve1:16;
    uint32_t Way:2;
}cache_opera_sw_t;  
    
/*缓存设置通过地址*/
typedef uint32_t cache_opera_addr_t;

#define SCB_CPUID ((volatile scb_cpuid_t *)0xE000ED00)   // 厂商,版本信息
#define SCB_ACTLR ((volatile scb_actlr_t *)0xE000E008)   // 辅助控制寄存器,异常方面的
#define SCB_VTOR ((volatile uint32_t *)0xE000ED08)       // 向量表偏移寄存器
#define SCB_AIRCR ((volatile scb_aircr_t *)0xE000ED0C)   // 应用中断和复位控制器
#define SCB_SCR ((volatile scb_scr_t *)0xE000ED10)       // 低功耗方面控制
#define SCB_CCR ((volatile scb_ccr_t *)0xE000ED14)       // 配置和控制寄存器
#define SCB_ICSR ((volatile scb_icsr_t *)0xE000ED04)     // 中断控制和状态寄存器
#define SCB_SHPR1 ((volatile uint8_t *)0xE000ED18)       // 系统中断优先级寄存器1,8位访问有12个
#define SCB_SHCSR ((volatile scb_shcsr_t *)0xE000ED24)   // 系统中断控制和状态寄存器
#define SCB_CFSR ((volatile scb_cfsr_u *)0xE000ED28)     // 可配置的故障状态寄存器
#define SCB_MMAR ((volatile uint32_t *)0xE000ED34)       // 内存管理出错地址寄存器
#define SCB_BFAR ((volatile uint32_t *)0xE000ED38)       // 总线错误出错地址寄存器
#define SCB_HFSR ((volatile scb_hfsr_t *)0xE000ED2C)     // 硬件错误状态寄存器
#define AC_ITCMR ((volatile ac_iodtcmr_t *)0xE000EF90)   // 指令紧密耦合内存控制寄存器
#define AC_DTCMR ((volatile ac_iodtcmr_t *)0xE000EF94)   // 数据紧密耦合内存控制寄存器
#define AC_AHBPCR ((volatile ac_ahbpcr_t *)0xE000EF98)   // AHBP控制寄存器
#define AC_CACR ((volatile ac_cacr_t *)0xE000EF9C)       // 辅助缓存控制寄存器
#define AC_AHBSCR ((volatile ac_ahbscr_t *)0xE000EFA0)   // AHB从控制寄存器
#define AC_ABFSR ((volatile ac_abfsr_t *)0xE000EFA8)     // 辅助总线错误状态寄存器
#define SYSTIC_CSR ((volatile systic_csr_t *)0xE000E010) // systick 控制和状态寄存器
#define SYSTIC_RVR ((volatile systic_rvr_t *)0xE000E014) // systick重装载值寄存器
#define SYSTIC_CVR ((volatile systic_cvr_t *)0xE000E018) // systick当前值寄存器
#define NVIC_ISER  ((volatile uint32_t*)0xE000E100)      //中断使能寄存器,有8个
#define NVIC_ICER  ((volatile uint32_t*)0XE000E180)      //中断清除使能寄存器,有8个
#define NVIC_ISPR  ((volatile uint32_t*)0XE000E200)      //中断设置挂起寄存器,有8个
#define NVIC_ICPR  ((volatile uint32_t*)0XE000E280)      //中断清除挂起寄存器,有8个
#define NVIC_IABR  ((volatile uint32_t*)0xE000E300)      //中断活跃寄存器,有8个
#define NVIC_IPR   ((volatile uint8_t*)0xE000E400)       //中断优先级寄存器,有60个,按字节访问有240个
#define NVIC_STIR  ((volatile uint32_t*)0xE000EF00)      //软件触发中断寄存器
#define FPU_FPCCR  ((volatile fpu_fpccr_t*)0xE000EF34)   //浮点上下文控制寄存器
#define FPU_FPCAR  ((volatile uint32_t*)0xE000EF38)      //浮点堆栈帧地址寄存器
#define FPU_FPDSCR ((volatile fpu_fpdscr_t*)0xE000EF3C)  //浮点状态默认值寄存器
#define MPU_CTRL   ((volatile mpu_ctrl_t*)0xE000ED94)    //MPU控制寄存器
#define MPU_RNR    ((volatile uint32_t*)0xE000ED98)      //MPU区域号寄存器
#define MPU_RBAR   ((volatile mpu_rbar_t*)0xE000ED9C)    //MPU基址寄存器
#define MPU_RASR   ((volatile mpu_rasr_t*)0xE000EDA0)    //MPU区域特性和尺寸寄存器
    
#define CACHE_CCSIDR    ((volatile cache_ccsidr*)0xE000ED80) //缓存信息寄存器    
#define CACHE_CCSELR    ((volatile cache_ccselr*)0xE000ED84) //缓存信息选择寄存器     
    
#define CACHE_ICIALLU   ((volatile uint32_t*)0xE000EF50)         //ICACHE无效化寄存器  
#define CACHE_DCISW     ((volatile uint32_t*)0xE000EF60) //数据缓存无效化寄存器 
#define CACHE_DCCSW     ((volatile uint32_t*)0xE000EF6C) //数据缓存清除寄存器 
#define CACHE_DCCISW    ((volatile uint32_t*)0xE000EF74) //数据缓存无效化,清除寄存器 
    
#define CACHE_ICIMVAU   ((volatile cache_opera_addr_t*)0xE000EF58) //指令缓存无效化通过设置地址,处理器内存--指令cache,自覆写代码
#define CACHE_DCIMVAC   ((volatile cache_opera_addr_t*)0xE000EF5C) //数据缓存无效化通过设置地址,处理器内存--数据cache,
#define CACHE_DCCMVAU   ((volatile cache_opera_addr_t*)0xE000EF64) //数据缓存清除  通过设置地址,处理器内存--指令cache,自覆写代码
#define CACHE_DCCMVAC   ((volatile cache_opera_addr_t*)0xE000EF68) //数据缓存清除  通过设置地址,处理器内存--数据cache
#define CACHE_DCCIMVAC  ((volatile cache_opera_addr_t*)0xE000EF70) //数据缓存清除无效化 通过设置地址,处理器内存--数据cache
    
/*失能MPU*/
#define DISABLE_MPU() MPU_CTRL->ENABLE=0

/*使能MPU*/
#define ENABLE_MPU() MPU_CTRL->ENABLE=1

/*使能MPU的操作即使在错误异常处理程序中*/
#define ENABLE_MPU_IN_EXCEPTION() MPU_CTRL->HFNMIENA=1

/*启用背景区*/
#define ENABLE_DEFAULT_MEMORY_MAP() MPU_CTRL->PRIVDEFENA=1


/*获得浮点堆栈上未填充浮点寄存器的地址*/
#define __GET_FPU_STACK_ADDRESS() *FPU_FPCAR

/*启用CONTROL的FPCA位*/
#define ENABLE_CONTROL_FPCA() FPU_FPCCR->ASPEN=1

/*启用浮点惰性压栈*/
#define ENABLE_FPU_LAZY_STACK() FPU_FPCCR->LSPEN=1

/*32位寄存器(原子)独占访问,异或操作,别的操作官方有*/
#define EXC_XOR_SET(REG,BIT) while (__STREXW (__LDREXW ((uint32_t *)&(REG))^(BIT), (uint32_t *)&(REG)))
    
/*进入NMI中断*/
// NMI挂起标志会自动清除,无需手动清除标志位,几乎监测不到
// 发挥不了作为状态标志位的作用
#define JUMP_TO_NMI() SCB_ICSR->NMIPENDSET = 1

/*进PendSV中断*/
#define JUMP_TO_PENDSV() SCB_ICSR->PENDSVSET = 1

/*获取PendSV挂起状态*/
// 这个获取状态的宏不好使
#define GET_PENDSV_STATE() (volatile uint8_t)SCB_ICSR->PENDSVSET

/*清除PendSV挂起状态*/
#define CLEAR_PENDSV_STATE() SCB_ICSR->PENDSVCLR = 1

/*进入systick中断*/
#define JUMP_TO_SYSTICK_HANDLE() SCB_ICSR->PENDSTSET = 1

/*查看systick中断状态*/
// 这个获取状态的宏不好使
#define GET_SYSTIC_HANDLE_STATE() (volatile uint8_t)SCB_ICSR->PENDSTSET

/*清除systick中断标志*/
#define CLEAR_SYSTIC_HANDLE_STATE() SCB_ICSR->PENDSTCLR = 1

/*禁用dual-issue*/
#define DISABLE_DUAL_ISSUE() SCB_ACTLR->DISFOLD = 1

/*禁用FPU异常输出*/
#define DISABLE_FPU_EXCEPTION_OUT() SCB_ACTLR->FPEXCODIS = 1

/*禁用读分配*/
// 对于Write-Back Write-Allocate的区域
#define DISABLE_READ_ALLOCATE() SCB_ACTLR->DISRAMODE = 1

/*禁用ITM and DWT ATB flush*/
#define DISBALE_ITM_AND_DWT_ATB_FLUSH SCB_ACTLR->DISITMATBFLUSH = 1

/*设置中断向量表的偏移*/
#define SET_VTOR_ADD(add) *SCB_VTOR = (add)

/*允许非特权软件访问STIR(软件触发中断)*/
#define ALLOW_UNPRIVILEGED_STIR() SCB_CCR->USERSETMPEND=1

/*捕获除0错误*/
// 可能是优化的原因,不好使,这位设了看不出效果
#define DIV_BY_0_TRAP_ENABLE() SCB_CCR->DIV_0_TRP = 1

/*捕获非对齐访问错误*/
#define UNALIGN_ACCESS_TRAP_ENABLE() SCB_CCR->UNALIGN_TRP = 1

/*获取当前中断分组*/
#define __GET_INTERRUPT_GROUPING() SCB_AIRCR->PRIGROUP

/*使能use fault异常*/
#define ENABLE_USE_FAULT_HANDLE() SCB_SHCSR->USGFAULTENA = 1

/*使能bus fault异常*/
#define ENABLE_BUS_FAULT_HANDLE() SCB_SHCSR->BUSFAULTENA = 1

/*使能MemManage fault异常*/
#define ENABLE_MEM_MANAGE_FAULT_HANDLE() SCB_SHCSR->MEMFAULTENA = 1

/*使能I-Cache*/
#define ENABLE_I_Cache() SCB_CCR->IC = 1

/*失能I-Cache*/
#define DISABLE_I_Cache() SCB_CCR->IC = 0

/*使能D-Cache*/
#define ENABLE_D_Cache() SCB_CCR->DC = 1

/*失能D-Cache*/
#define DISABLE_D_Cache() SCB_CCR->DC = 0

/*使能ITCM*/
#define ENBABLE_ITCM() AC_ITCMR->EN = 1

/*使能DTCM*/
#define ENBABLE_DTCM() AC_DTCMR->EN = 1

/*使能ITCM的读改写*/
#define ENBABLE_ITCM_RMW() AC_ITCMR->RMW = 1

/*使能DTCM的读改写*/
#define ENBABLE_DTCM_RMW() AC_DTCMR->RMW = 1

/*使能ITCM的重试输出*/
#define ENBABLE_ITCM_RET() AC_ITCMR->RETEN = 1

/*使能DTCM的重试输出*/
#define ENBABLE_DTCM_RET() AC_DTCMR->RETEN = 1

/*使能AHBP*/
#define ENABKE_AHBP() AC_AHBPCR->EN = 1

/*缓存一致性策略使用1的那种方式*/
// 这位需要时候看手册吧
#define CACHE_COHERENCY_1() AC_CACR->SIWT = 1

/*禁用缓存中ECC*/
//这位在缓存关闭的时候设置,并且设置完要无效化缓存.
#define DISABLE_ECC_IN_CACHE() AC_CACR->ECCEN = 1

/*开启强制透写*/
#define ENABLE_FORCE_WRITE_THROUGH() AC_CACR->FORCEWT = 1

/*禁用所有可配置优先级中断*/
#define DISNABLE_ALL_CONFIGURABLE_INTERRUPTS() __asm volatile("CPSID i")

/*启用所有可配置优先级中断*/
#define ENABLE_ALL_CONFIGURABLE_INTERRUPTS() __asm volatile("CPSIE i")

/*禁用所有除NMI的中断*/
#define DISNABLE_INTERRUPTS_EXCEPT_NMI() __asm volatile("CPSID f")
    
/*启用所有除NMI的中断*/
#define ENABLE_INTERRUPTS_EXCEPT_NMI() __asm volatile("CPSIE f")

/*得到当前优先级分组*/
#define __GET_INTERRUPT_GROUPING() SCB_AIRCR->PRIGROUP

/*使能中断*/
//注意值不能是负数
#define __enable_IRQ(irq_num) NVIC_ISER[(irq_num)>>5]=1<<((irq_num)&0x1f)

/*判断使能没使能*/
#define __is_enable_IRQ(irq_num) (uint8_t)((NVIC_ISER[(irq_num)>>5]>>((irq_num)&0x1f))&1)

/*软件触发IRQ*/
#define __trigger_IRQ(irq_num) *NVIC_STIR=irq_num

/*关闭中断*/
//注意值不能是负数
static inline void __disable_IRQ(uint8_t irq_num)
{   
    NVIC_ICER[irq_num>>5]=1<<(irq_num&0x1f);//相当于/32 , %32
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*挂起中断*/
#define __set_pending_IRQ(irq_num) NVIC_ISPR[(irq_num)>>5]=1<<((irq_num)&0x1f)

/*结束挂起中断*/
#define __clear_pending_IRQ(irq_num) NVIC_ICPR[(irq_num)>>5]=1<<((irq_num)&0x1f)

/*获取中断active*/
#define __get_IRQ_active(irq_num) ((NVIC_IABR[(irq_num)>>5]>>((irq_num)&0x1f))&1)

/*计算优先级*/
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

/*关闭此优先级及其以下中断*/
static inline void __disable_specific_interrupts(uint8_t GroupPriority,uint8_t SubPriority)
{
    __asm volatile("MSR basepri, %0" : : "r"(__trans_p_s_to_u8(GroupPriority,SubPriority)) : "memory");
}

/*开启中断,涉及basepri寄存器*/
#define __enable_all_interrupts_basepri() __asm("MSR basepri, %0"::"r"(0):"memory");


/*设置中断优先级*/
//包括设置内核中断
static inline void __set_priority(IRQn_e irq_num,uint8_t GroupPriority,uint8_t SubPriority)
{
    if(irq_num>=0)
        NVIC_IPR[irq_num]=__trans_p_s_to_u8(GroupPriority,SubPriority);
    else 
        SCB_SHPR1[(((uint32_t)irq_num)&0xf)-4]=__trans_p_s_to_u8(GroupPriority,SubPriority);
    //这里把负数内存中的补码取了出来运算 -1 -> 0xffff,-2 -> 0xfffe
}

/*设置中断分组*/
// PRIGROUP，stm只采取了8位中的前四位
// 参数见 interrupt_grouping_e
static inline void set_interrupt_grouping(uint8_t num)
{
    uint32_t temp = *(uint32_t *)SCB_AIRCR;
    ((scb_aircr_t *)&temp)->VECTKEY = 0x5FA; // 写密码
    ((scb_aircr_t *)&temp)->PRIGROUP = num;  // 写分组数
    *(uint32_t *)SCB_AIRCR = temp;           // 赋值
    while (SCB_AIRCR->PRIGROUP != num)
        ; // 回看
}

/*软件复位*/
static __inline void __software_reset(void)
{
    __asm("dsb");
    uint32_t temp = *(uint32_t *)SCB_AIRCR;
    ((scb_aircr_t *)&temp)->VECTKEY = 0x5FA; // 写密码
    ((scb_aircr_t *)&temp)->SYSRESETREQ = 1; //
    *(uint32_t *)SCB_AIRCR = temp;           // 赋值
    __asm("dsb");
    while (1)
        ;
}

/*详细地内核设置*/
static inline void detailed_kernel_Settings(void)
{
    ENABLE_USE_FAULT_HANDLE();        // 启用用法错误处理程序
    ENABLE_BUS_FAULT_HANDLE();        // 启用总线错误处理程序
    ENABLE_MEM_MANAGE_FAULT_HANDLE(); // 启用内存管理异常处理程序
    DIV_BY_0_TRAP_ENABLE();           // 捕获除0错误
    ALLOW_UNPRIVILEGED_STIR();        // 允许非特权软件访问STIR
    //  UNALIGN_ACCESS_TRAP_ENABLE(); //捕获非对齐访问错误
    ENABLE_CONTROL_FPCA();            //启用control.fpca位
    ENABLE_FPU_LAZY_STACK();          //启用浮点惰性压栈
//    ENBABLE_ITCM();//使能ITCM,reset处的函数带此功能
//    ENBABLE_DTCM();//使能DTCM,reset处的函数带此功能
//    ENBABLE_ITCM_RMW();//使能ITCM的读改写
//    ENBABLE_DTCM_RMW();//使能DTCM的读改写
//    ENBABLE_ITCM_RET();//使能ITCM的重试输出
//    ENBABLE_DTCM_RET();//使能DTCM的重试输出
} 

/*初始化滴答定时器*/
// use_exception:1,启用异常
// reload:重装载值,该值为你要的时钟脉冲数-1;
// clock_source,时钟源:1,处理器时钟;0,外部时钟
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

/*使能FPU*/
//全访问,特权和非特权代码均可使用,具体寄存器未编程.
static inline void enable_fpu(void)
{
     __asm volatile("LDR R0,=0xE000ED88"); 
     __asm volatile("MOV R1,#0xF00000");   
     __asm volatile("STR R1,[R0]");  
     __asm volatile("DSB"); 
     __asm volatile("ISB");    
}

/*使能I-Cache*/
static inline void enable_I_Cache(void)
{
    __asm volatile("dsb");
    __asm volatile("isb");
    *CACHE_ICIALLU=0;   //无效化I-Cache
    __asm volatile("dsb");
    __asm volatile("isb");
    ENABLE_I_Cache();   //使能I_Cache
    __asm volatile("dsb");
    __asm volatile("isb");
}

/*失能I-Cache*/
static inline void disable_I_Cache(void)
{
    __asm volatile("dsb");
    __asm volatile("isb");
    DISABLE_I_Cache();   //失能ICache
    *CACHE_ICIALLU=0;   //无效化I-Cache
    __asm volatile("dsb");
    __asm volatile("isb");
}
//#include "core_cm7.h"
/*使能D cache*/
// 不优化 __attribute__((optnone))
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



/*失能D_Cache*/
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

/*无效化D_Cache*/
//内存进cache
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
//cache进内存
static inline void clean_D_Cache(void)
{
    CACHE_CCSELR->InD=0;//选择查看D_Cache的信息
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

/*clean 并且无效化 D_Cache*/
//双向同步
static inline void clean_and_invalidaye_D_Cache(void)
{
    CACHE_CCSELR->InD=0;//选择查看D_Cache的信息
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

/*无效化数据缓存,按地址*/
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
    
/*清除数据缓存,按地址*/
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

/*清除,无效化数据缓存,按地址*/
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

/*无效化指令缓存,按地址*/
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

/*设置以下优先级*/
void SCB_CPUID_show_info(void);    // 显示CPUID信息
void SCB_ICSR_show_info(void);     // 展示SCB_ICSR提供的信息
void SCB_show_fault_info(void);    // 展示SCB_SHCSR信息
void SCB_show_detailed_info(void); // 展示详细的异常信息
void show_ipsr_info(void);         // 展示IPSR异常
void show_CONTROL_info(void);      // 展示CONTROL寄存器信息
void FPU_CCR_show_info(void);      // 展示关于FPU执行过程中的一些信息(不是计算状态寄存器,那个没编程)

void set_control(uint32_t value); // 设置控制寄存器
void mpu_set(void);               // 设置mpu
void cache_set(void);             // 设置缓存
void set_votor(uint32_t addr);    //

#endif
