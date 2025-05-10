#include "spi_flash.h"

/**********************************************************
 * �� �� �� �ƣ�bsp_spi_init
 * �� �� �� �ܣ���ʼ��SPI
 * �� �� �� ������
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
**********************************************************/
void bsp_spi_init(void)
{
	rcu_periph_clock_enable(BSP_GPIO_RCU);    // ʹ��A�˿�
	rcu_periph_clock_enable(BSP_SPI_RCU);     // ʹ��SPI0
	
	//���Ÿ���
	gpio_af_set(BSP_GPIO_PORT, GPIO_AF_5, BSP_SPI_SCK);
	gpio_af_set(BSP_GPIO_PORT, GPIO_AF_5, BSP_SPI_MISO);
	gpio_af_set(BSP_GPIO_PORT, GPIO_AF_5, BSP_SPI_MOSI);
	//����ģʽ
	gpio_mode_set(BSP_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_SPI_SCK);
	gpio_mode_set(BSP_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_SPI_MISO);
	gpio_mode_set(BSP_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_SPI_MOSI);
	//���ģʽ
	gpio_output_options_set(BSP_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_SPI_SCK);
	gpio_output_options_set(BSP_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_SPI_MISO);
	gpio_output_options_set(BSP_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_SPI_MOSI);
		
	//����CS����ʱ��
	rcu_periph_clock_enable(BSP_SPI_NSS_RCU);
	//����CS����ģʽ
	gpio_mode_set(BSP_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, BSP_SPI_NSS);
	//����CS���ģʽ
	gpio_output_options_set(BSP_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BSP_SPI_NSS);
	//GD25Q32��ѡ��
	gpio_bit_write(BSP_GPIO_PORT, BSP_SPI_NSS, SET);
	
	//SPI��������ṹ��
	spi_parameter_struct spi_init_struct;
	spi_init_struct.trans_mode        		= SPI_TRANSMODE_FULLDUPLEX;  // ����ģʽȫ˫��
	spi_init_struct.device_mode       		= SPI_MASTER;                // ����Ϊ����
	spi_init_struct.frame_size         		= SPI_FRAMESIZE_8BIT;        // 8λ����
	spi_init_struct.clock_polarity_phase 	= SPI_CK_PL_HIGH_PH_2EDGE;   // ������λ  
	spi_init_struct.nss               		= SPI_NSS_SOFT;              // ���cs
	spi_init_struct.prescale           		= SPI_PSC_4;                 // SPIʱ��Ԥ������Ϊ4
	spi_init_struct.endian            		= SPI_ENDIAN_MSB;            // ��λ��ǰ
	//����������SPI0
	spi_init(BSP_SPI, &spi_init_struct);
	//ʹ��SPI
	spi_enable(BSP_SPI);
}

uint8_t spi_read_write_byte(uint8_t dat)
{
    //�ȴ����ͻ�����Ϊ��
    while(RESET == spi_i2s_flag_get(BSP_SPI,  SPI_FLAG_TBE) );
    //ͨ��SPI4����һ���ֽ����� 
    spi_i2s_data_transmit(BSP_SPI, dat);
 
    //�ȴ����ջ��������ձ�־
    while(RESET == spi_i2s_flag_get(BSP_SPI,  SPI_FLAG_RBNE) );
    //��ȡ��������SPI4��ȡ���ĵ��ֽ�����
    return spi_i2s_data_receive(BSP_SPI);
}

//��ȡоƬID          
//��ȡ�豸ID
uint16_t GD25Q32_readID(void)
{
    uint16_t  temp = 0;     
    //��CS������Ϊ�͵�ƽ     
    W25QXX_CS_ON(0);        
    //����ָ��90h    
    spi_read_write_byte(0x90);//���Ͷ�ȡID����      
    //���͵�ַ  000000H    
    spi_read_write_byte(0x00);             
    spi_read_write_byte(0x00);             
    spi_read_write_byte(0x00); 
        
    //��������
    //����������ID
    temp |= spi_read_write_byte(0xFF)<<8;  
    //�����豸ID
    temp |= spi_read_write_byte(0xFF);        
    //�ָ�CS��Ϊ�ߵ�ƽ
    W25QXX_CS_ON(1);      
    //����ID                  
    return temp;
}

//����дʹ��
void GD25Q32_write_enable(void)   
{
    //����CS��Ϊ�͵�ƽ
    W25QXX_CS_ON(0);                          
    //����ָ��06h
    spi_read_write_byte(0x06);                  
    //����CS��Ϊ�ߵ�ƽ
    W25QXX_CS_ON(1);
}

/**********************************************************
 * �� �� �� �ƣ�GD25Q32_wait_busy
 * �� �� �� �ܣ������·�Ƿ�æ
 * �� �� �� ������
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
**********************************************************/
void GD25Q32_wait_busy(void)   
{   
    unsigned char byte = 0;
    do
     { 
        //����CS��Ϊ�͵�ƽ
        W25QXX_CS_ON(0); 
        //����ָ��05h                           
        spi_read_write_byte(0x05);                
        //����״̬�Ĵ���ֵ
        byte = spi_read_write_byte(0Xff);       
        //�ָ�CS��Ϊ�ߵ�ƽ
        W25QXX_CS_ON(1);      
     //�ж�BUSYλ�Ƿ�Ϊ1 ���Ϊ1˵����æ�����¶�дBUSYλֱ��Ϊ0   
     }while( ( byte & 0x01 ) == 1 );  
}

/**********************************************************
 * �� �� �� �ƣ�GD25Q32_erase_sector
 * �� �� �� �ܣ�����һ������
 * �� �� �� ����addr=������������
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע��addr=�����������ţ���Χ=0~15
**********************************************************/
void GD25Q32_erase_sector(uint32_t addr)   
{
        //���������ţ�һ������4KB=4096
        addr *= 4096;
        GD25Q32_write_enable();  //дʹ��   
        GD25Q32_wait_busy();     //�ж�æ�����æ��һֱ�ȴ�
        //����CS��Ϊ�͵�ƽ
        W25QXX_CS_ON(0);  
        //����ָ��20h                                     
        spi_read_write_byte(0x20);
        //����24λ������ַ�ĸ�8λ                
        spi_read_write_byte((uint8_t)((addr)>>16));      
        //����24λ������ַ����8λ    
        spi_read_write_byte((uint8_t)((addr)>>8));   
        //����24λ������ַ�ĵ�8λ    
        spi_read_write_byte((uint8_t)addr);
        //�ָ�CS��Ϊ�ߵ�ƽ  
        W25QXX_CS_ON(1);                  
        //�ȴ��������                                                  
        GD25Q32_wait_busy();   
}

/**********************************************************
 * �� �� �� �ƣ�GD25Q32_write
 * �� �� �� �ܣ�д���ݵ�GD25Q32���б���
 * �� �� �� ����buffer=д�����������        addr=д���ַ        numbyte=д�����ݵĳ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
**********************************************************/
void GD25Q32_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte)
{    
    unsigned int i = 0;
    //������������
    GD25Q32_erase_sector(addr/4096);
    //дʹ�� 
    GD25Q32_write_enable();  
    //æ���  
    GD25Q32_wait_busy();    
    //д������
    //����CS��Ϊ�͵�ƽ
    W25QXX_CS_ON(0);         
    //����ָ��02h                              
    spi_read_write_byte(0x02);                 
    //����д���24λ��ַ�еĸ�8λ   
    spi_read_write_byte((uint8_t)((addr)>>16));  
    //����д���24λ��ַ�е���8λ
    spi_read_write_byte((uint8_t)((addr)>>8));   
    //����д���24λ��ַ�еĵ�8λ
    spi_read_write_byte((uint8_t)addr);   
    //����д����ֽڳ�������д������buffer
    for(i=0;i<numbyte;i++)
    {
        spi_read_write_byte(buffer[i]);  
    }
    //�ָ�CS��Ϊ�ߵ�ƽ
    W25QXX_CS_ON(1);
    //æ��� 
    GD25Q32_wait_busy();      
}

/**********************************************************
 * �� �� �� �ƣ�GD25Q32_read
 * �� �� �� �ܣ���ȡGD25Q32������
 * �� �� �� ����buffer=�������ݵı����ַ  read_addr=��ȡ��ַ   read_length=��ȥ����
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
**********************************************************/
void GD25Q32_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length)   
{ 
        uint16_t i;                   
        //����CS��Ϊ�͵�ƽ
        W25QXX_CS_ON(0);    
        //����ָ��03h        
        spi_read_write_byte(0x03);  
        //����24λ��ȡ���ݵ�ַ�ĸ�8λ                         
        spi_read_write_byte((uint8_t)((read_addr)>>16));     
        //����24λ��ȡ���ݵ�ַ����8λ      
        spi_read_write_byte((uint8_t)((read_addr)>>8));   
        //����24λ��ȡ���ݵ�ַ�ĵ�8λ
        spi_read_write_byte((uint8_t)read_addr);   
        //���ݶ�ȡ���ȶ�ȡ����ַ���浽buffer��
        for(i=0;i<read_length;i++)
        { 
            buffer[i]= spi_read_write_byte(0XFF);  
        }
        //�ָ�CS��Ϊ�ߵ�ƽ
        W25QXX_CS_ON(1);                                    
}
