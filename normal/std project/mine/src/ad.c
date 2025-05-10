/**
 *******************************************************************************
 * @file        ad.c
 * @author      liqingxin0092
 * @version     V1.1
 * @date        2025-3-29
 * @brief       精细控制ad转换
 * 更新记录:    3.30 增加了硬件过采样相关控制
 *              4.1  增加了dma传输扫描多路ADC数据
 *****************************************************************************************
 */
#include "ad.h"
#include "stdint.h"
#include "gd32f4xx_adc.h"
#include "gd32f4xx_gpio.h"
#include <board.h>
#include "gd32f4xx_dma.h"

//ad这里输出结果是:
//硬件过采样外加软件过采样后的结果,本质都是取平均.
//过采样数的增加使得信号越来越平稳,但是出数率会降低.

/*通道到IO的映射表*/
const static chx_io_t chx_to_io_table[16] = {
    {GPIOA, GPIO_PIN_0},
    {GPIOA, GPIO_PIN_1},
    {GPIOA, GPIO_PIN_2},
    {GPIOA, GPIO_PIN_3},
    {GPIOA, GPIO_PIN_4},
    {GPIOA, GPIO_PIN_5},
    {GPIOA, GPIO_PIN_6},
    {GPIOA, GPIO_PIN_7},
    {GPIOB, GPIO_PIN_0},
    {GPIOB, GPIO_PIN_1},
    {GPIOC, GPIO_PIN_0},
    {GPIOC, GPIO_PIN_1},
    {GPIOC, GPIO_PIN_2},
    {GPIOC, GPIO_PIN_3},
    {GPIOC, GPIO_PIN_4},
    {GPIOC, GPIO_PIN_5},
};

/*校准*/
// periph_num  :ADC0,ADC1,ADC2
static void ad_calibration(uint32_t periph_num)
{
    if ((ADC_CTL1(periph_num) & 1) != 1) // ADC没启动在这里卡的.
        while (1)
            ;
    ADC_CTL1(periph_num) |= 0x8; // 复位校准
    delay_us(10);
    ADC_CTL1(periph_num) &= ~0x8; // 结束复位校准
    ADC_CTL1(periph_num) |= 0x4;  // 开始校准
    while (ADC_CTL1(periph_num) & 0x4)
        ; // 如果没校准完在这里等的.
}

/*io初始化*/
// chx:      通道编号,0~15
static void ad_io_init(uint8_t chx)
{
    uint8_t GPIOx_num = 0;
    GPIOx_num = (GPIO_PORT_chx(chx) - GPIOA) >> 10;
    if ((RCU_AHB1EN & (1 << GPIOx_num)) == 0) // 时钟门控
        RCU_AHB1EN |= 1 << GPIOx_num;
    gpio_mode_set(GPIO_PORT_chx(chx), GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_chx(chx));
}

/*配置扫描模式的DMA*/
//如果启用扫描模式,外部调用这个
//这里固定一点
//选择DMA1. 000,通道0,ADC0
//          001,通道2,ADC1
//          010,通道0,ADC2
void ad_dma_init(uint32_t periph_num)
{
    if((RCU_AHB1EN&0x400000)!=1)
    {
        RCU_AHB1EN|= 0x400000;
    }
    /*ADC0和ADC2的情况*/
    if(periph_num!=ADC1)
    {
        DMA_CH0CTL(DMA1)=0; //这寄存器复位
        /*这里默认外设到存储器*/
        /*这里默认选择m0寄存器*/
        /*如果是ADC0的话不会有什么操作,DMA1通道0默认选择ADC0*/
        if(periph_num==ADC2)
        {
            DMA_CH0CTL(DMA1)|=0x4000000; //DMA1通道0选择ADC2
        }
        DMA_CH0PADDR(DMA1)=(uint32_t)&ADC_RDATA(periph_num);
        /*优先级默认低*/
        DMA_CH0CTL(DMA1)|=0x2800   //设置双方16位传输
                        |0x400     //内存地址增加
                        |0x20;     //外设控制DMA 
    }
    /*ADC1的情况*/
    else 
    {
        DMA_CH2CTL(DMA1)=0; //这寄存器复位
        /*这里默认外设到存储器*/
        /*这里默认选择m0寄存器*/
        DMA_CH2CTL(DMA1)|=0x2000000; //DMA1通道2选择ADC1
        DMA_CH2PADDR(DMA1)=(uint32_t)&ADC_RDATA(periph_num);
        /*优先级默认低*/
        DMA_CH2CTL(DMA1)|=0x2800   //设置双方16位传输
                        |0x400    //内存地址增加
                        |0x20;     //外设控制DMA
    }
}

/*ad初始化*/
// 时钟问题:
// 主频168Mhz
// 理论上HCLK=168Mhz.   PCLK=HCLK/2=84Mhz
// ADC时钟选择,ADC_SYNCCTL的ADCCK,既选择来源,又配置了分频.
// 最大40Mhz
// periph_num  :ADC0,ADC1,ADC2
// src_div :000：PCLK2 2分频
//          001：PCLK2 4分频
//          010：PCLK2 6分频
//          011：PCLK2 8分频
//          100：HCLK 5 分频
//          101：HCLK 6 分频
//          110：HCLK 10 分频
//          111：HCLK 20 分频
// 如果000,就是41Mhz.
// chx_lenth:   初始化通道的数量
// chx_sets:    通道编号的集合  [3:0]: 0~15 ,第0个位置
//                              [7:4]: 0~15 ,第1个位置
//                             [11:8]: 0~15 ,第2个位置
//                            [15:12]: 0~15 ,第3个位置
//                            [19:16]: 0~15 ,第4个位置
//                            [23:20]: 0~15 ,第5个位置
//                            [27:24]: 0~15 ,第6个位置
//                            [31:28]: 0~15 ,第7个位置
//                            [35:32]: 0~15 ,第8个位置
//                            [39:36]: 0~15 ,第9个位置
//                            [43:40]: 0~15 ,第10个位置
//                            [47:44]: 0~15 ,第11个位置
//                            [51:48]: 0~15 ,第12个位置
//                            [55:52]: 0~15 ,第13个位置
//                            [59:56]: 0~15 ,第14个位置
//                            [63:60]: 0~15 ,第15个位置
// hw_ovsmpl:硬件过采样         [3:0]:0001：移1位 //这里相当于除法
//                                   0010：移2位
//                                   0011：移3位
//                                   0100：移4位
//                                   0101：移5位
//                                   0110：移6位
//                                   0111：移7位
//                                   1000：移8位     ->  /256
//                             [6:4]:000：2x  //这里是过采样数
//                                   001：4x
//                                   010：8x
//                                   011：16x
//                                   100：32x
//                                   101：64x
//                                   110：128x
//                                   111：256x
void ad_init(uint32_t periph_num, uint8_t src_div, uint8_t chx_lenth, uint64_t chx_sets, uint8_t hw_ovsmpl)
{
    uint8_t temp = 0;
    for (uint8_t i = 0; i < chx_lenth; i++)
    {
        temp = chx_sets & 0xf;
        ad_io_init(temp);
        chx_sets >>= 4;
    }
    
    uint8_t adc_num = (periph_num - ADC_BASE) >> 8; // 值0,1,2
    if ((RCU_APB2EN & (1 << (8 + adc_num))) == 0)   // 时钟门控使能
        RCU_APB2EN |= 1 << (8 + adc_num);
    ADC_SYNCCTL |= (src_div << 16)  // 源,div设置
                |   0x800000;       // 使能内部通道
    /*这里默认数据右对齐*/
    /*这里默认ADC为独立模式*/
    /*这里默认12位分辨率*/
    /*这里默认一次转换触发所有过采样*/
    ADC_OVSAMPCTL(periph_num) |= ((hw_ovsmpl & 0xf) << 5)         // 除法
                              |  (((hw_ovsmpl >> 4) & 7) << 2);   // 乘法
    ADC_OVSAMPCTL(periph_num) |= 1;                               // 过采样使能,注意使能位最好不要并行设置
    ADC_CTL1(periph_num) |= 1;                                    // adc on
    delay_us(1);
    ad_calibration(periph_num); // 校准
}

/*单次模式初始化*/
// 软件触发
// periph_num  :ADC0,ADC1,ADC2
// chx:            通道编号,0~18.
// smp_time:   000:通道采样时间为 3个周期
//             001:通道采样时间为 15个周期
//             010:通道采样时间为 28个周期
//             011:通道采样时间为 56个周期
//             100:通道采样时间为 84个周期
//             101:通道采样时间为 112个周期
//             110:通道采样时间为 144个周期
//             111:通道采样时间为 480个周期
// 返回值:这次的AD值
void ad_single_init(uint32_t periph_num, uint8_t chx, uint8_t smp_time)
{
    ADC_CTL0(periph_num) = 0;  // 寄存器复位
    ADC_CTL1(periph_num) &= 1; // 除了第一位都清
    ADC_SAMPT0(periph_num) = ADC_SAMPT1(periph_num) = 0;
    ADC_RSQ2(periph_num) |= chx; // RL不配,默认一个通道.
    (chx > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (chx - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * chx));
}
 
/*adc产出一个数据*/
uint16_t ad_start_once(uint32_t periph_num)
{
    uint16_t rel = 0;
    ADC_CTL1(periph_num) |= 0x40000000; // 开始
    while ((ADC_STAT(periph_num) & 2) == 0)
        ; // 没转换完在这里等的
    rel = ADC_RDATA(periph_num);
    return rel;
}


/*扫描模式初始化*/
void ad_scan_init(uint32_t periph_num, chx_sets_t * p_struct,uint8_t smp_time)
{
     ADC_CTL0(periph_num)|=0x100; //SM扫描
     ADC_CTL1(periph_num)|=0x100  //DMA请求
                         |0x200;  //DDM
     ADC_RSQ0(periph_num)|= (p_struct->chx_length-1)<<20;//设置数量
     for(uint8_t i=0;i<p_struct->chx_length;i++)     //填ADC_RSQ2
     {                                               //sample time 也一块干了.
        if(i>5)         //6就不行,超RSQ2了
            break;      //从0到5,这里填了前6个
        (p_struct->chx_buffer[i] > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (p_struct->chx_buffer[i] - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * p_struct->chx_buffer[i]));
        ADC_RSQ2(periph_num)|= (p_struct->chx_buffer[i])<<(5*i);        
     }
     if(p_struct->chx_length>6)     //至少7
     {
         for(uint8_t i=6;i<p_struct->chx_length;i++)    //填ADC_RSQ1
         {
             (p_struct->chx_buffer[i] > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (p_struct->chx_buffer[i] - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * p_struct->chx_buffer[i]));
             ADC_RSQ1(periph_num)|= (p_struct->chx_buffer[i])<<(5*(i-6));
         }
     }
     if(p_struct->chx_length>12)//前十二个都在RSQ2和RSQ1中设置
     {
         for(uint8_t i=12;i<p_struct->chx_length;i++)    //填ADC_RSQ0
         {
             (p_struct->chx_buffer[i] > 9) ? (ADC_SAMPT0(periph_num) |= smp_time << (3 * (p_struct->chx_buffer[i] - 10))) : (ADC_SAMPT1(periph_num) |= smp_time << (3 * p_struct->chx_buffer[i]));
             ADC_RSQ0(periph_num)|= (p_struct->chx_buffer[i])<<(5*(i-12));
         }
     }
}   

/*扫描一下*/
void ad_scan_once(uint32_t periph_num,uint16_t * m_addr)
{
    uint32_t i=0;
    /*ADC0,ADC2的情况*/
    if(periph_num!=ADC1)
    {
        DMA_CH0CTL(DMA1)&=~1; //dma失能
        while((DMA_CH0CTL(DMA1)&1)!=0);//没失能成功
        DMA_INTC0(DMA1)|=1<<5;//清接收完成标志
        DMA_CH0M0ADDR(DMA1)=(uint32_t)m_addr;//设置地址
        DMA_CH0CTL(DMA1)|=1;    //dma使能
    }
    /*ADC1的情况*/
    else 
    {
        DMA_CH2CTL(DMA1)&=~1; //dma失能
        while((DMA_CH2CTL(DMA1)&1)!=0);//没失能成功
        DMA_INTC0(DMA1)|=1<<21;//清接收完成标志
        DMA_CH2M0ADDR(DMA1)=(uint32_t)m_addr;//设置地址
        DMA_CH2CTL(DMA1)|=1;    //dma使能
    }
    ADC_CTL1(periph_num) |= 0x40000000; // 开始
    while((ADC_STAT(periph_num)&2)==0) // 没转换完在这里等的
    { 
        if(++i==0x50000)
            ADC_CTL1(periph_num)|= 0x40000000; // 开始
    }
}


/*内层软件过采样*/
// inter_fre:内层采样次数 ,>=3
static uint16_t ad_soft_filter_inter(uint32_t periph_num, uint16_t inter_fre)
{
    uint16_t max_val = 0;
    uint16_t min_val = 0;
    uint16_t ad_temp = 0;
    uint32_t inter_add = 0;
    max_val = min_val = ad_start_once(periph_num);
    for (uint16_t i = 0; i < inter_fre; i++)
    {
        ad_temp = ad_start_once(periph_num);
        (max_val > ad_temp) ? (0) : (max_val = ad_temp);
        (min_val < ad_temp) ? (0) : (min_val = ad_temp);
        inter_add += ad_temp;
    }
    return (inter_add - max_val - min_val) / (inter_fre - 2);
}

/*软件过采样*/
// inter_fre:内层采样次数 ,>=3
// ex_fre:外层采样次数,>=3
uint16_t ad_soft_filter(uint32_t periph_num, uint16_t inter_fre, uint16_t ex_fre)
{
    uint16_t max_val = 0;
    uint16_t min_val = 0;
    uint16_t ad_temp = 0;
    uint32_t add_val = 0;
    max_val = min_val = ad_soft_filter_inter(periph_num,  inter_fre);
    for (uint16_t i = 0; i < ex_fre; i++)
    {
        ad_temp = ad_soft_filter_inter(periph_num,  inter_fre);
        (max_val > ad_temp) ? (0) : (max_val = ad_temp);
        (min_val < ad_temp) ? (0) : (min_val = ad_temp);
        add_val += ad_temp;
    }
    return (add_val - max_val - min_val) / (ex_fre - 2);
}

