
#include "bsp_adc.h"


void bsp_adc_init(void)
{
	//ʹ������ʱ��
	rcu_periph_clock_enable(BSP_ADC_GPIO_RCU);      
	//ʹ��ADCʱ��
	rcu_periph_clock_enable(BSP_ADC_RCU);
    
	//����ADCʱ��
	adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
	
	//��������Ϊģ�⸡������ģʽ
	gpio_mode_set(BSP_ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, BSP_ADC_GPIO_PIN); 
	
	//ʹ��ɨ��ģʽ
	adc_special_function_config(BSP_ADC, ADC_SCAN_MODE, ENABLE);
	
	//����ADCΪ����ģʽ      
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	
	//�����Ҷ���     
	adc_data_alignment_config(BSP_ADC, ADC_DATAALIGN_RIGHT);
	
	//ADC0����Ϊ12λ�ֱ���               
	adc_resolution_config(BSP_ADC, ADC_RESOLUTION_12B);
	
	//ADC0����Ϊ������  һ��ʹ�� 1 ��ͨ��
	adc_channel_length_config(BSP_ADC,ADC_ROUTINE_CHANNEL, 1);
		
	//ADC0ʹ��
	adc_enable(BSP_ADC);
	
	//ADC�ⲿ��������, ��ֻ��ʹ���������
	adc_external_trigger_config(BSP_ADC,ADC_ROUTINE_CHANNEL,EXTERNAL_TRIGGER_DISABLE);
	
	//ʹ���������
	adc_software_trigger_enable(BSP_ADC, ADC_ROUTINE_CHANNEL);
	
	//����ADC��У׼
	adc_calibration_enable(BSP_ADC);
}

/**********************************************************
 * �� �� �� �ƣ�Get_ADC_Value
 * �� �� �� �ܣ���ȡADCֵ
 * �� �� �� ����ADC_CHANNEL_x=Ҫ�ɼ���ͨ��
 * �� �� �� �أ���������ֵ
 * ��       �ߣ�LC
 * ��       ע��Ĭ�ϲ�������Ϊ15��ADC����ʱ��
**********************************************************/
unsigned int Get_ADC_Value(uint8_t  ADC_CHANNEL_x)
{
    unsigned int adc_value = 0;
    //���òɼ�ͨ��
    adc_routine_channel_config(BSP_ADC, 0, ADC_CHANNEL_x, ADC_SAMPLETIME_15);
    //��ʼ���ת��
    adc_software_trigger_enable(BSP_ADC, ADC_ROUTINE_CHANNEL);
    //�ȴ� ADC0 ������� 
    while( adc_flag_get(BSP_ADC, ADC_FLAG_EOC) == RESET ) 
    {
        
    }
    //��ȡ����ֵ
    adc_value = adc_routine_data_read(BSP_ADC);
    //���ز���ֵ
    return adc_value;
}
