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

// 读一个通道
uint8_t PCF8591_Read_AD(PCF8591_typedef *struture)
{
    uint16_t Data;
    start();

    send(PCF8591_ADDRESS);  // 设备地址
    if (IIC_Delay_Time > 0) // 延时
        Delay_us(IIC_Delay_Time);
    if (receive_ack() == 1)
        return;

    send(struture->command); // 发命令
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    if (receive_ack() == 1)
        return;

    restart();

    send(PCF8591_ADDRESS | 1);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    if (receive_ack() == 1)
        return;

    receive(); // 空读
    send_ack(0);

    Data = receive();
    send_ack(1);

    stop();

    return Data;
}

// 读四个通道
uint8_t ReadsData[4];
void PCF8591_Reads_AD(PCF8591_typedef *struture)
{
    start();

    send(PCF8591_ADDRESS);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    receive_ack();

    send(struture->command);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    receive_ack();

    stop();
    start();

    send(PCF8591_ADDRESS | 1);
    if (IIC_Delay_Time > 0)
        Delay_us(IIC_Delay_Time);
    receive_ack();

    receive();
    send_ack(0);

    ReadsData[0] = receive();
    send_ack(0);

    ReadsData[1] = receive();
    send_ack(0);

    ReadsData[2] = receive();
    send_ack(0);

    ReadsData[3] = receive();
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
