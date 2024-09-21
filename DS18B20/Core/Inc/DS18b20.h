/*
 * DS18b20.h
 * 写程序时,我买的DS18B20,读取ROM为 28 5a fc 40 00 00 00,CRC校验码:98 (均为HEX格式)
 *  Created on: Sep 17, 2024
 *      Author: Lenovo
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include "main.h"

typedef uint8_t status;

extern const uint8_t DS18b20_address_code[8];

#define DQ_Port GPIOE
#define DQ_Pin GPIO_PIN_7

#define SKIP_ROM 0xCC
#define TEMPERATURE_CONVERT 0x44
#define READ_RAM 0xbe
#define READ_ROM 0x33
#define MATCH_ROM 0x55
#define WRITE_RAM 0x4e
#define COPY_EEPROM 0x48
#define CALL_EEPROM 0xb8

#define ACCURACY_9 0x1f  // 93.75ms
#define ACCURACY_10 0x3f // 187.5ms
#define ACCURACY_11 0x5f // 375ms
#define ACCURACY_12 0x7f // 750ms

/*时序层*/
void single_bus_IO_Init(void);

status single_bus_start(void);
void single_bus_send_bit(uint8_t bit);
uint8_t single_bus_receive_bit(void);
void single_bus_send_byte(uint8_t byte);
uint8_t single_bus_receive_byte(void);

/*应用层*/
void DS18B20_convert_tem(void);
float DS18B20_read_tem(void);
float DS18B20_convert_and_read_tem(void);
// 读设备号
void DS18B20_read_rom(uint8_t *p);
// 哪个设备输入设备号.
void _DS18B20_convert_tem(const uint8_t *address_code);
float _DS18B20_read_ram(const uint8_t *address_code, uint8_t count, uint8_t *p);
float _DS18B20_convert_and_read_tem(const uint8_t *address_code);
// 没有做单总线搜索,报警处理,直接在MCU层面做吧.
void _DS18B20_write_ram(const uint8_t *address_code, uint8_t High_Tem, uint8_t Low_Tem, uint8_t accuracy);
void _DS18B20_copy_eeprom(const uint8_t *address_code);
void _DS18B20_call_eeprom(const uint8_t *address_code);
#endif /* INC_DS18B20_H_ */
