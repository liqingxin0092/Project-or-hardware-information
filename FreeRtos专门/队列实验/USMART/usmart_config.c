#include "usmart.h"
#include "usmart_str.h"

/******************************************************************************************/
/* �û�������
 * ������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����)
 */

#include "delay.h"

/*��������Ҫ�õ��ĺ���*/
extern void write_addr(uint32_t addr, uint32_t val); //��ĳ���ط�����
extern uint32_t read_addr(uint32_t addr);            //��ĳ���ط�����
extern void LCD_Clear(uint16_t color);
extern void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint8_t mode);
extern void GBK_Show_StrMid(uint16_t x, uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color, uint8_t *str);
extern void set_LED1(uint8_t state);
extern void set_LED2(uint8_t state);
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* ���ʹ���˶�д���� */
    (void *)read_addr, "uint32_t read_addr(uint32_t addr)",
    (void *)write_addr, "void write_addr(uint32_t addr, uint32_t val)",
#endif
    (void *)delay_ms, "void delay_ms(uint16_t nms)",
    (void *)delay_us, "void delay_us(uint32_t nus)",
    (void *)LCD_Clear,"void LCD_Clear(uint16_t color)",
    (void *)GBK_Show_StrMid,"void GBK_Show_StrMid(uint16_t x, uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color, uint8_t *str)",
    (void *)LCD_ShowString,"void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color, uint8_t mode)",
    (void *)set_LED1,"void set_LED1(uint8_t state)",
    (void *)set_LED2,"void set_LED2(uint8_t state)"
};

/******************************************************************************************/

/* �������ƹ�������ʼ��
 * �õ������ܿغ���������
 * �õ�����������
 */
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), /* �������� */
    0,      /* �������� */
    0,      /* ����ID */
    1,      /* ������ʾ����,0,10����;1,16���� */
    0,      /* ��������.bitx:,0,����;1,�ַ��� */
    0,      /* ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ�� */
    0,      /* �����Ĳ���,��ҪPARM_LEN��0��ʼ�� */
};



















