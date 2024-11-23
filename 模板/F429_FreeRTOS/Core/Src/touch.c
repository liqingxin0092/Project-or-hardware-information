#include "touch.h"

/*引脚映射*/
// INT  -- PH7  //T
// SCL -- PH6   //T
// SDA  -- PI3  //T
// RST -- PI8 //T

_m_tp_dev tp_dev={0,0};        

const uint16_t GT9XXX_TPX_TBL[10] =
{
    GT9XXX_TP1_REG, GT9XXX_TP2_REG, GT9XXX_TP3_REG, GT9XXX_TP4_REG, GT9XXX_TP5_REG,
    GT9XXX_TP6_REG, GT9XXX_TP7_REG, GT9XXX_TP8_REG, GT9XXX_TP9_REG, GT9XXX_TP10_REG,
};

static void ct_iic_delay(void)
{
    delay_us(1);
}

void ct_iic_start(void)
{
    SDA_OUT=1;
    SCL=1;
    ct_iic_delay();
    SDA_OUT=0;      /* START信号: 当SCL为高时, SDA从高变成低, 表示起始信号 */
    ct_iic_delay();
    SCL=0;      /* 钳住I2C总线，准备发送或接收数据 */
    ct_iic_delay();
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void ct_iic_stop(void)
{
    SDA_OUT=0;      /* STOP信号: 当SCL为高时, SDA从低变成高, 表示停止信号 */
    ct_iic_delay();
    SCL=1;
    ct_iic_delay();
    SDA_OUT=1;      /* 发送I2C总线结束信号 */
    ct_iic_delay();
}

void ct_iic_wait_ack(void)
{
    SDA_OUT=1;      /* 主机释放SDA线(此时外部器件可以拉低SDA线) */
    ct_iic_delay();
    SCL=1;      /* SCL=1, 此时从机可以返回ACK */
    ct_iic_delay();
    while (SDA_IN); /* 等待应答 */
    SCL=0;      /* SCL=0, 结束ACK检查 */
    ct_iic_delay();
}
/**
 * @brief       产生ACK应答
 * @param       无
 * @retval      无
 */
void ct_iic_ack(void)
{
    SDA_OUT=0;  /* SCL 0 -> 1  时SDA = 0,表示应答 */
    ct_iic_delay();
    SCL=1;
    ct_iic_delay();
    SCL=0;
    ct_iic_delay(); 
    SDA_OUT=1;  /* 主机释放SDA线 */
    ct_iic_delay(); 
}

/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
void ct_iic_nack(void)
{
    SDA_OUT=1;  /* SCL 0 -> 1  时 SDA = 1,表示不应答 */
    ct_iic_delay();
    SCL=1;
    ct_iic_delay();
    SCL=0;
    ct_iic_delay();
}

/**
 * @brief       IIC发送一个字节
 * @param       data: 要发送的数据
 * @retval      无
 */
void ct_iic_send_byte(uint8_t data)
{
    uint8_t t;
    for (t = 0; t < 8; t++)
    {
        SDA_OUT=((data & 0x80) >> 7); /* 高位先发送 */
        ct_iic_delay();
        SCL=1;
        ct_iic_delay();
        SCL=0;
        data <<= 1;     /* 左移1位,用于下一次发送 */
    }
    SDA_OUT=1;      /* 发送完成, 主机释放SDA线 */
}

/**
 * @brief       IIC发送一个字节
 * @param       ack:  ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t ct_iic_read_byte(unsigned char ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* 接收1个字节数据 */
    {
        receive <<= 1;  /* 高位先输出,所以先收到的数据位要左移 */
        SCL=1;
        ct_iic_delay();
        if (SDA_IN)
        {
            receive++;
        }
        SCL=0;
        ct_iic_delay();
    }
    if (!ack)
    {
        ct_iic_nack();  /* 发送nACK */
    }
    else
    {
        ct_iic_ack();   /* 发送ACK */
    }
    return receive;
}

/**
 * @brief       从gt9xxx读出一次数据
 * @param       reg : 起始寄存器地址
 * @param       buf : 数据缓缓存区
 * @param       len : 读数据长度
 * @retval      无
 */
void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);    /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);         /* 发送高8位地址 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0XFF);       /* 发送低8位地址 */
    ct_iic_wait_ack();
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_RD);    /* 发送读命令 */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);  /* 发数据 */
    }
    ct_iic_stop();  /* 产生一个停止条件 */
}

/**
 * @brief       向gt9xxx写入一次数据
 * @param       reg : 起始寄存器地址
 * @param       buf : 数据缓缓存区
 * @param       len : 写数据长度
 * @retval      0, 成功; 1, 失败;
 */
void gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);    /* 发送写命令 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);         /* 发送高8位地址 */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0XFF);       /* 发送低8位地址 */
    ct_iic_wait_ack();

    for (i = 0; i < len; i++)
    {
        ct_iic_send_byte(buf[i]);       /* 发数据 */
       ct_iic_wait_ack();
    }
    ct_iic_stop();  /* 产生一个停止条件 */
}

void Touch_init(void)
{
    uint8_t temp[5];
    
    /*IO初始化*/
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*INT*/
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;       // 输入
    GPIO_InitStruct.Pull = GPIO_PULLUP;           // 上拉,这里是为了满足时序.
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; // speed
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    //让通讯地址成为0x28/0x29
    
    /*RST*/
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 推挽
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
    
    /*SCL*/
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    
    /*SDA*/
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
    
    /*复位*/
    __BIT_SET(GPIOI_ODR_Addr,8,0);
    delay_ms(40);
    __BIT_SET(GPIOI_ODR_Addr,8,1);
    delay_ms(40);
    
    /*INT*/
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;       // 输入
    GPIO_InitStruct.Pull = GPIO_NOPULL;           // 浮空
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    
    temp[0] = 0X02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);    /* 软复位GT9XXX */
    
    delay_ms(10);
    
    temp[0] = 0X00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);    /* 结束复位, 进入读坐标状态 */
}

/**
 * @brief       扫描触摸屏(采用查询方式)
 * @param       mode : 电容屏未用到次参数, 为了兼容电阻屏
 * @retval      当前触屏状态
 *   @arg       0, 触屏无触摸; 
 *   @arg       1, 触屏有触摸;
 */
uint8_t touch_num;
uint8_t touch_flag;
uint8_t gt9xxx_scan(uint8_t mode)
{
    uint8_t buf[4];
    uint8_t i = 0;
    static uint8_t t = 0;   /* 控制查询间隔,从而降低CPU占用率 */
    t++;
    if ((t % 50) == 0 || t < 50) //进10次这个函数才干活
    {
        t=0;//复位计数标志
        
        /* 读取触摸点的状态*/
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1); 
        
        if (mode & 0X80)//如果触摸
        {
            touch_num=mode&0xf;//记录下数量
            touch_flag= ~(0xff<<touch_num);
            /*读完状态手动清 */
            i = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1); 
            
            for (i = 0; i <touch_num ; i++)
            {
                gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4); //这是个数组,存了寄存器地址
                tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
            }
        }
        else 
        {
            touch_num=0;
            touch_flag=0;
        }
    }
    return touch_flag;
}



