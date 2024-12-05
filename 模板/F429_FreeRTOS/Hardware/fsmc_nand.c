#include "fsmc_nand.h"
#include "Delay.h"
#include "stdio.h"
/**
 * @brief 延时
 *
 * @param i 1个i 4ns
 */

nand_attriute nand_dev;

static void NAND_Delay(volatile uint32_t i)
{
    while (i > 0)
        i--;
}

/**
 * @brief 等待忙
 *
 * @param
 */
void NAND_wait_busy()
{
    /*RB引脚有延迟,不会立马进入0的等待状态*/
    while (NAND_read_RB)
        ;
    while (!NAND_read_RB)
        ;
}
// 读NAND状态
// 返回值:NAND状态值
// bit0:0,成功;1,错误(编程/擦除/READ)
// bit6:0,Busy;1,Ready
uint8_t NAND_ReadStatus(void)
{
    volatile uint8_t ndata = 0;
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x70; // 发送读状态命令
    NAND_Delay(25);                                // 等待tWHR,再读取状态寄存器
    ndata = *(volatile uint8_t *)NAND_BASE;        // 读取状态值
    return ndata;
}
/**
 * @brief 读id
 *
 * @param ptr
 */
void NAND_read_id(void)
{
    uint8_t ptr[5]={0};
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = NAND_READ_ID;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = 0;
    ptr[0] = *(volatile uint8_t *)(NAND_BASE | nand_data);
    ptr[1] = *(volatile uint8_t *)(NAND_BASE | nand_data);
    ptr[2] = *(volatile uint8_t *)(NAND_BASE | nand_data);
    ptr[3] = *(volatile uint8_t *)(NAND_BASE | nand_data);
    ptr[4] = *(volatile uint8_t *)(NAND_BASE | nand_data);
    printf("\r\nnand flash id:\r\n%d \r\n%d \r\n%d \r\n%d \r\n%d \r\n\r\n",ptr[0],ptr[1],ptr[2],ptr[3],ptr[4]);
}
/**
 * @brief 结构体初始化
 *
 */
//读id获取的信息
//block size 128kB ,一页 2kB(字节),所以印证了1个block 64 page
//plan number :2 ,plan size :1Gb ,一共2Gb = 256 MByte 
void NAND_Init(void)
{
    nand_dev.page_mainsize = 2048;   //T
    nand_dev.block_pagenum = 64;     //T
    nand_dev.block_totalnum = 2048;  //T
}
// 获取ECC的奇数位/偶数位
// oe:0,偶数位
//    1,奇数位
// eccval:输入的ecc值
// 返回值:计算后的ecc值(最多16位)
uint16_t NAND_ECC_Get_OE(uint8_t oe, uint32_t eccval)
{
    uint8_t i;
    uint16_t ecctemp = 0;
    for (i = 0; i < 24; i++)
    {
        if ((i % 2) == oe)
        {
            if ((eccval >> i) & 0X01)
                ecctemp += 1 << (i >> 1);
        }
    }
    return ecctemp;
}
// 等待NAND准备好
// 返回值:NSTA_TIMEOUT 等待超时了
//       NSTA_READY    已经准备好
uint8_t NAND_WaitForReady(void)
{
    uint8_t status = 0;
    volatile uint32_t time = 0;
    while (1) // 等待ready
    {
        status = NAND_ReadStatus(); // 获取状态值
        if (status & NSTA_READY)
            break;
        time++;
        if (time >= 0X1FFFFFFF)
            return 5; // 超时
    }
    return NSTA_READY; // 准备好
}
/**
 * @brief
 *
 * @param data_buf
 * @param eccrd
 * @param ecccl
 * @return uint8_t
 */
static uint8_t NAND_ECC_Correction(uint8_t *data_buf, uint32_t eccrd, uint32_t ecccl)
{
    uint16_t eccrdo, eccrde, eccclo, ecccle;
    uint16_t eccchk = 0;
    uint16_t errorpos = 0;
    uint32_t bytepos = 0;
    eccrdo = NAND_ECC_Get_OE(1, eccrd); // 获取eccrd的奇数位
    eccrde = NAND_ECC_Get_OE(0, eccrd); // 获取eccrd的偶数位
    eccclo = NAND_ECC_Get_OE(1, ecccl); // 获取ecccl的奇数位
    ecccle = NAND_ECC_Get_OE(0, ecccl); // 获取ecccl的偶数位
    eccchk = eccrdo ^ eccrde ^ eccclo ^ ecccle;
    if (eccchk == 0XFFF) // 全1,说明只有1bit ECC错误
    {
        errorpos = eccrdo ^ eccclo;
        //        printf("errorpos:%d\r\n", errorpos);
        bytepos = errorpos / 8;
        data_buf[bytepos] ^= 1 << (errorpos % 8);
    }
    else // 不是全1,说明至少有2bit ECC错误,无法修复
    {
        //        printf("2bit ecc error or more\r\n");
        return 1;
    }
    return 0;
}

/**
 * @brief 页读
 *
 * @param page_add
 * @param byte_add
 * @param buffer
 * @param num_byte_to_read
 */
uint8_t NAND_page_read(uint32_t page_add, uint16_t byte_add, uint8_t *buffer, uint16_t num_byte_to_read)
{
    uint16_t i = 0;
    uint8_t eccnum = 0;
    uint8_t eccstart = 0; // 第一个ECC值所属的地址范围
    uint8_t *p = NULL;
    uint8_t res = 0;
    uint8_t errsta = 0;

    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)byte_add;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(byte_add >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)page_add;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(page_add >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(page_add >> 16);
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x30;

    NAND_wait_busy();
    if (num_byte_to_read % NAND_ECC_SECTOR_SIZE) // 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验
    {
        // 读取NAND FLASH中的值
        for (i = 0; i < num_byte_to_read; i++)
        {
            buffer[i] = *(volatile uint8_t *)(NAND_BASE | nand_data);
        }
    }
    else
    {
        eccnum = num_byte_to_read / NAND_ECC_SECTOR_SIZE; // 得到ecc计算次数
        eccstart = byte_add / NAND_ECC_SECTOR_SIZE;       // 要读取的字节地址位于哪个区域
        p = buffer;
        for (res = 0; res < eccnum; res++)
        {
            // SCB_CleanInvalidateDCache();               // 清除无效的D-Cache
            FMC_Bank2_3->PCR3 |= 1 << 6;               // 使能ECC校验
            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) // 读取NAND_ECC_SECTOR_SIZE个数据
            {
                *(volatile uint8_t *)buffer++ = *(volatile uint8_t *)NAND_BASE;
            }
            while (!(FMC_Bank2_3->SR3 & (1 << 6)))
                ;                                                    // 等待FIFO空
            nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; // 读取硬件计算后的ECC值
            FMC_Bank2_3->PCR3 |= 1 << 6;                             // 禁止ECC校验
        }
        // 这里一个ECC值(512字节验证)需要3个字节,但是这里给他分配四个字节对齐,这里是字节寻址
        // 这个i算的是ECC值的页内地址
        i = nand_dev.page_mainsize + 0X10 + eccstart * 4; // 从spare区的0X10位置开始读取之前存储的ecc值
        NAND_Delay(25);                                   // 等待tRHW
        *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0X05;    // 随机读指令
        // 发送地址
        *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(i >> 8);
        *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0XE0; // 开始读数据
        NAND_Delay(25);                                // 等待tWHR
        /*读取先前的ECC值读到缓存区里*/
        buffer = (uint8_t *)&nand_dev.ecc_rdbuf[eccstart];
        for (i = 0; i < 4 * eccnum; i++) // 读取保存的ECC值
        {
            *(volatile uint8_t *)buffer++ = *(volatile uint8_t *)NAND_BASE;
        }
        /*注意每个循环中i的作用不同*/
        for (i = 0; i < eccnum; i++) // 检验ECC
        {
            /*原来存的ECC值 和 硬件算的ECC值 比较 ,如果不相等就矫正*/
            if (nand_dev.ecc_rdbuf[i + eccstart] != nand_dev.ecc_hdbuf[i + eccstart])
            {
                // printf("err hd,rd:0x%x,0x%x\r\n", nand_dev.ecc_hdbuf[i + eccstart], nand_dev.ecc_rdbuf[i + eccstart]); //%x是16进制
                // printf("eccnum,eccstart:%d,%d\r\n", eccnum, eccstart);
                // printf("page_add,byte_add:%d,%d\r\n", page_add, byte_add);
                res = NAND_ECC_Correction(p + NAND_ECC_SECTOR_SIZE * i, nand_dev.ecc_rdbuf[i + eccstart], nand_dev.ecc_hdbuf[i + eccstart]); // ECC校验
                if (res)
                    errsta = 2; // 标记2BIT及以上ECC错误
                else
                    errsta = 1; // 标记1BIT ECC错误
            }
        }
    }
    /*事后等待*/
    if (NAND_WaitForReady() != NSTA_READY)
        errsta = NSTA_ERROR; // 失败
    return errsta;           // 成功
}
/**
 * @brief
 *
 * @param page_add
 * @param byte_add
 * @param buffer
 * @param num_byte_to_read
 */
uint8_t NAND_Write_Page(uint32_t page_add, uint16_t byte_add, uint8_t *buffer, uint16_t num_byte_to_read)
{
    volatile uint16_t i = 0;
    uint8_t res = 0;
    uint8_t eccnum = 0;   // 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc
    uint8_t eccstart = 0; // 第一个ECC值所属的地址范围

    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x80;
    // 发送地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)byte_add;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(byte_add >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)page_add;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(page_add >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(page_add >> 16);
    NAND_Delay(20);                              // 等待tADL
    if (num_byte_to_read % NAND_ECC_SECTOR_SIZE) // 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验
    {
        for (i = 0; i < num_byte_to_read; i++) // 写入数据
        {
            *(volatile uint8_t *)NAND_BASE = *(volatile uint8_t *)buffer++;
        }
    }
    else
    {
        eccnum = num_byte_to_read / NAND_ECC_SECTOR_SIZE; // 得到ecc计算次数
        eccstart = byte_add / NAND_ECC_SECTOR_SIZE;
        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank2_3->PCR3 |= 1 << 6;               // 使能ECC校验
            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) // 写入NAND_ECC_SECTOR_SIZE个数据
            {
                *(volatile uint8_t *)NAND_BASE = *(volatile uint8_t *)buffer++;
            }
            while (!(FMC_Bank2_3->SR3 & (1 << 6)))
                ;                                                    // 等待FIFO空
            nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; // 读取硬件计算后的ECC值
            FMC_Bank2_3->PCR3 &= ~(1 << 6);                          // 禁止ECC校验
        }
        i = nand_dev.page_mainsize + 0X10 + eccstart * 4; // 计算写入ECC的spare区地址
        NAND_Delay(30);                                   // 等待
        *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0X85;    // 随机写指令
        // 发送地址
        *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(i >> 8);
        NAND_Delay(30); // 等待tADL
        buffer = (uint8_t *)&nand_dev.ecc_hdbuf[eccstart];
        for (i = 0; i < eccnum; i++) // 写入ECC
        {
            for (res = 0; res < 4; res++)
            {
                *(volatile uint8_t *)NAND_BASE = *(volatile uint8_t *)buffer++;
            }
        }
    }
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x10;
    if (NAND_WaitForReady() != NSTA_READY)
        return NSTA_ERROR; // 失败
    return 0;              // 成功
}
/**
 * @brief 复制到另一个区域但是不新写入
 *
 * @param Source_PageNum
 * @param Dest_PageNum
 * @return uint8_t
 */
int8_t NAND_CopyPageWithoutWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum)
{
    uint16_t source_block = 0, dest_block = 0;
    // 判断源页和目的页是否在同一个plane中
    source_block = Source_PageNum / nand_dev.block_pagenum;
    dest_block = Dest_PageNum / nand_dev.block_pagenum;
    if ((source_block % 2) != (dest_block % 2))
        return -1;                              // 不在同一个plane内
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0; // 发送命令0X00
    // 发送源页地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)Source_PageNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Source_PageNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Source_PageNum >> 16);
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x35; // 发送命令0X35

    NAND_wait_busy();                              // 超时退出
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x85; // 发送命令0X85
    // 发送目的页地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)Dest_PageNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Dest_PageNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Dest_PageNum >> 16);
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x10; // 发送命令0X10
    delay_us(250);                                 // 等待tPROG
    if (NAND_WaitForReady() != NSTA_READY)
        return NSTA_ERROR; // NAND未准备好
    return 0;              // 成功
}

// 将一页数据拷贝到另一页,并且可以写入数据
// 注意:源页和目的页要在同一个Plane内！
// Source_PageNo:源页地址,范围:0~(block_pagenum*block_totalnum-1)
// Dest_PageNo:目的页地址,范围:0~(block_pagenum*block_totalnum-1)
// ColNo:页内列地址,范围:0~(page_totalsize-1)
// pBuffer:要写入的数据
// NumByteToWrite:要写入的数据个数
// 返回值:0,成功
//     其他,错误代码
int8_t NAND_CopyPageWithWrite(uint32_t Source_PageNum, uint32_t Dest_PageNum, uint16_t byte_add, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    uint8_t res = 0;
    volatile uint16_t i = 0;
    uint16_t source_block = 0, dest_block = 0;
    uint8_t eccnum = 0;   // 需要计算的ECC个数，每NAND_ECC_SECTOR_SIZE字节计算一个ecc
    uint8_t eccstart = 0; // 第一个ECC值所属的地址范围
    // 判断源页和目的页是否在同一个plane中
    source_block = Source_PageNum / nand_dev.block_pagenum;
    dest_block = Dest_PageNum / nand_dev.block_pagenum;
    if ((source_block % 2) != (dest_block % 2))
        return -1;                              // 不在同一个plane内
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0; // 发送命令0X00
    // 发送源页地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)0;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)Source_PageNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Source_PageNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Source_PageNum >> 16);
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x35; // 发送命令0X35

    NAND_wait_busy(); // 超时退出

    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x85; // 发送命令0X85
    // 发送目的页地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)byte_add;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(byte_add >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)Dest_PageNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Dest_PageNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(Dest_PageNum >> 16);
    // 发送页内列地址
    NAND_Delay(30);                            // 等待tADL
    if (NumByteToWrite % NAND_ECC_SECTOR_SIZE) // 不是NAND_ECC_SECTOR_SIZE的整数倍，不进行ECC校验
    {
        for (i = 0; i < NumByteToWrite; i++) // 写入数据
        {
            *(volatile uint8_t *)NAND_BASE = *(volatile uint8_t *)pBuffer++;
        }
    }
    else
    {
        eccnum = NumByteToWrite / NAND_ECC_SECTOR_SIZE; // 得到ecc计算次数
        eccstart = byte_add / NAND_ECC_SECTOR_SIZE;
        for (res = 0; res < eccnum; res++)
        {
            FMC_Bank2_3->PCR3 |= 1 << 6;               // 使能ECC校验
            for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) // 写入NAND_ECC_SECTOR_SIZE个数据
            {
                *(volatile uint8_t *)NAND_BASE = *(volatile uint8_t *)pBuffer++;
            }
            while (!(FMC_Bank2_3->SR3 & (1 << 6)))
                ;                                                    // 等待FIFO空
            nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; // 读取硬件计算后的ECC值
            FMC_Bank2_3->PCR3 &= ~(1 << 6);                          // 禁止ECC校验
        }
        i = nand_dev.page_mainsize + 0X10 + eccstart * 4; // 计算写入ECC的spare区地址
        NAND_Delay(30);                                   // 等待
        *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0X85;    // 随机写指令
        // 发送地址
        *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)i;
        *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(i >> 8);
        NAND_Delay(30); // 等待tADL
        pBuffer = (uint8_t *)&nand_dev.ecc_hdbuf[eccstart];
        for (i = 0; i < eccnum; i++) // 写入ECC
        {
            for (res = 0; res < 4; res++)
            {
                *(volatile uint8_t *)NAND_BASE = *(volatile uint8_t *)pBuffer++;
            }
        }
    }
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x10; // 发送命令0X10
    delay_us(300);                                 // 等待tPROG
    if (NAND_WaitForReady() != NSTA_READY)
        return NSTA_ERROR; // 失败
    return 0;              // 成功
}

// 擦除一个块
// BlockNum:要擦除的BLOCK编号,范围:0-(block_totalnum-1)
// 返回值:0,擦除成功
//     其他,擦除失败
uint8_t NAND_EraseBlock(uint32_t BlockNum)
{

    BlockNum <<= 6; // 块地址变页地址
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x60;
    // 发送块地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)BlockNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(BlockNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(BlockNum >> 16);
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0xd0;
    delay_ms(4); // 等待擦除成功
    if (NAND_WaitForReady() != NSTA_READY)
        return NSTA_ERROR; // 失败
    return 0;              // 成功
}


/**
 * @brief
 *
 * @param PageNum
 * @param ColNum
 * @param cval
 * @param NumByteToWrite
 * @return uint8_t
 */
uint8_t NAND_WritePageConst(uint32_t PageNum, uint16_t ColNum, uint32_t cval, uint16_t NumByteToWrite)
{
    uint16_t i = 0;
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0X80;
    // 发送地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)ColNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(ColNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)PageNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(PageNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(PageNum >> 16);
    NAND_Delay(30);                      // 等待tADL
    for (i = 0; i < NumByteToWrite; i++) // 写入数据,每次写4字节
    {
        *(volatile uint32_t *)NAND_BASE = cval;
    }
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0X10;
    delay_us(250); // 等待tPROG
    if (NAND_WaitForReady() != NSTA_READY)
        return NSTA_ERROR; // 失败
    return 0;              // 成功
}
uint8_t NAND_ReadPageComp(uint32_t PageNum, uint16_t ColNum, uint32_t CmpVal, uint16_t NumByteToRead, uint16_t *NumByteEqual)
{
    uint16_t i = 0;
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0X00;
    // 发送地址
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)ColNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(ColNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)PageNum;
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(PageNum >> 8);
    *(volatile uint8_t *)(NAND_BASE | nand_addr) = (uint8_t)(PageNum >> 16);
    *(volatile uint8_t *)(NAND_BASE | nand_cmd) = 0x30;

    NAND_wait_busy();
    for (i = 0; i < NumByteToRead; i++) // 读取数据,每次读4字节
    {
        if (*(volatile uint32_t *)NAND_BASE != CmpVal)
            break; // 如果有任何一个值,与CmpVal不相等,则退出.
    }
    *NumByteEqual = i; // 与CmpVal值相同的个数
    if (NAND_WaitForReady() != NSTA_READY)
        return NSTA_ERROR; // 失败
    return 0;              // 成功
}

void NAND_EraseChip(void)
{
    uint8_t status;
    uint16_t i = 0;
    for (i = 0; i < nand_dev.block_totalnum; i++) // 循环擦除所有的块
    {
        status = NAND_EraseBlock(i);
        if (status)
            printf("Erase %d block fail!!,error code:%d\r\n", i, status); // 擦除失败
    }
}
