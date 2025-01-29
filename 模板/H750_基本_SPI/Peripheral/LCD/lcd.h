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

/*IO初始化结构体*/
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint32_t GPIO_Pin;
} gpio_t;

/*spi引脚结构体*/
typedef struct
{
    gpio_t sclk_io; // 时钟线
    gpio_t mosi_io; // 数据线
    gpio_t blk_io;  // 背光
    gpio_t rs_io;   // 指令or数据线
    gpio_t cs_io;   // 片选
} lcd_spi_io_t;

extern lcd_spi_io_t lcd_io;

#define LCD_DC_Command() lcd_io.rs_io.GPIOx->BSRR = 0x80000000 // 低电平，指令传输
#define LCD_DC_Data() lcd_io.rs_io.GPIOx->BSRR = 0x8000       // 高电平,数据传输

#define LCD_BLK_ON() lcd_io.blk_io.GPIOx->BSRR = 0x8000     // 高电平,开启背光
#define LCD_BLK_OFF() lcd_io.blk_io.GPIOx->BSRR = 0x80000000 // 低电平,关闭背光

void LCD_Init(void);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void LCD_DisplayString( uint16_t x, uint16_t y, char *p) ;
void  LCD_DisplayNumber( uint16_t x, uint16_t y, int32_t number, uint8_t len);

#endif
