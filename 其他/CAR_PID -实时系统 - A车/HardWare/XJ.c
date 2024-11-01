#include "XJ.h"

void XJ_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置循迹传感器引脚为输入模式
    GPIO_InitStruct.Pin = TRACK_SENSOR_1_PIN | TRACK_SENSOR_2_PIN | TRACK_SENSOR_3_PIN | TRACK_SENSOR_4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 根据实际传感器情况可能需要调整
    HAL_GPIO_Init(TRACK_SENSOR_GPIO_PORT, &GPIO_InitStruct);
}

uint8_t XJ_Read(void)
{
    uint8_t sensor_state = 0;

    // 读取四个循迹传感器的状态
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_1_PIN) << 0);
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_2_PIN) << 1);
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_3_PIN) << 2);
    sensor_state |= (HAL_GPIO_ReadPin(TRACK_SENSOR_GPIO_PORT, TRACK_SENSOR_4_PIN) << 3);

    return sensor_state;
}
