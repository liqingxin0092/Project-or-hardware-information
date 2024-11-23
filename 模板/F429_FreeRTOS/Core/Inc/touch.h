#ifndef __TOUCH_H
#define __TOUCH_H

#include "main.h"
#include "delay.h"
#include "stdio.h"
#include "LTDC.h"

/*引脚映射*/
// INT  -- PH7
// SCL -- PH6
// SDA  -- PI3
// RST -- PI8

typedef struct
{
    uint16_t x[5];   /* 当前坐标 */
    uint16_t y[5];   /* 电容屏有最多10组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
                                 * x[4],y[4]存储第一次按下时的坐标.*/
} _m_tp_dev;

extern _m_tp_dev tp_dev;        /* 触屏控制器在touch.c里面定义 */

/* IIC读写命令 */
#define GT9XXX_CMD_WR       0X28        /* 写命令 */
#define GT9XXX_CMD_RD       0X29        /* 读命令 */

/* GT9XXX 部分寄存器定义  */
#define GT9XXX_CTRL_REG     0X8040      /* GT9XXX控制寄存器 */
#define GT9XXX_CFGS_REG     0X8047      /* GT9XXX配置起始地址寄存器 */
#define GT9XXX_CHECK_REG    0X80FF      /* GT9XXX校验和寄存器 */
#define GT9XXX_PID_REG      0X8140      /* GT9XXX产品ID寄存器 */

#define GT9XXX_GSTID_REG    0X814E      /* GT9XXX当前检测到的触摸情况 */
#define GT9XXX_TP1_REG      0X8150      /* 第一个触摸点数据地址 */
#define GT9XXX_TP2_REG      0X8158      /* 第二个触摸点数据地址 */
#define GT9XXX_TP3_REG      0X8160      /* 第三个触摸点数据地址 */
#define GT9XXX_TP4_REG      0X8168      /* 第四个触摸点数据地址 */
#define GT9XXX_TP5_REG      0X8170      /* 第五个触摸点数据地址 */
#define GT9XXX_TP6_REG      0X8178      /* 第六个触摸点数据地址 */
#define GT9XXX_TP7_REG      0X8180      /* 第七个触摸点数据地址 */
#define GT9XXX_TP8_REG      0X8188      /* 第八个触摸点数据地址 */
#define GT9XXX_TP9_REG      0X8190      /* 第九个触摸点数据地址 */
#define GT9XXX_TP10_REG     0X8198      /* 第十个触摸点数据地址 */
 
#define TP_PRES_DOWN    0x8000      /* 触屏被按下 */
#define TP_CATH_PRES    0x4000      /* 有按键按下了 */
#define CT_MAX_TOUCH    5          /* 电容屏支持的点数,固定为5点 */

#define SCL __BIT(GPIOH_ODR_Addr,6)
#define SDA_OUT __BIT(GPIOI_ODR_Addr,3) //输出用
#define SDA_IN __BIT(GPIOI_IDR_Addr,3)//输入用

void Touch_init(void);
uint8_t gt9xxx_scan(uint8_t mode);

#endif
