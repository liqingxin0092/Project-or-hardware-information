#include "AT24C128.h"
#include "main.h"
#include "i2c.h"
#include "Delay.h"
//example:
//  uint8_t test[4] = {0x20,0x04,0x02,0x25};
//  AT24C128_Write(0xa0, 0, 0, test, 4);
//  AT24C128_Read(0xa0, 0,0, 4);


// 写数据：设备地址(1010+A2+A1+A0+读写位)+字节地址+页地址+数据
uint8_t AT24C128_test;
void AT24C128_Test(uint8_t DeviceAddress)
{
    start();
    send(DeviceAddress);           // 发送设备地址
    AT24C128_test = receive_ack(); // 判断从机有没有接收应答，为了拼接时序
    stop();
}

//  PageAddress范围0~255 ,ByteAddress范围0~63
void AT24C128_Write(uint8_t DeviceAddress, uint8_t PageAddress, uint8_t ByteAddress, uint8_t *ptr, uint8_t lenth)
{
    CLEAR_BIT(AT24C128_WP_PORT->ODR, AT24C128_WP_Pin); // 解除写保护

    start();

    send(DeviceAddress); // 发送设备地址
    receive_ack();       // 判断从机有没有接收应答，为了拼接时序

    send(PageAddress >> 2); // 发送字节地址
    receive_ack();          // 判断从机有没有接收应答，为了拼接时序

    send(ByteAddress | ((PageAddress & 3) << 6)); // 发送字节地址
    receive_ack();                                // 判断从机有没有接收应答，为了拼接时序

    uint8_t i;
    for (i = 0; i < lenth; i++)
    {
        send(ptr[i]); // 发送字节地址
       receive_ack();
            
        // 判断从机有没有接收应答，为了拼接时序
    }
    stop();

    Delay_ms(50);
    SET_BIT(AT24C128_WP_PORT->ODR, AT24C128_WP_Pin); // 写保护使能
}

uint8_t AT24C128ReceiveData[64];
void AT24C128_Read(uint8_t DeviceAddress, uint8_t PageAddress, uint8_t ByteAddress, uint8_t lenth)
{

    uint8_t i;
    start();

    send(DeviceAddress); // 发送设备地址
    receive_ack();       // 判断从机有没有接收应答，为了拼接时序

    send(PageAddress >> 2); // 发送字节地址
    receive_ack();          // 判断从机有没有接收应答，为了拼接时序

    send(ByteAddress | ((PageAddress & 3) << 6)); // 发送字节地址
    receive_ack();                                // 判断从机有没有接收应答，为了拼接时序

    restart();
    send(DeviceAddress | 1); // 意思是要读了
    receive_ack();
    for (i = 0; i < lenth - 1; i++)
    {
        *(AT24C128ReceiveData + i) = receive();
        send_ack(0); // 应答
    }
    *(AT24C128ReceiveData + i) = receive();
    send_ack(1); // 非应答

    stop();
}
