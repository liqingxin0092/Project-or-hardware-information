/**
 *******************************************************************************
 * @file        usart_m.c
 * @author      liqingxin0092
 * @version     V1.0
 * @date        2025-3-4
 * @brief       探究USART功能
 *
 ********************************************************************************
 */
#include "stdio.h"
#include "usart_m.h"
#include "stm32h750xx.h"
#include "h7_core.h"
#include "gpio.h"

//不着急先,我看看文档.
//回来了,这驱动挺好写的
//注意名字上的区别,有的串口不支持同步但还是命名成USART了.
//注意测试下,串口切换. 现在串口1和串口四功能很正常.
//当前这种中断方式可能有点太频繁了.
//没有实现fifo阈值的方式,这种慢,但是中断不频繁.

/*添加新串口*/
//1.串口fifo,结构体啥的一定要各自定义好,添加 宏USE_USARTx,使能.
//2.串口初始化函数里添加一份,注意改东西:使能时钟,IO引脚号,复用得改.波特率时钟源频率得看情况改.
//3.中断函数,注意名字要从向量表里复制.

/*串口的软件fifo-circle buffer*/
#if USE_USART1 == 1 /*串口1fifo相关内容*/
#define USART1_TX_FIFO_SIZE 512
#define USART1_RX_FIFO_SIZE 1024
uint8_t usart1_tx_fifo_buffer[USART1_TX_FIFO_SIZE + 1];
uint8_t usart1_rx_fifo_buffer[USART1_RX_FIFO_SIZE + 1];
usart_t usart1_app_struct;
#endif

#if USE_USART2 == 1 /*串口2fifo相关内容*/
#define USART2_TX_FIFO_SIZE 128
#define USART2_RX_FIFO_SIZE 128
uint8_t usart2_tx_fifo_buffer[USART2_TX_FIFO_SIZE + 1];
uint8_t usart2_rx_fifo_buffer[USART2_RX_FIFO_SIZE + 1];
usart_t usart2_app_struct;
#endif

#if USE_USART3 == 1 /*串口3fifo相关内容*/
#define USART3_TX_FIFO_SIZE 128
#define USART3_RX_FIFO_SIZE 128
uint8_t usart3_tx_fifo_buffer[USART3_TX_FIFO_SIZE + 1];
uint8_t usart3_rx_fifo_buffer[USART3_RX_FIFO_SIZE + 1];
usart_t usart3_app_struct;
#endif

#if USE_USART4 == 1 /*串口4fifo相关内容*/
#define USART4_TX_FIFO_SIZE 128
#define USART4_RX_FIFO_SIZE 128
uint8_t usart4_tx_fifo_buffer[USART4_TX_FIFO_SIZE + 1];
uint8_t usart4_rx_fifo_buffer[USART4_RX_FIFO_SIZE + 1];
usart_t usart4_app_struct;
#endif

/*使用方法*/
// init函数一定要放到最前面,相关gpio初始化后面
// 记得加enable 函数

/*波特率设置时钟源那里的查找表*/
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

/*一些信息*/
// tx,rx fifo都是16 lenth .tx宽度9bit,rx宽度12bit.
// SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);//开时钟例子
// 串口时钟开启和时钟源选择请单加,使用下面这些函数前先开时钟.
// 发送和接收fifo各有16.

/*串口初始化*/
// 注意RCC门控,开串口时钟,这个不包括在这函数内,注意另加.
// switch_state:    [0]: FIFO mode.1,开fifo.0,关fifo
//                [2:1]: 帧长度,00,8 bit. 01,9bit. 10,7bit.
//                [4:3]: 停止位宽度.00,1bit.01,0.5bit.10,2bit.11,1.5bit.
//                  [5]: 读 过采样.1,过采样8. 0,过采样16.
//                [9:6]: group_pri,NVIC优先级
//                 [10]: 1,关闭接收时逻辑电平三段采样(单采样). 0,开启接收时逻辑电平三段采样.
//              [12:11]: 10,奇校验. 01:偶校验. 00:无校验.
// fifo_init_num: 1,init usart1 fifo. 2,init usart2 fifo. ...
// group_pri:抢占优先级
void usart_init(USART_TypeDef *USARTx, uint16_t switch_state, uint8_t fifo_init_num)
{
    uint8_t temp;
    USARTx->CR1 |= ((switch_state & 1) << 29) | (((switch_state >> 2) & 1) << 28) | (((switch_state >> 1) & 1) << 12) // 和上面一起配置帧长度
                   | (((switch_state >> 5) & 1) << 15)                                                                // 设置过采样
                   | (3 << 2)                                                                                          // 使能收发.
                   | (1 << 5);                                                                                        // 开rx fifo不空中断
    USARTx->CR2 |= ((switch_state >> 3) & 3) << 12;                                                                   // 设置停止位宽度.
    USARTx->CR3 |= 1 & (switch_state >> 10);                                                                          // 逻辑电平采样

    temp = 3 & (switch_state >> 11);
    if (temp != 0)
    {
        USARTx->CR1 |= 0x400              // 开PCE位.
                    | ((temp - 1) << 9); // 设置PE位
    }
    switch (fifo_init_num)
    {
#if USE_USART1 == 1 /*串口1fifo相关内容*/   
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
    
#if USE_USART2 == 1 /*串口2fifo相关内容*/
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
 
#if USE_USART4 == 1 /*串口4fifo相关内容*/
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

/*波特率设置*/
// 这里仅支持standard USART.
// current_clk_fz:给串口提供的时钟源频率.
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
// baud_rate:目标波特率
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
                USARTx->BRR = (baud_temp & (~0xf)) | ((baud_temp & 0xf) >> 1); // 波特率设置
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
                USARTx->BRR = current_clk_fz / div_table[i][1] / baud_rate; // 波特率设置
                break;
            }
        }
    }
}

/*fifo设置*/
// 有fifo需要的话使用这个函数
// tx_fifo:
// rx_fifo:  fifo阈值设置,注意满了是16
//         000:Receive FIFO reaches 1/8 of its depth
//         001:Receive FIFO reaches 1/4 of its depth
//         010:Receive FIFO reaches 1/2 of its depth
//         011:Receive FIFO reaches 3/4 of its depth
//         100:Receive FIFO reaches 7/8 of its depth
//         101:Receive FIFO becomes full
void usart_set_hardware_fifo(USART_TypeDef *USARTx, uint8_t tx_fifo, uint8_t rx_fifo)
{
    USARTx->CR3 |= (tx_fifo << 29) | (rx_fifo << 25); // fifo阈值设置
}

/*硬件fifo,中断发送*/
// 把数据添加到发送缓冲区里
// 返回值: 1,因为缓冲区满,直接退出.0,传输正常
uint8_t usart_transmit_it(usart_t *usart_app, uint8_t *psrc, uint32_t lenth)
{
    uint32_t i;
    for (i = 0; i < lenth; i++)
    {
        if (write_buffer(&usart_app->tx_fifo_struct, psrc[i]) == 1) // 写到发送缓冲区域
        {
            usart_app->USARTx->CR1 |= 0x80; // 开TXFNF中断,能发多少发多少
            return 1;                       //  如果队满直接出这函数
        }
    }
    usart_app->USARTx->CR1 |= 0x80; // 开TXFNF中断
    return 0;
}

/*从软件fifo里读数据*/
// ptr:接收指针
// 返回值: 1,缓冲区没数据. 0,读成功.
uint8_t usart_receive_char(usart_t *usart_app, uint8_t *ptr)
{
    if (usart_app->rx_fifo_struct.read_ptr != usart_app->rx_fifo_struct.write_ptr) // 缓冲区还有数据
    {
        *ptr = usart_app->rx_fifo_struct.buffer[usart_app->rx_fifo_struct.read_ptr];    // 读
        if (++usart_app->rx_fifo_struct.read_ptr == usart_app->rx_fifo_struct.size + 1) // 变缓冲区指针
            usart_app->rx_fifo_struct.read_ptr = 0;
    }
    else // 缓冲区没数据
    {
        return 1;
    }
    return 0;
}

/*fputs的几种方案*/
#if FPUTC_MODE == 0
// 轮询
int fputc(int ch, FILE *f)
{
    USART_APP->USARTx->TDR = ch;
    while (!(USART_APP->USARTx->ISR & 0x40))
        ; // 等发完
    return ch;
}
#elif FPUTC_MODE == 1
// 串口硬件fifo
int fputc(int ch, FILE *f)
{
    while ((USART_APP.USARTx->ISR & 0x80) == 0)
        ; // 如果tx_fifo满了在这里等的.
    USART_APP.USARTx->TDR = ch;
    return ch;
}
#elif FPUTC_MODE == 2
// 写软件fifo中断发
int fputc(int ch, FILE *f)
{
    usart_send_byte(&USART_APP, ch);
    return ch;
}
#endif


extern uint8_t usmart_rcv_pack;//usmart信号量

/*串口1中断*/
void USART1_IRQHandler(void)
{
    /*中断发送*/
    if (USART1->ISR & 0x80) // 如果是TXFNF中断
    {
        if (usart1_app_struct.tx_fifo_struct.write_ptr != usart1_app_struct.tx_fifo_struct.read_ptr)
        { // 如果tx_fifo有数据
            USART1->TDR = usart1_app_struct.tx_fifo_struct.buffer[usart1_app_struct.tx_fifo_struct.read_ptr];
            if (++usart1_app_struct.tx_fifo_struct.read_ptr == usart1_app_struct.tx_fifo_struct.size + 1)
                usart1_app_struct.tx_fifo_struct.read_ptr = 0;
        }
        else // 如果tx_fifo没数据
        {
            USART1->CR1 &= ~0x80; // 关闭TXFNF中断
        }
    }    
    if (USART1->ISR & 0x20) // 如果发生rx fifo不空中断,接收到数据了
    {
        uint8_t temp=USART1->RDR;
        write_buffer(&usart1_app_struct.rx_fifo_struct, temp); // 数据写到软件缓冲区
#if USART_USED_BY_USMART==1
        if(temp=='\n')        //给usmart那里发消息
            usmart_rcv_pack=1;
#endif
    }
}


/*串口四中断*/
void UART4_IRQHandler(void)
{
    /*中断发送*/
    if (UART4->ISR & 0x80) // 如果是TXFNF中断
    {
        if (usart4_app_struct.tx_fifo_struct.write_ptr != usart4_app_struct.tx_fifo_struct.read_ptr)
        { // 如果tx_fifo有数据
            UART4->TDR = usart4_app_struct.tx_fifo_struct.buffer[usart4_app_struct.tx_fifo_struct.read_ptr];
            if (++usart4_app_struct.tx_fifo_struct.read_ptr == usart4_app_struct.tx_fifo_struct.size + 1)
                usart4_app_struct.tx_fifo_struct.read_ptr = 0;
        }
        else // 如果tx_fifo没数据
        {
            UART4->CR1 &= ~0x80; // 关闭TXFNF中断
        }
    }
    if (UART4->ISR & 0x20) // 如果发生rx fifo不空中断,接收到数据了
    {
        uint8_t temp=UART4->RDR;
        write_buffer(&usart4_app_struct.rx_fifo_struct, temp); // 数据写到软件缓冲区        
#if USART_USED_BY_USMART==4
        if(temp=='\n')        //给usmart那里发消息
            usmart_rcv_pack=1;
#endif
    }
}

/*应用层串口初始化*/
void usart_init_app(void)
{
#if USE_USART1 == 1 
    /*初始化串口1*/
    RCC->APB2ENR |= 1 << 4;                  // 开串口1时钟
    gpio_init_pin(GPIO_A,  9, 2, 0, 3, 0, 7); // 串口1 io初始化
    gpio_init_pin(GPIO_A, 10, 2, 0, 3, 0, 7);
    usart_init(USART1, 0xa1, 1);
    baud_rate_set(USART1, 120000000, 9, 115200);
    usart_set_hardware_fifo(USART1, 5, 5);
    enable_usart(USART1);
#endif

#if USE_USART4 == 1     
    /*初始化串口4*/                
    RCC->APB1LENR |=1<<19;                   // 开串口4时钟
    gpio_init_pin(GPIO_C, 10, 2, 0, 2, 1, 8); // 串口1 io初始化
    gpio_init_pin(GPIO_C, 11, 2, 0, 2, 1, 8);
    usart_init(UART4, 0xa1, 4);
    baud_rate_set(UART4, 120000000, 9, 115200);
    usart_set_hardware_fifo(UART4, 5, 5);
    enable_usart(UART4);
#endif
}