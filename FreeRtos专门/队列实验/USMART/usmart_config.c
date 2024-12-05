#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* 用户配置区
 * 这下面要包含所用到的函数所申明的头文件(用户自己添加)
 */

#include "delay.h"

/*这里声明要用到的函数*/
extern void write_addr(uint32_t addr, uint32_t val); //改某个地方的数
extern uint32_t read_addr(uint32_t addr);            //读某个地方的数
extern void LCD_Clear(uint16_t color);
extern void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint8_t mode);
extern void GBK_Show_StrMid(uint16_t x, uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color, uint8_t *str);
extern void set_LED1(uint8_t state);
extern void set_LED2(uint8_t state);
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* 如果使能了读写操作 */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr, uint32_t val)",
#endif
    (void *)delay_ms, "void delay_ms(uint16_t nms)",
    (void *)delay_us, "void delay_us(uint32_t nus)",
    (void *)LCD_Clear,"void LCD_Clear(uint16_t color)",
    (void *)GBK_Show_StrMid,"void GBK_Show_StrMid(uint16_t x, uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color, uint8_t *str)",
    (void *)LCD_ShowString,"void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint8_t mode)",
    (void *)set_LED1,"void set_LED1(uint8_t state)",
    (void *)set_LED2,"void set_LED2(uint8_t state)"
};

/******************************************************************************************/

/* 函数控制管理器初始化
 * 得到各个受控函数的名字
 * 得到函数总数量
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* 函数数量 */
    0,      /* 参数数量 */
    0,      /* 函数ID */
    1,      /* 参数显示类型,0,10进制;1,16进制 */
    0,      /* 参数类型.bitx:,0,数字;1,字符串 */
    0,      /* 每个参数的长度暂存表,需要MAX_PARM个0初始化 */
    0,      /* 函数的参数,需要PARM_LEN个0初始化 */
};



















