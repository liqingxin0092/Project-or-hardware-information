#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "gd32f4xx.h"

/* PC1 ADC0_IN11 */
#define BSP_ADC_GPIO_RCU     RCU_GPIOC
#define BSP_ADC_GPIO_PORT    GPIOC
#define BSP_ADC_GPIO_PIN     GPIO_PIN_1
 
/* PC1 ADC0_IN11 */
#define BSP_ADC_RCU         RCU_ADC1
#define BSP_ADC             ADC1
#define BSP_ADC_CHANNEL     ADC_CHANNEL_11

void bsp_adc_init(void);
unsigned int Get_ADC_Value(uint8_t  ADC_CHANNEL_x);

#endif