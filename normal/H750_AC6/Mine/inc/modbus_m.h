#ifndef __MODBUS_M_H
#define __MODBUS_M_H

#include "stdint.h"

/*�Ƿ����õ��Թ���*/
#define MODBUS_PRINTF 1 

extern uint8_t modbus_temp;
extern uint8_t modbus_buffer[30];
extern uint8_t modbus_buffer_cnt;

void modbus_hardware_init(void);
void modbus_handle(void);

#endif







