#include "malloc.h"
#include "usart.h"
#include "diskio.h"
#include "sdmmc_sdcard.h"
#include "stdio.h"

#define SD_CARD     0       /* SD��,���Ϊ0 */

/**
 * @brief       ��ô���״̬
 * @param       pdrv : ���̱��0~9
 * @retval      ִ�н��(�μ�FATFS, DSTATUS�Ķ���)
 */
DSTATUS disk_status (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    return RES_OK;
}

//f_mount()����������������
/**
 * @brief       ��ʼ������
 * @param       pdrv : ���̱��0~9
 * @retval      ִ�н��(�μ�FATFS, DSTATUS�Ķ���)
 */
DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    uint8_t res = 0;
    switch (pdrv)
    {
        case SD_CARD:              
            res =   SD_Init();     //sd����ʼ��
            break;

        default:
            res = 1;
            break;
    }
    if (res)
    {
        return STA_NOINIT;
    }
    else
    {
        return 0; /* ��ʼ���ɹ�*/
    }
}

/**
 * @brief       ������
 * @param       pdrv   : ���̱��0~9
 * @param       buff   : ���ݽ��ջ����׵�ַ
 * @param       sector : ������ַ
 * @param       count  : ��Ҫ��ȡ��������
 * @retval      ִ�н��(�μ�FATFS, DRESULT�Ķ���)
 */
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive number to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    uint8_t res = 0;
    if (!count) return RES_PARERR;   /* count���ܵ���0�����򷵻ز������� */
    switch (pdrv)
    {
        case SD_CARD:       /* SD�� */
            res = SD_ReadDisk(buff, sector, count);
            if (res)     /* ������ */
                printf("sd rd error:%d\r\n", res);
            break;

        default:
            res = 1;
    }

    /* ������ֵ��������ֵת��ff.c�ķ���ֵ */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}

/**
 * @brief       д����
 * @param       pdrv   : ���̱��0~9
 * @param       buff   : �������ݻ������׵�ַ
 * @param       sector : ������ַ
 * @param       count  : ��Ҫд���������
 * @retval      ִ�н��(�μ�FATFS, DRESULT�Ķ���)
 */
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive number to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR;  /* count���ܵ���0�����򷵻ز������� */

    switch (pdrv)
    {
        case SD_CARD:       /* SD�� */
            res = SD_WriteDisk((uint8_t *)buff, sector, count);

            while (res)     /* д���� */
            {
                printf("sd wr error:%d\r\n", res);
                res = SD_WriteDisk((uint8_t *)buff, sector, count);
            }

            break;
 
        default:
            res = 1;
    }

    /* ������ֵ��������ֵת��ff.c�ķ���ֵ */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}

/**
 * @brief       ��ȡ�������Ʋ���
 * @param       pdrv   : ���̱��0~9
 * @param       ctrl   : ���ƴ���
 * @param       buff   : ����/���ջ�����ָ��
 * @retval      ִ�н��(�μ�FATFS, DRESULT�Ķ���)
 */

DRESULT disk_ioctl ( BYTE pdrv, BYTE cmd,void *buff)
{
    DRESULT res;

    if (pdrv == SD_CARD)    /* SD�� */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(DWORD *)buff = 512;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff =512;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = SDCardInfo.CardBlockSize;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else
    {
        res = RES_ERROR;    /* �����Ĳ�֧�� */
    }
    
    return res;
}













