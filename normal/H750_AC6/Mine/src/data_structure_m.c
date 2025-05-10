/**
 *******************************************************************************
 * @file        data_structure_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-19
 * @brief       һЩ���õ����ݽṹ��
 *              
 *****************************************************************************************
 */
#include "data_structure_m.h"


/*���������Ŀռ�*/
__attribute__((section(".RAM_DTCM"))) uint64_t AppMallocSRAM[LINK_LIST_MALLOC_SZIE * 1024 / 8];



/*д����*/
//����ֵ:   1,������.  0,���п�
uint8_t write_buffer(circle_buffer_t* circle_buffer, uint8_t val)
{
    uint32_t temp=circle_buffer->write_ptr+1;
    (temp==circle_buffer->size+1)?(temp=0):(temp=temp);
    if(circle_buffer->read_ptr!=temp)
    {
        circle_buffer->buffer[circle_buffer->write_ptr]=val;
    }
    else     
        return 1;//������,

    if(++circle_buffer->write_ptr==circle_buffer->size+1)
        circle_buffer->write_ptr=0;
    return 0;
}

/*�ж϶�����û��*/
//1,����. 0,û��
uint8_t buffer_is_full(circle_buffer_t* circle_buffer)
{
    uint8_t temp=circle_buffer->write_ptr+1;
    (temp==circle_buffer->size+1)?(temp=0):(temp=temp);
    if(circle_buffer->read_ptr!=temp)
        return 0;
    else return 1;
}


/*������*/
uint8_t read_buffer(circle_buffer_t* circle_buffer)
{
    uint8_t return_val; 
    if(circle_buffer->read_ptr!=circle_buffer->write_ptr)
    {
        return_val=circle_buffer->buffer[circle_buffer->read_ptr];
    }
    else 
    {
         return 0;//���п�
    } 
    if(++circle_buffer->read_ptr==circle_buffer->size+1)
        circle_buffer->read_ptr=0;
    return return_val;
}

/*������*/
link_node_t *link_list_creat(void)
{
    link_node_t *p = (link_node_t *)osRtxMemoryAlloc(LINK_LIST_BUFFER_NAME, sizeof(link_node_t), 0);
    p->next = NULL;
    return p;
}

/*���������*/
// ��β�����
void link_list_add_node(link_node_t *link_list, content_t content)
{
    link_node_t *ptemp = link_list; // Ѱַ����ӵ�����
    link_node_t *p;                 // �½ڵ�
    while (ptemp->next != NULL)
    {
        ptemp = ptemp->next;
    }
    // ������˵��ptemp->next==NULL

    /*���½������ռ�*/
    p = (link_node_t *)osRtxMemoryAlloc(LINK_LIST_BUFFER_NAME, sizeof(link_node_t), 0);
    p->content = content;
    p->next = NULL;
    ptemp->next = p; // ������������һ�����������һ��ָ���½ڵ�.
}

/*ɾ��β�͵Ľڵ�*/
void link_list_free_node(link_node_t *link_list)
{
    link_node_t *ptemp = link_list; // Ѱַ����ӵ�����
    if (ptemp->next == NULL)        // û����
    {
        return;
    }
    else // �ж���
    {
        while (ptemp->next->next != NULL)
        {
            ptemp = ptemp->next;
        }
        // ������˵��ptemp->next->next==NULL

        osRtxMemoryFree(LINK_LIST_BUFFER_NAME, ptemp->next); // �ڴ��ͷ�
        ptemp->next = NULL;
    }
}

/*չʾ���е��б���*/
// �õ�ʱ�򻹵ø�
void link_list_show(link_node_t *link_list)
{
    link_node_t *ptemp = link_list; // Ѱַ����ӵ�����
    printf("\r\n");
    if (ptemp->next == NULL) // û��
    {
        printf("no link list\r\n");
    }
    else // �кö���
    {
        while (ptemp->next != NULL) // �ж���
        {
            printf("co:%d,pow:%d,\r\n", ptemp->next->content.coefficient, ptemp->next->content.power);
            ptemp = ptemp->next;
        }
    }
}
