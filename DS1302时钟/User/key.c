#include "key.h"
#include "OLED.h"
#include "DS1302.h"
extern uint8_t Mode;
// 按键PB3,PB4
// PB3双击切换模式，单击数减，长摁数一直减少
// PB4单击增加，长摁数一直增加
// 单击：在按一次后一段时间内没有后续按键按下
int8_t key3_while()
{
    uint16_t count = 0;
    int8_t temp = 0;
    if (((GPIOB->IDR >> 3) & 1) == 0)
    {
        Delay_ms(20);
        if (((GPIOB->IDR >> 3) & 1) == 0)
        {
            temp = 1;
            while (((GPIOB->IDR >> 3) & 1) == 0)
                ;
        }
        Delay_ms(20);
        while (count < 200) // 检测后续有没有按键
        {
            Delay_ms(1);
            count++;
            if (((GPIOB->IDR >> 3) & 1) == 0)
            {
                temp = 2;
                break;
            }
        }
        Delay_ms(100);
    }
    return temp;
}
