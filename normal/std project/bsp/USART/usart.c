
#include "usart.h"
#include "stdio.h"

uint8_t 	g_recv_buff[USART_RECEIVE_LENGTH];   // ���ջ�����
uint16_t 	g_recv_length = 0;					 // �������ݳ���
uint8_t		g_recv_complete_flag = 0; 			 // ����������ɱ�־λ

/************************************************
�������� �� usart_gpio_config
��    �� �� ��������GPIO
��    �� �� band_rate:������
�� �� ֵ �� ��
��    �� �� LC
*************************************************/
void usart_gpio_config(uint32_t band_rate)
{
	/* ����ʱ�� */
	rcu_periph_clock_enable(BSP_USART_TX_RCU);   // ��������ʱ��
	rcu_periph_clock_enable(BSP_USART_RX_RCU);   // �����˿�ʱ��
	rcu_periph_clock_enable(BSP_USART_RCU);      // �����˿�ʱ��
	
	/* ����GPIO���ù��� */
	gpio_af_set(BSP_USART_TX_PORT,BSP_USART_AF,BSP_USART_TX_PIN);	
	gpio_af_set(BSP_USART_RX_PORT,BSP_USART_AF,BSP_USART_RX_PIN);	
	
	/* ����GPIO��ģʽ */
	/* ����TXΪ����ģʽ ����ģʽ */
	gpio_mode_set(BSP_USART_TX_PORT,GPIO_MODE_AF,GPIO_PUPD_PULLUP,BSP_USART_TX_PIN);
	/* ����RXΪ����ģʽ ����ģʽ */
	gpio_mode_set(BSP_USART_RX_PORT, GPIO_MODE_AF,GPIO_PUPD_PULLUP,BSP_USART_RX_PIN);
	
	/* ����TXΪ������� 50MHZ */
	gpio_output_options_set(BSP_USART_TX_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BSP_USART_TX_PIN);
	/* ����RXΪ������� 50MHZ */
	gpio_output_options_set(BSP_USART_RX_PORT,GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_USART_RX_PIN);

	/* ���ô��ڵĲ��� */
	usart_deinit(BSP_USART);                                 // ��λ����
	usart_baudrate_set(BSP_USART,band_rate);                 // ���ò�����
	usart_parity_config(BSP_USART,USART_PM_NONE);            // û��У��λ
	usart_word_length_set(BSP_USART,USART_WL_8BIT);          // 8λ����λ
	usart_stop_bit_set(BSP_USART,USART_STB_1BIT);     			 // 1λֹͣλ

  /* ʹ�ܴ��� */
	usart_enable(BSP_USART);                                 // ʹ�ܴ���
	usart_transmit_config(BSP_USART,USART_TRANSMIT_ENABLE);  // ʹ�ܴ��ڷ���
	usart_receive_config(BSP_USART,USART_RECEIVE_ENABLE);    // ʹ�ܴ��ڽ���
	
    /* �ж����� */
    nvic_irq_enable(BSP_USART_IRQ, 2, 2);              // �����ж����ȼ�
#if !USB_USART_DMA                                     // ʹ���ж�
    usart_interrupt_enable(BSP_USART, USART_INT_RBNE); // �����ݻ������ǿ��жϺ���������ж�
#endif
    usart_interrupt_enable(BSP_USART, USART_INT_IDLE); // ���м���ж�
	
}


/************************************************
�������� �� usart_send_data
��    �� �� �����ط���һ���ֽ�
��    �� �� ucch��Ҫ���͵��ֽ�
�� �� ֵ �� 
��    �� �� LC
*************************************************/
void usart_send_data(uint8_t ucch)
{
	usart_data_transmit(BSP_USART,(uint8_t)ucch);							 // ��������
	while(RESET == usart_flag_get(BSP_USART,USART_FLAG_TBE));  // �ȴ��������ݻ�������־��λ
}


/************************************************
�������� �� usart_send_String
��    �� �� ���ڷ����ַ���
��    �� �� ucstr:Ҫ���͵��ַ���
�� �� ֵ �� 
��    �� �� LC
*************************************************/
void usart_send_string(uint8_t *ucstr)
{
	while(ucstr && *ucstr)        // ��ַΪ�ջ���ֵΪ������
	{
	  usart_send_data(*ucstr++);  // ���͵����ַ�
	}
}

#if !defined(__MICROLIB)
//��ʹ��΢��Ļ�����Ҫ�������ĺ���
#if (__ARMCLIB_VERSION <= 6000000)
//�����������AC5  �Ͷ�����������ṹ��
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
#endif

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}

/************************************************
�������� �� BSP_USART_IRQHandler
��    �� �� ���ڽ����жϷ�����
��    �� �� ��
�� �� ֵ �� ��
��    �� �� LC
*************************************************/
void BSP_USART_IRQHandler(void)
{
#if !USB_USART_DMA                                                       // ʹ���ж�
    if (usart_interrupt_flag_get(BSP_USART, USART_INT_FLAG_RBNE) == SET) // ���ջ�������Ϊ��
    {
        g_recv_buff[g_recv_length++] = usart_data_receive(BSP_USART); // �ѽ��յ������ݷŵ���������
    }
#endif

    if (usart_interrupt_flag_get(BSP_USART, USART_INT_FLAG_IDLE) == SET) // ��⵽֡�ж�
    {
		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_IDLE);
        usart_data_receive(BSP_USART); // ����Ҫ������������ֵ����Ҫ

#if USB_USART_DMA // ʹ��DMA
        /* ����DMA���յ������� */
        g_recv_length = USART_RECEIVE_LENGTH - dma_transfer_number_get(DMA1, BSP_DMA_CH); // ����ʵ�ʽ��յ����ݳ���
        /* ��������DMA���� */
        dma_channel_disable(DMA1, BSP_DMA_CH); 		  // ʧ��DMAͨ��
        usart_dma_config();                           // ��������DMA���д���
#endif

        g_recv_buff[g_recv_length] = '\0'; // ���ݽ�����ϣ����������־
        g_recv_complete_flag = 1;          // �������
    }
}



/*!
    \brief      configure USART DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_dma_config(void)
{
    dma_single_data_parameter_struct dma_init_struct;
    /* enable DMA1 */
    rcu_periph_clock_enable(RCU_DMA1);
	
    /* ��ʼ��DMAͨ�� */
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_deinit(DMA1, BSP_DMA_CH);
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr = (uint32_t)g_recv_buff;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_init_struct.number = ARRAYNUM(g_recv_buff);
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(BSP_USART);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA1, BSP_DMA_CH, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA1, BSP_DMA_CH);
    dma_channel_subperipheral_select(DMA1, BSP_DMA_CH, DMA_SUBPERI4);
 	
	 /* enable USART0 DMA channel transmission and reception */
    dma_channel_enable(DMA1, BSP_DMA_CH);
    

	/* ʹ��DMAͨ���ж� */
	nvic_irq_enable(BSP_DMA_CH_IRQ,2,1); 
	/* ʹ��DMA�ж� */
	dma_interrupt_enable(DMA1,BSP_DMA_CH,DMA_CHXCTL_FTFIE);  // ʹ�ܴ�������ж�
	
	/* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
}

void BSP_DMA_CH_IRQHandler(void)
{    
    /* ��������ж� */ 
    if(dma_interrupt_flag_get(DMA1, BSP_DMA_CH, DMA_INT_FLAG_FTF))
     {   
         dma_interrupt_flag_clear(DMA1, BSP_DMA_CH, DMA_INT_FLAG_FTF);
        //  g_transfer_complete = SET;  // ���ݴ������  
    }
}

