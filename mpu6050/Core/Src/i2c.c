#include "i2c.h"
void deal(uint16_t Pin, GPIO_PinState PinState)
{
    if (PinState == 1)
    {
        GPIO_I2C->ODR |= 1 << Pin;
    }
    else if (PinState == 0)
    {
        GPIO_I2C->ODR &= ~(1 << Pin);
    }
}
void start()
{
    SCL(1); // �ڸߵ�ʱ���SDA����ʾ����ʼ����ֹ
    SDA(1);
    SDA(0);
    SCL(0);
}
void restart()
{
    SDA(1);
    SCL(1);
    SDA(0);
    SCL(0);
}
void stop()
{
    SDA(0); // ��ΪSDA�Ŀ�ʼ��ƽ��ȷ�����Ȳ���SDA
    SCL(0);
    SCL(1);
    SDA(1);
}
void send(uint8_t bite) // ��һ���ֽ�
{
    for (uint8_t i = 0; i < 8; i++)
    {
        SCL(0);
        SDA((bite & (0x80 >> i)) >> (7 - i));
        SCL(1);
    }
    SCL(0);
}
uint8_t receive() // ��һ���ֽ�
{
    uint8_t receive_data = 0;
    SDA(1); // �����ͷ�SDA����©�ߵ�ƽ���൱������
    for (uint8_t i = 0; i < 8; i++)
    {
        SCL(0); // �ӻ���
        SCL(1); // ������
        receive_data |= (((GPIO_I2C->IDR) >> GPIO_PIN_SDA) & 1) << (7 - i);
    }
    SCL(0);
    return receive_data;
}
void send_ack(uint8_t i)
{
    SCL(0); // ��ʼ�ı�
    SDA(i); // Ӧ��
    SCL(1); // �ӻ���ȡ
    SCL(0); // ��һ��
}
uint8_t receive_ack(void)
{
    SDA(1); // �����ͷ�
    uint8_t receive_ack;
    SCL(0); // �ӻ�������
    SCL(1); // ������ȡ
    receive_ack = ((GPIO_I2C->IDR) >> GPIO_PIN_SDA) & 1;
    SCL(0); // ��һ��
    return receive_ack;
}
// ָ����ַдһ���ֽ�
void WriteReg(uint8_t address, uint8_t reg_address, uint8_t data)
{
    start();

    send(address); // �����豸��ַ
    receive_ack(); // �жϴӻ���û�н���Ӧ��Ϊ��ƴ��ʱ��

    send(reg_address); // �����豸�Ĵ�����ַ
    receive_ack();

    send(data); // ָ����ַ�µ�����
    receive_ack();

    stop();
}
uint16_t ReadReg(uint8_t address, uint8_t reg_address)
{
    uint16_t data;
    start();

    send(address); // �豸��ַ
    receive_ack(); // �жϴӻ���û�н���Ӧ��

    send(reg_address); // �����豸�Ĵ�����ַ
    receive_ack();

    restart();
    send(address | 1); // ��˼��Ҫ����
    receive_ack();

    data = receive();
    send_ack(1); // ��Ӧ��

    stop();
    return data;
}

void ReadRegs(uint8_t address, uint8_t reg_address, uint8_t *buffer, uint8_t lenth)
{
    uint8_t i;
    start();

    send(address); // �豸��ַ
    receive_ack(); // �жϴӻ���û�н���Ӧ��

    send(reg_address); // �����豸�Ĵ�����ַ
    receive_ack();

    restart();
    send(address | 1); // ��˼��Ҫ����
    receive_ack();
    for (i = 0; i < lenth - 1; i++)
    {
        *(buffer + i) = receive();
        send_ack(0); // Ӧ��
    }
    *(buffer + i) = receive();
    send_ack(1); // ��Ӧ��

    stop();
}

// Ӳ����ʽ
void GPIOx_I2c_ToFast_Init()
{
    // I2C1��ӳ��
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // ʹ�ܸ���ʱ��
    AFIO->MAPR |= 1 << 1;

    // ��ʼ��I2c����
    RCC->APB1ENR |= 1 << 21; // ����i2c1����ʱ��
    I2C1->CR2 |= 11110;      // ����ʱ��Ƶ��30MHZ
    I2C1->CR2 &= ~((1 << 11) | (1 << 8) | (1 << 9));
    I2C1->CCR |= 1 << 15;    // ����ģʽ
    I2C1->CCR &= ~(1 << 14); // ռ�ձ�1��2
    I2C1->CCR |= 25;         // ��ʱ��Ƶ�� ��400khz
    I2C1->TRISE = 7;         // ��������ʱ�� 200ns

    // ��ʼ��GPIO��
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // ����GPIOBʱ��
    GPIOB->CRH = 0x444444ff;            // PB8,9�˿�����Ϊ���ÿ�©��� 50MHZ
    GPIOB->ODR |= (1 << 8) | (1 << 9);  // ����
    // i2cģ��ʹ��
    I2C1->CR1 |= 1;
}
void Hardare_WriteReg(uint8_t address, uint8_t reg_address, uint8_t data)
{

    uint8_t i = 0;

    I2C1->CR1 |= 1 << 8; // ������ʼ����
    while (((I2C1->SR1) & 1) == 0)
        ; // �ȴ���ʼ�������� ��

    I2C1->DR = address; // ���͵�ַ
    while ((((I2C1->SR1) >> 1) & 1) == 0)
        ; // �ȴ���ַ���ͽ���   ��

    I2C1->DR = reg_address; // ���ͼĴ�����ַ
    I2C1->DR = data;        // ��������
    while ((((I2C1->SR1) >> 2) & 1) == 1)
        ;

    I2C1->CR1 |= 1 << 9; // ������ֹ����
}
uint16_t Hardare_ReadReg(uint8_t address, uint8_t reg_address)
{
    uint16_t data = 0;

    I2C1->CR1 |= 1 << 8; // ������ʼ����
    while (((I2C1->SR1) & 1) == 0)
        ; // �ȴ���ʼ��������

    I2C1->DR = address; // ���͵�ַ
    while ((((I2C1->SR1) >> 1) & 1) == 0)
        ; // �ȴ���ַ���ͽ���

    I2C1->DR = reg_address; // ���ͼĴ�����ַ
    while ((((I2C1->SR1) >> 2) & 1) == 1)
        ;

    I2C1->CR1 |= 1 << 9; // ������ֹ����

    I2C1->CR1 |= 1 << 10; // Ӧ��ʹ��
    I2C1->CR1 |= 1 << 8;  // ������ʼ����
    while (((I2C1->SR1) & 1) == 0)
        ; // �ȴ���ʼ��������

    I2C1->DR = address | 1; // ���͵�ַ
    while ((((I2C1->SR1) >> 1) & 1) == 1)
        ; // �ȴ���ַ���ͽ���

    I2C1->CR1 &= ~(1 << 10); // Ӧ��ʧ��

    data = I2C1->DR;
    while ((((I2C1->SR1) >> 2) & 1) == 1)
        ;
    data = I2C1->DR;
    I2C1->CR1 |= 1 << 9; // ������ֹ����
    data = I2C1->DR;
    while ((((I2C1->SR1) >> 6) & 1) == 0)
        ;

    return data;
}
