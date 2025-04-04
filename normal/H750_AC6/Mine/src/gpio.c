/**
 *******************************************************************************
 * @file        gpio.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-2-20
 * @brief       配置,控制GPIO
 * 更新记录:    2025-3-22 :增加了对IO补偿的支持.
 * 
 *****************************************************************************************
 */
#include "gpio.h"
#include "stm32h750xx.h"

/*设置一个引脚*/
//mode:00,输入模式
//     01,输出模式
//     10,复用模式
//     11,模拟模式
//out_type:0,推挽输出
//         1,开漏输出
//speed:00,low
//      01,medium
//      10,high
//      11,very high
//up_or_down:00,浮空
//           01,上拉
//           10,下拉
//alterfun:0000~1111 --> AF0~AF15
void gpio_init_pin(gpio_t* GPIOx,uint8_t pin,uint8_t mode,uint8_t out_type,uint8_t speed,uint8_t up_or_down,uint8_t alterfun)
{
    GPIOx->MODER&=~(3<<(pin<<1));//模式对应位清除复位值
    GPIOx->MODER|=mode<<(pin<<1);//模式设置
    GPIOx->OTYPER|=out_type<<pin;//输出类型设置
    GPIOx->OSPEEDR&=~(3<<(pin<<1));//速度对应位清除复位值
    GPIOx->OSPEEDR|=speed<<(pin<<1);//速度设置
    GPIOx->PUPDR&=~(3<<(pin<<1));//上下拉对应位清除复位值
    GPIOx->PUPDR|=up_or_down<<(pin<<1);//上下拉设置
    (0<=pin&&pin<=7)?(GPIOx->AFRL|=alterfun<<(pin<<2)):(GPIOx->AFRH|=alterfun<<((pin-8)<<2));
}
 
/*锁定引脚配置(锁定对应控制和功能寄存器)*/
//pin_group: msb ~~ lsb , 1111 1111 1111 1111 ,锁哪个,对应位置1
void gpio_lock_pin(gpio_t* GPIOx,uint16_t pin_group)
{
    uint32_t key=0x10000|(uint32_t)pin_group;
    GPIOx->LCKR=key;
    GPIOx->LCKR=(uint32_t)pin_group;
    GPIOx->LCKR=key;
    (void)GPIOx->LCKR;
    while(!GPIOx->LCKR);
}

/*io端口使能*/
//注意对应选项字节得check
//注意一下官方的警告:
//This bit is active only if IO_HSLV user option bit is set.
//It must be used only if the product supply voltage is below 2.7 V.
//Setting this bit when VDD is higher than 2.7 V might be destructive.
static inline void en_io_compensation(void)
{
    if((RCC->CR&0x100)==0)//如果CSI没开
    {
        RCC->CR|=0x80;//开CSI
        while((RCC->CR&0x100)==0);//如果CSI还没开在这里等的.
    }
    SYSCFG->CCCSR|=0x10000;//16bit,速度优化
    /*这里选择默认的补偿code*/
    while((SYSCFG->CCCSR&0x100)==0)//如果io补偿单元没准备好,在这里等的.
    SYSCFG->CCCSR|=1;//en io 补偿单元.      
}
    
/*应用层*/
//use_io_comp: 1,使用io补偿单元. 0,不使用io补偿单元. 
void RCC_GPIO_Init(uint8_t use_io_comp)
{
    RCC->AHB4ENR|=0x1f;//开GPIOABCDE时钟.   
    while((RCC->AHB4ENR&0x1f)!=0x1f); //上面的检测机制
    
    if(use_io_comp==1)        //启用io补偿单元
        en_io_compensation();
    gpio_init_pin(GPIO_C,13,1,0,2,0,0);//gpioc 13 推挽输出,测试灯
}



