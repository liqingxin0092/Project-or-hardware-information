#ifndef __LCD_H
#define __LCD_H

#include "sys.h"

/*注意:本驱动文件不初始化GPIO时钟*/
/*注意:io还得改复用关系*/

#define LCD_Width 240  // LCD的像素长度
#define LCD_Height 240 // LCD的像素宽度

// 显示方向参数
// 使用示例：LCD_DisplayDirection(Direction_H) 设置屏幕横屏显示
#define Direction_H 0      // LCD横屏显示
#define Direction_H_Flip 1 // LCD横屏显示,上下翻转
#define Direction_V 2      // LCD竖屏显示
#define Direction_V_Flip 3 // LCD竖屏显示,上下翻转

/*用的哪个SPI */
#define ACTIVE_SPI SPI4

/*常用颜色,格式RGB888 */
#define LCD_WHITE 0xFFFFFF // 纯白色
#define LCD_BLACK 0x000000 // 纯黑色

#define LCD_BLUE 0x0000FF    //	纯蓝色
#define LCD_GREEN 0x00FF00   //	纯绿色
#define LCD_RED 0xFF0000     //	纯红色
#define LCD_CYAN 0x00FFFF    //	蓝绿色
#define LCD_MAGENTA 0xFF00FF //	紫红色
#define LCD_YELLOW 0xFFFF00  //	黄色
#define LCD_GREY 0x2C2C2C    //	灰色

#define LIGHT_BLUE 0x8080FF    //	亮蓝色
#define LIGHT_GREEN 0x80FF80   //	亮绿色
#define LIGHT_RED 0xFF8080     //	亮红色
#define LIGHT_CYAN 0x80FFFF    //	亮蓝绿色
#define LIGHT_MAGENTA 0xFF80FF //	亮紫红色
#define LIGHT_YELLOW 0xFFFF80  //	亮黄色
#define LIGHT_GREY 0xA3A3A3    //	亮灰色

#define DARK_BLUE 0x000080    //	暗蓝色
#define DARK_GREEN 0x008000   //	暗绿色
#define DARK_RED 0x800000     //	暗红色
#define DARK_CYAN 0x008080    //	暗蓝绿色
#define DARK_MAGENTA 0x800080 //	暗紫红色
#define DARK_YELLOW 0x808000  //	暗黄色
#define DARK_GREY 0x404040    //	暗灰色

typedef struct // LCD相关参数结构体
{
    uint32_t Color;       //	LCD当前画笔颜色
    uint32_t BackColor;   //	背景色
    uint8_t ShowNum_Mode; // 数字显示模式
    uint8_t Direction;    //	显示方向
    uint16_t Width;       // 屏幕像素长度
    uint16_t Height;      // 屏幕像素宽度
    uint8_t X_Offset;     // X坐标偏移，用于设置屏幕控制器的显存写入方式
    uint8_t Y_Offset;     // Y坐标偏移，用于设置屏幕控制器的显存写入方式
}lcd_t; 

extern lcd_t LCD;

#define LCD_DC_Command() GPIO_OUT(GPIO_E,15,0) // 低电平，指令传输
#define LCD_DC_Data() GPIO_OUT(GPIO_E,15,1)

#define LCD_BLK_ON() GPIO_OUT(GPIO_D,15,1)     // 高电平,开启背光
#define LCD_BLK_OFF() GPIO_OUT(GPIO_D,15,0)   // 低电平,关闭背光

void LCD_Init(void);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color);
void LCD_DisplayString( uint16_t x, uint16_t y, char *p) ;
void LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number, uint8_t len);
void LCD_Fill_Buffer_Colors(u16 x, u16 y, u16 width, u16 height,u16 *src);

/*提供给PICTURE的接口*/
void piclib_draw_hline(u16 x0,u16 y0,u16 len,u16 color);
void picture_fill_rect(u16 sx, u16 sy, u16 ex, u16 ey, u32 color);
#endif
