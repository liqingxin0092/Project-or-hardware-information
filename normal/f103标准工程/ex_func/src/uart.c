#include "uart.h"
#include "stm32f10x.h"   
#include "stdio.h"
#include "string.h"

/*串口1*/
volatile uint8_t  uart1_receive;
volatile uint16_t uart1_current_length;     //这个会得到实际的字符数+2
volatile uint8_t  uart1_buffer[UART1_BUFFER_LENGTH];

/*串口2*/
volatile uint8_t  uart2_receive;
volatile uint16_t uart2_current_length;     //这个会得到实际的字符数+2
volatile uint8_t  uart2_buffer[UART2_BUFFER_LENGTH];

/*串口3*/
volatile uint8_t  uart3_receive;
volatile uint16_t uart3_current_length;     //这个会得到实际的字符数+2
volatile uint8_t  uart3_buffer[UART3_BUFFER_LENGTH];

//串口1初始化
void USART1_Config(void)		//初始化 配置USART1
{
	GPIO_InitTypeDef     GPIO_InitStructure;   //串口端口配置结构体变量
	USART_InitTypeDef    USART_InitStructure;  //串口参数配置结构体变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//打开串口复用时钟
	USART_DeInit(USART1);  //复位串口1

	//第2步：将USART1 Tx（发送脚）的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			   //串口1发送脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //输出速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   //初始化GPIOA
														  
	//第3步：将USART Rx（接收脚）的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			   //串口1接收脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   //初始化GPIOA

	//第4步：配置USART1参数
	USART_InitStructure.USART_BaudRate             = 9600;							 //波特率设置：115200
	USART_InitStructure.USART_WordLength           = USART_WordLength_8b;			 //数据位数设置：8位
	USART_InitStructure.USART_StopBits             = USART_StopBits_1;				 //停止位设置：1位
	USART_InitStructure.USART_Parity               = USART_Parity_No;				 //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl  = USART_HardwareFlowControl_None; //硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;	 //接收与发送都使能
	USART_Init(USART1, &USART_InitStructure);										 //初始化USART1

  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能指定的USART1接收中断 ；
    NVIC_EnableIRQ(USART1_IRQn);
    
	//第5步：使能 USART1， 配置完毕
	USART_Cmd(USART1, ENABLE);							   //使能 USART1

    //如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART1, USART_FLAG_TC);                //清串口1发送标志
}


//串口2初始化
void USART2_Config(void)   //初始化 配置USART2
{
	GPIO_InitTypeDef    GPIO_InitStructure;	   //串口端口配置结构体变量
	USART_InitTypeDef   USART_InitStructure;   //串口参数配置结构体变量

	//使能 USART2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//打开串口复用时钟

	//将USART2 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				  //选定哪个IO口 现选定PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);    				  //初始化GPIOA

	//将USART2 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				  //选定哪个IO口 现选定PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);                    //初始化GPIOA
	  
	//配置USART2参数
	USART_InitStructure.USART_BaudRate = 9600;	                    //波特率设置：115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(USART2, &USART_InitStructure);                       //初始化USART2
	
  	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能指定的USART2接收中断
    NVIC_EnableIRQ(USART2_IRQn);
    
	//使能 USART2， 配置完毕
	USART_Cmd(USART2, ENABLE);                             // USART2使能

	//如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART2, USART_FLAG_TC);                //清串口2发送标志
}

//串口3初始化
void USART3_Config(void)   //初始化 配置USART3
{
	GPIO_InitTypeDef GPIO_InitStructure;	  //串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;	  //串口参数配置结构体变量

	//使能 USART3 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//打开串口复用时钟
  	
	//将USART3 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //选定哪个IO口 现选定PB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);    				  //初始化GPIOB
	
	//将USART3 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //选定哪个IO口 现选定PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	          //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);                    //初始化GPIOA
	  
	//配置USART3参数
	USART_InitStructure.USART_BaudRate = 9600;	                    //波特率设置：2400
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(USART3, &USART_InitStructure);                       //初始化USART3
	
  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 使能指定的USART3接收中断
    NVIC_EnableIRQ(USART3_IRQn);
    
	//使能 USART3， 配置完毕
	USART_Cmd(USART3, ENABLE);                             // USART3使能

	//如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART3, USART_FLAG_TC);                //清串口3发送标志
}

int fputc(int ch, FILE *f)
{   
    UARTx->DR = (uint8_t)ch;
    while (!(UARTx->SR & 0X40));         
    return ch;
}

//串口发送函数
void uart_transmit(USART_TypeDef * USARTx, uint8_t *ptr,uint16_t length )
{
    for(uint16_t i=0;i<length;i++)
    {
        USARTx->DR =ptr[i];
        while (!(USARTx->SR & 0X40));                  
    }
}

//串口1接收
void USART1_IRQHandler(void)
{
    if(USART1->SR&0x20) //有数中断
    {
        uint8_t temp= USART1->DR;
        uart1_buffer[uart1_current_length++]=temp;
        if(temp=='J')
        {
             uart1_receive=1;//接收到了
        }        
    }        
}

//串口2接收
void USART2_IRQHandler(void)
{
    if(USART2->SR&0x20) //有数中断
    {
        uint8_t temp= USART2->DR;
        uart2_buffer[uart2_current_length++]=temp;
        if(temp=='J')
        {
             uart2_receive=1;//接收到了
        }        
    }        
}

//串口3接收
void USART3_IRQHandler(void)
{
    if(USART3->SR&0x20) //有数中断
    {
        uint8_t temp= USART3->DR;
        uart3_buffer[uart3_current_length++]=temp;
        if(temp=='J')
        {
             uart3_receive=1;//接收到了
        }        
    }        
}

