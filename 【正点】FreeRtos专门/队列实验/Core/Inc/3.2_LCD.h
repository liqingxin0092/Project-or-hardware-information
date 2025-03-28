#ifndef __32_LCD_H
#define __32_LCD_H

/*写程序注意!!!!!!*/
// 像素:320 240
//  RS引脚,高电平是数据,低电平是命令..哭了..

/*当前映射*/

// D0 ~ PD14
// D1 ~ PD15
// D2 ~ PD0
// D3 ~ PD1
// D4 ~ PE7
// D5 ~ PE8
// D6 ~ PE9
// D7 ~ PE10
// D8 ~ PE11
// D9 ~ PE12
// D10 ~ PE13
// D11 ~ PE14
// D12 ~ PE15
// D13 ~ PD8
// D14 ~ PD9
// D15 ~ PD10

// WE ~ PD5
// LCD_CS ~ PG12
// RS ~ PF12
// RD ~ PD4
// BL ~ PB15(经过三极管驱动反转后,低电平灭,高电平亮.)
// RST ~ RST(单片机复位)

/*触摸部分*/
// T_CS ~ PC13
// T_MISO ~ PB12
// T_MOSI ~ PF11
// T_SCK ~ PB0
// T_PEN ~ PB1

/*PORT基地址*/
//  GPIOB ~ 0x4002 0400
//  GPIOD ~ 0x4002 0C00
//  GPIOE ~ 0x4002 1000
//  GPIOF ~ 0x4002 1400
//  GPIOG ~ 0x4002 1800
//  ODR ~ 0x14
//  IDR ~ 0x10

#include "stdint.h"
#include "fsmc.h"
#include "font.h"
#include "pic.h"

// LCD地址结构体
typedef struct
{
    volatile uint16_t command;
    volatile uint16_t data;
} LCD_TypeDef;

extern u16 POINT_COLOR;	//画笔颜色
extern u16 BACK_COLOR;  //背景色 

#define FSMC_BASE 0x6c000000
#define FSMC_RS_ADD 128

#define LCD_BASE ((uint32_t)(FSMC_BASE | FSMC_RS_ADD - 2))
#define LCD ((LCD_TypeDef *)LCD_BASE)

typedef enum
{
    lcd_success,
    lcd_error,
} lcd_status_t;

// LCD重要参数集
typedef struct
{
    uint16_t width;  // LCD 宽度
    uint16_t height; // LCD 高度
    uint8_t dir;     // 横屏还是竖屏控制：0，竖屏；1，横屏。
} lcd_t;
extern lcd_t lcd;
/*常见指令*/
#define LCD_READ_ID 0xD3        // 0x00009431
#define LCD_ACCESS_CONTROL 0x36 // 访问控制,LCD读写方向
#define LCD_COLUMN_ADDRESS 0x2A // 一般用于设置X坐标
#define LCD_PAGE_ADDRESS 0x2B   // 一般用于设置Y坐标
#define LCD_WIRTE_GRAM 0x2C     // 写GRAM,发这个后面跟一串数据,地址自增
#define LCD_READ_GRAM 0x2E      // 读GRAM

/*最多点数*/
#define POINT_MAX 76800

/*移植用的...*/
#define LCD_DIR_Mode  	  2	    //4种工作模式，0和1是竖屏模式，2和3是横屏模式
#define USE_HORIZONTAL  	1     //方向设置： 		0,竖屏模式   1,横屏模式.

/*8种模式*/
#define L2R_U2D 0 // 从左到右,从上到下
#define L2R_D2U 1 // 从左到右,从下到上
#define R2L_U2D 2 // 从右到左,从上到下
#define R2L_D2U 3 // 从右到左,从下到上

#define U2D_L2R 4 // 从上到下,从左到右
#define U2D_R2L 5 // 从上到下,从右到左
#define D2U_L2R 6 // 从下到上,从左到右
#define D2U_R2L 7 // 从下到上,从右到左

/*一些颜色*/
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define YELLOW 0xFFE0
#define CYAN 0x7FFF       // 白蓝色
#define PURPLE 0xF81F     // 紫色
#define BROWN 0XBC40      // 棕色
#define BRRED 0XFC07      // 棕红色
#define GRAY 0X8430       // 灰色
#define DARKBLUE 0X01CF   // 深蓝色
#define LIGHTBLUE 0X7D7C  // 浅蓝色
#define GRAYBLUE 0X5458   // 灰蓝色
#define LIGHTGREEN 0X841F // 浅绿色
#define LIGHTGRAY 0XEF5B  // 浅灰色(PANNEL),我感觉白的
#define LGRAY 0XC618      // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE 0XA651  // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12     // 浅棕蓝色(选择条目的反色)

extern uint16_t background_color;
extern uint16_t pen_color;

/*功能函数*/
uint32_t LCD_read_id(void);
lcd_status_t LCD_set_x_coordinate(int16_t start_position, int16_t end_position);
void LCD_set_x(uint16_t start_position);
lcd_status_t LCD_set_y_coordinate(int16_t start_position, int16_t end_position);
void LCD_set_y(uint16_t start_position);
lcd_status_t LCD_read_gram(int16_t x_start, int16_t x_end, int16_t y_start, int16_t y_end, uint16_t *ptr, uint32_t lenth);
uint16_t LCD_read_gram_one(int16_t x_start, int16_t y_start);
void LCD_write_gram(uint16_t *ptr, uint32_t lenth);

/*常规功能*/
void LCD_Clear(uint16_t color);
void LCD_Init(uint16_t color, uint16_t Mode);
void LCD_Draw_Point_FSMC(uint16_t x, uint16_t y, uint16_t color);
void LCD_set_cursor(uint16_t x, uint16_t y);
 void LCD_Show_Picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const unsigned char* ptr);
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t color, uint8_t mode);
void LCD_ShowChar_colorful(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t *color, uint8_t color_time, uint8_t mode);
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint8_t mode);
void LCD_ShowString_colorful(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint16_t *color, uint8_t color_time, uint8_t mode);
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void LCD_fill_color_arr(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color_arr);
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint8_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void LCD_test_runing(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color1, uint16_t color2, uint8_t *flag);
// LCD_test_runing(310,230, 319, 239, BLUE,YELLOW,&flag);//测试运行(横屏)
void LCD_DrawPoint(uint16_t x, uint16_t y);


#endif
