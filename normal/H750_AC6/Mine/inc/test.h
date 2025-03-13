#ifndef __TEST_H
#define __TEST_H

#include "stdint.h"

// ע��Ҫʹ��������ĺ���һ��Ҫʹ�������.
#define USE_TEST_FUNC 0

/*ʹ������*/
// ���ѡ������沢��
// ע������ʹ���˸�������,��������ռ�
#define USE_LINK_LIST 1

#if USE_TEST_FUNC == 1
/*һЩ�쳣*/
void a1(void);
void a2(void);
void a3(void);
void a4(void);
void a5(void);

void test_ultrasonic(void);
float get_distance(void);
void test_led_init(void);
void ram_excute(uint32_t a);

#endif

#if USE_LINK_LIST == 1

#include "rtx_lib.h"
#include "stdio.h"

/*���ݽṹ��*/
// �û��Զ���
typedef struct
{
    int16_t coefficient;
    int16_t power;
} content_t;

struct link
{
    content_t content;
    struct link *next;
};

/*��������*/
typedef struct link link_node_t;

extern uint64_t AppMallocAXISRAM[128 * 1024 / 8];

/*�������ռ��ʼ��*/
// һ��Ҫ��ʹ������ǰʹ��
static inline void link_list_init(void)
{
    osRtxMemoryInit(AppMallocAXISRAM, sizeof(AppMallocAXISRAM));
}

link_node_t *link_list_creat(void);                                 // ������
void link_list_add_node(link_node_t *link_list, content_t content); // β����ӽڵ�
void link_list_free_node(link_node_t *link_list);                   // ɾ��β�͵Ľڵ�
void link_list_show(link_node_t *link_list);                        // չʾ����

void test_addition_of_polynomials(void); // Ӧ�ò�

#endif

#endif