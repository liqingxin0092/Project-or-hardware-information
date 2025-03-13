#ifndef __TEST_H
#define __TEST_H

#include "stdint.h"

// 注意要使用这里面的函数一定要使能这个宏.
#define USE_TEST_FUNC 0

/*使用链表*/
// 这个选项和上面并列
// 注意里面使用了个大数组,分配链表空间
#define USE_LINK_LIST 1

#if USE_TEST_FUNC == 1
/*一些异常*/
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

/*内容结构体*/
// 用户自定义
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

/*链表类型*/
typedef struct link link_node_t;

extern uint64_t AppMallocAXISRAM[128 * 1024 / 8];

/*链表分配空间初始化*/
// 一定要在使用链表前使用
static inline void link_list_init(void)
{
    osRtxMemoryInit(AppMallocAXISRAM, sizeof(AppMallocAXISRAM));
}

link_node_t *link_list_creat(void);                                 // 链表创建
void link_list_add_node(link_node_t *link_list, content_t content); // 尾巴添加节点
void link_list_free_node(link_node_t *link_list);                   // 删除尾巴的节点
void link_list_show(link_node_t *link_list);                        // 展示链表

void test_addition_of_polynomials(void); // 应用层

#endif

#endif