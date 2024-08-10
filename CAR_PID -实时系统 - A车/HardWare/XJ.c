#include "XJ.h"

void XJ_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // ʹ��GPIOAʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // ����ѭ������������Ϊ����ģʽ
    GPIO_InitStruct.Pin = TRACK_SENSOR_1_PIN | TRACK_SENSOR_2_PIN | TRACK_SENSOR_3_PIN | TRACK_SENSOR_4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // ����ʵ�ʴ��������������Ҫ����
    HAL_GPIO_Init(TRACK_SENSOR_GPIO_PORT, &GPIO_InitStruct);
}

uint8_t XJ_Read(void)
{
    uint8_t sensor_state = 0;

    // ��ȡ�ĸ�ѭ����������״̬
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_1_PIN) << 0);
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_2_PIN) << 1);
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_3_PIN) << 2);
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_4_PIN) << 3);

    return sensor_state;
}
