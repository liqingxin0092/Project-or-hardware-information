#include "malloc.h"
#include "usart.h"
#include "diskio.h"
#include "TF-card.h"
#include "stdio.h"
#include "fsmc_nand.h"
#include "ftl.h"

#define SD_CARD     0       /* SD��,���Ϊ0 */
#define EX_FLASH    1       /* �ⲿspi flash,���Ϊ1 */

/**
 * ����25Q128 FLASHоƬ, ���ǹ涨ǰ 12M ��FATFSʹ��, 12M�Ժ�
 * �����ֿ�, 3���ֿ� + UNIGBK.BIN, �ܴ�С3.09M, ��ռ��15.09M
 * 15.09M�Ժ�Ĵ洢�ռ��ҿ������ʹ��. 
 */

#define NAND_FLASH_SECTOR_SIZE   512
#define NAND_FLASH_SECTOR_COUNT  12 * 1024 * 2   /* 25Q128, ǰ12M�ֽڸ�FATFSռ�� */
#define NAND_FLASH_FATFS_BASE    0               /* FATFS ���ⲿFLASH����ʼ��ַ ��0��ʼ */


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
        case SD_CARD:           /* SD�� */
            res = sd_init();    /* SD����ʼ�� */
            show_sdcard_info();
            break;
        case EX_FLASH:          /* �ⲿflash */
            res = FTL_Init(); 
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
            res = sd_read_disk(buff, sector, count);
            while (res)     /* ������ */
            {
                printf("sd rd error:%d\r\n", res);
                res = sd_read_disk(buff, sector, count);
            }
            break;
        case EX_FLASH:      /* �ⲿflash */
            for (; count > 0; count--)
            {
                FTL_ReadSectors(buff, sector, 512, 1);
                sector++;
                buff += 512;
            }
            res = 0;
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
            res = sd_write_disk((uint8_t *)buff, sector, count);

            while (res)     /* д���� */
            {
                printf("sd wr error:%d\r\n", res);
                res = sd_write_disk((uint8_t *)buff, sector, count);
            }

            break;
        case EX_FLASH:      /* �ⲿflash */
            for (; count > 0; count--)
            {
                FTL_WriteSectors((uint8_t*)buff, sector, 512,1);
                sector++;
                buff += 512;
            }

            res = 0;
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
                *(DWORD *)buff = g_sd_card_info_handle.LogBlockNbr;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else if (pdrv == EX_FLASH)  /* �ⲿFLASH */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;
            case GET_SECTOR_SIZE:
                *(WORD *)buff = 512;      //T
                res = RES_OK;
                break;
            case GET_BLOCK_SIZE:
                *(WORD *)buff = 512;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff =nand_dev.good_blocknum*64*4;
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

void ff_test(void)
{
    FRESULT ret;
    FATFS *fs_obj;
    FIL* fil_obj;
    uint8_t read_buffer[50]={0};
    uint16_t fsize=0;
    uint16_t write_count=0;
    uint16_t read_count=0;
    fs_obj=mymalloc(SRAMIN,sizeof(FATFS));
    fil_obj=mymalloc(SRAMIN,sizeof(FIL));
    
    /*����*/
    ret = f_mount(fs_obj,"0:",1);//1:��������
    if(ret)
    {
        printf("mount failed,err code:%d\r\n",ret);
    }
    else 
    {
        printf("mount succeed!!\r\n");
    }
    
    /*���ļ�*/  //�ǵùر�
    ret=f_open(fil_obj,"0:123.txt",FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
    if(ret)
    {
        printf("open failed,err code:%d\r\n",ret);
    }
    else 
    {
        printf("open succeed!!\r\n");
    }
    
    /*д����*/    //��дע��ָ��
     f_lseek(fil_obj,0);
    f_write(fil_obj,"123456789",9,(UINT*)&write_count);
    f_printf(fil_obj,"pxd_nihao ");
    if(write_count!=0)
    {
        printf("write succeed!!\r\n");
    }
    else 
    {
        printf("write failed!!!\r\n");
    }
    
    /*������*/
    f_lseek(fil_obj,0);//ָ�뻹ԭ
    fsize=f_size(fil_obj);
    f_read(fil_obj,read_buffer,fsize,(UINT*)&read_count);
    if(read_count!=0)
    {
        printf("succeed,read_buff:%s\r\n",read_buffer);
    }
    else 
    {
        printf("read failed!!\r\n");
    }
    
    //С����
    printf("fil_obj locationg:%d\r\n",(int)f_tell(fil_obj));//���ļ���ַ
    f_setlabel((const TCHAR*)"0:pxd");//�����̷�����
    char  buffe[20];
    f_getlabel((const TCHAR*)"0:",(TCHAR*)buffe,0);//��ȡ�̷�����
    printf("labal: %s\r\n",buffe);
    /*�ر��ļ�*/
    f_close(fil_obj);
}














