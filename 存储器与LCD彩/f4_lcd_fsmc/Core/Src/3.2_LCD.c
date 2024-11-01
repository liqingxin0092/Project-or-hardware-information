#include "3.2_LCD.h"
#include "stddef.h"
#include "Delay.h"
#include "Picture.h"
#include "FONT.H"
#include "main.h "
// static volatile void LCD_Delay(uint16_t time)
//{
//     for (uint16_t i = 0; i < time; i++)
//         while (0)
//             ;
// }

typedef struct
{
    uint16_t temp0 : 1;
    uint16_t temp1 : 1;
    uint16_t temp2 : 1;
    uint16_t temp3 : 1;
    uint16_t temp4 : 1;
    uint16_t temp5 : 1;
    uint16_t temp6 : 1;
    uint16_t temp7 : 1;
    uint16_t temp8 : 1;
    uint16_t temp9 : 1;
    uint16_t temp10 : 1;
    uint16_t temp11 : 1;
    uint16_t temp12 : 1;
    uint16_t temp13 : 1;
    uint16_t temp14 : 1;
    uint16_t temp15 : 1;
} uint16_mix_t;

/**
 * @brief 测试用的,HAL库驱动IO
 *
 * @param data
 */
// void __DATA_SET(uint16_t data)
// {
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, (GPIO_PinState)(data & 1));
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, (GPIO_PinState)((data >> 1) & 1));
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, (GPIO_PinState)((data >> 2) & 1));
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, (GPIO_PinState)((data >> 3) & 1));

//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, (GPIO_PinState)((data >> 4) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, (GPIO_PinState)((data >> 5) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, (GPIO_PinState)((data >> 6) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, (GPIO_PinState)((data >> 7) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, (GPIO_PinState)((data >> 8) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, (GPIO_PinState)((data >> 9) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, (GPIO_PinState)((data >> 10) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, (GPIO_PinState)((data >> 11) & 1));
//     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, (GPIO_PinState)((data >> 12) & 1));

//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, (GPIO_PinState)((data >> 13) & 1));
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, (GPIO_PinState)((data >> 14) & 1));
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, (GPIO_PinState)((data >> 15) & 1));
// }
uint16_t IO_READ(void)
{
    uint16_mix_t temp = {0};
    temp.temp0 = *(uint32_t *)(0x42000000 + ((D0_D3_IDR & 0x0fffffff) << 5) + ((D0_Pin) << 2));
    temp.temp1 = *(uint32_t *)(0x42000000 + ((D0_D3_IDR & 0x0fffffff) << 5) + ((D1_Pin) << 2));
    temp.temp2 = *(uint32_t *)(0x42000000 + ((D0_D3_IDR & 0x0fffffff) << 5) + ((D2_Pin) << 2));
    temp.temp3 = *(uint32_t *)(0x42000000 + ((D0_D3_IDR & 0x0fffffff) << 5) + ((D3_Pin) << 2));
    temp.temp4 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D4_Pin) << 2));
    temp.temp5 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D5_Pin) << 2));
    temp.temp6 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D6_Pin) << 2));
    temp.temp7 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D7_Pin) << 2));
    temp.temp8 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D8_Pin) << 2));
    temp.temp9 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D9_Pin) << 2));
    temp.temp10 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D10_Pin) << 2));
    temp.temp11 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D11_Pin) << 2));
    temp.temp12 = *(uint32_t *)(0x42000000 + ((D4_D12_IDR & 0x0fffffff) << 5) + ((D12_Pin) << 2));
    temp.temp13 = *(uint32_t *)(0x42000000 + ((D13_D15_IDR & 0x0fffffff) << 5) + ((D13_Pin) << 2));
    temp.temp14 = *(uint32_t *)(0x42000000 + ((D13_D15_IDR & 0x0fffffff) << 5) + ((D14_Pin) << 2));
    temp.temp15 = *(uint32_t *)(0x42000000 + ((D13_D15_IDR & 0x0fffffff) << 5) + ((D15_Pin) << 2));
    return (temp.temp15 << 15) | (temp.temp14 << 14) | (temp.temp13 << 13) | (temp.temp12 << 12) | (temp.temp11 << 11) | (temp.temp10 << 10) | (temp.temp9 << 9) | (temp.temp8 << 8) | (temp.temp7 << 7) | (temp.temp6 << 6) | (temp.temp5 << 5) | (temp.temp4 << 4) | (temp.temp3 << 3) | (temp.temp2 << 2) | (temp.temp1 << 1) | (temp.temp0);
}

/*定义lcd的重要参数集*/
lcd_t lcd;

void LCD_IO_Init(void)
{
    __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // CS拉高
    __BIT_SET(RD_ODR, RD_Pin, 1);     // 辅助读时序
    __BIT_SET(BL_ODR, BL_Pin, 1);     // 开启背光
}

void LCD_write_data(volatile uint16_t data)
{ // FSMC驱动
#if IS_FSMC == 1
    LCD->data = data;
#else // 软件驱动
    __BIT_SET(LCD_CS_ODR, CS_Pin, 0); // 选中

    __BIT_SET(RD_ODR, RD_Pin, 1); // 写数据期间,RD保持高电平
    __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据

    __DATA_SET(data);             // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // 通讯结束
#endif
}

void LCD_write_command(volatile uint16_t command)
{
#if IS_FSMC == 1
    LCD->command = command;
#else // 软件驱动
    {
        __BIT_SET(LCD_CS_ODR, CS_Pin, 0); // 选中

        __BIT_SET(RD_ODR, RD_Pin, 1); // 写数据期间,RD保持高电平
        __BIT_SET(RS_ODR, RS_Pin, 0); // 操作命令

        __DATA_SET(command);          // 摆放数据
        __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
        __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

        __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // 通讯结束
    }
#endif
}

uint16_t LCD_read_data(void)
{

#if IS_FSMC == 1

    return LCD->data;
#else
    uint16_t temp;
    __BIT_SET(LCD_CS_ODR, CS_Pin, 0); // 选中

    __BIT_SET(WE_ODR, WE_Pin, 1); // 写数据期间,WE保持高电平
    __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据

    __BIT_SET(RD_ODR, RD_Pin, 0); // LCD开始准备数据
    temp = IO_READ();             // 读
    __BIT_SET(RD_ODR, RD_Pin, 1); // 为下一读做准备

    __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // 选中
    return temp;
#endif
}

uint32_t LCD_read_id(void)
{
    uint16_t temp0 = 0, temp1 = 0, temp2 = 0;
    LCD_write_command(LCD_READ_ID);
#if IS_FSMC == 1
    LCD_read_data(); // 空读
    temp0 = LCD_read_data() & 0xff;
    temp1 = LCD_read_data() & 0xff;
    temp2 = LCD_read_data() & 0xff;
#else
    __IO_IN_MODE(); // 输入模式

    LCD_read_data(); // 空读
    temp0 = LCD_read_data() & 0xff;
    temp1 = LCD_read_data() & 0xff;
    temp2 = LCD_read_data() & 0xff;

    __IO_OUT_MODE(); // 输出模式
#endif

    return (temp0 << 16) | (temp1 << 8) | temp2;
}

/**
 * @brief 设置x开始和结束
 *
 * @param start_position
 * @param end_position
 * @return lcd_status_t
 */
lcd_status_t LCD_set_x_coordinate(int16_t start_position, int16_t end_position)
{
    if (0 <= start_position && start_position <= 239 && 0 <= end_position && end_position <= 239 && start_position < end_position)
    {
        LCD_write_command(LCD_COLUMN_ADDRESS);
        LCD_write_data(start_position >> 8);
        LCD_write_data(start_position & 0xff);
        LCD_write_data(end_position >> 8);
        LCD_write_data(end_position & 0xff);
        return lcd_success;
    }
    else
        return lcd_error;
}

/**
 * @brief 单独设置X起点
 *
 * @param start_position
 */
void LCD_set_x(uint16_t start_position)
{
    LCD_write_command(LCD_COLUMN_ADDRESS);
    LCD_write_data(start_position >> 8);
    LCD_write_data(start_position & 0xff);
}
/**
 * @brief 设置Y坐标范围
 *
 * @param start_position
 * @param end_position
 * @return lcd_status_t
 */
lcd_status_t LCD_set_y_coordinate(int16_t start_position, int16_t end_position)
{
    if (0 <= start_position && start_position <= 319 && 0 <= end_position && end_position <= 319 && start_position < end_position)
    {
        LCD_write_command(LCD_PAGE_ADDRESS);
        LCD_write_data(start_position >> 8);
        LCD_write_data(start_position & 0xff);
        LCD_write_data(end_position >> 8);
        LCD_write_data(end_position & 0xff);
        return lcd_success;
    }
    else
        return lcd_error;
}
/**
 * @brief 单独设置Y起点
 *
 * @param start_position
 */
void LCD_set_y(uint16_t start_position)
{
    LCD_write_command(LCD_PAGE_ADDRESS);
    LCD_write_data(start_position >> 8);
    LCD_write_data(start_position & 0xff);
}
/**
 * @brief 写GRAM
 *
 * @param ptr 数据数组
 * @param lenth 数据长度
 */
void LCD_write_gram(uint16_t *ptr, uint32_t lenth)
{
    LCD_write_command(LCD_WIRTE_GRAM);

    for (uint16_t i = 0; i < lenth; i++)
        LCD_write_data(ptr[i]);
}
/**
 * @brief 读一串RGB数据,解析好的
 *
 * @param x_start
 * @param x_end
 * @param y_start
 * @param y_end
 * @param ptr
 * @param lenth
 * @return lcd_status_t
 */
lcd_status_t LCD_read_gram(int16_t x_start, int16_t x_end, int16_t y_start, int16_t y_end, uint16_t *ptr, uint32_t lenth)
{
    if (LCD_set_x_coordinate(x_start, x_end) != lcd_success ||
        LCD_set_y_coordinate(y_start, y_end) != lcd_success)
        return lcd_error;
    uint16_t temp, read_temp,
        true_count = 0;

    LCD_write_command(LCD_READ_GRAM); // 读命令

    __IO_IN_MODE(); // 输入模式

    LCD_read_data(); // 空读

    /*计算需要读的次数*/
    if (lenth % 2 == 0)
    {
        temp = (float)lenth * 1.5f;
    }
    else
    {
        temp = (float)(lenth + 1) * 1.5f - 1;
    }
    /*读出的数据转化为真正的RGB数据*/
    for (uint16_t i = 0; i < temp; i++)
    {
        read_temp = LCD_read_data();
        if (temp % 3 == 0)
        {
            ptr[true_count] |= (read_temp & 0xf800) | ((read_temp & 0xfc) << 3);
        }
        else if (temp % 3 == 1)
        {
            ptr[true_count++] |= read_temp >> 11; // 拼成第一个数据
            ptr[true_count] |= ((read_temp & 0xf8) << 8);
        }
        else if (temp % 3 == 2)
        {
            ptr[true_count++] |= ((read_temp & 0xfc00) >> 5) | ((read_temp & 0xf8) >> 3); // 拼成第二个数据
        }
    }
    __IO_OUT_MODE(); // 退出输入模式
    return lcd_success;
}
/**
 * @brief 读一个点
 *
 * @param x_start
 * @param y_start
 * @return uint16_t
 */
uint16_t LCD_read_gram_one(int16_t x_start, int16_t y_start)
{
    LCD_set_x(x_start);
    LCD_set_y(y_start);
    LCD_write_command(LCD_READ_GRAM); // 读命令

    __IO_IN_MODE(); // 输入模式

    LCD_read_data(); // 空读

    uint16_t temp0 = LCD_read_data();
    uint16_t temp1 = LCD_read_data();

    uint16_t temp3 = (temp0 & 0xf800) | ((temp0 & 0xfc) << 3) | (temp1 >> 11);

    __IO_OUT_MODE(); // 退出输入模式

    return temp3;
}

/**
 * @brief 设置窗口范围和使8种模式生效
 *
 * @param dir
 */
static void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;

    switch (dir)
    {
    case L2R_U2D: // 从左到右,从上到下
        regval |= (0 << 7) | (0 << 6) | (0 << 5);
        break;
    case L2R_D2U: // 从左到右,从下到上
        regval |= (1 << 7) | (0 << 6) | (0 << 5);
        break;
    case R2L_U2D: // 从右到左,从上到下
        regval |= (0 << 7) | (1 << 6) | (0 << 5);
        break;
    case R2L_D2U: // 从右到左,从下到上
        regval |= (1 << 7) | (1 << 6) | (0 << 5);
        break;
    case U2D_L2R: // 从上到下,从左到右
        regval |= (0 << 7) | (0 << 6) | (1 << 5);
        break;
    case U2D_R2L: // 从上到下,从右到左
        regval |= (0 << 7) | (1 << 6) | (1 << 5);
        break;
    case D2U_L2R: // 从下到上,从左到右
        regval |= (1 << 7) | (0 << 6) | (1 << 5);
        break;
    case D2U_R2L: // 从下到上,从右到左
        regval |= (1 << 7) | (1 << 6) | (1 << 5);
        break;
    }
    /*写命令*/
    LCD_write_command(0x36);
    LCD_write_data(regval | 0x08); // |0x08是BGR顺序
}

/**
 * @brief 设置屏幕范围(全屏),清屏
 *
 * @param color
 */
void LCD_Clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcd.width * lcd.height; // 得到总点数

    /*设置清除范围*/
    LCD_write_command(LCD_COLUMN_ADDRESS);
    LCD_write_data(0);
    LCD_write_data(0);
    LCD_write_data((lcd.width - 1) >> 8);
    LCD_write_data((lcd.width - 1) & 0XFF);

    LCD_write_command(LCD_PAGE_ADDRESS);
    LCD_write_data(0);
    LCD_write_data(0);
    LCD_write_data((lcd.height - 1) >> 8);
    LCD_write_data((lcd.height - 1) & 0XFF);

    /*设置光标位置*/
    LCD_set_x(0);
    LCD_set_y(0);

    /*准备写GRAM*/
    LCD->command = LCD_WIRTE_GRAM;

#if IS_FSMC == 1
    {
        for (index = 0; index < totalpoint; index++)
        {
            LCD->data = color;
        }
    }
#else
    {
        /*写GRAM*/
        __BIT_SET(LCD_CS_ODR, CS_Pin, 0); // 选中

        __BIT_SET(RD_ODR, RD_Pin, 1); // 写数据期间,RD保持高电平
        __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据

        for (index = 0; index < totalpoint; index++)
        {
            __DATA_SET(color);            // 摆放数据
            __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
            __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备
        }
        __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // 通讯结束
    }
#endif
}

/**
 * @brief 设置LCD显示方向
 *
 * @param dir   0,竖屏  正
 *              1,竖屏  反
 *              2,横屏  左
 *              3,横屏  右
 */
static void LCD_Display_Dir(uint8_t dir, uint16_t color)
{
    uint8_t SCAN_DIR;

    if (dir == 0) // 竖屏  正
    {
        lcd.dir = 0; // 竖屏
        lcd.width = 240;
        lcd.height = 320;

        SCAN_DIR = L2R_U2D; // 选择扫描方向
    }

    else if (dir == 1) // 横屏
    {
        lcd.dir = 0; // 竖屏
        lcd.width = 240;
        lcd.height = 320;

        SCAN_DIR = R2L_D2U; // 选择扫描方向
    }

    else if (dir == 2) // 横屏
    {
        lcd.dir = 1; // 横屏
        lcd.width = 320;
        lcd.height = 240;

        SCAN_DIR = U2D_R2L; // 选择扫描方向
    }
    else if (dir == 3) // 横屏
    {
        lcd.dir = 1; // 横屏
        lcd.width = 320;
        lcd.height = 240;

        SCAN_DIR = D2U_L2R; // 选择扫描方向
    }
    else // 设置默认为竖屏--正
    {
        lcd.dir = 0; // 竖屏
        lcd.width = 240;
        lcd.height = 320;

        SCAN_DIR = L2R_U2D; // 选择扫描方向
    }

    /*采用上述设置*/
    LCD_Scan_Dir(SCAN_DIR);

    /*设置全屏范围并且清屏*/
    LCD_Clear(color);
}

/**
 * @brief 初始化LCD
 *
 * @param color 默认颜色
 * @param Mode 刷新方式 : 0和1是竖屏模式，2和3是横屏模式
 *                       虽然一共有8种配置模式,该系列函数只提供4种,很常见也够用了
 */
/*                   四种模式坐标分布             */
// 0:            :::::::::::::::::::::::::            (:::是8080时序排针)
//              0----------------------->239(X)
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              V
//             319(Y)

// 1:           0----------------------->239(X)
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              |
//              V
//             319(Y)
//                 :::::::::::::::::::::::::

// 2:     0--------------------------------->319 (X)
//    ..  |
//    ..  |
//    ..  |
//    ..  |
//    ..  |
//    ..  |
//    ..  |
//    ..  V
//       239 (Y)

// 3:     0--------------------------------->319 (X)
//        |                                         ::
//        |                                         ::
//        |                                         ::
//        |                                         ::
//        |                                         ::
//        |                                         ::
//        |                                         ::
//        |                                         ::
//        V
//       239 (Y)
void LCD_Init(uint16_t color, uint16_t Mode)
{
// 如果不是FSMC驱动才有IO_Init.
#if IS_FSMC == 0
    {
        LCD_IO_Init();
    }
#endif
    LCD_write_command(0xCF);
    LCD_write_data(0x00);
    LCD_write_data(0xC1);
    LCD_write_data(0X30);
    LCD_write_command(0xED);
    LCD_write_data(0x64);
    LCD_write_data(0x03);
    LCD_write_data(0X12);
    LCD_write_data(0X81);
    LCD_write_command(0xE8);
    LCD_write_data(0x85);
    LCD_write_data(0x10);
    LCD_write_data(0x7A);
    LCD_write_command(0xCB);
    LCD_write_data(0x39);
    LCD_write_data(0x2C);
    LCD_write_data(0x00);
    LCD_write_data(0x34);
    LCD_write_data(0x02);
    LCD_write_command(0xF7);
    LCD_write_data(0x20);
    LCD_write_command(0xEA);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_command(0xC0); // Power control
    LCD_write_data(0x1B);    // VRH[5:0]
    LCD_write_command(0xC1); // Power control
    LCD_write_data(0x01);    // SAP[2:0];BT[3:0]
    LCD_write_command(0xC5); // VCM control
    LCD_write_data(0x30);    // 3F
    LCD_write_data(0x30);    // 3C
    LCD_write_command(0xC7); // VCM control2
    LCD_write_data(0XB7);
    LCD_write_command(0x36); // Memory Access Control
    LCD_write_data(0x48);
    LCD_write_command(0x3A);
    LCD_write_data(0x55);
    LCD_write_command(0xB1);
    LCD_write_data(0x00);
    LCD_write_data(0x1A);
    LCD_write_command(0xB6); // Display Function Control
    LCD_write_data(0x0A);
    LCD_write_data(0xA2);
    LCD_write_command(0xF2); // 3Gamma Function Disable
    LCD_write_data(0x00);
    LCD_write_command(0x26); // Gamma curve selected
    LCD_write_data(0x01);
    LCD_write_command(0xE0); // Set Gamma
    LCD_write_data(0x0F);
    LCD_write_data(0x2A);
    LCD_write_data(0x28);
    LCD_write_data(0x08);
    LCD_write_data(0x0E);
    LCD_write_data(0x08);
    LCD_write_data(0x54);
    LCD_write_data(0XA9);
    LCD_write_data(0x43);
    LCD_write_data(0x0A);
    LCD_write_data(0x0F);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_command(0XE1); // Set Gamma
    LCD_write_data(0x00);
    LCD_write_data(0x15);
    LCD_write_data(0x17);
    LCD_write_data(0x07);
    LCD_write_data(0x11);
    LCD_write_data(0x06);
    LCD_write_data(0x2B);
    LCD_write_data(0x56);
    LCD_write_data(0x3C);
    LCD_write_data(0x05);
    LCD_write_data(0x10);
    LCD_write_data(0x0F);
    LCD_write_data(0x3F);
    LCD_write_data(0x3F);
    LCD_write_data(0x0F);
    LCD_write_command(0x2B);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x01);
    LCD_write_data(0x3f);
    LCD_write_command(0x2A);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0xef);
    LCD_write_command(0x11); // Exit Sleep
    Delay_ms(120);
    LCD_write_command(0x29); // display on

    LCD_Display_Dir(Mode, color); // 选择--屏幕显示方式
}

/**
 * @brief 设置光标
 *
 * @param x
 * @param y
 */
void LCD_set_cursor(uint16_t x, uint16_t y)
{
    LCD_set_x(x);
    LCD_set_y(y);
}

/**
 * @brief 显示图片
 *
 * @param way 1横屏显示, 0竖屏显示
 */
void LCD_Show_Picture(void)
{
    uint32_t i, j, k = 0;
    uint16_t picH, picL;

    /*设置光标位置*/
    LCD_set_x(0);
    LCD_set_x(0);

#if USE_HORIZONTAL == 1
    {
        /*准备写GRAM*/
        LCD_write_command(LCD_WIRTE_GRAM);

        /*写GRAM*/
        __BIT_SET(LCD_CS_ODR, CS_Pin, 0); // 选中,开始通讯

        __BIT_SET(RD_ODR, RD_Pin, 1); // 写数据期间,RD保持高电平
        __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据

        for (i = 0; i < 240; i++)
            for (j = 0; j < 320; j++)
            {
                /*计算并且摆放数据*/
                picH = gImage_LCD280[k++];
                picL = gImage_LCD280[k++];
                __DATA_SET((picH << 8) + picL);
                /*发送数据*/
                __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
                __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备
            }
        __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // 通讯结束
    }
#else
    {
        /*准备写GRAM*/
        LCD_write_command(LCD_WIRTE_GRAM);

        /*写GRAM*/
        __BIT_SET(LCD_CS_ODR, CS_Pin, 0); // 选中,开始通讯

        __BIT_SET(RD_ODR, RD_Pin, 1); // 写数据期间,RD保持高电平
        __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据

        for (i = 0; i < 240; i++)
            for (j = 0; j < 225; j++)
            {
                /*计算并且摆放数据*/
                picH = gImage_LCD280[k++];
                picL = gImage_LCD280[k++];
                __DATA_SET((picH << 8) + picL);
                /*发送数据*/
                __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
                __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备
            }
        __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // 通讯结束
    }
#endif
}

void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color)
{
    __BIT_SET(LCD_CS_ODR, CS_Pin, 0); // 选中
    __BIT_SET(RD_ODR, RD_Pin, 1);     // 写数据期间,RD保持高电平

    /*发送设置X命令*/
    __BIT_SET(RS_ODR, RS_Pin, 0);   // 操作命令
    __DATA_SET(LCD_COLUMN_ADDRESS); // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0);   // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1);   // 默认低,为上升沿做准备

    /*发送X数据高位*/
    __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据
    __DATA_SET(x >> 8);           // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    /*发送X数据低位*/
    __DATA_SET(x & 0xff);         // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    /*发送设置Y命令*/
    __BIT_SET(RS_ODR, RS_Pin, 0); // 操作命令
    __DATA_SET(LCD_PAGE_ADDRESS); // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    /*发送Y数据高位*/
    __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据
    __DATA_SET(y >> 8);           // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    /*发送Y数据低位*/
    __DATA_SET(y & 0xff);         // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    /*发送写GRAM命令*/
    __BIT_SET(RS_ODR, RS_Pin, 0); // 操作命令
    __DATA_SET(LCD_WIRTE_GRAM);   // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    /*发送数据*/
    __BIT_SET(RS_ODR, RS_Pin, 1); // 操作数据

    __DATA_SET(color);            // 摆放数据
    __BIT_SET(WE_ODR, WE_Pin, 0); // 制造上升沿发送数据
    __BIT_SET(WE_ODR, WE_Pin, 1); // 默认低,为上升沿做准备

    __BIT_SET(LCD_CS_ODR, CS_Pin, 1); // 通讯结束
}
/**
 * @brief FSMC专用
 *
 * @param x
 * @param y
 * @param color
 */
void LCD_Draw_Point_FSMC(uint16_t x, uint16_t y, uint16_t color)
{
    LCD->command = LCD_COLUMN_ADDRESS;
    LCD->data = x >> 8;
    LCD->data = x & 0xff;
    LCD->command = LCD_PAGE_ADDRESS;
    LCD->data = y >> 8;
    LCD->data = y & 0xff;
    LCD->command = LCD_WIRTE_GRAM;
    LCD->data = color;
}
/**
 * @brief 指定位置显示字符
 *
 * @param x
 * @param y
 * @param ascii 要显示的字符
 * @param size 尺寸,12,16,24
 * @param color 颜色
 * @param mode 1:叠加 , 0:非叠加(就是带背景色)
 */
/*背景色设置*/

void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t color, uint8_t mode)
{
    const uint8_t *temp;
    uint8_t temp_;
    uint8_t t1, t;
    uint16_t y0 = y;

    /*获得取模软件得到的不同尺寸符号的字节个数*/
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // 得到字体一个字符对应点阵集所占的字节数

    /*获得符号在字库中的偏移*/
    ascii = ascii - ' ';

    /*获取字库*/
    if (size == 12)
        temp = &asc2_1206[ascii][0]; // 调用1206字体
    else if (size == 16)
        temp = &asc2_1608[ascii][0]; // 调用1608字体
    else if (size == 24)
        temp = &asc2_2412[ascii][0]; // 调用2412字体
    else
        return; // 没有的字库

    /*打印字符*/
    for (t = 0; t < csize; t++)
    {
        /*依次获取到字节*/
        temp_ = temp[t];
        /*对每个字节的每一位进行判断*/
        for (t1 = 0; t1 < 8; t1++)
        {
#if IS_FSMC == 1

            if (temp_ & 0x80) // 依次判断首位
                LCD_Draw_Point_FSMC(x, y, color);
            else if (mode == 0) // 带背景的模式
                LCD_Draw_Point_FSMC(x, y, BACKGROUND_COLOR);
#else
            if (temp_ & 0x80) // 依次判断首位
                LCD_Draw_Point(x, y, color);
            else if (mode == 0) // 带背景的模式
                LCD_Draw_Point(x, y, BACKGROUND_COLOR);
#endif
            temp_ <<= 1; // 判断下次
            y++;
            /*如果发现y超界*/ /*y异常*/
            if (y >= lcd.height)
            {
                y = y0; // 让y等于最开始设置的y并且x++
                x++;
                /*根据字符尺寸跳转到后面的字节开始打印*/
                if (size == 24)
                {
                    if (t % 3 == 0)
                    {
                        t += 2;
                    }
                    else if (t % 3 == 1)
                    {
                        t += 1;
                    }
                }
                else if (size == 16 || size == 12)
                {
                    if (!(t % 2))
                        t++;
                }
                /*X异常直接退出整个函数*/
                if (x >= lcd.width)
                    return;
                break;
            }
            /*如果发现写完了1列就退出扫描8个字节的循环,开始下一列*/ /*y正常*/
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                /*X异常直接退出整个函数*/
                if (x >= lcd.width)
                    return;
                break;
            }
        }
    }
}
/**
 * @brief
 *
 * @param x
 * @param y
 * @param ascii
 * @param size
 * @param color
 * @param color_time:颜色数组的循环次数
 * @param mode 1:叠加 , 0:非叠加(就是带背景色)
 */
void LCD_ShowChar_colorful(uint16_t x, uint16_t y, uint8_t ascii, uint8_t size, uint16_t *color, uint8_t color_time, uint8_t mode)
{
    const uint8_t *temp;
    uint8_t temp_;
    uint8_t t1, t;
    uint16_t y0 = y;
    uint32_t color_i = 0;
    /*获得取模软件得到的不同尺寸符号的字节个数*/
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // 得到字体一个字符对应点阵集所占的字节数

    /*获得符号在字库中的偏移*/
    ascii = ascii - ' ';

    /*获取字库*/
    if (size == 12)
        temp = &asc2_1206[ascii][0]; // 调用1206字体
    else if (size == 16)
        temp = &asc2_1608[ascii][0]; // 调用1608字体
    else if (size == 24)
        temp = &asc2_2412[ascii][0]; // 调用2412字体
    else
        return; // 没有的字库

    /*打印字符*/
    for (t = 0; t < csize; t++)
    {
        /*依次获取到字节*/
        temp_ = temp[t];
        /*对每个字节的每一位进行判断*/
        for (t1 = 0; t1 < 8; t1++)
        {
#if IS_FSMC == 1

            if (temp_ & 0x80) // 依次判断首位
                LCD_Draw_Point_FSMC(x, y, color[color_i++]);
            else if (mode == 0) // 带背景的模式
                LCD_Draw_Point_FSMC(x, y, BACKGROUND_COLOR);
#else
            if (temp_ & 0x80) // 依次判断首位
                LCD_Draw_Point(x, y, color[color_i++]);
            else if (mode == 0) // 带背景的模式
                LCD_Draw_Point(x, y, BACKGROUND_COLOR);
#endif
            if (color_i == color_time)
                color_i = 0;
            temp_ <<= 1; // 判断下次
            y++;
            /*如果发现y超界*/ /*y异常*/
            if (y >= lcd.height)
            {
                y = y0; // 让y等于最开始设置的y并且x++
                x++;
                /*根据字符尺寸跳转到后面的字节开始打印*/
                if (size == 24)
                {
                    if (t % 3 == 0)
                    {
                        t += 2;
                    }
                    else if (t % 3 == 1)
                    {
                        t += 1;
                    }
                }
                else if (size == 16 || size == 12)
                {
                    if (!(t % 2))
                        t++;
                }
                /*X异常直接退出整个函数*/
                if (x >= lcd.width)
                    return;
                break;
            }
            /*如果发现写完了1列就退出扫描8个字节的循环,开始下一列*/ /*y正常*/
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                /*X异常直接退出整个函数*/
                if (x >= lcd.width)
                    return;
                break;
            }
        }
    }
}

void LCD_ShowString_colorful(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint16_t *color, uint8_t color_time, uint8_t mode)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
            break; // 退出

        LCD_ShowChar_colorful(x, y, *p, size, color, color_time, mode);
        x += size / 2;
        p++;
    }
}

/**
 * @brief 填充指定颜色
 *
 * @param sx  (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
 * @param sy
 * @param ex
 * @param ey
 * @param color 指定颜色
 */
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;

    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++)
    {
        LCD_set_cursor(sx, i);             // 设置光标位置,变y不变x.
        LCD_write_command(LCD_WIRTE_GRAM); // 开始写入GRAM
        for (j = 0; j < xlen; j++)         // 沿着x轴的方向连续写
            LCD_write_data(color);         // 显示颜色
    }
}
/**
 * @brief 指定区域填充任意颜色,区域大小为:(ex-sx+1)*(ey-sy+1)
 *
 * @param sx
 * @param sy
 * @param ex
 * @param ey
 * @param color_arr
 */
void LCD_fill_color_arr(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color_arr)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    uint32_t m = 0;
    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++)
    {
        LCD_set_cursor(sx, i);             // 设置光标位置,变y不变x.
        LCD_write_command(LCD_WIRTE_GRAM); // 开始写入GRAM
        for (j = 0; j < xlen; j++)         // 沿着x轴的方向连续写
        {
            LCD_write_data(color_arr[m]); // 显示颜色
            m++;
        }
    }
}
/**
 * @brief 显示字符串
 *
 * @param x
 * @param y
 * @param width
 * @param height
 * @param size
 * @param p
 */
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p, uint16_t color, uint8_t mode)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
            break; // 退出

        LCD_ShowChar(x, y, *p, size, color, mode);
        x += size / 2;
        p++;
    }
}

static uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
        result *= m;

    return result;
}
/**
 * @brief 显示数字
 *
 * @param x
 * @param y
 * @param num
 * @param len 数字的位数
 * @param size
 * @param mode
 * @param mode: [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 */
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint8_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size, color, mode & 0X01);
                else
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, color, mode & 0X01);

                continue;
            }
            else
                enshow = 1;
        }

        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, color, mode & 0X01);
    }
}

/**
 * @brief
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 * @param color_time:循环次数
 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *color, uint32_t color_time)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    uint32_t color_i = 0;
    delta_x = x2 - x1; // 计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)
        incx = 1; // 设置单步方向
    else if (delta_x == 0)
        incx = 0; // 垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; // 水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x; // 选取基本增量坐标轴
    else
        distance = delta_y;

    for (t = 0; t <= distance + 1; t++) // 画线输出
    {
        LCD_Draw_Point_FSMC(uRow, uCol, color[color_i++]);
        if (color_i == color_time)
            color_i = 0;
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}
// 画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *color, uint32_t color_time)
{
    LCD_DrawLine(x1, y1, x2, y1, color, color_time);
    LCD_DrawLine(x1, y1, x1, y2, color, color_time);
    LCD_DrawLine(x1, y2, x2, y2, color, color_time);
    LCD_DrawLine(x2, y1, x2, y2, color, color_time);
}
// 在指定位置画一个指定大小的圆
//(x,y):中心点
// r    :半径
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t *color, uint32_t color_time)
{
    int a, b;
    int di;
    uint32_t color_i;
    a = 0;
    b = r;
    di = 3 - (r << 1); // 判断下个点位置的标志

    while (a <= b)
    {
        LCD_Draw_Point_FSMC(x0 + a, y0 - b, color[color_i++]); // 5
        LCD_Draw_Point_FSMC(x0 + b, y0 - a, color[color_i++]); // 0
        LCD_Draw_Point_FSMC(x0 + b, y0 + a, color[color_i++]); // 4
        LCD_Draw_Point_FSMC(x0 + a, y0 + b, color[color_i++]); // 6
        LCD_Draw_Point_FSMC(x0 - a, y0 + b, color[color_i++]); // 1
        LCD_Draw_Point_FSMC(x0 - b, y0 + a, color[color_i++]);
        LCD_Draw_Point_FSMC(x0 - a, y0 - b, color[color_i++]); // 2
        LCD_Draw_Point_FSMC(x0 - b, y0 - a, color[color_i++]); // 7
        a++;
        if (color_i == color_time)
            color_i = 0;
        // 使用Bresenham算法画圆
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
