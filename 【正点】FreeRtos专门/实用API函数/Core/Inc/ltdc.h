#ifndef __LTDC_H__
#define __LTDC_H__

#include "main.h"

extern LTDC_HandleTypeDef hltdc;
extern uint16_t RGB_GRAM[1024][600] __attribute__((at(0XC0000000)));
extern uint16_t background_color;
/**LTDC GPIO ӳ��
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

/*һЩ��ɫ*/
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
#define PURPLE 0xF81F     // ��ɫ
#define BROWN 0XBC40      // ��ɫ
#define BRRED 0XFC07      // �غ�ɫ
#define GRAY 0X8430       // ��ɫ
#define DARKBLUE 0X01CF   // ����ɫ
#define LIGHTBLUE 0X7D7C  // ǳ��ɫ
#define GRAYBLUE 0X5458   // ����ɫ
#define LIGHTGREEN 0X841F // ǳ��ɫ
#define LIGHTGRAY 0XEF5B  // ǳ��ɫ(PANNEL)
#define LGRAY 0XC618      // ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE 0XA651  // ǳ����ɫ(�м����ɫ)
#define LBBLUE 0X2B12     // ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

/*LTDC��Ҫ��������*/
#define LTDC_PWIDTH 1024 // �����,��λ:����
#define LTDC_PHEIGHT 600 // ���߶�,��λ:����
#define LTDC_HSW 20      // ˮƽͬ�����
#define LTDC_VSW 3       // ��ֱͬ�����
#define LTDC_HBP 140     // ˮƽ����
#define LTDC_VBP 20      // ��ֱ����
#define LTDC_HFP 160     // ˮƽǰ��
#define LTDC_VFP 12      // ��ֱǰ��

/*�û����ú�*/
#define DISPLAY_DIR 0        // 0,����;1,����

/*�����к�����������*/
#if DISPLAY_DIR == 0 // 0,����
#define LTDC_HEIGHT LTDC_PHEIGHT
#define LTDC_WIDTH LTDC_PWIDTH
#define LTDC_Draw_Point __LTDC_draw_point_across
#elif DISPLAY_DIR == 1 // 1,����
#define LTDC_HEIGHT LTDC_PWIDTH
#define LTDC_WIDTH LTDC_PHEIGHT
#define LTDC_Draw_Point __LTDC_draw_point_vertical
#endif

/*           ����ͼ(����)               */
//                                 ^
//                                 |
//                                 |
//                                 | - ST-Link
//                                 |
//                                 y
// <-----------------------------x
#define __LTDC_draw_point_across(x, y, color) *(uint16_t *)((uint32_t)RGB_GRAM + 2 * (LTDC_PWIDTH * (y) + (x))) = (color) // T
/*           ����ͼ(����)               */
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

/*���ܺ��� */
void MX_LTDC_Init(uint16_t color);                                                                                                            // ��ʼ��LTDC��RGB��Ļ                                                                                                     // ����
void LTDC_ShowChar(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t color, uint8_t mode);                                 // ��ʾ�ַ�
void LTDC_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint16_t color, uint8_t mode); // ��ʾ�ַ���
void DMA2D_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);                                                   // �����ɫ
void DMA2D_fill_color(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height, const unsigned char *source_addr);                           // ��ʾͼƬ
void LTDC_CLear(uint16_t color);
void LTDC_turn_area_color(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color1 ,uint16_t color2);
#endif
