#include "rtc.h"
#include "bsp_uart.h"

void bsp_rtc_init(void)
{
	//ʹ�ܵ�Դ����ʱ��
	rcu_periph_clock_enable(RCU_PMU);
		
	//���öԱ������мĴ�����д����
	pmu_backup_write_enable();
	

     
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);    
	rcu_osci_on(RCU_LXTAL);   //������ 32768 Hz
	//�ȴ�����32.768 KHz�ȶ��ı�־���û�����������ʱ
        
	rcu_osci_stab_wait(RCU_LXTAL);
		
	//����RTCʱ��Դѡ��LXTAL=32.768 KHz����
	
	
	//����RTCʱ��
	rcu_periph_clock_enable(RCU_RTC);
	//�ȵ�ʱ��ͬ�����ٽ��мĴ�������
	rtc_register_sync_wait();
		
	//��ȡRTCʱ����ѡ��
	//��ȡ��������ƼĴ�����RCU_BDCTL��
	//�ĵ�8��9λ״̬
	//RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);
	
	//���RTC_BKP0 ֵΪ 0x1234��˵���Ѿ��ϵ��
	if( RTC_BKP0 == 4 )
	{
	
	}
	else  //���ݼĴ���0 �����Ϊ4,˵���ǵ�һ���ϵ�
	{
			RTC_BKP0 = 4;//����Ϊ0X1234����ʾ�ϵ����
			//����RTCʱ��
			RtcTimeConfig(0x25,0x3,0x27,0x4,0x17,0x33,0x30);
	}
}

/***********************************
 * ������  ��RtcTimeConfig
 * �������ܣ���������ʱ��
 ***********************************/
void RtcTimeConfig(uint8_t year, uint8_t month, uint8_t date, uint8_t week, \
uint8_t hour, uint8_t minute, uint8_t second)
{
    rtc_parameter_struct   rtc_initpara;
    rtc_initpara.factor_asyn = 0x7F;        //RTC�첽Ԥ��Ƶֵ:0x0 ~ 0x7F
    rtc_initpara.factor_syn  = 0xFF;        //RTCͬ��Ԥ��Ƶֵ:0x0 - 0x7FFF
    rtc_initpara.year        = year;        //�������
    rtc_initpara.month       = month;       //�����·�  
    rtc_initpara.date        = date;        //��������
    rtc_initpara.day_of_week = week;        //��������
    rtc_initpara.hour        = hour;        //����ʱ
    rtc_initpara.minute      = minute;      //���÷���  
    rtc_initpara.second      = second;      //������
    rtc_initpara.display_format = RTC_24HOUR;//24Сʱ��    
     //    rtc_initpara.am_pm = RTC_PM;//���  //12Сʱ�Ʋ�ʹ�õ� 
    
	rtc_init(&rtc_initpara);// RTC��ǰʱ������
}

/**********************************************************
 * �� �� �� �ƣ�BcdToDecimal
 * �� �� �� �ܣ�BCDת10����
 * �� �� �� ����bcd = BCD��
 * �� �� �� �أ�ת����ɵ�10����
 * ��       �ߣ�LCKFB
 * ��       ע����
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
//ע���⺯������static ,������ע���.
void RtcShowTime(void)
{
        //��ȡRTCʱ����Ϣ
        rtc_current_time_get(&rtc_initpara_time); 
        static uint8_t last_temp=0;
    
        if(rtc_initpara_time.second!=last_temp)
        {
            //����ȡ����RTCʱ���BCD�룬תΪ10������ͨ���������
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