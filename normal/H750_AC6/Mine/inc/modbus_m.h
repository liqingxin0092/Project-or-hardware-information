#ifndef __MODBUS_M_H
#define __MODBUS_M_H

#include "stdint.h"

/*是否启用调试功能*/
#define MODBUS_PRINTF 1 

extern uint8_t modbus_temp;
extern uint8_t modbus_buffer[30];
extern uint8_t modbus_buffer_cnt;

void modbus_hardware_init(void);
void modbus_handle(void);

#endif







