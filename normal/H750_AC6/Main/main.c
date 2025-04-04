#include "bsp.h"
#include "h7_core.h"
#include "test.h"
#include "usart_m.h"

/* 不用的功能注意注释 */


int main()
{
    __asm("bkpt");//ram里用的
    SET_VTOR_ADD(0x24000000);  //向量表放到ITCM里了
    bsp_init();
//    picture_scan_and_display(0,0,240,240,1000);
    freertos_demo();
}


