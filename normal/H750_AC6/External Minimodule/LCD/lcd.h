#ifndef __LCD_H
#define __LCD_H

#include "sys.h"

/*ע��:�������ļ�����ʼ��GPIOʱ��*/
/*ע��:io���øĸ��ù�ϵ*/

#define LCD_Width 240  // LCD�����س���
#define LCD_Height 240 // LCD�����ؿ��

// ��ʾ�������
// ʹ��ʾ����LCD_DisplayDirection(Direction_H) ������Ļ������ʾ
#define Direction_H 0      // LCD������ʾ
#define Direction_H_Flip 1 // LCD������ʾ,���·�ת
#define Direction_V 2      // LCD������ʾ
#define Direction_V_Flip 3 // LCD������ʾ,���·�ת

/*�õ��ĸ�SPI */
#define ACTIVE_SPI SPI4

/*������ɫ,��ʽRGB888 */
#define LCD_WHITE 0xFFFFFF // ����ɫ
#define LCD_BLACK 0x000000 // ����ɫ

#define LCD_BLUE 0x0000FF    //	����ɫ
#define LCD_GREEN 0x00FF00   //	����ɫ
#define LCD_RED 0xFF0000     //	����ɫ
#define LCD_CYAN 0x00FFFF    //	����ɫ
#define LCD_MAGENTA 0xFF00FF //	�Ϻ�ɫ
#define LCD_YELLOW 0xFFFF00  //	��ɫ
#define LCD_GREY 0x2C2C2C    //	��ɫ

#define LIGHT_BLUE 0x8080FF    //	����ɫ
#define LIGHT_GREEN 0x80FF80   //	����ɫ
#define LIGHT_RED 0xFF8080     //	����ɫ
#define LIGHT_CYAN 0x80FFFF    //	������ɫ
#define LIGHT_MAGENTA 0xFF80FF //	���Ϻ�ɫ
#define LIGHT_YELLOW 0xFFFF80  //	����ɫ
#define LIGHT_GREY 0xA3A3A3    //	����ɫ

#define DARK_BLUE 0x000080    //	����ɫ
#define DARK_GREEN 0x008000   //	����ɫ
#define DARK_RED 0x800000     //	����ɫ
#define DARK_CYAN 0x008080    //	������ɫ
#define DARK_MAGENTA 0x800080 //	���Ϻ�ɫ
#define DARK_YELLOW 0x808000  //	����ɫ
#define DARK_GREY 0x404040    //	����ɫ

typedef struct // LCD��ز����ṹ��
{
    uint32_t Color;       //	LCD��ǰ������ɫ
    uint32_t BackColor;   //	����ɫ
    uint8_t ShowNum_Mode; // ������ʾģʽ
    uint8_t Direction;    //	��ʾ����
    uint16_t Width;       // ��Ļ���س���
    uint16_t Height;      // ��Ļ���ؿ��
    uint8_t X_Offset;     // X����ƫ�ƣ�����������Ļ���������Դ�д�뷽ʽ
    uint8_t Y_Offset;     // Y����ƫ�ƣ�����������Ļ���������Դ�д�뷽ʽ
}lcd_t; 

extern lcd_t LCD;

#define LCD_DC_Command() GPIO_OUT(GPIO_E,15,0) // �͵�ƽ��ָ���
#define LCD_DC_Data() GPIO_OUT(GPIO_E,15,1)

#define LCD_BLK_ON() GPIO_OUT(GPIO_D,15,1)     // �ߵ�ƽ,��������
#define LCD_BLK_OFF() GPIO_OUT(GPIO_D,15,0)   // �͵�ƽ,�رձ���

void LCD_Init(void);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color);
void LCD_DisplayString( uint16_t x, uint16_t y, char *p) ;
void LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number, uint8_t len);
void LCD_Fill_Buffer_Colors(u16 x, u16 y, u16 width, u16 height,u16 *src);

/*�ṩ��PICTURE�Ľӿ�*/
void piclib_draw_hline(u16 x0,u16 y0,u16 len,u16 color);
void picture_fill_rect(u16 sx, u16 sy, u16 ex, u16 ey, u32 color);
#endif
