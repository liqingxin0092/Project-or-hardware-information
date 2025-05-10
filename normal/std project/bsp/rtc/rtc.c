#include "rtc.h"
#include "bsp_uart.h"

void bsp_rtc_init(void)
{
	//使能电源管理时钟
	rcu_periph_clock_enable(RCU_PMU);
		
	//启用对备份域中寄存器的写访问
	pmu_backup_write_enable();
	

     
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);    
	rcu_osci_on(RCU_LXTAL);   //打开振荡器 32768 Hz
	//等待振荡器32.768 KHz稳定的标志设置或振荡器启动超时
        
	rcu_osci_stab_wait(RCU_LXTAL);
		
	//配置RTC时钟源选择LXTAL=32.768 KHz晶振
	
	
	//开启RTC时钟
	rcu_periph_clock_enable(RCU_RTC);
	//等到时钟同步后，再进行寄存器更新
	rtc_register_sync_wait();
		
	//获取RTC时钟项选择
	//获取备份域控制寄存器（RCU_BDCTL）
	//的第8第9位状态
	//RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);
	
	//如果RTC_BKP0 值为 0x1234，说明已经上电过
	if( RTC_BKP0 == 4 )
	{
	
	}
	else  //备份寄存器0 如果不为4,说明是第一次上电
	{
			RTC_BKP0 = 4;//设置为0X1234，表示上电过了
			//配置RTC时间
			RtcTimeConfig(0x25,0x3,0x27,0x4,0x17,0x33,0x30);
	}
}

/***********************************
 * 函数名  ：RtcTimeConfig
 * 函数功能：设置日期时间
 ***********************************/
void RtcTimeConfig(uint8_t year, uint8_t month, uint8_t date, uint8_t week, \
uint8_t hour, uint8_t minute, uint8_t second)
{
    rtc_parameter_struct   rtc_initpara;
    rtc_initpara.factor_asyn = 0x7F;        //RTC异步预分频值:0x0 ~ 0x7F
    rtc_initpara.factor_syn  = 0xFF;        //RTC同步预分频值:0x0 - 0x7FFF
    rtc_initpara.year        = year;        //设置年份
    rtc_initpara.month       = month;       //设置月份  
    rtc_initpara.date        = date;        //设置日期
    rtc_initpara.day_of_week = week;        //设置星期
    rtc_initpara.hour        = hour;        //设置时
    rtc_initpara.minute      = minute;      //设置分钟  
    rtc_initpara.second      = second;      //设置秒
    rtc_initpara.display_format = RTC_24HOUR;//24小时制    
     //    rtc_initpara.am_pm = RTC_PM;//午后  //12小时制才使用到 
    
	rtc_init(&rtc_initpara);// RTC当前时间配置
}

/**********************************************************
 * 函 数 名 称：BcdToDecimal
 * 函 数 功 能：BCD转10进制
 * 传 入 参 数：bcd = BCD码
 * 函 数 返 回：转换完成的10进制
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
int BcdToDecimal(int bcd) 
{
	int decimal = 0;
	int temp = 1;
	int number = 0;
			
	while(bcd > 0) 
	{
		number = bcd % 16;
		decimal += number * temp;
		temp *= 10;
		bcd /= 16;
	} 
		return decimal;
}

rtc_parameter_struct   rtc_initpara_time;
//注意这函数用了static ,用起来注意点.
void RtcShowTime(void)
{
        //获取RTC时间信息
        rtc_current_time_get(&rtc_initpara_time); 
        static uint8_t last_temp=0;
    
        if(rtc_initpara_time.second!=last_temp)
        {
            //将获取到的RTC时间从BCD码，转为10进制再通过串口输出
            printf("Current time: %d:%d:%d\n\r", \
            BcdToDecimal(rtc_initpara_time.hour),  
            BcdToDecimal(rtc_initpara_time.minute), 
            BcdToDecimal(rtc_initpara_time.second));
                    
            printf("%d-%d-%d\n\r", \
            BcdToDecimal(rtc_initpara_time.year), 
            BcdToDecimal(rtc_initpara_time.month), 
            BcdToDecimal(rtc_initpara_time.date));
        }                 ;
        last_temp= rtc_initpara_time.second;
}