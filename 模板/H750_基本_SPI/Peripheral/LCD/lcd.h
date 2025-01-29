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

/*IO��ʼ���ṹ��*/
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint32_t GPIO_Pin;
} gpio_t;

/*spi���Žṹ��*/
typedef struct
{
    gpio_t sclk_io; // ʱ����
    gpio_t mosi_io; // ������
    gpio_t blk_io;  // ����
    gpio_t rs_io;   // ָ��or������
    gpio_t cs_io;   // Ƭѡ
} lcd_spi_io_t;

extern lcd_spi_io_t lcd_io;

#define LCD_DC_Command() lcd_io.rs_io.GPIOx->BSRR = 0x80000000 // �͵�ƽ��ָ���
#define LCD_DC_Data() lcd_io.rs_io.GPIOx->BSRR = 0x8000       // �ߵ�ƽ,���ݴ���

#define LCD_BLK_ON() lcd_io.blk_io.GPIOx->BSRR = 0x8000     // �ߵ�ƽ,��������
#define LCD_BLK_OFF() lcd_io.blk_io.GPIOx->BSRR = 0x80000000 // �͵�ƽ,�رձ���

void LCD_Init(void);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void LCD_DisplayString( uint16_t x, uint16_t y, char *p) ;
void  LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number, uint8_t len);

#endif
