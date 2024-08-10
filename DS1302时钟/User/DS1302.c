#include "stm32f10x.h"
#include "DS1302.h"
#include "Delay.h"

static uint8_t read(uint8_t pin)
{
    if ((GPIO_DS1302->IDR >> pin) & 1)
        return 1;
    else
        return 0;
}

void DS1302_Init()
{
    RCC->APB2ENR |= DS1302_RCC|0xd; // ����ʱ��
    GPIO_Init();                // ����GPIO��
    OUTPUT(RST_Pin, 0);         // RST����״̬�͵�ƽ
    OUTPUT(CLK_Pin, 0);         // CLK����״̬�͵�ƽ
}
void WriteByte(uint8_t command, uint8_t data)
{
    uint8_t i;
    OUTPUT(RST_Pin, 1); // RST�ߵ�ƽ  //����һ��ʱ��
    Delay_us(1);
    for (i = 0; i < 8; i++)
    {
        if (((command >> i) & 1) == 1) // ��������
            OUTPUT(DA_Pin, 1);
        else
            OUTPUT(DA_Pin, 0);

        OUTPUT(CLK_Pin, 1); // ʱ���߸�
        Delay_us(1);
        OUTPUT(CLK_Pin, 0); // ʱ���ߵ�
    }
    for (i = 0; i < 8; i++)
    {
        if (((data >> i) & 1) == 1) // ��������
            OUTPUT(DA_Pin, 1);
        else
            OUTPUT(DA_Pin, 0);

        OUTPUT(CLK_Pin, 1); // ʱ���߸�
        Delay_us(1);
        OUTPUT(CLK_Pin, 0); // ʱ���ߵ�
    }

    OUTPUT(RST_Pin, 0); // RST�͵�ƽ  //����һ��ʱ��
}

uint8_t ReadByte(uint8_t command)
{
    uint8_t data = 0;
    uint8_t i;
    OUTPUT(RST_Pin, 1); // RST�ߵ�ƽ  //����һ��ʱ��
    Delay_us(1);
    for (i = 0; i < 8; i++)
    {
        if (((command >> i) & 1) == 1) // ��������
            OUTPUT(DA_Pin, 1);
        else
            OUTPUT(DA_Pin, 0);

        OUTPUT(CLK_Pin, 0); // ʱ���ߵ�
        OUTPUT(CLK_Pin, 1); // ʱ���߸�
        Delay_us(1);
    }

    GPIO_DS1302->CRH = 0x44444434; // ����������Ϊ����ģʽ

    for (i = 0; i < 8; i++)
    {
        OUTPUT(CLK_Pin, 1); // ʱ���߸�
        OUTPUT(CLK_Pin, 0); // ʱ���ߵ�

        if (read(DA_Pin) == 1) // ��ȡ
            data |= 1 << i;
    }
    OUTPUT(RST_Pin, 0); // RST�͵�ƽ  //����һ��ʱ��

    GPIO_DS1302->CRH = 0x44444433; // ����������Ϊ���ģʽ
    return data;
}
void WriteEnable(void)
{
    WriteByte(WP_W, 0); // ���д����
}

void WriteDisable(void)
{
    WriteByte(WP_W, 1); // ����д����
}

void SetSecond(uint8_t sec)
{
    WriteEnable();
    WriteByte(SEC_W, sec / 10 * 16 + sec % 10);
    WriteDisable();
}
uint16_t ReadSeconcd()
{
    uint8_t temp;
    temp = ReadByte(SEC_R); // ����Ĵ���
    return temp / 16 * 10 + temp % 16;
}
void SetMin(uint8_t min)
{
    WriteEnable();
    WriteByte(MIN_W, min / 10 * 16 + min % 10);
    WriteDisable();
}
uint8_t ReadMin()
{
    uint8_t temp;
    temp = ReadByte(MIN_R);
    return temp / 16 * 10 + temp % 16;
}

void SetHour(uint8_t hour)
{
    WriteEnable();
    WriteByte(HOUR_W, hour / 10 * 16 + hour % 10);
    WriteDisable();
}
uint8_t ReadHour()
{
    uint8_t temp;
    temp = ReadByte(HOUR_R);
    return temp / 16 * 10 + temp % 16;
}
