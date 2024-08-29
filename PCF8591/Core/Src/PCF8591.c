/*
 * PCF8591.c
 *
 *  Created on: Aug 28, 2024
 *      Author: Lenovo
 */
#include "i2c.h"
#include "PCF8591.h"
#include "Delay.h"
#include "OLED.h"
// 初始化,配置命令

extern const uint16_t IIC_Delay_Time;

void PCF8591_Typedef_StructInit(PCF8591_typedef *struture)
{
    struture->command = 0;
    struture->command |= struture->DA_Enable_Flag << 6;
    struture->command |= struture->AD_Mode << 4;
    struture->command |= struture->Auto_Increment_Flag << 2;
    struture->command |= struture->AD_Chanle << 0;
}

uint8_t PCF8591_test;
void PCF8591_Test()
{
    start();

    send(PCF8591_ADDRESS); // 设备地址
    receive_ack();         // 判断从机有没有接收应答

    stop();
}

uint8_t PCF8591_Read_AD(PCF8591_typedef *struture)
{
    uint16_t Data;
    start();

    send(PCF8591_ADDRESS);  // 设备地址
    if (IIC_Delay_Time > 0) // 延时
        Delay_us(IIC_Delay_Time);
    PCF8591_test = receive_ack(); // 是否应答
    OLED_ShowNum(0, 16, PCF8591_test, 4, OLED_8X16);
    OLED_Update();

    send(struture->command); // 发命令
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    PCF8591_test = receive_ack();
    OLED_ShowNum(0, 16, PCF8591_test, 4, OLED_8X16);
    OLED_Update();

    restart();

    send(PCF8591_ADDRESS | 1);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    PCF8591_test = receive_ack();
    OLED_ShowNum(0, 16, PCF8591_test, 4, OLED_8X16);
    OLED_Update();

    Data = receive();
    send_ack(1);

    stop();

    return Data;
}

uint8_t ReadsData;
void PCF8591_Reads_AD(PCF8591_typedef *struture, uint16_t length)
{
    start();

    send(PCF8591_ADDRESS);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    PCF8591_test = receive_ack();
    OLED_ShowNum(0, 16, PCF8591_test, 4, OLED_8X16);
    OLED_Update();

    send(struture->command);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    PCF8591_test = receive_ack();
    OLED_ShowNum(0, 16, PCF8591_test, 4, OLED_8X16);
    OLED_Update();

    restart();

    send(PCF8591_ADDRESS | 1);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    PCF8591_test = receive_ack();
    OLED_ShowNum(0, 16, PCF8591_test, 4, OLED_8X16);
    OLED_Update();

    for (uint16_t i = 0; i < length - 1; i++)
    {
        Delay_ms(100);
        ReadsData = receive();
        send_ack(0);
        OLED_ShowNum(0, 0, ReadsData, 4, OLED_8X16);
        OLED_Update();
    }

    ReadsData = receive();
    send_ack(1);

    stop();
}

// DA OUT
void PCF8591_DA_OUT(uint8_t DA_num_Out)
{
    uint8_t SendData[] = {
        PCF8591_ADDRESS,
        0x40,
        DA_num_Out};
    WriteRegs(SendData, 3);
}