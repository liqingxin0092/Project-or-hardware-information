/**
 *******************************************************************************
 * @file        h750_flash_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-8
 * @brief       ��h750/743д��flash�����,����ѡ���ֽ�
 *              
 *****************************************************************************************
 */
#ifndef __FLASH_M_H
#define __FLASH_M_H

#include "stdint.h"

void flash_single_write(uint32_t addr,uint32_t data_source,uint16_t lenth);                    //flash���
void option_bytes_modify_RDP(uint8_t val);                                                     //ѡ���ֽڸ�RDP
void option_bytes_modify_IDOG(uint8_t stop_en_cnt,uint8_t standby_en_cnt,uint8_t hard_or_soft);//IDOG�������
void option_bytes_modify_reset(uint8_t bor ,uint8_t stop_d1,uint8_t stby_d1);                  //��λ�������
void option_bytes_modify_swap(uint8_t swap);                                                   //bank��������
void option_bytes_modify_IO_SPEED_OPTIMIZE(uint8_t is_optimize);                               //IO�Ż�����
void option_bytes_modify_PCROP(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMEP);         //����ר�д����������
void option_bytes_modify_Secure_area(uint8_t bank,uint16_t start,uint16_t end,uint8_t DMES);   //��ȫר����������
void option_bytes_clear_Secure(void);                                                          //�����ȫѡ��

#endif 





