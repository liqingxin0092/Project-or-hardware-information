#ifndef __DS1302_H
#define __DS1302_H

// 选择对应时钟及引脚
#define DS1302_RCC RCC_APB2ENR_IOPBEN
#define GPIO_DS1302 GPIOB
#define DA_Pin 8
#define CLK_Pin 7
#define RST_Pin 9

// DS1302:PB9~RST~50MHz推挽，PB8~DA~50MHz推挽，需要时候输入，PB7~CLK~50MHz推挽
// OLED:PB5 ~ SDA上拉开漏, PB6 ~ SCL上拉开漏
// 按键：PB3上拉输入 ,PB4上拉输入
#define GPIO_Init()                                              \
    do                                                           \
    {                                                            \
        GPIOB->CRL = 0x37788444;                           \
        GPIO_DS1302->CRH = 0x44444433;                           \
        GPIOB->ODR |= (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6); \
        AFIO->MAPR |= 2 << 24;                                   \
    } while (0)
// CRL   3 7 7 8 8 4 4 4
//       7 6 5 4 3 2 1 0
// CRH   4  4  4  4  4  4  3 3
//       15 14 13 12 11 10 9 8
#define OUTPUT(b, c)                       \
    do                                     \
    {                                      \
        if (c == 1)                        \
            GPIO_DS1302->ODR |= 1 << b;    \
        else                               \
            GPIO_DS1302->ODR &= ~(1 << b); \
    } while (0)

#define SEC_W 0x80
#define SEC_R 0x81
#define MIN_W 0x82
#define MIN_R 0x83
#define HOUR_W 0x84
#define HOUR_R 0x85
#define DAY_W 0x86
#define DAY_R 0x87
#define MONTH_W 0x88
#define MONTH_R 0x89
#define WEEK_W 0x8A
#define WEEK_R 0x8B
#define YEAR_W 0x8C
#define YEAR_R 0x8D
#define WP_W 0x8E
#define WP_R 0x8F
#define MIX_W 0x90
#define MIX_R 0x91

typedef struct
{
    int8_t second;
    int8_t min;
    int8_t hour;
    int8_t day;
    int8_t month;
    int8_t week;
    int8_t year;
} ds_time_t;

void DS1302_Init(void);

void WriteEnable(void);
void WriteDisable(void);

void WriteByte(uint8_t command, uint8_t data);
uint8_t ReadByte(uint8_t command);

void SetSecond(uint8_t sec);
uint16_t ReadSeconcd(void);

void SetMin(uint8_t min);
uint8_t ReadMin(void);
void SetHour(uint8_t hour);
uint8_t ReadHour(void);

#endif
