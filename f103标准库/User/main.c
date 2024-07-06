#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "RTC.h"

int main(void)
{
    RTC_Init(GetTimeStamp(3, 0, 0, 2024, 6, 23), 1, 3);
	while (1)
    {
       CurrentTime= GetRTC_CNT();
        CurrentTimeStruct=*localtime(&CurrentTime);
    }
	
}
