#include "lcd.h"
#include "delay.h"
#include "lcd_fonts.h"
#include "stdio.h"

/*外部不可见宏*/
/*数据位数配置宏,需要在spi disable时候用*/
//初始化配置默认8bit
//#define __SPI_8BIT()  ACTIVE_SPI->CFG1&=~8
//#define __SPI_16BIT() ACTIVE_SPI->CFG1|=15

/*上述内联版本*/
__attribute__((always_inline)) static void __SPI_8BIT(void) 
{
    ACTIVE_SPI->CFG1&=~8;
}
__attribute__((always_inline)) static void __SPI_16BIT(void) 
{
    ACTIVE_SPI->CFG1|=15;
}

/*显示填充参数*/
#define  Fill_Zero  0		//填充0
#define  Fill_Space 1		//填充空格

lcd_t LCD;

static pFONT *LCD_AsciiFonts;		// 英文字体，ASCII字符集
uint16_t  LCD_Buff[200];//ascii显示缓冲,

/*lcd引脚组定义*/
lcd_spi_io_t lcd_io = {
    {GPIOE, GPIO_PIN_12}, // clk
    {GPIOE, GPIO_PIN_14}, // mosi
    {GPIOD, GPIO_PIN_15}, // blk
    {GPIOE, GPIO_PIN_15}, // rs
    {GPIOE, GPIO_PIN_11}, // cs
};

/*lcd io接口初始化*/
 void lcd_io_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*clk*/
    GPIO_InitStruct.Pin = lcd_io.sclk_io.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(lcd_io.sclk_io.GPIOx, &GPIO_InitStruct);
    /*mosi*/
    GPIO_InitStruct.Pin = lcd_io.mosi_io.GPIO_Pin;
    HAL_GPIO_Init(lcd_io.mosi_io.GPIOx, &GPIO_InitStruct);
    /*blk*/
    GPIO_InitStruct.Pin = lcd_io.blk_io.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(lcd_io.blk_io.GPIOx, &GPIO_InitStruct);
    /*rs*/
    GPIO_InitStruct.Pin = lcd_io.rs_io.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(lcd_io.rs_io.GPIOx, &GPIO_InitStruct);
    /*cs*/
    GPIO_InitStruct.Pin = lcd_io.cs_io.GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(lcd_io.cs_io.GPIOx, &GPIO_InitStruct);
}

// 初始化SPI
void lcd_spi_init(void)
{
    u32 cfg1_temp,cfg2_temp;
    /*RCC */
    RCC->APB2ENR |= 1 << 13; // 开启SPI4 bus_interface时钟s
    RCC->APB2RSTR|=1<<13;		//复位SPI4
    delay_us(2);
	RCC->APB2RSTR&=~(1<<13);	//停止复位SPI4
    
    /*********************** CFG1 *******************************/
    // 30:28,    011b,     MBR=0,波特率为APB2/2,即120MHz/2=60MHz .011b时可抓波形,/16=7.5MHz
    // 22,          0,     no CRC
    // 20:16,       0,     CRC lenth ignored
    // 15,          0,     no tx DMA
    // 14,          0,     no rx DMA
    // 12:11,       0,     从机时欠载检测
    // 10:9,        0,     从机时欠载行为
    // 8:5,       11b,     4 data, The size of the packet must not exceed 1/2 of FIFO space,FIFO space:8 byte,* 1/2 = 4
    //                     byte, 如果32位传输刚好就4byte.例子给的意思是,怎么能正好填满数据寄存器怎么来.
    //                     eg: 8bit * 4 ,16bit * 2, 32bit * 1 , 都是4byte.这里默认8bit *1.
    // 4:0          7,     8 bit
    cfg1_temp = 0x30000067;

    /*********************** CFG2 *******************************/
    // 31,          1,    the peripheral keeps always control of all associated GPIOs.
    // 30,          0,    SSOM, 不启用数据间延时
    // 29,          1,    SSOE, SS输出使能
    // 28,          0,    SS低时活跃
    // 26,          0,    SSM,硬件SS.
    // 25,          0,    CLK空闲低电平
    // 24,          0,    第一个边沿有效
    // 23,          0,    MSB
    // 22,          1,    SPI主模式
    // 21:19,       0,    SPI Motorola,即可以自定义的SPI模式
    // 18:17,       1,    仅传输
    // 15,          0,    no swap io function
    // 7:4,         0,    MIDI,数据间无延时
    // 3:0,         0,    第一个延时
    cfg2_temp = 0xA0420000;
    
    while((ACTIVE_SPI->CFG2!=(vu32)cfg2_temp)||(ACTIVE_SPI->CFG1!=(vu32)cfg1_temp))
    {                   //优化等级高,某些寄存器位可能设置不上.
//        RCC->APB2RSTR|=1<<13;		//复位SPI4
//        delay_us(2);
//        RCC->APB2RSTR&=~(1<<13);	//停止复位SPI4
        ACTIVE_SPI->CFG1 = cfg1_temp;
        ACTIVE_SPI->CFG2 = cfg2_temp;
    }
}

/*发送一个byte命令*/
static void lcd_transmit_command( vu8 command)
{
    LCD_DC_Command();//传输命令
    
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = command;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

/*发送一个byte数据*/
static void lcd_transmit_data( uint8_t data)
{
    LCD_DC_Data();//传输数据
    
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = data;
    while(!(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

/*标准配置*/
//fifo level = 4data 
//这里只针对8bit
//默认是8bit,如果默认位数被修改,使用前 加上 __SPI_8BIT();
static void my_spi_transmit_8bit(volatile uint8_t * src,uint16_t size)
{
    u16 fercount=size;
    ACTIVE_SPI->CR2=size;//配置传输长度,这里直接=size,因为其他位就是0.
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP标志位,如果有空闲位置才进行
        {
            if(fercount>3)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) = *((uint32_t *)src);
              src += 4;
              fercount -= 4;                
            }
            else if (fercount>1)
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR) = *((uint16_t *)src);
              src+= 2;
              fercount -= 2;
            }
            else//只剩一个数据
            {
                *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = *((uint8_t *)src);
                src+= 1;
                fercount--;                
            }
        }
    }
    while(!(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

/*标准配置*/
//fifo level = 4data 
//这里只针对16bit
//使用前 加上 __SPI_16BIT();
static void my_spi_transmit_16bit( uint8_t * src,uint16_t size)
{
    u16 fercount=size;
    ACTIVE_SPI->CR2=size;//配置传输长度,这里直接=size,因为其他位就是0.
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP标志位,如果有空闲位置才进行
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) = *((uint32_t *)src);
              src += 4;
              fercount -= 2;               
            }
            else//只剩一个数据
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = *((uint16_t *)src);
              src += 2;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

/*设置显示方向*/
static void LCD_SetDirection(uint8_t direction)
{
    __SPI_8BIT();//以下均为8bit传输
    LCD.Direction = direction; // 写入全局LCD参数
    if (direction == Direction_H) // 横屏显示
    {
        lcd_transmit_command(0x36);   // 显存访问控制 指令，用于设置访问显存的方式
        lcd_transmit_data(0x70); // 横屏显示
        LCD.X_Offset = 0;         // 设置控制器坐标偏移量
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Height; // 重新赋值长、宽
        LCD.Height = LCD_Width;
    }
    else if (direction == Direction_V)
    {
        lcd_transmit_command(0x36);   // 显存访问控制 指令，用于设置访问显存的方式
        lcd_transmit_data(0x00); // 垂直显示
        LCD.X_Offset = 0;         // 设置控制器坐标偏移量
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Width; // 重新赋值长、宽
        LCD.Height = LCD_Height;
    }
    else if (direction == Direction_H_Flip)
    {
        lcd_transmit_command(0x36);   // 显存访问控制 指令，用于设置访问显存的方式
        lcd_transmit_data(0xA0); // 横屏显示，并上下翻转，RGB像素格式
        LCD.X_Offset = 80;        // 设置控制器坐标偏移量
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Height; // 重新赋值长、宽
        LCD.Height = LCD_Width;
    }
    else if (direction == Direction_V_Flip)
    {
        lcd_transmit_command(0x36);   // 显存访问控制 指令，用于设置访问显存的方式
        lcd_transmit_data(0xC0); // 垂直显示 ，并上下翻转，RGB像素格式
        LCD.X_Offset = 0;         // 设置控制器坐标偏移量
        LCD.Y_Offset = 80;
        LCD.Width = LCD_Width; // 重新赋值长、宽
        LCD.Height = LCD_Height;
    }
}

/*设置要显示的颜色*/
//格式RGB888,自动转换为RGB565
static void LCD_SetColor(uint32_t Color)
{
    uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; // 各个颜色通道的值

    Red_Value = (uint16_t)((Color & 0x00F80000) >> 8); // 转换成 16位 的RGB565颜色
    Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
    Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);

    LCD.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value); // 将颜色写入全局LCD参数
}

/*设置要显示的背景颜色*/
//格式RGB888,自动转换为RGB565
static void LCD_SetBackColor(uint32_t Color)
{
    uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; // 各个颜色通道的值

    Red_Value = (uint16_t)((Color & 0x00F80000) >> 8); // 转换成 16位 的RGB565颜色
    Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
    Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);

    LCD.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value); // 将颜色写入全局LCD参数
}

/*设置要显示的坐标区域*/
static void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)		
{
    uint16_t temp[2];
    __SPI_8BIT();
	lcd_transmit_command(0x2a);			//	列地址设置，即X坐标
    
    __SPI_16BIT();
    LCD_DC_Data();
    temp[0]=x1+LCD.X_Offset;
    temp[1]=x2+LCD.X_Offset;
    my_spi_transmit_16bit((uint8_t *)temp,2);

    __SPI_8BIT();
	lcd_transmit_command(0x2b);			//	行地址设置，即Y坐标
    
    __SPI_16BIT();
    LCD_DC_Data();
    temp[0]=y1+LCD.Y_Offset;
    temp[1]=y2+LCD.Y_Offset;
    my_spi_transmit_16bit((uint8_t *)temp,2);
    
    __SPI_8BIT();
	lcd_transmit_command(0x2c);			//	开始写入显存，即要显示的颜色数据
}

/*画点函数*/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{
    __SPI_8BIT();
    LCD_DC_Command();//传输命令 
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = 0x2a;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
    
    __SPI_16BIT();
    LCD_DC_Data();//传输数据    
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint16_t *)&ACTIVE_SPI->TXDR) = x+LCD.X_Offset;
    while(!(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位

    __SPI_8BIT();
    LCD_DC_Command();//传输命令
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = 0x2b;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
    
    __SPI_16BIT();
    LCD_DC_Data();
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint16_t *)&ACTIVE_SPI->TXDR) = y+LCD.Y_Offset;
    while(!(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
    
    __SPI_8BIT();
    LCD_DC_Command();//传输命令
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = 0x2c;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
    
    __SPI_16BIT();
    LCD_DC_Data();
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(!(ACTIVE_SPI->SR&2));//TXP标志位,如果有空闲位置才进行
    *((__IO uint16_t *)&ACTIVE_SPI->TXDR) = color;
    while(!(ACTIVE_SPI->SR&8));//EOT,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

/*清屏*/
void LCD_Clear(void)
{
   LCD_SetAddress(0,0,LCD.Width-1,LCD.Height-1);	// 设置坐标 以__SPI_8BIT()结束.
	
	LCD_DC_Data();     // 数据指令选择 引脚输出高电平，代表本次传输 数据	

   __SPI_16BIT();
    u16 fercount=(LCD.Width) * (LCD.Height);
    ACTIVE_SPI->CR2=0;//初始化无限事务
    ACTIVE_SPI->CR1=1;//enable spi,
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP标志位,如果有空闲位置才进行
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((LCD.BackColor<<16)|LCD.BackColor);
              fercount -= 2;               
            }
            else//只剩一个数据
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)LCD.BackColor;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

void LCD_Init(void)
{
    __IO uint8_t temp[14];
    lcd_io_init();  // io初始化
    lcd_spi_init(); // spi初始化
    
    delay_ms(10);
    lcd_transmit_command(0x36); // 显存访问控制 指令，用于设置访问显存的方式
    lcd_transmit_data(0x00);    // 配置成 从上到下、从左到右，RGB像素格式

    lcd_transmit_command(0x3A); // 接口像素格式 指令，用于设置使用 12位、16位还是18位色
    lcd_transmit_data(0x05);    // 此处配置成 16位 像素格式

    // 接下来很多都是电压设置指令，直接使用厂家给设定值
    lcd_transmit_command(0xB2);
    temp[0]=0x0C;
    temp[1]=0x0C;
    temp[2]=0x00;
    temp[3]=0x33;
    temp[4]=0x33;
    my_spi_transmit_8bit( temp,5);

    lcd_transmit_command(0xB7); // 栅极电压设置指令
    lcd_transmit_data(0x35);    // VGH = 13.26V，VGL = -10.43V

    lcd_transmit_command(0xBB); // 公共电压设置指令
    lcd_transmit_data(0x19);    // VCOM = 1.35V

    lcd_transmit_command(0xC0);
    lcd_transmit_data(0x2C);

    lcd_transmit_command(0xC2); // VDV 和 VRH 来源设置
    lcd_transmit_data(0x01);    // VDV 和 VRH 由用户自由配置

    lcd_transmit_command(0xC3); // VRH电压 设置指令
    lcd_transmit_data(0x12);    // VRH电压 = 4.6+( vcom+vcom offset+vdv)

    lcd_transmit_command(0xC4); // VDV电压 设置指令
    lcd_transmit_data(0x20);    // VDV电压 = 0v

    lcd_transmit_command(0xC6); // 正常模式的帧率控制指令
    lcd_transmit_data(0x0F);    // 设置屏幕控制器的刷新帧率为60帧

    lcd_transmit_command(0xD0); // 电源控制指令
    temp[0]=0xA4;
    temp[1]=0xA1;
    my_spi_transmit_8bit( temp,2);
    
    lcd_transmit_command(0xE0); // 正极电压伽马值设定
    temp[0]=0xD0;
    temp[1]=0x04;
    temp[2]=0x0D;
    temp[3]=0x11;
    temp[4]=0x13;
    temp[5]=0x2B;
    temp[6]=0x3F;
    temp[7]=0x54;
    temp[8]=0x4C;
    temp[9]=0x18;
    temp[10]=0x0D;
    temp[11]=0x0B;
    temp[12]=0x1F;
    temp[13]=0x23;
    my_spi_transmit_8bit( temp,14);
    
    lcd_transmit_command(0xE1); // 负极电压伽马值设定
    temp[0]=0xD0;
    temp[1]=0x04;
    temp[2]=0x0C;
    temp[3]=0x11;
    temp[4]=0x13;
    temp[5]=0x2C;
    temp[6]=0x3F;
    temp[7]=0x44;
    temp[8]=0x51;
    temp[9]=0x2F;
    temp[10]=0x1F;
    temp[11]=0x1F;
    temp[12]=0x20;
    temp[13]=0x23;
    my_spi_transmit_8bit( temp,14);
    
    lcd_transmit_command(0x21); // 打开反显，因为面板是常黑型，操作需要反过来

    // 退出休眠指令，LCD控制器在刚上电、复位时，会自动进入休眠模式 ，因此操作屏幕之前，需要退出休眠
    lcd_transmit_command(0x11); // 退出休眠 指令
    delay_ms(120);              // 需要等待120ms，让电源电压和时钟电路稳定下来

    // 打开显示指令，LCD控制器在刚上电、复位时，会自动关闭显示
    lcd_transmit_command(0x29); // 打开显示

    // 以下进行一些驱动的默认设置
    LCD_SetDirection(Direction_V);   //设置显示方向
    LCD_SetBackColor(LCD_BLACK);   // 设置背景色
    LCD_SetColor(LCD_BLACK);       // 设置画笔色
    LCD_Clear();                   // 清屏

     LCD_AsciiFonts=&ASCII_Font20; // 设置默认字体
     LCD.ShowNum_Mode = Fill_Space;      // 设置变量显示模式，多余位填充空格还是填充0

    // 全部设置完毕之后，打开背光
    LCD_BLK_ON(); // 引脚输出高电平点亮背光
}

/*填充矩形*/
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
   __SPI_8BIT();
   LCD_SetAddress( x, y, x+width-1, y+height-1);	// 设置坐标	
	
	LCD_DC_Data();     // 数据指令选择 引脚输出高电平，代表本次传输 数据	

    __SPI_16BIT(); 
    u16 fercount=width*height;
    ACTIVE_SPI->CR2=0;//初始化无限事务
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP标志位,如果有空闲位置才进行
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((LCD.Color<<16)|LCD.Color);
              fercount -= 2;               
            }
            else//只剩一个数据
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)LCD.Color;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

/*画线函数,提供给PICTURE的接口*/
void piclib_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
   __SPI_8BIT();
   LCD_SetAddress( x0, y0, x0+len-1,y0);	// 设置坐标	
	
	LCD_DC_Data();     // 数据指令选择 引脚输出高电平，代表本次传输 数据	

    __SPI_16BIT(); 
    u16 fercount=len;
    ACTIVE_SPI->CR2=0;//初始化无限事务
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP标志位,如果有空闲位置才进行
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((LCD.Color<<16)|LCD.Color);
              fercount -= 2;               
            }
            else//只剩一个数据
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)LCD.Color;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}

/*填充矩形函数,提供给PICTURE的接口*/
void picture_fill_rect(u16 sx, u16 sy, u16 ex, u16 ey, u32 color)
{
   __SPI_8BIT();
   LCD_SetAddress( sx, sy, ex, ey);	// 设置坐标	
	
	LCD_DC_Data();     // 数据指令选择 引脚输出高电平，代表本次传输 数据	

    __SPI_16BIT(); 
    u32 fercount=(ex-sx+1)*(ey-sy+1);
    ACTIVE_SPI->CR2=0;//初始化无限事务
    ACTIVE_SPI->CR1=1;//enable spi,等于1,原因同上
    ACTIVE_SPI->CR1|=0x200;//设置cstart位. 这两位不支持并行设置.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP标志位,如果有空闲位置才进行
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((color<<16)|color);
              fercount -= 2;               
            }
            else//只剩一个数据
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)color;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,等待直到传输完成
    ACTIVE_SPI->IFCR=0x18;//清EOT标志 与 TXTF标志
    ACTIVE_SPI->CR1&=~1;//清spi使能位
}


static void  LCD_WriteBuff(uint16_t *DataBuff, uint16_t DataSize)
{
	LCD_DC_Data();     // 数据指令选择 引脚输出高电平，代表本次传输 数据	

    __SPI_16BIT();
	
	my_spi_transmit_16bit((uint8_t *)DataBuff, DataSize); // 启动SPI传输
	
    __SPI_8BIT();
}

/*显示单个字体*/
static void LCD_DisplayChar(uint16_t x, uint16_t y,uint8_t c)
{
	uint16_t  index = 0, counter = 0 ,i = 0, w = 0;		// 计数变量
   uint8_t   disChar;		//存储字符的地址

	c = c - 32; 	// 计算ASCII字符的偏移

	for(index = 0; index < LCD_AsciiFonts->Sizes; index++)	
	{
		disChar = LCD_AsciiFonts->pTable[c*LCD_AsciiFonts->Sizes + index]; //获取字符的模值
		for(counter = 0; counter < 8; counter++)
		{ 
			if(disChar & 0x01)	
			{		
            LCD_Buff[i] =  LCD.Color;			// 当前模值不为0时，使用画笔色绘点
			}
			else		
			{		
            LCD_Buff[i] = LCD.BackColor;		//否则使用背景色绘制点
			}
                disChar >>= 1;
                i++;
                w++;
 			if( w == LCD_AsciiFonts->Width ) // 如果写入的数据达到了字符宽度，则退出当前循环
            {                // 进入下一字符的写入的绘制
                w = 0;
                break;
			}        
		}	
	}		
   LCD_SetAddress( x, y, x+LCD_AsciiFonts->Width-1, y+LCD_AsciiFonts->Height-1);	   // 设置坐标	
   LCD_WriteBuff(LCD_Buff,LCD_AsciiFonts->Width*LCD_AsciiFonts->Height);          // 写入显存
}

/*显示字符串*/
void LCD_DisplayString( uint16_t x, uint16_t y, char *p) 
{  
	while ((x < LCD.Width) && (*p != 0))	//判断显示坐标是否超出显示区域并且字符是否为空字符
	{
		 LCD_DisplayChar( x,y,*p);
		 x += LCD_AsciiFonts->Width; //显示下一个字符
		 p++;	//取下一个字符地址
	}
}

void  LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number, uint8_t len) 
{  
	char Number_Buffer[15];				// 用于存储转换后的字符串
	if( LCD.ShowNum_Mode == Fill_Zero)	// 多余位补0
	{
		sprintf( Number_Buffer , "%0.*d",len, number );	// 将 number 转换成字符串，便于显示		
	}
	else			// 多余位补空格
	{	
		sprintf( Number_Buffer , "%*d",len, number );	// 将 number 转换成字符串，便于显示		
	}
	LCD_DisplayString( x, y,(char *)Number_Buffer) ;  // 将转换得到的字符串显示出来
	
}

void LCD_Fill_Buffer_Colors(u16 x, u16 y, u16 width, u16 height,u16 *src)
{
   __SPI_8BIT();
   LCD_SetAddress( x, y, x+width-1, y+height-1);	// 设置坐标	
	
	LCD_DC_Data();     // 数据指令选择 引脚输出高电平，代表本次传输 数据	

    __SPI_16BIT(); 
	my_spi_transmit_16bit((uint8_t *)src, width*height); // 启动SPI传输
}
