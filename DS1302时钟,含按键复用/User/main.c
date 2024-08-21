#include "Delay.h"
#include "stm32f10x.h" // Device header
#include "DS1302.h"
#include "OLED.h"
#include "key.h"
#include "string.h"
ds_time_t Time;
ds_time_t Temp;
uint8_t Mode;
uint8_t KeyNum3;
uint8_t KeyNum4;
void TIM_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
}
void TimeChange(const char *time, const char *way)
{
    if (!strcmp(time, "hour"))
    {
        if (!strcmp(way, "--"))
        {
            Temp.hour--;
            if (Temp.hour == -1)
                Temp.hour = 23;
        }
        else if (!strcmp(way, "++"))
        {
            Temp.hour++;
            if (Temp.hour == 24)
                Temp.hour = 0;
        }
    }
    else if (!strcmp(time, "min"))
    {
        if (!strcmp(way, "--"))
        {
            Temp.min--;
            if (Temp.min == -1)
                Temp.min = 59;
        }
        else if (!strcmp(way, "++"))
        {
            Temp.min++;
            if (Temp.min == 60)
                Temp.min = 0;
        }
    }
    else if (!strcmp(time, "sec"))
    {
        if (!strcmp(way, "--"))
        {
            Temp.second--;
            if (Temp.second == -1)
                Temp.second = 59;
        }
        else if (!strcmp(way, "++"))
        {
            Temp.second++;
            if (Temp.second == 60)
                Temp.second = 0;
        }
    }
}
int main(void)
{
    DS1302_Init(); // 初始化DS1302
    OLED_Init();   // 初始化OLED
    TIM_Init();

    SetSecond(59);
    SetMin(59);
    SetHour(13);

    while (1)
    {
        KeyNum3 = key3_while();

        if (KeyNum3 == 2) // 如果是双击就切换模式
        {
            Mode++;
            if (Mode == 4)
                Mode = 0;
            KeyNum3 = 0;
        }
        if (Mode == 0) // 正常显示
        {
            Delay_ms(150);

            Time.second = ReadSeconcd(); // 读秒寄存器
            Time.min = ReadMin();
            Time.hour = ReadHour();

            OLED_Printf(0, 0, OLED_8X16, "%2d:%2d:%2d", Time.hour, Time.min, Time.second);

            OLED_UpdateArea(0, 0, 64, 16);
        }

        // 按键PB3,PB4
        // PB3双击切换模式，单击数减，长摁数一直减少
        // PB4单击增加，长摁数一直增加
        else if (Mode == 1) // 设置时
        {
            Temp = Time;
            if (KeyNum3 == 1)
            {
                TimeChange("hour", "--");
                KeyNum3 = 0;
                Time = Temp;
                SetHour(Time.hour);
                OLED_ShowNum(0, 0, Temp.hour, 2, OLED_8X16);
            }
            uint16_t count = 0;
            if (((GPIOB->IDR >> 4) & 1) == 0)
            {
                Delay_ms(20);
                if (((GPIOB->IDR >> 4) & 1) == 0)
                {
                    TimeChange("hour", "++");
                    Time = Temp;
                    SetHour(Time.hour);
                    OLED_ShowNum(0, 0, Temp.hour, 2, OLED_8X16);
                    OLED_UpdateArea(0, 0, 64, 16);
                    while (((GPIOB->IDR >> 4) & 1) == 0)
                    {
                        count++;
                        Delay_ms(20);
                        if (count > 15)
                        {
                            Delay_ms(20);
                            TimeChange("hour", "++");
                            Time = Temp;
                            SetHour(Time.hour);
                            OLED_ShowNum(0, 0, Temp.hour, 2, OLED_8X16);
                            OLED_UpdateArea(0, 0, 64, 16);
                        }
                    }
                }
                Delay_ms(20);
            }
            OLED_ReverseArea(0, 0, 16, 16);
            OLED_UpdateArea(0, 0, 64, 16);
            Delay_ms(200);
        }

        else if (Mode == 2) // 设置分
        {
            Temp = Time;
            if (KeyNum3 == 1)
            {
                TimeChange("min", "--");
                KeyNum3 = 0;
                Time = Temp;
                SetMin(Time.min);
                OLED_ShowNum(24, 0, Temp.min, 2, OLED_8X16);
            }
            uint16_t count = 0;
            if (((GPIOB->IDR >> 4) & 1) == 0)
            {
                Delay_ms(20);
                if (((GPIOB->IDR >> 4) & 1) == 0)
                {
                    TimeChange("min", "++");
                    Time = Temp;
                    SetMin(Time.min);
                    OLED_ShowNum(24, 0, Temp.min, 2, OLED_8X16);
                    OLED_UpdateArea(0, 0, 64, 16);
                    while (((GPIOB->IDR >> 4) & 1) == 0)
                    {
                        count++;
                        Delay_ms(20);
                        if (count > 15)
                        {
                            Delay_ms(20);
                            TimeChange("min", "++");
                            Time = Temp;
                            SetMin(Time.min);
                            OLED_ShowNum(24, 0, Temp.min, 2, OLED_8X16);
                            OLED_UpdateArea(0, 0, 64, 16);
                        }
                    }
                }
                Delay_ms(20);
            }
            OLED_ReverseArea(24, 0, 16, 16);
            OLED_UpdateArea(0, 0, 64, 16);
            Delay_ms(200);
        }
        else if (Mode == 3) // 设置秒
        {
            Temp = Time;
            if (KeyNum3 == 1)
            {
                TimeChange("sec", "--");
                KeyNum3 = 0;
                Time = Temp;
                SetSecond(Time.second);
                OLED_ShowNum(48, 0, Temp.second, 2, OLED_8X16);
            }
            uint16_t count = 0;
            if (((GPIOB->IDR >> 4) & 1) == 0)
            {
                Delay_ms(20);
                if (((GPIOB->IDR >> 4) & 1) == 0)
                {
                    TimeChange("sec", "++");
                    Time = Temp;
                    SetSecond(Time.second);
                    OLED_ShowNum(48, 0, Temp.second, 2, OLED_8X16);
                    OLED_UpdateArea(0, 0, 64, 16);
                    while (((GPIOB->IDR >> 4) & 1) == 0)
                    {
                        count++;
                        Delay_ms(20);
                        if (count > 15)
                        {
                            Delay_ms(20);
                            TimeChange("sec", "++");
                            Time = Temp;
                            SetSecond(Time.second);
                            OLED_ShowNum(48, 0, Temp.second, 2, OLED_8X16);
                            OLED_UpdateArea(0, 0, 64, 16);
                        }
                    }
                }
                Delay_ms(20);
            }
            OLED_ReverseArea(48, 0, 16, 16);
            OLED_UpdateArea(0, 0, 64, 16);
            Delay_ms(200);
        }
    }
}
