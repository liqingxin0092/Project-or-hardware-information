#include "bsp.h"
#include "h7_core.h"
#include "test.h"
#include "usart_m.h"

/* ���õĹ���ע��ע�� */


int main()
{
    __asm("bkpt");//ram���õ�
    SET_VTOR_ADD(0x24000000);  //������ŵ�ITCM����
    bsp_init();
//    picture_scan_and_display(0,0,240,240,1000);
    freertos_demo();
}


