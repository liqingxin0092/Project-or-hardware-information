#ifndef __LTDC_H__
#define __LTDC_H__

#include "main.h"

extern LTDC_HandleTypeDef hltdc;
extern uint16_t RGB_GRAM[1024][600] __attribute__((at(0XC0000000)));
extern uint16_t background_color;
/**LTDC GPIO 映射
PI9     ------> LTDC_VSYNC
PI10     ------> LTDC_HSYNC
PF10     ------> LTDC_DE
PH9     ------> LTDC_R3
PH10     ------> LTDC_R4
PH11     ------> LTDC_R5
PH12     ------> LTDC_R6
PG6     ------> LTDC_R7
PG7     ------> LTDC_CLK
PH13     ------> LTDC_G2
PH14     ------> LTDC_G3
PH15     ------> LTDC_G4
PI0     ------> LTDC_G5
PI1     ------> LTDC_G6
PI2     ------> LTDC_G7
PG11     ------> LTDC_B3
PG12     ------> LTDC_B4
PI5     ------> LTDC_B5
PI6     ------> LTDC_B6
PI7     ------> LTDC_B7
*/

/*一些颜色*/
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define PURPLE 0xF81F     // 紫色
#define BROWN 0XBC40      // 棕色
#define BRRED 0XFC07      // 棕红色
#define GRAY 0X8430       // 灰色
#define DARKBLUE 0X01CF   // 深蓝色
#define LIGHTBLUE 0X7D7C  // 浅蓝色
#define GRAYBLUE 0X5458   // 灰蓝色
#define LIGHTGREEN 0X841F // 浅绿色
#define LIGHTGRAY 0XEF5B  // 浅灰色(PANNEL)
#define LGRAY 0XC618      // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE 0XA651  // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12     // 浅棕蓝色(选择条目的反色)

/*LTDC重要参数配置*/
#define LTDC_PWIDTH 1024 // 面板宽度,单位:像素
#define LTDC_PHEIGHT 600 // 面板高度,单位:像素
#define LTDC_HSW 20      // 水平同步宽度
#define LTDC_VSW 3       // 垂直同步宽度
#define LTDC_HBP 140     // 水平后廊
#define LTDC_VBP 20      // 垂直后廊
#define LTDC_HFP 160     // 水平前廊
#define LTDC_VFP 12      // 垂直前廊

/*用户配置宏*/
#define DISPLAY_DIR 0        // 0,横屏;1,竖屏

/*函数中横竖屏的区别*/
#if DISPLAY_DIR == 0 // 0,横屏
#define LTDC_HEIGHT LTDC_PHEIGHT
#define LTDC_WIDTH LTDC_PWIDTH
#define LTDC_Draw_Point __LTDC_draw_point_across
#elif DISPLAY_DIR == 1 // 1,竖屏
#define LTDC_HEIGHT LTDC_PWIDTH
#define LTDC_WIDTH LTDC_PHEIGHT
#define LTDC_Draw_Point __LTDC_draw_point_vertical
#endif

/*           方向图(横向)               */
//                                 ^
//                                 |
//                                 |
//                                 | - ST-Link
//                                 |
//                                 y
// <-----------------------------x
#define __LTDC_draw_point_across(x, y, color) *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * (y) + (x))) = (color) // T
/*           方向图(纵向)               */
//        ST-Link
//          |
//   x-------------->
//  y
//  |
//  |
//  |
//  |
//  |
//  |
//  |
//  |
//  V
#define __LTDC_draw_point_vertical(x, y, color) *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * (LTDC_PHEIGHT - (x) - 1) + (y))) = (color) // T
#define __LTDC_read_point_across(x, y) *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * (y) + (x)))                                         //
#define __LTDC_read_point_vertical(x, y) *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * (LTDC_PHEIGHT - (x) - 1) + (y)))                  // T

/*功能函数 */
void MX_LTDC_Init(uint16_t color);                                                                                                            // 初始化LTDC及RGB屏幕                                                                                                     // 清屏
void LTDC_ShowChar(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t color, uint8_t mode);                                 // 显示字符
void LTDC_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint16_t color, uint8_t mode); // 显示字符串
void DMA2D_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);                                                   // 填充颜色
void DMA2D_fill_color(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height, const unsigned char *source_addr);                           // 显示图片
void LTDC_CLear(uint16_t color);
void LTDC_turn_area_color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color1 ,uint16_t color2);
#endif
