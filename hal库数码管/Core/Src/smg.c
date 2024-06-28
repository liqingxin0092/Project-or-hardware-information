 #include "smg.h"
 void one()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_0();       
     UP_1();
     MID_1();
     DOWN_1();
     LEFT_UP_1();
     LEFT_DOWN_1(); 
 }
 void zero()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_0();       
     UP_0();
     MID_1();
     DOWN_0();
     LEFT_UP_0();
     LEFT_DOWN_0(); 
 } void two()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_1();       
     UP_0();
     MID_0();
     DOWN_0();
     LEFT_UP_1();
     LEFT_DOWN_0(); 
 } void three()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_0();       
     UP_0();
     MID_0();
     DOWN_0();
     LEFT_UP_1();
     LEFT_DOWN_1(); 
 } void four()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_0();       
     UP_1();
     MID_0();
     DOWN_1();
     LEFT_UP_0();
     LEFT_DOWN_1(); 
 } void five()
 {
     RIGHT_UP_1();
     RIGHT_DOWN_0();       
     UP_0();
     MID_0();
     DOWN_0();
     LEFT_UP_0();
     LEFT_DOWN_1(); 
 } void six()
 {
     RIGHT_UP_1();
     RIGHT_DOWN_0();       
     UP_0();
     MID_0();
     DOWN_0();
     LEFT_UP_0();
     LEFT_DOWN_0(); 
 } void seven()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_0();       
     UP_0();
     MID_1();
     DOWN_1();
     LEFT_UP_1();
     LEFT_DOWN_1(); 
 } void eight()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_0();       
     UP_0();
     MID_0();
     DOWN_0();
     LEFT_UP_0();
     LEFT_DOWN_0(); 
 } void nine()
 {
     RIGHT_UP_0();
     RIGHT_DOWN_0();       
     UP_0();
     MID_0();
     DOWN_0();
     LEFT_UP_0();
     LEFT_DOWN_1(); 
 }
 void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x00000005;
	while(!(SysTick->CTRL & 0x00010000));
	SysTick->CTRL = 0x00000004;
}
void Delay_ms(uint32_t xms)
{
    uint32_t i=xms;
    while(i--)
    {
        Delay_us(1000);
    }
}
void Flash_write(uint32_t add,uint16_t data  )
{       
    //解锁
    FLASH->KEYR = 0x45670123;  
    FLASH->KEYR = 0xcdef89ab;

    //页擦除
    FLASH->CR|= 1<<1;      //即将擦除 
    while (FLASH->SR & 1);   //等待忙
    
    FLASH->AR =add;   //选择页数
    FLASH->CR|=1<<6;    //使能
    while (FLASH->SR & 1);   //等待忙
    
    FLASH->CR&= ~(1<<1);      //结束擦除
    
   //页写入
   FLASH->CR|=1;   //即将要写入数据
   while (FLASH->SR & 1);//等待忙

   *(__IO uint16_t *)(add )= data;  //写入数据
    while (FLASH->SR & 1);    //等待忙
    
    FLASH->CR&=~(1<<0);  //结束写入
  
    //加锁
    FLASH->CR|=1<<7; 
} 


uint16_t  FlashRead(uint32_t add)
{
    return   *(__IO uint16_t *)add;
}
