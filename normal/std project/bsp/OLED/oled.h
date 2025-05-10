#ifndef __OLED_H
#define __OLED_H 

#include "gd32f4xx.h"
#include "stdlib.h"	

//-----------------OLED�˿���ֲ----------------
//GND - GND
//VCC - 3.3V
//SCL - PB10 I2C1_SCL
//SDA - PB10 I2C1_SDA
#define RCU_LCD_SCL     RCU_GPIOB//SCL
#define PORT_LCD_SCL    GPIOB
#define GPIO_LCD_SCL    GPIO_PIN_10

#define RCU_LCD_SDA     RCU_GPIOB//SDA
#define PORT_LCD_SDA    GPIOB
#define GPIO_LCD_SDA    GPIO_PIN_11

#define GPIO_AF_I2C    GPIO_AF_4
#define BSP_I2C        I2C1

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

//-----------------OLED�˿ڶ���---------------- 

#define OLED_SCL_Clr() gpio_bit_write(PORT_LCD_SCL, GPIO_LCD_SCL, RESET)//SCL
#define OLED_SCL_Set() gpio_bit_write(PORT_LCD_SCL, GPIO_LCD_SCL, SET)//SCL

#define OLED_SDA_Clr() gpio_bit_write(PORT_LCD_SDA, GPIO_LCD_SDA, RESET)//SDA
#define OLED_SDA_Set()gpio_bit_write(PORT_LCD_SDA, GPIO_LCD_SDA, SET)//SDA

//#define OLED_RES_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_4)//RES
//#define OLED_RES_Set() GPIO_SetBits(GPIOD,GPIO_Pin_4)


#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
//void I2C_Start(void);
//void I2C_Stop(void);
//void I2C_WaitAck(void);
//void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);   //8,12,16,24
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void oled_show_float(u8 x,u8 y,float num,u8 len,u8 size1,u8 mode);  //mine
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
void OLED_Init(void);

#endif

