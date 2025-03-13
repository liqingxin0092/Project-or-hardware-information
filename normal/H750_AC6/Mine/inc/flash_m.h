/**
 *******************************************************************************
 * @file        h750_flash_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       给h750/743写个flash管理库,包括选项字节
 *              
 *****************************************************************************************
 */
#ifndef __FLASH_M_H
#define __FLASH_M_H

#include "stdint.h"

void flash_single_write(uint32_t addr,uint32_t data_source,uint16_t lenth);                    //flash编程
void option_bytes_modify_RDP(uint8_t val);                                                     //选项字节改RDP
void option_bytes_modify_IDOG(uint8_t stop_en_cnt,uint8_t standby_en_cnt,uint8_t hard_or_soft);//IDOG相关设置
void option_bytes_modify_reset(uint8_t bor ,uint8_t stop_d1,uint8_t stby_d1);                  //复位相关设置
void option_bytes_modify_swap(uint8_t swap);                                                   //bank交换操作
void option_bytes_modify_IO_SPEED_OPTIMIZE(uint8_t is_optimize);                               //IO优化操作
void option_bytes_modify_PCROP(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMEP);         //设置专有代码读出保护
void option_bytes_modify_Secure_area(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMES);   //安全专用区域设置
void option_bytes_clear_Secure(void);                                                          //清除安全选项

#endif 





