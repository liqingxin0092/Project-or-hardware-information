#include "usart.h"
#include "delay.h"
#include "stmflash.h"


/**
 * @brief       ����STM32��FLASH
 * @param       ��
 * @retval      ��
 */
static void stmflash_unlock(void)
{
    FLASH->KEYR = STM32_FLASH_KEY1;     /* FLASH д���������. */
    FLASH->KEYR = STM32_FLASH_KEY2;
}

/**
 * @brief       flash����
 * @param       ��
 * @retval      ��
 */
static void stmflash_lock(void)
{
    FLASH->CR |= (uint32_t)1 << 31;     /* FLASH ���� */
}

/**
 * @brief       �õ�FLASH�Ĵ���״̬
 * @param       ��
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       ���� : ������
 */
static uint8_t stmflash_get_error_status(void)
{
    uint32_t res = 0;
    res = FLASH->SR;

    if (res & (1 << 16)) return 1;  /* BSY=1, ��æ */
    if (res & (1 << 7))  return 2;  /* PGSERR=1,������д��� */
    if (res & (1 << 6))  return 3;  /* PGPERR=1,��̲���λ������ */
    if (res & (1 << 5))  return 4;  /* PGAERR=1,��̶������ */
    if (res & (1 << 4))  return 5;  /* WRPERR=1,д�������� */

    return 0;   /* û���κ�״̬/�������. */
}

/**
 * @brief       �ȴ��������
 * @param       time : Ҫ��ʱ�ĳ���
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       0XFF: ��ʱ
 *   @arg       ���� : ������
 */
static uint8_t stmflash_wait_done(uint32_t time)
{
    uint8_t res;

    do
    {
        res = stmflash_get_error_status();

        if (res != 1)
        {
            break;      /* ��æ, ����ȴ���, ֱ���˳� */
        }
        
        time--;
    } while (time);

    if (time == 0)res = 0XFF;   /* ��ʱ */

    return res;
}

/**
 * @brief       ��������
 * @param       saddr   : ������ַ 0 ~ 11
 *                        0~3, 16K����; 4,64K����; 5~11, 128K����.
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       0XFF: ��ʱ
 *   @arg       ���� : ������
 */
static uint8_t stmflash_erase_sector(uint32_t saddr)
{
    uint8_t res = 0;

    res = stmflash_wait_done(0XFFFFFFFF);   /* �ȴ��ϴβ������� */

    if (res == 0)
    {
        FLASH->CR &= ~(3 << 8);             /* ���PSIZEԭ�������� */
        FLASH->CR |= 2 << 8;                /* ����Ϊ32bit��,ȷ��VCC=2.7~3.6V֮��!! */
        FLASH->CR &= ~(0X1F << 3);          /* ���ԭ�������� */
        FLASH->CR |= saddr << 3;            /* ����Ҫ���������� */
        FLASH->CR |= 1 << 1;                /* �������� */
        FLASH->CR |= 1 << 16;               /* ��ʼ���� */
        res = stmflash_wait_done(0XFFFFFFFF);   /* �ȴ��������� */

        if (res != 1)                       /* ��æ */
        {
            FLASH->CR &= ~(1 << 1);         /* �������������־ */
        }
    }

    return res;
}

/**
 * @brief       ��FLASHָ����ַдһ���� (32λ����)
 *   @note      ����д��һ����, ��ָ4���ֽ�
 * @param       faddr   : д���ַ (�˵�ַ����Ϊ4�ı���!!)
 * @param       data    : Ҫд�������(32λ)
 * @retval      ִ�н��
 *   @arg       0   : �����
 *   @arg       0XFF: ��ʱ
 *   @arg       ���� : ������
 */
static uint8_t stmflash_write_word(uint32_t faddr, uint32_t data)
{
    uint8_t res;

    res = stmflash_wait_done(0XFFFFF);

    if (res == 0)   /* OK */
    {
        FLASH->CR &= ~(3 << 8);             /* ���PSIZEԭ�������� */
        FLASH->CR |= 2 << 8;                /* ����Ϊ32bit��,ȷ��VCC=2.7~3.6V֮��!! */
        FLASH->CR |= 1 << 0;                /* ���ʹ�� */
        *(volatile uint32_t *)faddr = data; /* д������ */
        res = stmflash_wait_done(0XFFFFF);  /* �ȴ��������,һ���ֱ�� */

        if (res != 1)                       /* �����ɹ� */
        {
            FLASH->CR &= ~(1 << 0);         /* ���PGλ */
        }
    }

    return res;
}

/**
 * @brief       ��ָ����ַ��ȡһ���� (32λ����)
 * @param       faddr   : ��ȡ��ַ (�˵�ַ����Ϊ4�ı���!!)
 * @retval      ��ȡ�������� (32λ)
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}

/**
 * @brief       ��ȡĳ����ַ���ڵ�flash����
 * @param       faddr   : flash��ַ
 * @retval      0~11, ��addr���ڵ�����
 */
static uint8_t stmflash_get_flash_sector(uint32_t addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)return 0;
    else if (addr < ADDR_FLASH_SECTOR_2)return 1;
    else if (addr < ADDR_FLASH_SECTOR_3)return 2;
    else if (addr < ADDR_FLASH_SECTOR_4)return 3;
    else if (addr < ADDR_FLASH_SECTOR_5)return 4;
    else if (addr < ADDR_FLASH_SECTOR_6)return 5;
    else if (addr < ADDR_FLASH_SECTOR_7)return 6;
    else if (addr < ADDR_FLASH_SECTOR_8)return 7;
    else if (addr < ADDR_FLASH_SECTOR_9)return 8;
    else if (addr < ADDR_FLASH_SECTOR_10)return 9;
    else if (addr < ADDR_FLASH_SECTOR_11)return 10;

    return 11;
}

/**
 * @brief       ��FLASH ָ��λ��, д��ָ�����ȵ�����(�Զ�����)
 *   @note      ��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����д��ַ�����0XFF
 *              ,��ô���Ȳ������������Ҳ�������������.����д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.
 *              ����д֮ǰȷ��������û����Ҫ����,��������������Ȳ�����,Ȼ����������д.
 *              �ú�����OTP����Ҳ��Ч!��������дOTP��!
 *              OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F(ע�⣺���16�ֽڣ�����OTP���ݿ�����������д����)
 * @param       waddr   : ��ʼ��ַ (�˵�ַ����Ϊ4�ı���!!,����д�����!)
 * @param       pbuf    : ����ָ��
 * @param       length  : Ҫд��� ��(32λ)��(����Ҫд���32λ���ݵĸ���)
 * @retval      ��
 */
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{

    uint8_t status = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;

    if (waddr < STM32_FLASH_BASE || waddr % 4 ||        /* д���ַС�� STM32_FLASH_BASE, ����4��������, �Ƿ�. */
        waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE))  /* д���ַ���� STM32_FLASH_BASE + STM32_FLASH_SIZE, �Ƿ�. */
    {
        return;
    }
    
    stmflash_unlock();              /* FLASH���� */
    
    FLASH->ACR &= ~(1 << 10);       /* FLASH�����ڼ�,�����ֹ���ݻ���!!!�����������ϲŷ����������! */
    addrx = waddr;                  /* д�����ʼ��ַ */
    endaddr = waddr + length * 4;   /* д��Ľ�����ַ */

    if (addrx < 0X1FFF0000)         /* ֻ�����洢��,����Ҫִ�в�������!! */
    {
        while (addrx < endaddr)     /* ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���) */
        {
            if (stmflash_read_word(addrx) != 0XFFFFFFFF)    /* �з�0XFFFFFFFF�ĵط�,Ҫ����������� */
            {
                status = stmflash_erase_sector(stmflash_get_flash_sector(addrx));

                if (status)break;   /* ���������� */
            }
            else
            {
                addrx += 4;
            }
        }
    }

    if (status == 0)
    {
        while (waddr < endaddr)     /* д���� */
        {
            if (stmflash_write_word(waddr, *pbuf))  /* д������ */
            {
                break;              /* д���쳣 */
            }

            waddr += 4;
            pbuf++;
        }
    }

    FLASH->ACR |= 1 << 10;          /* FLASH��������,��������fetch */
    
    stmflash_lock();                /* ���� */
}

/**
 * @brief       ��ָ����ַ��ʼ����ָ�����ȵ�����
 * @param       raddr : ��ʼ��ַ
 * @param       pbuf  : ����ָ��
 * @param       length: Ҫ��ȡ�� ��(32λ)��(����Ҫ��ȡ��32λ���ݵĸ���)
 * @retval      ��
 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        pbuf[i] = stmflash_read_word(raddr);    /* ��ȡ4���ֽ�(1����) */
        raddr += 4;                             /* ƫ��4���ֽ� */
    }
}

/******************************************************************************************/
/* �����ô��� */

/**
 * @brief       ����д����(д1����)
 * @param       waddr : ��ʼ��ַ
 * @param       wdata : Ҫд�������
 * @retval      ��
 */
void test_write(uint32_t waddr, uint32_t wdata)
{
    stmflash_write(waddr, &wdata, 1);       /* д��һ���� */
}


















