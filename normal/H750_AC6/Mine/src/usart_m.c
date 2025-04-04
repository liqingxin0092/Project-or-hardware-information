/**
 *******************************************************************************
 * @file        usart_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-4
 * @brief       ̽��USART����
 *
 ********************************************************************************
 */
#include "stdio.h"
#include "usart_m.h"
#include "stm32h750xx.h"
#include "h7_core.h"
#include "gpio.h"

//���ż���,�ҿ����ĵ�.
//������,������ͦ��д��
//ע�������ϵ�����,�еĴ��ڲ�֧��ͬ��������������USART��.
//ע�������,�����л�. ���ڴ���1�ʹ����Ĺ��ܺ�����.
//��ǰ�����жϷ�ʽ�����е�̫Ƶ����.
//û��ʵ��fifo��ֵ�ķ�ʽ,������,�����жϲ�Ƶ��.

/*����´���*/
//1.����fifo,�ṹ��ɶ��һ��Ҫ���Զ����,��� ��USE_USARTx,ʹ��.
//2.���ڳ�ʼ�����������һ��,ע��Ķ���:ʹ��ʱ��,IO���ź�,���õø�.������ʱ��ԴƵ�ʵÿ������.
//3.�жϺ���,ע������Ҫ���������︴��.

/*���ڵ����fifo-circle buffer*/
#if USE_USART1 == 1 /*����1fifo�������*/
#define USART1_TX_FIFO_SIZE 512
#define USART1_RX_FIFO_SIZE 1024
uint8_t usart1_tx_fifo_buffer[USART1_TX_FIFO_SIZE + 1];
uint8_t usart1_rx_fifo_buffer[USART1_RX_FIFO_SIZE + 1];
usart_t usart1_app_struct;
#endif

#if USE_USART2 == 1 /*����2fifo�������*/
#define USART2_TX_FIFO_SIZE 128
#define USART2_RX_FIFO_SIZE 128
uint8_t usart2_tx_fifo_buffer[USART2_TX_FIFO_SIZE + 1];
uint8_t usart2_rx_fifo_buffer[USART2_RX_FIFO_SIZE + 1];
usart_t usart2_app_struct;
#endif

#if USE_USART3 == 1 /*����3fifo�������*/
#define USART3_TX_FIFO_SIZE 128
#define USART3_RX_FIFO_SIZE 128
uint8_t usart3_tx_fifo_buffer[USART3_TX_FIFO_SIZE + 1];
uint8_t usart3_rx_fifo_buffer[USART3_RX_FIFO_SIZE + 1];
usart_t usart3_app_struct;
#endif

#if USE_USART4 == 1 /*����4fifo�������*/
#define USART4_TX_FIFO_SIZE 128
#define USART4_RX_FIFO_SIZE 128
uint8_t usart4_tx_fifo_buffer[USART4_TX_FIFO_SIZE + 1];
uint8_t usart4_rx_fifo_buffer[USART4_RX_FIFO_SIZE + 1];
usart_t usart4_app_struct;
#endif

/*ʹ�÷���*/
// init����һ��Ҫ�ŵ���ǰ��,���gpio��ʼ������
// �ǵü�enable ����

/*����������ʱ��Դ����Ĳ��ұ�*/
const uint16_t div_table[][2] = {
    {0, 1},
    {1, 2},
    {2, 4},
    {3, 6},
    {4, 8},
    {5, 10},
    {6, 12},
    {7, 16},
    {8, 32},
    {9, 64},
    {10, 128},
    {11, 256},
};

/*һЩ��Ϣ*/
// tx,rx fifo����16 lenth .tx���9bit,rx���12bit.
// SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);//��ʱ������
// ����ʱ�ӿ�����ʱ��Դѡ���뵥��,ʹ��������Щ����ǰ�ȿ�ʱ��.
// ���ͺͽ���fifo����16.

/*���ڳ�ʼ��*/
// ע��RCC�ſ�,������ʱ��,������������⺯����,ע�����.
// switch_state:    [0]: FIFO mode.1,��fifo.0,��fifo
//                [2:1]: ֡����,00,8 bit. 01,9bit. 10,7bit.
//                [4:3]: ֹͣλ���.00,1bit.01,0.5bit.10,2bit.11,1.5bit.
//                  [5]: �� ������.1,������8. 0,������16.
//                [9:6]: group_pri,NVIC���ȼ�
//                 [10]: 1,�رս���ʱ�߼���ƽ���β���(������). 0,��������ʱ�߼���ƽ���β���.
//              [12:11]: 10,��У��. 01:żУ��. 00:��У��.
// fifo_init_num: 1,init usart1 fifo. 2,init usart2 fifo. ...
// group_pri:��ռ���ȼ�
void usart_init(USART_TypeDef *USARTx, uint16_t switch_state, uint8_t fifo_init_num)
{
    uint8_t temp;
    USARTx->CR1 |= ((switch_state & 1) << 29) | (((switch_state >> 2) & 1) << 28) | (((switch_state >> 1) & 1) << 12) // ������һ������֡����
                   | (((switch_state >> 5) & 1) << 15)                                                                // ���ù�����
                   | (3 << 2)                                                                                          // ʹ���շ�.
                   | (1 << 5);                                                                                        // ��rx fifo�����ж�
    USARTx->CR2 |= ((switch_state >> 3) & 3) << 12;                                                                   // ����ֹͣλ���.
    USARTx->CR3 |= 1 & (switch_state >> 10);                                                                          // �߼���ƽ����

    temp = 3 & (switch_state >> 11);
    if (temp != 0)
    {
        USARTx->CR1 |= 0x400              // ��PCEλ.
                    | ((temp - 1) << 9); // ����PEλ
    }
    switch (fifo_init_num)
    {
#if USE_USART1 == 1 /*����1fifo�������*/   
    case 1:
        __set_priority(USART1_IRQn, (switch_state >> 6) & 0xf, 0);
        __enable_IRQ(USART1_IRQn);
        usart1_app_struct.tx_fifo_struct.buffer = usart1_tx_fifo_buffer;
        usart1_app_struct.tx_fifo_struct.size = USART1_TX_FIFO_SIZE;
        usart1_app_struct.rx_fifo_struct.buffer = usart1_rx_fifo_buffer;
        usart1_app_struct.rx_fifo_struct.size = USART1_RX_FIFO_SIZE;
        usart1_app_struct.USARTx = USART1;
        break;
#endif
    
#if USE_USART2 == 1 /*����2fifo�������*/
    case 2:
        __set_priority(USART2_IRQn, (switch_state >> 6) & 0xf, 0);
        __enable_IRQ(USART2_IRQn);
        usart2_app_struct.tx_fifo_struct.buffer = usart2_tx_fifo_buffer;
        usart2_app_struct.tx_fifo_struct.size = USART2_TX_FIFO_SIZE;
        usart2_app_struct.rx_fifo_struct.buffer = usart2_rx_fifo_buffer;
        usart2_app_struct.rx_fifo_struct.size = USART2_RX_FIFO_SIZE;
        usart2_app_struct.USARTx = USART2;
        break;
#endif
 
#if USE_USART4 == 1 /*����4fifo�������*/
    case 4:
        __set_priority(UART4_IRQn, (switch_state >> 6) & 0xf, 0);
        __enable_IRQ(UART4_IRQn);
        usart4_app_struct.tx_fifo_struct.buffer =  usart4_tx_fifo_buffer;
        usart4_app_struct.tx_fifo_struct.size   =  USART4_TX_FIFO_SIZE;
        usart4_app_struct.rx_fifo_struct.buffer =  usart4_rx_fifo_buffer;
        usart4_app_struct.rx_fifo_struct.size   =  USART4_RX_FIFO_SIZE;
        usart4_app_struct.USARTx = UART4;
        break;
#endif
    }
}

/*����������*/
// �����֧��standard USART.
// current_clk_fz:�������ṩ��ʱ��ԴƵ��.
// div:  0000: input clock not divided
//       0001: input clock divided by 2
//       0010: input clock divided by 4
//       0011: input clock divided by 6
//       0100: input clock divided by 8
//       0101: input clock divided by 10
//       0110: input clock divided by 12
//       0111: input clock divided by 16
//       1000: input clock divided by 32
//       1001: input clock divided by 64
//       1010: input clock divided by 128
//       1011: input clock divided by 256
// baud_rate:Ŀ�겨����
void baud_rate_set(USART_TypeDef *USARTx, uint32_t current_clk_fz, uint8_t div, uint32_t baud_rate)
{
    uint32_t i;
    USARTx->PRESC = div;
    if ((USARTx->CR1 & 0x8000) != 0) // 8
    {
        for (i = 0; i < 12; i++)
        {
            if (div_table[i][0] == div)
            {
                uint32_t baud_temp;
                baud_temp = 2 * current_clk_fz / div_table[i][1] / baud_rate;
                USARTx->BRR = (baud_temp & (~0xf)) | ((baud_temp & 0xf) >> 1); // ����������
                break;
            }
        }
    }
    else //==0                      //16
    {
        for (i = 0; i < 12; i++)
        {
            if (div_table[i][0] == div)
            {
                USARTx->BRR = current_clk_fz / div_table[i][1] / baud_rate; // ����������
                break;
            }
        }
    }
}

/*fifo����*/
// ��fifo��Ҫ�Ļ�ʹ���������
// tx_fifo:
// rx_fifo:  fifo��ֵ����,ע��������16
//         000:Receive FIFO reaches 1/8 of its depth
//         001:Receive FIFO reaches 1/4 of its depth
//         010:Receive FIFO reaches 1/2 of its depth
//         011:Receive FIFO reaches 3/4 of its depth
//         100:Receive FIFO reaches 7/8 of its depth
//         101:Receive FIFO becomes full
void usart_set_hardware_fifo(USART_TypeDef *USARTx, uint8_t tx_fifo, uint8_t rx_fifo)
{
    USARTx->CR3 |= (tx_fifo << 29) | (rx_fifo << 25); // fifo��ֵ����
}

/*Ӳ��fifo,�жϷ���*/
// ��������ӵ����ͻ�������
// ����ֵ: 1,��Ϊ��������,ֱ���˳�.0,��������
uint8_t usart_transmit_it(usart_t *usart_app, uint8_t *psrc, uint32_t lenth)
{
    uint32_t i;
    for (i = 0; i < lenth; i++)
    {
        if (write_buffer(&usart_app->tx_fifo_struct, psrc[i]) == 1) // д�����ͻ�������
        {
            usart_app->USARTx->CR1 |= 0x80; // ��TXFNF�ж�,�ܷ����ٷ�����
            return 1;                       //  �������ֱ�ӳ��⺯��
        }
    }
    usart_app->USARTx->CR1 |= 0x80; // ��TXFNF�ж�
    return 0;
}

/*�����fifo�������*/
// ptr:����ָ��
// ����ֵ: 1,������û����. 0,���ɹ�.
uint8_t usart_receive_char(usart_t *usart_app, uint8_t *ptr)
{
    if (usart_app->rx_fifo_struct.read_ptr != usart_app->rx_fifo_struct.write_ptr) // ��������������
    {
        *ptr = usart_app->rx_fifo_struct.buffer[usart_app->rx_fifo_struct.read_ptr];    // ��
        if (++usart_app->rx_fifo_struct.read_ptr == usart_app->rx_fifo_struct.size + 1) // �仺����ָ��
            usart_app->rx_fifo_struct.read_ptr = 0;
    }
    else // ������û����
    {
        return 1;
    }
    return 0;
}

/*fputs�ļ��ַ���*/
#if FPUTC_MODE == 0
// ��ѯ
int fputc(int ch, FILE *f)
{
    USART_APP->USARTx->TDR = ch;
    while (!(USART_APP->USARTx->ISR & 0x40))
        ; // �ȷ���
    return ch;
}
#elif FPUTC_MODE == 1
// ����Ӳ��fifo
int fputc(int ch, FILE *f)
{
    while ((USART_APP.USARTx->ISR & 0x80) == 0)
        ; // ���tx_fifo����������ȵ�.
    USART_APP.USARTx->TDR = ch;
    return ch;
}
#elif FPUTC_MODE == 2
// д���fifo�жϷ�
int fputc(int ch, FILE *f)
{
    usart_send_byte(&USART_APP, ch);
    return ch;
}
#endif


extern uint8_t usmart_rcv_pack;//usmart�ź���

/*����1�ж�*/
void USART1_IRQHandler(void)
{
    /*�жϷ���*/
    if (USART1->ISR & 0x80) // �����TXFNF�ж�
    {
        if (usart1_app_struct.tx_fifo_struct.write_ptr != usart1_app_struct.tx_fifo_struct.read_ptr)
        { // ���tx_fifo������
            USART1->TDR = usart1_app_struct.tx_fifo_struct.buffer[usart1_app_struct.tx_fifo_struct.read_ptr];
            if (++usart1_app_struct.tx_fifo_struct.read_ptr == usart1_app_struct.tx_fifo_struct.size + 1)
                usart1_app_struct.tx_fifo_struct.read_ptr = 0;
        }
        else // ���tx_fifoû����
        {
            USART1->CR1 &= ~0x80; // �ر�TXFNF�ж�
        }
    }    
    if (USART1->ISR & 0x20) // �������rx fifo�����ж�,���յ�������
    {
        uint8_t temp=USART1->RDR;
        write_buffer(&usart1_app_struct.rx_fifo_struct, temp); // ����д�����������
#if USART_USED_BY_USMART==1
        if(temp=='\n')        //��usmart���﷢��Ϣ
            usmart_rcv_pack=1;
#endif
    }
}


/*�������ж�*/
void UART4_IRQHandler(void)
{
    /*�жϷ���*/
    if (UART4->ISR & 0x80) // �����TXFNF�ж�
    {
        if (usart4_app_struct.tx_fifo_struct.write_ptr != usart4_app_struct.tx_fifo_struct.read_ptr)
        { // ���tx_fifo������
            UART4->TDR = usart4_app_struct.tx_fifo_struct.buffer[usart4_app_struct.tx_fifo_struct.read_ptr];
            if (++usart4_app_struct.tx_fifo_struct.read_ptr == usart4_app_struct.tx_fifo_struct.size + 1)
                usart4_app_struct.tx_fifo_struct.read_ptr = 0;
        }
        else // ���tx_fifoû����
        {
            UART4->CR1 &= ~0x80; // �ر�TXFNF�ж�
        }
    }
    if (UART4->ISR & 0x20) // �������rx fifo�����ж�,���յ�������
    {
        uint8_t temp=UART4->RDR;
        write_buffer(&usart4_app_struct.rx_fifo_struct, temp); // ����д�����������        
#if USART_USED_BY_USMART==4
        if(temp=='\n')        //��usmart���﷢��Ϣ
            usmart_rcv_pack=1;
#endif
    }
}

/*Ӧ�ò㴮�ڳ�ʼ��*/
void usart_init_app(void)
{
#if USE_USART1 == 1 
    /*��ʼ������1*/
    RCC->APB2ENR |= 1 << 4;                  // ������1ʱ��
    gpio_init_pin(GPIO_A,  9, 2, 0, 3, 0, 7); // ����1 io��ʼ��
    gpio_init_pin(GPIO_A, 10, 2, 0, 3, 0, 7);
    usart_init(USART1, 0xa1, 1);
    baud_rate_set(USART1, 120000000, 9, 115200);
    usart_set_hardware_fifo(USART1, 5, 5);
    enable_usart(USART1);
#endif

#if USE_USART4 == 1     
    /*��ʼ������4*/                
    RCC->APB1LENR |=1<<19;                   // ������4ʱ��
    gpio_init_pin(GPIO_C, 10, 2, 0, 2, 1, 8); // ����1 io��ʼ��
    gpio_init_pin(GPIO_C, 11, 2, 0, 2, 1, 8);
    usart_init(UART4, 0xa1, 4);
    baud_rate_set(UART4, 120000000, 9, 115200);
    usart_set_hardware_fifo(UART4, 5, 5);
    enable_usart(UART4);
#endif
}