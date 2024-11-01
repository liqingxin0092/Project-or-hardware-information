#ifndef __32_LCD_H
#define __32_LCD_H

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
#define GPIOB_ODR_M 0x40020414 // 数据输出
#define GPIOD_ODR_M 0x40020C14
#define GPIOE_ODR_M 0x40021014
#define GPIOF_ODR_M 0x40021414
#define GPIOG_ODR_M 0x40021814

#define GPIOD_IDR_M 0x40020C10 // 数据输入
#define GPIOE_IDR_M 0x40021010

#define GPIOD_MODER_M 0x40020C00 // 数据输入输出模式,上拉下拉.
#define GPIOE_MODER_M 0x40021000

#define D0_D3_ODR GPIOD_ODR_M // 具体数据输出端
#define D4_D12_ODR GPIOE_ODR_M
#define D13_D15_ODR GPIOD_ODR_M

#define D0_D3_IDR GPIOD_IDR_M // 具体数据输入端
#define D4_D12_IDR GPIOE_IDR_M
#define D13_D15_IDR GPIOD_IDR_M

#define D0_D3_MODER GPIOD_MODER_M // 具体数据模式
#define D4_D12_MODER GPIOE_MODER_M
#define D13_D15_MODER GPIOD_MODER_M

#define WE_ODR GPIOD_ODR_M // 具体控制脚输出端
#define LCD_CS_ODR GPIOG_ODR_M
#define RS_ODR GPIOF_ODR_M
#define RD_ODR GPIOD_ODR_M
#define BL_ODR GPIOB_ODR_M

#define D0_Pin 14
#define D1_Pin 15
#define D2_Pin 0
#define D3_Pin 1
#define D4_Pin 7
#define D5_Pin 8
#define D6_Pin 9
#define D7_Pin 10
#define D8_Pin 11
#define D9_Pin 12
#define D10_Pin 13
#define D11_Pin 14
#define D12_Pin 15
#define D13_Pin 8
#define D14_Pin 9
#define D15_Pin 10

#define WE_Pin 5
#define CS_Pin 12
#define RS_Pin 12
#define RD_Pin 4
#define BL_Pin 15

typedef enum
{
    lcd_success,
    lcd_error,
} lcd_status_t;

/*注意这个只针对0x4000 0000的区域!*/
#define __BIT_SET(WORD_ADDR, BIT_ADDR, BIT) *(uint32_t *)(0x42000000 + (((WORD_ADDR) & 0x0fffffff) << 5) + ((BIT_ADDR) << 2)) = (BIT)

#define __DATA_SET(BYTE)                                     \
    do                                                       \
    {                                                        \
        __BIT_SET(D0_D3_ODR, D0_Pin, (BYTE) & 1);            \
        __BIT_SET(D0_D3_ODR, D1_Pin, ((BYTE) >> 1) & 1);     \
        __BIT_SET(D0_D3_ODR, D2_Pin, ((BYTE) >> 2) & 1);     \
        __BIT_SET(D0_D3_ODR, D3_Pin, ((BYTE) >> 3) & 1);     \
        __BIT_SET(D4_D12_ODR, D4_Pin, ((BYTE) >> 4) & 1);    \
        __BIT_SET(D4_D12_ODR, D5_Pin, ((BYTE) >> 5) & 1);    \
        __BIT_SET(D4_D12_ODR, D6_Pin, ((BYTE) >> 6) & 1);    \
        __BIT_SET(D4_D12_ODR, D7_Pin, ((BYTE) >> 7) & 1);    \
        __BIT_SET(D4_D12_ODR, D8_Pin, ((BYTE) >> 8) & 1);    \
        __BIT_SET(D4_D12_ODR, D9_Pin, ((BYTE) >> 9) & 1);    \
        __BIT_SET(D4_D12_ODR, D10_Pin, ((BYTE) >> 10) & 1);  \
        __BIT_SET(D4_D12_ODR, D11_Pin, ((BYTE) >> 11) & 1);  \
        __BIT_SET(D4_D12_ODR, D12_Pin, ((BYTE) >> 12) & 1);  \
        __BIT_SET(D13_D15_ODR, D13_Pin, ((BYTE) >> 13) & 1); \
        __BIT_SET(D13_D15_ODR, D14_Pin, ((BYTE) >> 14) & 1); \
        __BIT_SET(D13_D15_ODR, D15_Pin, ((BYTE) >> 15) & 1); \
    } while (0)

#define __IO_IN_MODE()                            \
    do                                            \
    {                                             \
        __BIT_SET(D0_D3_MODER, 2 * D0_Pin, 0);    \
        __BIT_SET(D0_D3_MODER, 2 * D1_Pin, 0);    \
        __BIT_SET(D0_D3_MODER, 2 * D2_Pin, 0);    \
        __BIT_SET(D0_D3_MODER, 2 * D3_Pin, 0);    \
        __BIT_SET(D4_D12_MODER, 2 * D4_Pin, 0);   \
        __BIT_SET(D4_D12_MODER, 2 * D5_Pin, 0);   \
        __BIT_SET(D4_D12_MODER, 2 * D6_Pin, 0);   \
        __BIT_SET(D4_D12_MODER, 2 * D7_Pin, 0);   \
        __BIT_SET(D4_D12_MODER, 2 * D8_Pin, 0);   \
        __BIT_SET(D4_D12_MODER, 2 * D9_Pin, 0);   \
        __BIT_SET(D4_D12_MODER, 2 * D10_Pin, 0);  \
        __BIT_SET(D4_D12_MODER, 2 * D11_Pin, 0);  \
        __BIT_SET(D4_D12_MODER, 2 * D12_Pin, 0);  \
        __BIT_SET(D13_D15_MODER, 2 * D13_Pin, 0); \
        __BIT_SET(D13_D15_MODER, 2 * D14_Pin, 0); \
        __BIT_SET(D13_D15_MODER, 2 * D15_Pin, 0); \
    } while (0)

#define __IO_OUT_MODE()                           \
    do                                            \
    {                                             \
        __BIT_SET(D0_D3_MODER, 2 * D0_Pin, 1);    \
        __BIT_SET(D0_D3_MODER, 2 * D1_Pin, 1);    \
        __BIT_SET(D0_D3_MODER, 2 * D2_Pin, 1);    \
        __BIT_SET(D0_D3_MODER, 2 * D3_Pin, 1);    \
        __BIT_SET(D4_D12_MODER, 2 * D4_Pin, 1);   \
        __BIT_SET(D4_D12_MODER, 2 * D5_Pin, 1);   \
        __BIT_SET(D4_D12_MODER, 2 * D6_Pin, 1);   \
        __BIT_SET(D4_D12_MODER, 2 * D7_Pin, 1);   \
        __BIT_SET(D4_D12_MODER, 2 * D8_Pin, 1);   \
        __BIT_SET(D4_D12_MODER, 2 * D9_Pin, 1);   \
        __BIT_SET(D4_D12_MODER, 2 * D10_Pin, 1);  \
        __BIT_SET(D4_D12_MODER, 2 * D11_Pin, 1);  \
        __BIT_SET(D4_D12_MODER, 2 * D12_Pin, 1);  \
        __BIT_SET(D13_D15_MODER, 2 * D13_Pin, 1); \
        __BIT_SET(D13_D15_MODER, 2 * D14_Pin, 1); \
        __BIT_SET(D13_D15_MODER, 2 * D15_Pin, 1); \
    } while (0)

// LCD重要参数集
typedef struct
{
    uint16_t width;  // LCD 宽度
    uint16_t height; // LCD 高度
    uint8_t dir;     // 横屏还是竖屏控制：0，竖屏；1，横屏。
} lcd_t;

/*常见指令*/
#define LCD_READ_ID 0xD3        // 0x00009431
#define LCD_ACCESS_CONTROL 0x36 // 访问控制,LCD读写方向
#define LCD_COLUMN_ADDRESS 0x2A // 一般用于设置X坐标
#define LCD_PAGE_ADDRESS 0x2B   // 一般用于设置Y坐标
#define LCD_WIRTE_GRAM 0x2C     // 写GRAM,发这个后面跟一串数据,地址自增
#define LCD_READ_GRAM 0x2E      // 读GRAM

/*最多点数*/
#define POINT_MAX 76800

/*8种模式*/
#define L2R_U2D 0 // 从左到右,从上到下
#define L2R_D2U 1 // 从左到右,从下到上
#define R2L_U2D 2 // 从右到左,从上到下
#define R2L_D2U 3 // 从右到左,从下到上

#define U2D_L2R 4 // 从上到下,从左到右
#define U2D_R2L 5 // 从上到下,从右到左
#define D2U_L2R 6 // 从下到上,从左到右
#define D2U_R2L 7 // 从下到上,从右到左

/*常见颜色*/
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
#define BROWN 0XBC40 // 棕色
#define BRRED 0XFC07 // 棕红色
#define GRAY 0X8430  // 灰色

#define DARKBLUE 0X01CF  // 深蓝色
#define LIGHTBLUE 0X7D7C // 浅蓝色
#define GRAYBLUE 0X5458  // 灰蓝色

#define LIGHTGREEN 0X841F // 浅绿色
#define LIGHTGRAY 0XEF5B
#define LGRAY 0XC618 // 浅灰色,窗体背景色

#define GRAY0 0xEF7D // 灰色0
#define GRAY1 0x8410 // 灰色1
#define GRAY2 0x4208 // 灰色2

#define LGRAYBLUE 0XA651 // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12    // 浅棕蓝色(选择条目的反色)

/*基础函数*/
// void __DATA_SET(uint16_t data);
void LCD_IO_Init(void);
void LCD_write_data(uint16_t data);
void LCD_write_command(uint16_t command);
uint16_t LCD_read_data(void);

/*功能函数*/
uint32_t LCD_read_id(void);
void LCD_access_control(uint16_t num);
lcd_status_t LCD_set_x_coordinate(int16_t start_position, int16_t end_position);
void LCD_set_x(uint16_t start_position);
lcd_status_t LCD_set_y_coordinate(int16_t start_position, int16_t end_position);
void LCD_set_y(uint16_t start_position);
lcd_status_t LCD_read_gram(int16_t x_start, int16_t x_end, int16_t y_start, int16_t y_end, uint16_t *ptr, uint32_t lenth);
uint16_t LCD_read_gram_one(int16_t x_start, int16_t y_start);
void LCD_write_gram(uint16_t *ptr, uint32_t lenth);
void LCD_Clear(uint16_t color);
void LCD_Init(uint16_t color, uint16_t Mode);
void LCD_set_cursor(uint16_t x, uint16_t y);
void LCD_Show_Picture(void);
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t color, uint8_t mode);
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void LCD_fill_color_arr(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color_arr);

#endif
