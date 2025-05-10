
#include "bsp_adc.h"


void bsp_adc_init(void)
{
	//使能引脚时钟
	rcu_periph_clock_enable(BSP_ADC_GPIO_RCU);      
	//使能ADC时钟
	rcu_periph_clock_enable(BSP_ADC_RCU);
    
	//配置ADC时钟
	adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
	
	//配置引脚为模拟浮空输入模式
	gpio_mode_set(BSP_ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, BSP_ADC_GPIO_PIN); 
	
	//使能扫描模式
	adc_special_function_config(BSP_ADC, ADC_SCAN_MODE, ENABLE);
	
	//配置ADC为独立模式      
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	
	//数据右对齐     
	adc_data_alignment_config(BSP_ADC, ADC_DATAALIGN_RIGHT);
	
	//ADC0设置为12位分辨率               
	adc_resolution_config(BSP_ADC, ADC_RESOLUTION_12B);
	
	//ADC0设置为规则组  一共使用 1 个通道
	adc_channel_length_config(BSP_ADC,ADC_ROUTINE_CHANNEL, 1);
		
	//ADC0使能
	adc_enable(BSP_ADC);
	
	//ADC外部触发禁用, 即只能使用软件触发
	adc_external_trigger_config(BSP_ADC,ADC_ROUTINE_CHANNEL,EXTERNAL_TRIGGER_DISABLE);
	
	//使能软件触发
	adc_software_trigger_enable(BSP_ADC, ADC_ROUTINE_CHANNEL);
	
	//开启ADC自校准
	adc_calibration_enable(BSP_ADC);
}

/**********************************************************
 * 函 数 名 称：Get_ADC_Value
 * 函 数 功 能：读取ADC值
 * 传 入 参 数：ADC_CHANNEL_x=要采集的通道
 * 函 数 返 回：测量到的值
 * 作       者：LC
 * 备       注：默认采样周期为15个ADC采样时间
**********************************************************/
unsigned int Get_ADC_Value(uint8_t  ADC_CHANNEL_x)
{
    unsigned int adc_value = 0;
    //设置采集通道
    adc_routine_channel_config(BSP_ADC, 0, ADC_CHANNEL_x, ADC_SAMPLETIME_15);
    //开始软件转换
    adc_software_trigger_enable(BSP_ADC, ADC_ROUTINE_CHANNEL);
    //等待 ADC0 采样完成 
    while( adc_flag_get(BSP_ADC, ADC_FLAG_EOC) == RESET ) 
    {
        
    }
    //读取采样值
    adc_value = adc_routine_data_read(BSP_ADC);
    //返回采样值
    return adc_value;
}
