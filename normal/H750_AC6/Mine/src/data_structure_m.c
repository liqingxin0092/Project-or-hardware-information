/**
 *******************************************************************************
 * @file        data_structure_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-19
 * @brief       一些常用的数据结构体
 *              
 *****************************************************************************************
 */
#include "data_structure_m.h"


/*给链表分配的空间*/
__attribute__((section(".RAM_DTCM"))) uint64_t AppMallocSRAM[LINK_LIST_MALLOC_SZIE * 1024 / 8];



/*写队列*/
//返回值:   1,队列满.  0,队列空
uint8_t write_buffer(circle_buffer_t* circle_buffer, uint8_t val)
{
    uint32_t temp=circle_buffer->write_ptr+1;
    (temp==circle_buffer->size+1)?(temp=0):(temp=temp);
    if(circle_buffer->read_ptr!=temp)
    {
        circle_buffer->buffer[circle_buffer->write_ptr]=val;
    }
    else     
        return 1;//队列满,

    if(++circle_buffer->write_ptr==circle_buffer->size+1)
        circle_buffer->write_ptr=0;
    return 0;
}

/*判断队列满没满*/
//1,满了. 0,没满
uint8_t buffer_is_full(circle_buffer_t* circle_buffer)
{
    uint8_t temp=circle_buffer->write_ptr+1;
    (temp==circle_buffer->size+1)?(temp=0):(temp=temp);
    if(circle_buffer->read_ptr!=temp)
        return 0;
    else return 1;
}


/*读队列*/
uint8_t read_buffer(circle_buffer_t* circle_buffer)
{
    uint8_t return_val; 
    if(circle_buffer->read_ptr!=circle_buffer->write_ptr)
    {
        return_val=circle_buffer->buffer[circle_buffer->read_ptr];
    }
    else 
    {
         return 0;//队列空
    } 
    if(++circle_buffer->read_ptr==circle_buffer->size+1)
        circle_buffer->read_ptr=0;
    return return_val;
}

/*链表创建*/
link_node_t *link_list_creat(void)
{
    link_node_t *p = (link_node_t *)osRtxMemoryAlloc(LINK_LIST_BUFFER_NAME, sizeof(link_node_t), 0);
    p->next = NULL;
    return p;
}

/*添加链表项*/
// 在尾巴添加
void link_list_add_node(link_node_t *link_list, content_t content)
{
    link_node_t *ptemp = link_list; // 寻址被添加的链表
    link_node_t *p;                 // 新节点
    while (ptemp->next != NULL)
    {
        ptemp = ptemp->next;
    }
    // 到这里说明ptemp->next==NULL

    /*给新结点申请空间*/
    p = (link_node_t *)osRtxMemoryAlloc(LINK_LIST_BUFFER_NAME, sizeof(link_node_t), 0);
    p->content = content;
    p->next = NULL;
    ptemp->next = p; // 待添加链表最后一个链表项的下一个指向新节点.
}

/*删除尾巴的节点*/
void link_list_free_node(link_node_t *link_list)
{
    link_node_t *ptemp = link_list; // 寻址被添加的链表
    if (ptemp->next == NULL)        // 没东西
    {
        return;
    }
    else // 有东西
    {
        while (ptemp->next->next != NULL)
        {
            ptemp = ptemp->next;
        }
        // 到这里说明ptemp->next->next==NULL

        osRtxMemoryFree(LINK_LIST_BUFFER_NAME, ptemp->next); // 内存释放
        ptemp->next = NULL;
    }
}

/*展示现有的列表项*/
// 用的时候还得改
void link_list_show(link_node_t *link_list)
{
    link_node_t *ptemp = link_list; // 寻址被添加的链表
    printf("\r\n");
    if (ptemp->next == NULL) // 没有
    {
        printf("no link list\r\n");
    }
    else // 有好多项
    {
        while (ptemp->next != NULL) // 有东西
        {
            printf("co:%d,pow:%d,\r\n", ptemp->next->content.coefficient, ptemp->next->content.power);
            ptemp = ptemp->next;
        }
    }
}
