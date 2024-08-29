/*
 * PCF8591.h
 *
 *  Created on: Aug 28, 2024
 *      Author: Lenovo
 */

#ifndef INC_PCF8591_H_
#define INC_PCF8591_H_

#include "main.h"

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

    CHANNEL0 = 0,
    CHANNEL1 = 1,
    CHANNEL2 = 2,
    CHANNEL3 = 3,
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

uint8_t PCF8591_Read_AD(PCF8591_typedef *struture);
extern uint8_t ReadsData;
void PCF8591_Reads_AD(PCF8591_typedef *struture, uint16_t length);
void PCF8591_DA_OUT(uint8_t DA_num_Out); // True

#endif /* INC_PCF8591_H_ */
