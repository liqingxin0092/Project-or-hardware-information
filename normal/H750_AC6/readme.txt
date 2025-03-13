//注意:cubemx仅供参考,不要用它生成文件!!!!!!!!

//注：T表示已经实现

移植了

freertos    //T
usmart      //T
QSPI FLASH  //T
LCD         //T
sys文件     //T
MPU,cache   //T

SD卡        //T
文件系统    //T

SD卡 DMA
LCD 改善

资源分配情况:

串口1:调试printf,USMART  //T
串口4:MODBUS             //T

定时器2:微秒计时.soft_timer. 32bit计数值 ,16 bit分频.APB1,120MB. //T
定时器3:freeRTOS的计时,有时间再看看那个是干嘛的 //T
定时器4:USMART扫描.      //T

/*IO分配情况*/
//A组
//0,1,2,3:test_led. 9,10:usart1.  11,12:buffer_key
//B组
//2,6:qspi.
//C组
//6,7:test_超声波.  10,11:modbus_uart4  !!!!8,9,10,11,12:sdmmc1. 13:bsp_led
//D组
//2:sdmmc1. 11,12,13:qspi   15:lcd_spi4_clk
//E组
//2:qspi. 11,12,14,15:lcd_spi4

