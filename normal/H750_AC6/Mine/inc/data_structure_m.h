/**
 *******************************************************************************
 * @file        data_structure_m.h
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-19
 * @brief       一些常用的数据结构体
 *              
 *****************************************************************************************
 */
#ifndef __DATA_STRUCTURE_M_H
#define __DATA_STRUCTURE_M_H

#include "stdint.h"
#include "rtx_lib.h"
#include "stdio.h"

/*判断队列空*/
extern uint8_t data_flag;

/*给链表malloc的空间*/
//单位是kbytes 
#define LINK_LIST_MALLOC_SZIE  10

/*上面那个数组的名字*/
//malloc填参数填这个宏
#define LINK_LIST_BUFFER_NAME AppMallocSRAM

/*circle fifo结构体*/
typedef struct
{
    uint32_t write_ptr;
    uint32_t read_ptr;
    uint8_t * buffer; //这样可以自己定义不同大小的数组
    uint32_t size;
}circle_buffer_t;


/*链表内容结构体*/
//用户自定义
typedef struct
{
    int16_t coefficient;
    int16_t power;
} content_t;

/*链表中间*/
struct link
{
    content_t content;
    struct link *next;
};

/*链表类型*/
typedef struct link link_node_t;

/*给链表malloc的*/
extern uint64_t AppMallocSRAM[LINK_LIST_MALLOC_SZIE * 1024 / 8];

/*链表分配空间初始化*/
// 一定要在使用链表前使用
static inline void link_list_init(void)
{
    osRtxMemoryInit(LINK_LIST_BUFFER_NAME, sizeof(LINK_LIST_BUFFER_NAME));
}

/*链表函数*/
link_node_t *link_list_creat(void);                                 // 链表创建
void link_list_add_node(link_node_t *link_list, content_t content); // 尾巴添加节点
void link_list_free_node(link_node_t *link_list);                   // 删除尾巴的节点
void link_list_show(link_node_t *link_list);                        // 展示链表

/*circle fifo函数*/
uint8_t  write_buffer(circle_buffer_t* circle_buffer, uint8_t val);
uint8_t buffer_is_full(circle_buffer_t* circle_buffer);
uint8_t read_buffer(circle_buffer_t* circle_buffer);

#endif

