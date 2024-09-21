#include "Delay.h"
#include "DS18b20.h"

const uint8_t DS18b20_address_code[8] = {0x28, 0x5a, 0xfc, 0x40, 0x00, 0x00, 0x00, 0x98};

void (*Write_API)(GPIO_TypeDef *PORT, short unsigned int Pin, GPIO_PinState state);
GPIO_PinState (*Read_API)(GPIO_TypeDef *PORT, short unsigned int Pin);

void single_bus_IO_Init()
{
    Write_API = HAL_GPIO_WritePin;
    Read_API = HAL_GPIO_ReadPin;
    Write_API(DQ_Port, DQ_Pin, 1);
}

// 返回1说明未应答,0表示应答.
status single_bus_start()
{
    Write_API(DQ_Port, DQ_Pin, 0);
    Delay_us(480);
    Write_API(DQ_Port, DQ_Pin, 1);

    Delay_us(70);
    uint8_t temp = Read_API(DQ_Port, DQ_Pin);
    Delay_us(410); // 走完时序,时序要求最小时间

    return temp;
}

void single_bus_send_bit(uint8_t bit)
{
    if (bit == 1)
    {
        Write_API(DQ_Port, DQ_Pin, 0);
        Delay_us(10);
        Write_API(DQ_Port, DQ_Pin, 1);
        Delay_us(55);
    }
    else if (bit == 0)
    {
        Write_API(DQ_Port, DQ_Pin, 0);
        Delay_us(70);
        Write_API(DQ_Port, DQ_Pin, 1);
    }
}

uint8_t single_bus_receive_bit()
{
    Write_API(DQ_Port, DQ_Pin, 0);
    Delay_us(5);
    Write_API(DQ_Port, DQ_Pin, 1);
    Delay_us(5);
    uint8_t temp = Read_API(DQ_Port, DQ_Pin);
    Delay_us(51);

    return temp;
}

void single_bus_send_byte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        single_bus_send_bit((byte >> i) & 1);
    }
}

uint8_t single_bus_receive_byte()
{
    uint8_t temp = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (single_bus_receive_bit())
        {
            temp |= 1 << i;
        }
    }
    return temp;
}

void DS18B20_convert_tem()
{
    single_bus_start();

    single_bus_send_byte(SKIP_ROM);

    single_bus_send_byte(TEMPERATURE_CONVERT);
    while (!single_bus_receive_bit())
        ;
}

float DS18B20_read_tem()
{
    single_bus_start();
    single_bus_send_byte(SKIP_ROM);
    single_bus_send_byte(READ_RAM);
    // 这里只读其中的前两个字节.
    uint8_t lsb = single_bus_receive_byte();
    uint8_t msb = single_bus_receive_byte();
    // 总线复位结束读取.
    Write_API(DQ_Port, DQ_Pin, 0);
    Delay_us(482);
    Write_API(DQ_Port, DQ_Pin, 1);

    uint16_t temp = (msb << 8) | lsb;
    int16_t temp_ = *((int16_t *)&temp);

    return temp_ / 16.0;
}

float DS18B20_convert_and_read_tem()
{
    DS18B20_convert_tem();
    return DS18B20_read_tem();
}

void DS18B20_read_rom(uint8_t *p)
{
    single_bus_start();
    single_bus_send_byte(READ_ROM);
    for (uint8_t i = 0; i < 8; i++)
    {
        p[i] = single_bus_receive_byte();
    }
}

void DS18B20_match_rom(const uint8_t *p)
{
    single_bus_start();
    single_bus_send_byte(MATCH_ROM);
    for (uint8_t i = 0; i < 8; i++)
    {
        single_bus_send_byte(p[i]);
    }
}

void _DS18B20_convert_tem(const uint8_t *address_code)
{
    single_bus_start();
    DS18B20_match_rom(address_code);
    single_bus_send_byte(TEMPERATURE_CONVERT);
    while (!single_bus_receive_bit())
        ;
}

// 可以选择读多少,返回温度值.
//  count参数:2~8
float _DS18B20_read_ram(const uint8_t *address_code, uint8_t count, uint8_t *p)
{
    single_bus_start();
    DS18B20_match_rom(address_code);
    single_bus_send_byte(READ_RAM);
    // 这里只读其中的前两个字节.
    uint8_t lsb = single_bus_receive_byte();
    uint8_t msb = single_bus_receive_byte();
    if (p)
    {
        p[0] = lsb;
        p[1] = msb;
        for (uint8_t i = 0; i < count - 2; i++)
        {
            p[i + 2] = single_bus_receive_byte();
        }
    }
    // 总线复位结束读取.
    Write_API(DQ_Port, DQ_Pin, 0);
    Delay_us(482);
    Write_API(DQ_Port, DQ_Pin, 1);

    uint16_t temp = (msb << 8) | lsb;
    int16_t temp_ = *((int16_t *)&temp);
    return temp_ / 16.0;
}

float _DS18B20_convert_and_read_tem(const uint8_t *address_code)
{
    _DS18B20_convert_tem(address_code);
    return _DS18B20_read_ram(address_code, 2, NULL);
}

// 写入,警报上限值,警报下限值(但是没做芯片自带的警报功能...,在MCU层面上实现吧),写精度.
// 两个温度不带小数,accurary格式固定,参数为宏.
void _DS18B20_write_ram(const uint8_t *address_code, uint8_t High_Tem, uint8_t Low_Tem, uint8_t accuracy)
{
    single_bus_start();
    DS18B20_match_rom(address_code);
    single_bus_send_byte(WRITE_RAM);

    single_bus_send_byte(High_Tem);
    single_bus_send_byte(Low_Tem);
    single_bus_send_byte(accuracy);
}

// 拷贝
void _DS18B20_copy_eeprom(const uint8_t *address_code)
{
    single_bus_start();
    DS18B20_match_rom(address_code);
    single_bus_send_byte(COPY_EEPROM);
    while (!single_bus_receive_bit())
        ;
}

// 召回
void _DS18B20_call_eeprom(const uint8_t *address_code)
{
    single_bus_start();
    DS18B20_match_rom(address_code);
    single_bus_send_byte(CALL_EEPROM);
    while (!single_bus_receive_bit())
        ;
}