/**
 *******************************************************************************
 * @file        h750_flash_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       给h750/743写个flash管理库
 *         3.13 : 用的时候再加吧.
 *              
 *****************************************************************************************
 */
#include "flash_m.h"
#include "stm32h750xx.h"

/*flash解锁操作*/
static void flash_unlock(void)
{
    FLASH->KEYR1= 0x45670123;
    FLASH->KEYR1= 0xCDEF89AB;
    while((FLASH->CR1&1)==1);//如果没解锁在这里卡的
}

/*选项字节解锁操作*/
static void option_bytes_unlock(void)
{
    FLASH->OPTKEYR=0x08192A3B;
    FLASH->OPTKEYR= 0x4C5D6E7F;
    while((FLASH->OPTCR&1)==1);//如果还锁的就在这等    
}

/*读操作就直接指针操作*/

/*flash写操作*/
//h750范围闪存地址范围:0x0800 0000 ~ 0x0802 0000
//lenth:字节数
//一缓存32个字节
void flash_single_write(uint32_t addr,uint32_t data_source,uint16_t lenth)
{
    uint16_t fifo_div=lenth>>5; //能装多少缓存
    uint16_t fifo_rem=lenth&0x1f;//缓存剩余
    if((FLASH->CR1&1)==1)//如果没解锁
    {
         flash_unlock();       //flash解锁操作
     }
     FLASH->CR1|=2;//PG位置1
     FLASH->CR1&=~(3<<4);//清除原来设置
     FLASH->CR1|=3<<4;//64位并行度
     for(uint32_t i=0;i<fifo_div;i++)//整个缓存操作
     {
         while((FLASH->SR1&1)==1);//如果忙,在这里卡的
         for(uint32_t j=0;j<4;j++)
         {
             *(uint64_t*)addr=*(uint64_t*)data_source;
             addr+=8;
             data_source+=8;
         }
         FLASH->CCR1|=10000;//清eop
     }
     while((FLASH->SR1&1)==1);//如果忙,在这里卡的
     while(fifo_rem>0)      //不足一个缓存
     {
         if(fifo_rem>7)
         {
             FLASH->CR1&=~(3<<4);//清除原来设置
             FLASH->CR1|=3<<4;//64位并行度
             *(uint64_t*)addr=*(uint64_t*)data_source;
             addr+=8;
             data_source+=8;
             fifo_rem-=8;
         }
         else if(fifo_rem>3)
         {
             FLASH->CR1&=~(3<<4);//清除原来设置
             FLASH->CR1|=2<<4;//32位并行度
             *(uint32_t*)addr=*(uint32_t*)data_source;
             addr+=4;
             data_source+=4;
             fifo_rem-=4;
         }
         else if(fifo_rem>1)
         {
             FLASH->CR1&=~(3<<4);//清除原来设置
             FLASH->CR1|=1<<4;//16位并行度
             *(uint16_t*)addr=*(uint16_t*)data_source;
             addr+=2;
             data_source+=2;
             fifo_rem-=2;
         }
         else 
         {
             FLASH->CR1&=~(3<<4);//清除原来设置,0:8位并行度
             FLASH->CR1|=0<<4;//8位并行度
             *(uint8_t*)addr=*(uint8_t*)data_source;
             addr+=1;
             data_source+=1;
             fifo_rem-=1;
         }
         if(fifo_rem==0)
         {
            FLASH->CR1|=1<<6;//强制写操作
         }
     }
     FLASH->CR1&=~2;//PG清0
}

/*擦除扇区*/
//这个先不着急,h750一擦全没了
void flash_erase_sector(void)
{
    
    
}


/*选项字节--RDP修改*/
//val:0xaa                  :level 0,如果没有其他保护措施,flash的所有操作都可以执行 
//    0xff(非0xaa,0xcc的数) :level 1,禁止调试器和ram中的代码对flash的任何访问(入侵操作),非入侵操作可以对
//                           flash的任何操作.
//    0xcc                  :level 2,调试特性全关,允许用户对flash进行操作,不允许从ram启动,选项字节不能修改.
//                           这个操作非常危险!!!
//注意:level从1变0,闪存全擦
void option_bytes_modify_RDP(uint8_t val)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    while((FLASH->SR1&1)==1);//如果忙,在这里卡的,这里包括了普通闪存操作和选项字节修改操作
    FLASH->OPTSR_PRG&=~(0xff<<8);//清除原有值
    FLASH->OPTSR_PRG|=val;//赋值
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙
}

/*修改看门狗的相关配置*/
// stop_en_cnt     : 1,stop模式下,idog仍然计数.    0,stop模式下,idog不计数
// standby_en_cnt  : 1,standby模式下,idog仍然计数. 0,standby模式下,idog不计数
// hard_or_soft    : 1,软件控制idog.  0,硬件控制idog(上电时自动使能)
void option_bytes_modify_IDOG(uint8_t stop_en_cnt,uint8_t standby_en_cnt,uint8_t hard_or_soft)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    while((FLASH->SR1&1)==1);//如果忙,在这里卡的,这里包括了普通闪存操作和选项字节修改操作
    FLASH->OPTSR_PRG&=~(1<<4);//sw
    FLASH->OPTSR_PRG&=~(3<<17);
    FLASH->OPTSR_PRG|=stop_en_cnt    << 17;
    FLASH->OPTSR_PRG|=standby_en_cnt << 18;
    FLASH->OPTSR_PRG|=hard_or_soft   <<  4;
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙
}


/*复位管理*/
//bor: 掉电复位水平,单片机电压低至一定的电压就会产生一次复位,以下按典型值举例.
//     0,Vbor0,单片机大概会在电压低至1.62~1.67时产生复位信号.
//     1,Vbor1,单片机大概会在电压低至2.00~2.10时产生复位信号.
//     2,Vbor2,单片机大概会在电压低至2.31~2.41时产生复位信号.
//     3,Vbor3,单片机大概会在电压低至2.61~2.70时产生复位信号.
//stop_d1:1,在d1 domain进stop模式不复位.0,复位
//stby_d1:1,在d1 domain进standby模式不复位.0,复位
//这里没有找到d2 domain相关设置,要设置的话看programmer吧. 
void option_bytes_modify_reset(uint8_t bor ,uint8_t stop_d1,uint8_t stby_d1)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    while((FLASH->SR1&1)==1);//如果忙,在这里卡的,这里包括了普通闪存操作和选项字节修改操作
    FLASH->OPTSR_PRG&=~(3<<2);
    FLASH->OPTSR_PRG&=~(3<<6);
    FLASH->OPTSR_PRG|= bor<<2;   
    FLASH->OPTSR_PRG|= stop_d1 << 6;
    FLASH->OPTSR_PRG|= stby_d1 << 7;
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙
}

/*交换bank1和2*/
//1,交换. 0，不交换.
//h750此项不可用
void option_bytes_modify_swap(uint8_t swap)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    while((FLASH->SR1&1)==1);//如果忙,在这里卡的,这里包括了普通闪存操作和选项字节修改操作
    FLASH->OPTSR_PRG&=~(1<<31);
    FLASH->OPTSR_PRG|= swap<<31;   
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙
}    

/*是否启用IO优化*/
//1,启用.     0,不启用.
void option_bytes_modify_IO_SPEED_OPTIMIZE(uint8_t is_optimize)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    while((FLASH->SR1&1)==1);//如果忙,在这里卡的,这里包括了普通闪存操作和选项字节修改操作
    FLASH->OPTSR_PRG&=~(1<<29);
    FLASH->OPTSR_PRG|= is_optimize<<29;   
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙 
}

/*设置专有代码读出保护*/
//h750flash:0x0800 0000 ~ 0x0802 0000   共128kb
//bank:选择扇区,1 or 2 , h750只能1
//start:地址代码开始
//end:地址代码结束
//DMEP:1,上述设置的保护区允许被擦除.0,不允许擦除
//保护规则: 保护区域大小 = [(END - START) + 1] x 256,基址为bank基址.
//eg: bank=1,start=0 ,end=0xf.  保护区域从0x0800 0000 到 0x0800 0FFF 共16 * 256 = 4kbytes
//建议DMEP这个得开启,读出保护等级从1变零会刷新这个配置.
void option_bytes_modify_PCROP(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMEP)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    while((FLASH->SR1&1)==1);//如果忙,在这里卡的,这里包括了普通闪存操作和选项字节修改操作
    if(bank==1)
    {
        FLASH->PRAR_PRG1=0;//清原来值.
        FLASH->PRAR_PRG1|=start;
        FLASH->PRAR_PRG1|=end<<16;
        FLASH->PRAR_PRG1|=DMEP<<31;
    }
    else if(bank==2)
    {
        FLASH->PRAR_PRG2=0;//清原来值.
        FLASH->PRAR_PRG2|=start;
        FLASH->PRAR_PRG2|=end<<16;
        FLASH->PRAR_PRG2|=DMEP<<31;
    }
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙 
}

/*安全专用区域设置*/
//地址规则和上一个一样  
//DMES置1,留后手,RDP归0可以复位这些操作
//这个函数最多开启secure位.区域设置不上.
//STM32H743 has no "secure area" (a.k.a. hide protection, HDP).
//This is the reason of "Error: Activating security Failure".
//H743的programer里都没有这个选项(设置保护区域)
//H750有设置区域选项但是设置不上.
//官方说h753才有.
//但都可以设置安全那一位.
void option_bytes_modify_Secure_area(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMES)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    while((FLASH->SR1&1)==1);//如果忙,在这里卡的,这里包括了普通闪存操作和选项字节修改操作
    if((FLASH->OPTSR_CUR&0x200000)==0)    //如果安全位没设置
    {
        /*这个选项好恐怖,会让programmer用stlink连不上*/
        FLASH->OPTSR_PRG|=1<<21;
        FLASH->OPTCR|=2;//开始修改
        while((FLASH->OPTSR_CUR&1)==1);//等待它忙
    }
    if(bank==1)
    {
        FLASH->SCAR_PRG1=0;//清原来值.
        FLASH->SCAR_PRG1|=start;
        FLASH->SCAR_PRG1|=end<<16;
        FLASH->SCAR_PRG1|=DMES<<31;
    }
    else if(bank==2)
    {
        FLASH->SCAR_PRG2=0;//清原来值.
        FLASH->SCAR_PRG2|=start;
        FLASH->SCAR_PRG2|=end<<16;
        FLASH->SCAR_PRG2|=DMES<<31;
    }
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙 
}

/*清除安全选项*/
void option_bytes_clear_Secure(void)
{
    if((FLASH->OPTCR&1)==1)//如果没解锁
    {
        option_bytes_unlock();//解锁
    }
    /*这个选项好恐怖,会让programmer用stlink连不上*/
    FLASH->OPTSR_PRG&=~(1<<21);
    FLASH->OPTCR|=2;//开始修改
    while((FLASH->OPTSR_CUR&1)==1);//等待它忙 
}



