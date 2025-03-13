/**
 *******************************************************************************
 * @file        H7_CORE.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-14
 * @brief       CMSIS层级代码
 *              深入挖h7系统级寄存器
 *****************************************************************************************
 */

#include "H7_CORE.h"
#include "stdio.h"

/*获取ipsr信息*/
void show_ipsr_info(void)
{
    uint32_t ulCurrentInterrupt;//                 写              读
    __asm volatile ( "mrs %0, ipsr" : "=r" ( ulCurrentInterrupt ):    :"memory" );
    printf("\r\n异常编号(0表示线程模式):%d\r\n",ulCurrentInterrupt);
}

/*设置CONTROL寄存器*/
//处理模式下不能设置栈指针,固定用的MSP
//00:msp,特权.  01:msp ,用户. 10:psp,特权. 11:psp,用户
void set_control(uint32_t value)
{
    __asm volatile( "msr control, %0" : : "r"(value)   :"memory" );
    __asm volatile("isb");
}

/*展示CONTROL寄存器信息*/
void show_CONTROL_info(void)
{
    uint32_t val;
    __asm volatile ( "mrs %0, CONTROL" : "=r" ( val ):    :"memory" );
    printf("\r\n以下是CONTROL寄存器信息:\r\n");
    if((val>>2)&1)
        printf("浮点单元上下文存在\r\n");
    else
        printf("浮点单元上下文不存在\r\n");
    if((val>>1)&1)
        printf("当前用的PSP\r\n");
    else
        printf("当前用的MSP\r\n");
    if(val&1)
        printf("当前软件等级为非特权\r\n");
    else
        printf("当前软件等级为特权\r\n");
}
    

/*显示CPUID信息*/
void SCB_CPUID_show_info(void)
{
    printf("\r\n厂商(实现者):%#x\r\n", SCB_CPUID->Implementer);
    if (SCB_CPUID->Implementer == 0x41)
    {
        printf("代表ARM\r\n");
    }
    printf("处理器号:%#x\r\n", SCB_CPUID->PartNo);
    if (SCB_CPUID->PartNo == 0xC27)
    {
        printf("代表Cortex-M7\r\n");
    }
    printf("版本号:%d\r\n", SCB_CPUID->Variant);
    printf("补丁号 :%d\r\n", SCB_CPUID->Revision);
    printf("r%dp%d\r\n", SCB_CPUID->Variant, SCB_CPUID->Revision);
}

/*展示SCB_ICSR提供的信息*/
// 提供的信息部分不准确比如RETTOBASE,挂起状态标志
void SCB_ICSR_show_info(void)
{
    volatile scb_icsr_t temp = *SCB_ICSR;
    if (temp.VECTACTIVE)
    {
        printf("\r\n正执行的异常编号:%d\r\n", temp.VECTACTIVE);
    }
    else
    {
        printf("现在没有中断执行\r\n");
    }
    if (temp.RETTOBASE)
    {
        printf("没有发生中断抢占行为\r\n");
    }
    else
    {
        printf("发生了中断抢占行为\r\n");
    }
    if (temp.VECTPENDING)
    {
        printf("待处理的异常编号:%d\r\n", temp.VECTPENDING);
    }
    else
    {
        printf("没有待处理的中断\r\n");
    }
    if (temp.ISRPENDING)
    {
        printf("有中断挂起,(不包括NMI,FAULT)\r\n");
    }
    else
    {
        printf("没有中断挂起,(不包括NMI,FAULT)\r\n");
    }
    if (temp.PENDSTSET)
    {
        printf("systic中断挂起了\r\n");
    }
    else
    {
        printf("systic中断没有挂起\r\n");
    }
    if (temp.PENDSVSET)
    {
        printf("PendSV中断挂起了\r\n");
    }
    else
    {
        printf("PendSV没有挂起\r\n");
    }
    if (temp.NMIPENDSET)
    {
        printf("NMI中断挂起了\r\n");
    }
    else
    {
        printf("NMI中断没有挂起\r\n");
    }
}

/*展示异常信息*/
void SCB_show_fault_info(void)
{
    printf("\r\nSCB_SHCSR整体值:%#x\r\n", *(uint32_t *)SCB_SHCSR);
    if (SCB_SHCSR->MEMFAULTACT)
        printf("MemManage exception正在执行\r\n");
    if (SCB_SHCSR->BUSFAULTACT)
        printf("BusFault exception正在执行\r\n");
    if (SCB_SHCSR->USGFAULTACT)
        printf("UsageFault exception正在执行\r\n");
    if (SCB_SHCSR->SVCALLACT)
        printf("SVCall正在执行\r\n");
    if (SCB_SHCSR->MONITORACT)
        printf("Debug monitor正在执行\r\n");
    if (SCB_SHCSR->PENDSVACT)
        printf("pendSV正在执行\r\n");
    if (SCB_SHCSR->SYSTICKACT)
        printf("SysTick handle正在执行\r\n");
    if (SCB_SHCSR->USGFAULTPENDED)
        printf("UsageFault exception挂起了\r\n");
    if (SCB_SHCSR->MEMFAULTPENDED)
        printf("MemManage exception挂起了\r\n");
    if (SCB_SHCSR->BUSFAULTPENDED)
        printf("BusFault exception挂起了\r\n");
    if (SCB_SHCSR->SVCALLPENDED)
        printf("SVCall挂起了\r\n");
}

/*展示稍微详细点的信息*/
// 详见异常手册
void SCB_show_detailed_info(void)
{
    uint8_t flag = 0;
    uint32_t mm_add = *SCB_MMAR;
    uint32_t bus_add = *SCB_BFAR;
    volatile scb_cfsr_u * temp = SCB_CFSR;
    if (temp->mini_struct.MMFSR)
    {
        flag = 1;
        printf("\r\n发生了内存管理方面的错误\r\n");
        printf("更多详细内容见异常手册\r\n");
        if (temp->bit_struct.IACCVIOL)
            printf("处理器尝试从不允许执行的位置获取指令\r\n");
        if (temp->bit_struct.DACCVIOL)
            printf("处理器尝试在不允许操作的位置进行加载或存储\r\n");
        if (temp->bit_struct.MUNSTKERR)
            printf("异常返回时的出栈导致访问冲突\r\n");
        if (temp->bit_struct.MSTKERR)
            printf("异常返回时的入栈导致访问冲突\r\n");
        if (temp->bit_struct.MLSPERR)
            printf("浮点惰性压栈期间出现内存管理错误\r\n");
        if (temp->bit_struct.MMARVALID)
            printf("MMAR寄存器中存在有效错误地址,值为:%#x\r\n", mm_add);
    }
    if (temp->mini_struct.BFSR)
    {
        flag = 1;
        printf("\r\n发生了总线错误\r\n");
        printf("更多详细内容见异常手册\r\n");
        if (temp->bit_struct.IBUSERR)
        {
            printf("指令总线错误\r\n");
            printf("可能是错误的函数指针跳转\r\n");
            printf("可能无效的返回，因为栈崩溃\r\n");
        }
        if (temp->bit_struct.PRECISERR)
            printf("精确的数据总线错误\r\n");
        if (temp->bit_struct.IMPRECISERR)
            printf("不精确的数据总线错误\r\n");
        if (temp->bit_struct.UNSTKERR)
            printf("异常返回时的出栈导致了一个或多个总线错误\r\n");
        if (temp->bit_struct.STKERR)
            printf("异常进入时的压栈导致了一个或多个总线错误\r\n");
        if (temp->bit_struct.LSPERR)
            printf("一个总线错误发生在浮点惰性压栈期间\r\n");
        if (temp->bit_struct.BFARVALID)
            printf("BFAR中存在有效的错误地址:%#x\r\n", bus_add);
    }
    if (temp->mini_struct.UFSR)
    {
        flag = 1;
        printf("\r\n发生了用法错误\r\n");
        printf("更多详细内容见异常手册\r\n");
        if (temp->bit_struct.UNDEFINSTR)
            printf("处理器尝试执行未定义的指令\r\n");
        if (temp->bit_struct.INVSTATE)
        {
            printf("处理器尝试执行非法使用EPSR的指令\r\n");
            printf("或者不是thumb状态,指令地地址lsb==0了\r\n");
        }
        if (temp->bit_struct.INVPC)
            printf("异常出栈时,EXC_RETURN值无效\r\n");
        if (temp->bit_struct.NOCP)
            printf("处理器尝试访问协处理器\r\n");
        if (temp->bit_struct.UNALIGNED)
            printf("处理器进行了未对齐的内存访问\r\n");
        if (temp->bit_struct.DIVBYZERO)
            printf("发生了除0错误\r\n");
    }
    if (SCB_HFSR->VECTTBL)
    {
        flag = 1;
        printf("\r\n发生了硬件错误\r\n");
        printf("异常处理时读向量表发生总线错误\r\n");
    }
    if (SCB_HFSR->FORCED)
    {
        flag = 1;
        printf("\r\n发生了硬件错误\r\n");
        printf("由其它错误处理程序升级而来\r\n");
    }

    /*展示关于ABFSR的信息*/
    switch (AC_ABFSR->AXIMTYPE)
    {
    case 0:
        printf("AC_ABFSR指示信息:\r\n");
        printf("\r\n可能存在地方欠妥\r\n");
        break;
    case 1:
        printf("AC_ABFSR指示信息:\r\n");
        printf("独占访问成功\r\n");
        break;
    case 2:
        printf("AC_ABFSR指示信息:\r\n");
        printf("从机错误\r\n");
        flag = 1;
        break;
    case 3:
        printf("AC_ABFSR指示信息:\r\n");
        printf("解码错误\r\n");
        flag = 1;
        break;
    }
    if (AC_ABFSR->ITCM)
        printf("总线错误出现在ITCM接口处\r\n");
    if (AC_ABFSR->DTCM)
        printf("总线错误出现在DTCM接口处\r\n");
    if (AC_ABFSR->AHBP)
        printf("总线错误出现在AHBP接口处\r\n");
    if (AC_ABFSR->AXIM)
        printf("总线错误出现在AXIM接口处\r\n");
    if (AC_ABFSR->EPPB)
        printf("总线错误出现在EPPB接口处\r\n");
    if (!flag)
        printf("\r\n没有错误发生\r\n");
}

/*展示FPCCR的信息*/
void FPU_CCR_show_info(void)
{
    printf("\r\n展示FPCCR的信息:\r\n");
    if(FPU_FPCCR->LSPACT)    
    {
        printf("浮点惰性压栈处于活跃状态,浮点栈帧已经被分配\r\n");
        printf("浮点堆栈上未填充浮点寄存器的地址%#x\r\n",__GET_FPU_STACK_ADDRESS());
    }
    else
        printf("浮点惰性压栈没有进行\r\n");
    printf("当浮点栈帧被分配时:");
    if(FPU_FPCCR-> USER)
        printf("软件处于用户级\r\n");
    else
        printf("软件处于特权级\r\n");
    if(FPU_FPCCR-> THREAD)
        printf("软件处于线程模式\r\n");
    else
        printf("软件处于处理模式\r\n");
    if(FPU_FPCCR-> HFRDY)
        printf("硬件错误允许挂起\r\n");
    else
        printf("硬件错误不允许挂起\r\n");
    if(FPU_FPCCR->MMRDY)
        printf("内存管理异常允许挂起\r\n");
    else
        printf("内存管理异常不允许挂起\r\n");
    if(FPU_FPCCR->BFRDY)
        printf("总线错误异常允许挂起\r\n");
    else
        printf("总线错误异常不允许挂起\r\n");
    if(FPU_FPCCR->MONRDY)
        printf("debug监视异常允许挂起\r\n");
    else
        printf("debug监视异常不允许挂起\r\n");
}
    

// 以下中断在别处用的时候注释
void NMI_Handler(void)
{
    printf("触发了NMI中断\r\n");
}

/*设置单个区域的内存特性*/
//region_code:0~15,区域号
//sub_dis:8位,写1禁用掉这1/8区域的配置,低位对应低位,
//        高位对应高位,从而暴露出与之重叠区域的配置
//        256 bytes大小及以上尺寸才能设置此位.
static void mpu_set_region(uint8_t region_code,uint32_t base_addr,mpu_attribute_t* mpu_para,uint8_t sub_dis,uint32_t size)
{
//    MPU_RASR->ENABLE=0;//失能设置位
    *MPU_RNR=region_code;//设置区域号
    *(uint32_t*)MPU_RBAR=base_addr;//设置基地址
    MPU_RASR->SRD =sub_dis;//设置禁用子区域
    MPU_RASR->SIZE=size;//设置尺寸
    MPU_RASR->XN=mpu_para->XN;
    MPU_RASR->AP=mpu_para->AP;
    MPU_RASR->TEX=mpu_para->TEX;
    MPU_RASR->C=mpu_para->C;
    MPU_RASR->B=mpu_para->B;
    MPU_RASR->S=mpu_para->S;
    MPU_RASR->ENABLE=1;//使能这个区域的配置
}

/*设置总体MPU*/
//应用级的函数
//经过测试8~15的号码也可以设置区域,甚至16也可以设置,注意测试.
void mpu_set(void)
{
    DISNABLE_ALL_CONFIGURABLE_INTERRUPTS();
    mpu_attribute_t mpu_struct={0};
    DISABLE_MPU();//失能MPU
    mpu_struct.AP=3;//全访问
    mpu_struct.XN=0;//允许指令访问,1才是禁止
    mpu_struct.TEX=0;//1级别
    mpu_struct.B=1;//允许缓冲
    mpu_struct.C=1;//允许缓存
    mpu_struct.S=0;//禁止共享
    mpu_set_region(0,0x20000000,&mpu_struct,0,MPU_REGION_SIZE_64_KBYTES);//保护整个DTCM,共64K字节
    mpu_set_region(1,0x20010000,&mpu_struct,0,MPU_REGION_SIZE_256_KBYTES);//保护整个SRAM1/SRAM2,共256K字节
    mpu_set_region(2,0x00000000,&mpu_struct,0,MPU_REGION_SIZE_16_KBYTES);//保护整个ITCM,共16K字节
    
    ENABLE_DEFAULT_MEMORY_MAP();//背景区的使能
    ENABLE_MPU();//使能MPU
    __asm volatile("dsb");
    __asm volatile("isb");
    ENABLE_ALL_CONFIGURABLE_INTERRUPTS();
}

/*应用级函数设置缓存*/
void cache_set(void)
{
    enable_I_Cache();
    enable_D_Cache();
}

//void PendSV_Handler(void)
//{
//    printf("进入了PendSV中断\r\n");
//    SCB_ICSR_show_info();  // 展示SCB_ICSR提供的信息
//    SCB_show_fault_info(); // 展示SCB_SHCSR信息
//    CLEAR_PENDSV_STATE();  // 清除标志位
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
	printf("Mem Access Error!!\r\n"); 	//输出错误信息
	while(1);
}
