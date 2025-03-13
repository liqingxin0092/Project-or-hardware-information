#include "lcd.h"
#include "delay.h"
#include "lcd_fonts.h"
#include "stdio.h"

/*�ⲿ���ɼ���*/
/*����λ�����ú�,��Ҫ��spi disableʱ����*/
//��ʼ������Ĭ��8bit
//#define __SPI_8BIT()  ACTIVE_SPI->CFG1&=~8
//#define __SPI_16BIT() ACTIVE_SPI->CFG1|=15

/*���������汾*/
__attribute__((always_inline)) static void __SPI_8BIT(void) 
{
    ACTIVE_SPI->CFG1&=~8;
}
__attribute__((always_inline)) static void __SPI_16BIT(void) 
{
    ACTIVE_SPI->CFG1|=15;
}

/*��ʾ������*/
#define  Fill_Zero  0		//���0
#define  Fill_Space 1		//���ո�

lcd_t LCD;

static pFONT *LCD_AsciiFonts;		// Ӣ�����壬ASCII�ַ���
uint16_t  LCD_Buff[200];//ascii��ʾ����,

/*lcd�����鶨��*/
lcd_spi_io_t lcd_io = {
    {GPIOE, GPIO_PIN_12}, // clk
    {GPIOE, GPIO_PIN_14}, // mosi
    {GPIOD, GPIO_PIN_15}, // blk
    {GPIOE, GPIO_PIN_15}, // rs
    {GPIOE, GPIO_PIN_11}, // cs
};

/*lcd io�ӿڳ�ʼ��*/
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

// ��ʼ��SPI
void lcd_spi_init(void)
{
    u32 cfg1_temp,cfg2_temp;
    /*RCC */
    RCC->APB2ENR |= 1 << 13; // ����SPI4 bus_interfaceʱ��s
    RCC->APB2RSTR|=1<<13;		//��λSPI4
    delay_us(2);
	RCC->APB2RSTR&=~(1<<13);	//ֹͣ��λSPI4
    
    /*********************** CFG1 *******************************/
    // 30:28,    011b,     MBR=0,������ΪAPB2/2,��120MHz/2=60MHz .011bʱ��ץ����,/16=7.5MHz
    // 22,          0,     no CRC
    // 20:16,       0,     CRC lenth ignored
    // 15,          0,     no tx DMA
    // 14,          0,     no rx DMA
    // 12:11,       0,     �ӻ�ʱǷ�ؼ��
    // 10:9,        0,     �ӻ�ʱǷ����Ϊ
    // 8:5,       11b,     4 data, The size of the packet must not exceed 1/2 of FIFO space,FIFO space:8 byte,* 1/2 = 4
    //                     byte, ���32λ����պþ�4byte.���Ӹ�����˼��,��ô�������������ݼĴ�����ô��.
    //                     eg: 8bit * 4 ,16bit * 2, 32bit * 1 , ����4byte.����Ĭ��8bit *1.
    // 4:0          7,     8 bit
    cfg1_temp = 0x30000067;

    /*********************** CFG2 *******************************/
    // 31,          1,    the peripheral keeps always control of all associated GPIOs.
    // 30,          0,    SSOM, ���������ݼ���ʱ
    // 29,          1,    SSOE, SS���ʹ��
    // 28,          0,    SS��ʱ��Ծ
    // 26,          0,    SSM,Ӳ��SS.
    // 25,          0,    CLK���е͵�ƽ
    // 24,          0,    ��һ��������Ч
    // 23,          0,    MSB
    // 22,          1,    SPI��ģʽ
    // 21:19,       0,    SPI Motorola,�������Զ����SPIģʽ
    // 18:17,       1,    ������
    // 15,          0,    no swap io function
    // 7:4,         0,    MIDI,���ݼ�����ʱ
    // 3:0,         0,    ��һ����ʱ
    cfg2_temp = 0xA0420000;
    
    while((ACTIVE_SPI->CFG2!=(vu32)cfg2_temp)||(ACTIVE_SPI->CFG1!=(vu32)cfg1_temp))
    {                   //�Ż��ȼ���,ĳЩ�Ĵ���λ�������ò���.
//        RCC->APB2RSTR|=1<<13;		//��λSPI4
//        delay_us(2);
//        RCC->APB2RSTR&=~(1<<13);	//ֹͣ��λSPI4
        ACTIVE_SPI->CFG1 = cfg1_temp;
        ACTIVE_SPI->CFG2 = cfg2_temp;
    }
}

/*����һ��byte����*/
static void lcd_transmit_command( vu8 command)
{
    LCD_DC_Command();//��������
    
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = command;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

/*����һ��byte����*/
static void lcd_transmit_data( uint8_t data)
{
    LCD_DC_Data();//��������
    
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = data;
    while(!(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

/*��׼����*/
//fifo level = 4data 
//����ֻ���8bit
//Ĭ����8bit,���Ĭ��λ�����޸�,ʹ��ǰ ���� __SPI_8BIT();
static void my_spi_transmit_8bit(volatile uint8_t * src,uint16_t size)
{
    u16 fercount=size;
    ACTIVE_SPI->CR2=size;//���ô��䳤��,����ֱ��=size,��Ϊ����λ����0.
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP��־λ,����п���λ�òŽ���
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
            else//ֻʣһ������
            {
                *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = *((uint8_t *)src);
                src+= 1;
                fercount--;                
            }
        }
    }
    while(!(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

/*��׼����*/
//fifo level = 4data 
//����ֻ���16bit
//ʹ��ǰ ���� __SPI_16BIT();
static void my_spi_transmit_16bit( uint8_t * src,uint16_t size)
{
    u16 fercount=size;
    ACTIVE_SPI->CR2=size;//���ô��䳤��,����ֱ��=size,��Ϊ����λ����0.
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP��־λ,����п���λ�òŽ���
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) = *((uint32_t *)src);
              src += 4;
              fercount -= 2;               
            }
            else//ֻʣһ������
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = *((uint16_t *)src);
              src += 2;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

/*������ʾ����*/
static void LCD_SetDirection(uint8_t direction)
{
    __SPI_8BIT();//���¾�Ϊ8bit����
    LCD.Direction = direction; // д��ȫ��LCD����
    if (direction == Direction_H) // ������ʾ
    {
        lcd_transmit_command(0x36);   // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
        lcd_transmit_data(0x70); // ������ʾ
        LCD.X_Offset = 0;         // ���ÿ���������ƫ����
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Height; // ���¸�ֵ������
        LCD.Height = LCD_Width;
    }
    else if (direction == Direction_V)
    {
        lcd_transmit_command(0x36);   // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
        lcd_transmit_data(0x00); // ��ֱ��ʾ
        LCD.X_Offset = 0;         // ���ÿ���������ƫ����
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Width; // ���¸�ֵ������
        LCD.Height = LCD_Height;
    }
    else if (direction == Direction_H_Flip)
    {
        lcd_transmit_command(0x36);   // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
        lcd_transmit_data(0xA0); // ������ʾ�������·�ת��RGB���ظ�ʽ
        LCD.X_Offset = 80;        // ���ÿ���������ƫ����
        LCD.Y_Offset = 0;
        LCD.Width = LCD_Height; // ���¸�ֵ������
        LCD.Height = LCD_Width;
    }
    else if (direction == Direction_V_Flip)
    {
        lcd_transmit_command(0x36);   // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
        lcd_transmit_data(0xC0); // ��ֱ��ʾ �������·�ת��RGB���ظ�ʽ
        LCD.X_Offset = 0;         // ���ÿ���������ƫ����
        LCD.Y_Offset = 80;
        LCD.Width = LCD_Width; // ���¸�ֵ������
        LCD.Height = LCD_Height;
    }
}

/*����Ҫ��ʾ����ɫ*/
//��ʽRGB888,�Զ�ת��ΪRGB565
static void LCD_SetColor(uint32_t Color)
{
    uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; // ������ɫͨ����ֵ

    Red_Value = (uint16_t)((Color & 0x00F80000) >> 8); // ת���� 16λ ��RGB565��ɫ
    Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
    Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);

    LCD.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value); // ����ɫд��ȫ��LCD����
}

/*����Ҫ��ʾ�ı�����ɫ*/
//��ʽRGB888,�Զ�ת��ΪRGB565
static void LCD_SetBackColor(uint32_t Color)
{
    uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; // ������ɫͨ����ֵ

    Red_Value = (uint16_t)((Color & 0x00F80000) >> 8); // ת���� 16λ ��RGB565��ɫ
    Green_Value = (uint16_t)((Color & 0x0000FC00) >> 5);
    Blue_Value = (uint16_t)((Color & 0x000000F8) >> 3);

    LCD.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value); // ����ɫд��ȫ��LCD����
}

/*����Ҫ��ʾ����������*/
static void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)		
{
    uint16_t temp[2];
    __SPI_8BIT();
	lcd_transmit_command(0x2a);			//	�е�ַ���ã���X����
    
    __SPI_16BIT();
    LCD_DC_Data();
    temp[0]=x1+LCD.X_Offset;
    temp[1]=x2+LCD.X_Offset;
    my_spi_transmit_16bit((uint8_t *)temp,2);

    __SPI_8BIT();
	lcd_transmit_command(0x2b);			//	�е�ַ���ã���Y����
    
    __SPI_16BIT();
    LCD_DC_Data();
    temp[0]=y1+LCD.Y_Offset;
    temp[1]=y2+LCD.Y_Offset;
    my_spi_transmit_16bit((uint8_t *)temp,2);
    
    __SPI_8BIT();
	lcd_transmit_command(0x2c);			//	��ʼд���Դ棬��Ҫ��ʾ����ɫ����
}

/*���㺯��*/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color)
{
    __SPI_8BIT();
    LCD_DC_Command();//�������� 
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = 0x2a;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
    
    __SPI_16BIT();
    LCD_DC_Data();//��������    
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint16_t *)&ACTIVE_SPI->TXDR) = x+LCD.X_Offset;
    while(!(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ

    __SPI_8BIT();
    LCD_DC_Command();//��������
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = 0x2b;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
    
    __SPI_16BIT();
    LCD_DC_Data();
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint16_t *)&ACTIVE_SPI->TXDR) = y+LCD.Y_Offset;
    while(!(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
    
    __SPI_8BIT();
    LCD_DC_Command();//��������
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint8_t *)&ACTIVE_SPI->TXDR) = 0x2c;
    while(!(vu32)(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
    
    __SPI_16BIT();
    LCD_DC_Data();
    ACTIVE_SPI->CR2=1;
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(!(ACTIVE_SPI->SR&2));//TXP��־λ,����п���λ�òŽ���
    *((__IO uint16_t *)&ACTIVE_SPI->TXDR) = color;
    while(!(ACTIVE_SPI->SR&8));//EOT,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

/*����*/
void LCD_Clear(void)
{
   LCD_SetAddress(0,0,LCD.Width-1,LCD.Height-1);	// �������� ��__SPI_8BIT()����.
	
	LCD_DC_Data();     // ����ָ��ѡ�� ��������ߵ�ƽ�������δ��� ����	

   __SPI_16BIT();
    u16 fercount=(LCD.Width) * (LCD.Height);
    ACTIVE_SPI->CR2=0;//��ʼ����������
    ACTIVE_SPI->CR1=1;//enable spi,
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP��־λ,����п���λ�òŽ���
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((LCD.BackColor<<16)|LCD.BackColor);
              fercount -= 2;               
            }
            else//ֻʣһ������
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)LCD.BackColor;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

void LCD_Init(void)
{
    __IO uint8_t temp[14];
    lcd_io_init();  // io��ʼ��
    lcd_spi_init(); // spi��ʼ��
    
    delay_ms(10);
    lcd_transmit_command(0x36); // �Դ���ʿ��� ָ��������÷����Դ�ķ�ʽ
    lcd_transmit_data(0x00);    // ���ó� ���ϵ��¡������ң�RGB���ظ�ʽ

    lcd_transmit_command(0x3A); // �ӿ����ظ�ʽ ָ���������ʹ�� 12λ��16λ����18λɫ
    lcd_transmit_data(0x05);    // �˴����ó� 16λ ���ظ�ʽ

    // �������ܶ඼�ǵ�ѹ����ָ�ֱ��ʹ�ó��Ҹ��趨ֵ
    lcd_transmit_command(0xB2);
    temp[0]=0x0C;
    temp[1]=0x0C;
    temp[2]=0x00;
    temp[3]=0x33;
    temp[4]=0x33;
    my_spi_transmit_8bit( temp,5);

    lcd_transmit_command(0xB7); // դ����ѹ����ָ��
    lcd_transmit_data(0x35);    // VGH = 13.26V��VGL = -10.43V

    lcd_transmit_command(0xBB); // ������ѹ����ָ��
    lcd_transmit_data(0x19);    // VCOM = 1.35V

    lcd_transmit_command(0xC0);
    lcd_transmit_data(0x2C);

    lcd_transmit_command(0xC2); // VDV �� VRH ��Դ����
    lcd_transmit_data(0x01);    // VDV �� VRH ���û���������

    lcd_transmit_command(0xC3); // VRH��ѹ ����ָ��
    lcd_transmit_data(0x12);    // VRH��ѹ = 4.6+( vcom+vcom offset+vdv)

    lcd_transmit_command(0xC4); // VDV��ѹ ����ָ��
    lcd_transmit_data(0x20);    // VDV��ѹ = 0v

    lcd_transmit_command(0xC6); // ����ģʽ��֡�ʿ���ָ��
    lcd_transmit_data(0x0F);    // ������Ļ��������ˢ��֡��Ϊ60֡

    lcd_transmit_command(0xD0); // ��Դ����ָ��
    temp[0]=0xA4;
    temp[1]=0xA1;
    my_spi_transmit_8bit( temp,2);
    
    lcd_transmit_command(0xE0); // ������ѹ٤��ֵ�趨
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
    
    lcd_transmit_command(0xE1); // ������ѹ٤��ֵ�趨
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
    
    lcd_transmit_command(0x21); // �򿪷��ԣ���Ϊ����ǳ����ͣ�������Ҫ������

    // �˳�����ָ�LCD�������ڸ��ϵ硢��λʱ�����Զ���������ģʽ ����˲�����Ļ֮ǰ����Ҫ�˳�����
    lcd_transmit_command(0x11); // �˳����� ָ��
    delay_ms(120);              // ��Ҫ�ȴ�120ms���õ�Դ��ѹ��ʱ�ӵ�·�ȶ�����

    // ����ʾָ�LCD�������ڸ��ϵ硢��λʱ�����Զ��ر���ʾ
    lcd_transmit_command(0x29); // ����ʾ

    // ���½���һЩ������Ĭ������
    LCD_SetDirection(Direction_V);   //������ʾ����
    LCD_SetBackColor(LCD_BLACK);   // ���ñ���ɫ
    LCD_SetColor(LCD_BLACK);       // ���û���ɫ
    LCD_Clear();                   // ����

     LCD_AsciiFonts=&ASCII_Font20; // ����Ĭ������
     LCD.ShowNum_Mode = Fill_Space;      // ���ñ�����ʾģʽ������λ���ո������0

    // ȫ���������֮�󣬴򿪱���
    LCD_BLK_ON(); // ��������ߵ�ƽ��������
}

/*������*/
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
   __SPI_8BIT();
   LCD_SetAddress( x, y, x+width-1, y+height-1);	// ��������	
	
	LCD_DC_Data();     // ����ָ��ѡ�� ��������ߵ�ƽ�������δ��� ����	

    __SPI_16BIT(); 
    u16 fercount=width*height;
    ACTIVE_SPI->CR2=0;//��ʼ����������
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP��־λ,����п���λ�òŽ���
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((LCD.Color<<16)|LCD.Color);
              fercount -= 2;               
            }
            else//ֻʣһ������
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)LCD.Color;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

/*���ߺ���,�ṩ��PICTURE�Ľӿ�*/
void piclib_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
   __SPI_8BIT();
   LCD_SetAddress( x0, y0, x0+len-1,y0);	// ��������	
	
	LCD_DC_Data();     // ����ָ��ѡ�� ��������ߵ�ƽ�������δ��� ����	

    __SPI_16BIT(); 
    u16 fercount=len;
    ACTIVE_SPI->CR2=0;//��ʼ����������
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP��־λ,����п���λ�òŽ���
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((LCD.Color<<16)|LCD.Color);
              fercount -= 2;               
            }
            else//ֻʣһ������
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)LCD.Color;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}

/*�����κ���,�ṩ��PICTURE�Ľӿ�*/
void picture_fill_rect(u16 sx, u16 sy, u16 ex, u16 ey, u32 color)
{
   __SPI_8BIT();
   LCD_SetAddress( sx, sy, ex, ey);	// ��������	
	
	LCD_DC_Data();     // ����ָ��ѡ�� ��������ߵ�ƽ�������δ��� ����	

    __SPI_16BIT(); 
    u32 fercount=(ex-sx+1)*(ey-sy+1);
    ACTIVE_SPI->CR2=0;//��ʼ����������
    ACTIVE_SPI->CR1=1;//enable spi,����1,ԭ��ͬ��
    ACTIVE_SPI->CR1|=0x200;//����cstartλ. ����λ��֧�ֲ�������.
    while(fercount>0)
    {
        if(ACTIVE_SPI->SR&2)//TXP��־λ,����п���λ�òŽ���
        {
            if(fercount>1)
            {
              *((__IO uint32_t *)&ACTIVE_SPI->TXDR) =  (uint32_t)((color<<16)|color);
              fercount -= 2;               
            }
            else//ֻʣһ������
            {
              *((__IO uint16_t *)&ACTIVE_SPI->TXDR)  = (uint16_t)color;
              fercount--;              
            }
        }
    }
    while(!(ACTIVE_SPI->SR&2));//txc,�ȴ�ֱ���������
    ACTIVE_SPI->IFCR=0x18;//��EOT��־ �� TXTF��־
    ACTIVE_SPI->CR1&=~1;//��spiʹ��λ
}


static void  LCD_WriteBuff(uint16_t *DataBuff, uint16_t DataSize)
{
	LCD_DC_Data();     // ����ָ��ѡ�� ��������ߵ�ƽ�������δ��� ����	

    __SPI_16BIT();
	
	my_spi_transmit_16bit((uint8_t *)DataBuff, DataSize); // ����SPI����
	
    __SPI_8BIT();
}

/*��ʾ��������*/
static void LCD_DisplayChar(uint16_t x, uint16_t y,uint8_t c)
{
	uint16_t  index = 0, counter = 0 ,i = 0, w = 0;		// ��������
   uint8_t   disChar;		//�洢�ַ��ĵ�ַ

	c = c - 32; 	// ����ASCII�ַ���ƫ��

	for(index = 0; index < LCD_AsciiFonts->Sizes; index++)	
	{
		disChar = LCD_AsciiFonts->pTable[c*LCD_AsciiFonts->Sizes + index]; //��ȡ�ַ���ģֵ
		for(counter = 0; counter < 8; counter++)
		{ 
			if(disChar & 0x01)	
			{		
            LCD_Buff[i] =  LCD.Color;			// ��ǰģֵ��Ϊ0ʱ��ʹ�û���ɫ���
			}
			else		
			{		
            LCD_Buff[i] = LCD.BackColor;		//����ʹ�ñ���ɫ���Ƶ�
			}
                disChar >>= 1;
                i++;
                w++;
 			if( w == LCD_AsciiFonts->Width ) // ���д������ݴﵽ���ַ���ȣ����˳���ǰѭ��
            {                // ������һ�ַ���д��Ļ���
                w = 0;
                break;
			}        
		}	
	}		
   LCD_SetAddress( x, y, x+LCD_AsciiFonts->Width-1, y+LCD_AsciiFonts->Height-1);	   // ��������	
   LCD_WriteBuff(LCD_Buff,LCD_AsciiFonts->Width*LCD_AsciiFonts->Height);          // д���Դ�
}

/*��ʾ�ַ���*/
void LCD_DisplayString( uint16_t x, uint16_t y, char *p) 
{  
	while ((x < LCD.Width) && (*p != 0))	//�ж���ʾ�����Ƿ񳬳���ʾ�������ַ��Ƿ�Ϊ���ַ�
	{
		 LCD_DisplayChar( x,y,*p);
		 x += LCD_AsciiFonts->Width; //��ʾ��һ���ַ�
		 p++;	//ȡ��һ���ַ���ַ
	}
}

void  LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number, uint8_t len) 
{  
	char Number_Buffer[15];				// ���ڴ洢ת������ַ���
	if( LCD.ShowNum_Mode == Fill_Zero)	// ����λ��0
	{
		sprintf( Number_Buffer , "%0.*d",len, number );	// �� number ת�����ַ�����������ʾ		
	}
	else			// ����λ���ո�
	{	
		sprintf( Number_Buffer , "%*d",len, number );	// �� number ת�����ַ�����������ʾ		
	}
	LCD_DisplayString( x, y,(char *)Number_Buffer) ;  // ��ת���õ����ַ�����ʾ����
	
}

void LCD_Fill_Buffer_Colors(u16 x, u16 y, u16 width, u16 height,u16 *src)
{
   __SPI_8BIT();
   LCD_SetAddress( x, y, x+width-1, y+height-1);	// ��������	
	
	LCD_DC_Data();     // ����ָ��ѡ�� ��������ߵ�ƽ�������δ��� ����	

    __SPI_16BIT(); 
	my_spi_transmit_16bit((uint8_t *)src, width*height); // ����SPI����
}
