#ifndef __GBK_LibDrive_H__
#define __GBK_LibDrive_H__

#include "main.h"
#include "delay.h"
#include "string.h"
#include "3.2_LCD.h"
#include "usart.h"

/*�ַ��Ŵ�С֧��*/
//ascii:12,16,24,32,48,64
//����:12,16,24,32

#define FontRead_CMD 0x03 // �ӿڶ�ȡ�ֿ�--������

#define Used_FontIO 1 // ����Ƕ����������ֿ�ӿ�IO����ʹ�ö����Ľӿ�����

#if Used_FontIO // ʹ�õ�Ƭ��I/Oģ�� SPI�ӿ�����

#define FCS_SET __BIT_SET(GPIOC_ODR_Addr, 13,1) // Ƭѡ�ź� PC13
#define FCS_CLR __BIT_SET(GPIOC_ODR_Addr, 13,0) // Ƭѡ�ź� PC13

#define FDI_SET __BIT_SET(GPIOF_ODR_Addr, 11, 1) // ������� PF11
#define FDI_CLR __BIT_SET(GPIOF_ODR_Addr, 11, 0) // ������� PF11

#define FDO_IN __BIT_READ(GPIOB_IDR_Addr, 2) // �������� PB2

#define FCLK_SET __BIT_SET(GPIOB_ODR_Addr, 0, 1) // ����ʱ����  PB0
#define FCLK_CLR __BIT_SET(GPIOB_ODR_Addr, 0, 0) // ����ʱ����  PB0

#define Font_CS_EN \
    FCS_SET;       \
    GBK_delay(5); // Ƭѡ�ź�  --ʹ��
#define Font_CS_SN \
    FCS_CLR;       \
    GBK_delay(5); // Ƭѡ�ź�  --ʧ��
#endif

// �����ֿ�洢ƫ��λ��

#define ASCII6x12_ST 0x00080000
#define ASCII8x16_ST 0x00080800
#define ASCII12x24_ST 0x00081200
#define ASCII16x32_ST 0x00082600

#define ASCII24x48_ST 0x00084800
#define ASCII32x64_ST 0x00089200

#define GBK12x12_ST 0x00091400
#define GBK16x16_ST 0x0011DD00
#define GBK24x24_ST 0x001DA000
#define GBK32x32_ST 0x00380000

void GBK_Lib_Init(void); // ��ʼ�����Žӿ�

void FontLib_Read(u8 *pBuffer, u32 ReadAddr, u16 Num_Read); // �ֿ��д�ӿں���

void GBK_ReadID(void); // ���ֿ�ID

void GBK_delay(unsigned char Time); // ��ʱ����

void GBK_GetASC_Point(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num);                                             // ��ȡ����ASCII�����ַ�
void GBK_ShowASCII(uint16_t x, uint16_t y, uint8_t *N_Word, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode); // ��ָ��λ����ʾASCII�� �ַ�--�����ַ�

void GBK_Lib_GetHz(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num);                                              // �õ����ֵĵ�����
void GBK_Show_Font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode); // ��ָ��λ����ʾһ������

void GBK_Show_Str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *str, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode); // ��ָ��λ����ʾһ���ַ���
void GBK_Show_StrMid(uint16_t x, uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color, uint8_t *str);                                // ��ָ����ȵ��м���ʾ�ַ���

void DrawFont_GBK12B(u16 x, u16 y, u16 color, u8 *str); // ��ָ��λ�ÿ�ʼ��ʾһ��12x12�����--�ַ�����֧���Զ�����(����Ļһ�е��յ���Զ�������һ��x���λ��)
void DrawFont_GBK16B(u16 x, u16 y, u16 color, u8 *str); // ��ָ��λ�ÿ�ʼ��ʾһ��16x16�����--�ַ�����֧���Զ�����(����Ļһ�е��յ���Զ�������һ��x���λ��)
void DrawFont_GBK24B(u16 x, u16 y, u16 color, u8 *str); // ��ָ��λ�ÿ�ʼ��ʾһ��24x24�����--�ַ�����֧���Զ�����(����Ļһ�е��յ���Զ�������һ��x���λ��)
void DrawFont_GBK32B(u16 x, u16 y, u16 color, u8 *str); // ��ָ��λ�ÿ�ʼ��ʾһ��32x32�����--�ַ�����֧���Զ�����(����Ļһ�е��յ���Զ�������һ��x���λ��)

void DrawFontASC_GBK48B(u16 x, u16 y, u16 color, u8 *str); // ��ָ��λ�ÿ�ʼ��ʾһ��24x48�����--ACSII��

void DrawFontASC_GBK64B(u16 x, u16 y, u16 color, u8 *str); // ��ָ��λ�ÿ�ʼ��ʾһ��32x64�����--ACSII��

void GBK_LibFont_Test(void); // GBK�ֿ���Ժ���

#endif
