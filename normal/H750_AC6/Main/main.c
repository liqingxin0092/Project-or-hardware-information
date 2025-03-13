#include "bsp.h"
#include "h7_core.h"
#include "test.h"


/*不用的功能注意注释*/
int main()
{
    set_votor(0);  //向量表放到ITCM里了
    bsp_init();
    link_list_init();
    
    test_addition_of_polynomials();
    
//    picture_scan_and_display(0,0,240,240,1000);
    freertos_demo();
}


