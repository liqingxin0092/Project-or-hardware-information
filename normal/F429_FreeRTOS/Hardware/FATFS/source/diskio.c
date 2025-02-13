#include "malloc.h"
#include "usart.h"
#include "diskio.h"
#include "TF-card.h"
#include "stdio.h"
#include "fsmc_nand.h"
#include "ftl.h"

#define SD_CARD     0       /* SD卡,卷标为0 */
#define EX_FLASH    1       /* 外部spi flash,卷标为1 */

/**
 * 对于25Q128 FLASH芯片, 我们规定前 12M 给FATFS使用, 12M以后
 * 紧跟字库, 3个字库 + UNIGBK.BIN, 总大小3.09M, 共占用15.09M
 * 15.09M以后的存储空间大家可以随便使用. 
 */

#define NAND_FLASH_SECTOR_SIZE   512
#define NAND_FLASH_SECTOR_COUNT  12 * 1024 * 2   /* 25Q128, 前12M字节给FATFS占用 */
#define NAND_FLASH_FATFS_BASE    0               /* FATFS 在外部FLASH的起始地址 从0开始 */


/**
 * @brief       获得磁盘状态
 * @param       pdrv : 磁盘编号0~9
 * @retval      执行结果(参见FATFS, DSTATUS的定义)
 */
DSTATUS disk_status (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    return RES_OK;
}

/**
 * @brief       初始化磁盘
 * @param       pdrv : 磁盘编号0~9
 * @retval      执行结果(参见FATFS, DSTATUS的定义)
 */
DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    uint8_t res = 0;
    switch (pdrv)
    {
        case SD_CARD:           /* SD卡 */
            res = sd_init();    /* SD卡初始化 */
            show_sdcard_info();
            break;
        case EX_FLASH:          /* 外部flash */
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
        return 0; /* 初始化成功*/
    }
}

/**
 * @brief       读扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 数据接收缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive number to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    uint8_t res = 0;
    if (!count) return RES_PARERR;   /* count不能等于0，否则返回参数错误 */
    switch (pdrv)
    {
        case SD_CARD:       /* SD卡 */
            res = sd_read_disk(buff, sector, count);
            while (res)     /* 读出错 */
            {
                printf("sd rd error:%d\r\n", res);
                res = sd_read_disk(buff, sector, count);
            }
            break;
        case EX_FLASH:      /* 外部flash */
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

    /* 处理返回值，将返回值转成ff.c的返回值 */
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
 * @brief       写扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 发送数据缓存区首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive number to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR;  /* count不能等于0，否则返回参数错误 */

    switch (pdrv)
    {
        case SD_CARD:       /* SD卡 */
            res = sd_write_disk((uint8_t *)buff, sector, count);

            while (res)     /* 写出错 */
            {
                printf("sd wr error:%d\r\n", res);
                res = sd_write_disk((uint8_t *)buff, sector, count);
            }

            break;
        case EX_FLASH:      /* 外部flash */
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

    /* 处理返回值，将返回值转成ff.c的返回值 */
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
 * @brief       获取其他控制参数
 * @param       pdrv   : 磁盘编号0~9
 * @param       ctrl   : 控制代码
 * @param       buff   : 发送/接收缓冲区指针
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */

DRESULT disk_ioctl ( BYTE pdrv, BYTE cmd,void *buff)
{
    DRESULT res;

    if (pdrv == SD_CARD)    /* SD卡 */
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
    else if (pdrv == EX_FLASH)  /* 外部FLASH */
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
        res = RES_ERROR;    /* 其他的不支持 */
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
    
    /*挂载*/
    ret = f_mount(fs_obj,"0:",1);//1:立即挂载
    if(ret)
    {
        printf("mount failed,err code:%d\r\n",ret);
    }
    else 
    {
        printf("mount succeed!!\r\n");
    }
    
    /*打开文件*/  //记得关闭
    ret=f_open(fil_obj,"0:123.txt",FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
    if(ret)
    {
        printf("open failed,err code:%d\r\n",ret);
    }
    else 
    {
        printf("open succeed!!\r\n");
    }
    
    /*写数据*/    //读写注意指针
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
    
    /*读数据*/
    f_lseek(fil_obj,0);//指针还原
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
    
    //小功能
    printf("fil_obj locationg:%d\r\n",(int)f_tell(fil_obj));//看文件地址
    f_setlabel((const TCHAR*)"0:pxd");//设置盘符名称
    char  buffe[20];
    f_getlabel((const TCHAR*)"0:",(TCHAR*)buffe,0);//读取盘符名称
    printf("labal: %s\r\n",buffe);
    /*关闭文件*/
    f_close(fil_obj);
}














