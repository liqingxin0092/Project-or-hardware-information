/*
 * PCF8591.h
 *
 *  Created on: Aug 28, 2024
 *      Author: Lenovo
 */

#ifndef INC_PCF8591_H_
#define INC_PCF8591_H_

#include "main.h"

// 注意: 使用此模块IO口主频务必降低,供电务必得稳定
#define PCF8591_ADDRESS 0x90

enum PCF8591_pram_data
{
    COMMAND = 0,

    DA_ENABLE = 1,
    DA_DISENABLE = 0,

    FOUR_SINGLE_ENDED_INPUTS = 0,
    THREE_DIFFERENTIAL_INPUTS = 1,
    SINGLE_ENDED_AND_DIFFERENTIAL_MIXED = 2,
    TWO_DIFFERENTIAL_INPUTS = 3,

    AUTO_INCREMENT = 1,
    NOT_AUTO_INCREMENT = 0,

    CHANNEL0 = 0, // 光敏电阻
    CHANNEL1 = 1, // 热敏电阻
    CHANNEL2 = 2, // 自行连接
    CHANNEL3 = 3, // 电位器
};

typedef struct PCF8591
{
    uint8_t command;

    uint8_t DA_Enable_Flag;
    uint8_t AD_Mode;
    uint8_t Auto_Increment_Flag;
    uint8_t AD_Chanle;

} PCF8591_typedef;

void PCF8591_Test(void);
void PCF8591_Typedef_StructInit(PCF8591_typedef *struture);

// 读一个通道
uint8_t PCF8591_Read_AD(PCF8591_typedef *struture);

// 读四个通道
// 结构体务必: 设置为通道自增模式,从通道0开始,并且使能,否则数据会乱流
extern uint8_t ReadsData[4]; // 数组内容依次是: 通道0,通道1,通道2,通道3
void PCF8591_Reads_AD(PCF8591_typedef *struture);

// DA输出
void PCF8591_DA_OUT(uint8_t DA_num_Out); // True

#endif /* INC_PCF8591_H_ */
