#include "main.h"

volatile int cnt, end;
volatile uint8_t data[4] = {0};
// 等待红外信息,并且返回解析结果
int wait_for_IR(uint8_t *addr)
{
    cnt = -2;
    end = 0;
    data[0] = data[1] = data[2] = data[3] = 0;
}
