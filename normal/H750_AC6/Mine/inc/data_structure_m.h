/**
 *******************************************************************************
 * @file        data_structure_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-19
 * @brief       һЩ���õ����ݽṹ��
 *              
 *****************************************************************************************
 */
#ifndef __DATA_STRUCTURE_M_H
#define __DATA_STRUCTURE_M_H

#include "stdint.h"
#include "rtx_lib.h"
#include "stdio.h"

/*�ж϶��п�*/
extern uint8_t data_flag;

/*������malloc�Ŀռ�*/
//��λ��kbytes 
#define LINK_LIST_MALLOC_SZIE  10

/*�����Ǹ����������*/
//malloc������������
#define LINK_LIST_BUFFER_NAME AppMallocSRAM

/*circle fifo�ṹ��*/
typedef struct
{
    uint32_t write_ptr;
    uint32_t read_ptr;
    uint8_t * buffer; //���������Լ����岻ͬ��С������
    uint32_t size;
}circle_buffer_t;


/*�������ݽṹ��*/
//�û��Զ���
typedef struct
{
    int16_t coefficient;
    int16_t power;
} content_t;

/*�����м�*/
struct link
{
    content_t content;
    struct link *next;
};

/*��������*/
typedef struct link link_node_t;

/*������malloc��*/
extern uint64_t AppMallocSRAM[LINK_LIST_MALLOC_SZIE * 1024 / 8];

/*�������ռ��ʼ��*/
// һ��Ҫ��ʹ������ǰʹ��
static inline void link_list_init(void)
{
    osRtxMemoryInit(LINK_LIST_BUFFER_NAME, sizeof(LINK_LIST_BUFFER_NAME));
}

/*������*/
link_node_t *link_list_creat(void);                                 // ������
void link_list_add_node(link_node_t *link_list, content_t content); // β����ӽڵ�
void link_list_free_node(link_node_t *link_list);                   // ɾ��β�͵Ľڵ�
void link_list_show(link_node_t *link_list);                        // չʾ����

/*circle fifo����*/
uint8_t  write_buffer(circle_buffer_t* circle_buffer, uint8_t val);
uint8_t buffer_is_full(circle_buffer_t* circle_buffer);
uint8_t read_buffer(circle_buffer_t* circle_buffer);

#endif

