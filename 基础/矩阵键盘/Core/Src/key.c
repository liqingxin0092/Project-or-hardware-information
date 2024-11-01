#include "main.h"
#include "key.h"
#include <string.h>

// 数组下标即为行列
uint8_t KeyNum[5][5];
void KeyScan()
{
    uint8_t i, m;
    for (m = 0; m < 4; m++)
    {
        KEY_PORT->ODR |= 1 << 0;
        KEY_PORT->ODR |= 1 << 1;
        KEY_PORT->ODR |= 1 << 2;
        KEY_PORT->ODR |= 1 << 3;
        KEY_PORT->ODR &= ~(1 << m);
        for (i = 4; i <= 7; i++)
        {
            if (((GPIOA->IDR >> i) & 1) == 0)
            {
                HAL_Delay(20);
                if (((GPIOA->IDR >> i) & 1) == 0)
                {
                    KeyNum[m + 1][i - 3] = 1;
                    while (((GPIOA->IDR >> i) & 1) == 0)
                        ;
                    HAL_Delay(20);
                }
            }
        }
    }
    memset(KeyNum, 0, sizeof(KeyNum));
}
