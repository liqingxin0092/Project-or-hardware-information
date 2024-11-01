#include "NAND FLASH.h"
#include "Delay.h"

static void NAND_Delay(volatile uint32_t i)
{
    while (i > 0)
        i--;
}
void NAND_Init()
{
    __GPIO_OUT(CLE_PORT, CLE_Pin, 0); // 不是命令
    __GPIO_OUT(CE_PORT, CE_Pin, 1);   // 未选中状态
    __GPIO_OUT(WE_PORT, WE_Pin, 1);   //
    __GPIO_OUT(ALE_PORT, ALE_Pin, 0); // 不是地址
    __GPIO_OUT(RE_PORT, RE_Pin, 1);   // 为下降沿做准备
}
static uint8_t IO_READ(void)
{
    uint8_t temp0 = *(uint32_t *)(0x42000000 + (((GPIOD_IDR_M) & 0x0fffffff) << 5) + ((D0_Pin) << 2));
    uint8_t temp1 = *(uint32_t *)(0x42000000 + (((GPIOD_IDR_M) & 0x0fffffff) << 5) + ((D1_Pin) << 2));
    uint8_t temp2 = *(uint32_t *)(0x42000000 + (((GPIOD_IDR_M) & 0x0fffffff) << 5) + ((D2_Pin) << 2));
    uint8_t temp3 = *(uint32_t *)(0x42000000 + (((GPIOD_IDR_M) & 0x0fffffff) << 5) + ((D3_Pin) << 2));
    uint8_t temp4 = *(uint32_t *)(0x42000000 + (((GPIOE_IDR_M) & 0x0fffffff) << 5) + ((D4_Pin) << 2));
    uint8_t temp5 = *(uint32_t *)(0x42000000 + (((GPIOE_IDR_M) & 0x0fffffff) << 5) + ((D5_Pin) << 2));
    uint8_t temp6 = *(uint32_t *)(0x42000000 + (((GPIOE_IDR_M) & 0x0fffffff) << 5) + ((D6_Pin) << 2));
    uint8_t temp7 = *(uint32_t *)(0x42000000 + (((GPIOE_IDR_M) & 0x0fffffff) << 5) + ((D7_Pin) << 2));
    return (temp7 << 7) | (temp6 << 6) | (temp5 << 5) | (temp4 << 4) | (temp3 << 3) | (temp2 << 2) | (temp1 << 1) | temp0;
}
void NAND_read_ID(uint8_t arr[])
{
    __GPIO_OUT(CE_PORT, CE_Pin, 0); // 选中

    __GPIO_OUT(CLE_PORT, CLE_Pin, 1); // 准备发送命令
    __GPIO_OUT(WE_PORT, WE_Pin, 0);   //
    __IO_SET(NAND_READ_ID);           // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1);   // 上升沿发送数据
    __GPIO_OUT(CLE_PORT, CLE_Pin, 0); // 发送命令完毕

    __GPIO_OUT(ALE_PORT, ALE_Pin, 1); // 准备发送地址
    __GPIO_OUT(WE_PORT, WE_Pin, 0);   //
    __IO_SET(0x0);                    // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1);   // 上升沿发送数据
    __GPIO_OUT(ALE_PORT, ALE_Pin, 0); // 发送地址完毕

    __IO_IN_MODE(); // 切换上拉输入

    __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
    arr[0] = IO_READ();
    __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备

    __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
    arr[1] = IO_READ();
    __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备

    __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
    arr[2] = IO_READ();
    __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备

    __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
    arr[3] = IO_READ();
    __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备

    __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
    arr[4] = IO_READ();
    __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备

    __IO_OUT_MODE();                // 切换为输出模式
    __GPIO_OUT(CE_PORT, CE_Pin, 1); // 未选中状态,结束通讯
}

// 注意没有CE,注意这个返回的东西究竟是什么!!!!
uint8_t NAND_read_status()
{
    __GPIO_OUT(CLE_PORT, CLE_Pin, 1); // 准备发送命令
    __GPIO_OUT(WE_PORT, WE_Pin, 0);   //
    __IO_SET(NAND_STATUS1);           // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1);   // 上升沿发送数据
    __GPIO_OUT(CLE_PORT, CLE_Pin, 0); // 发送命令完毕

    NAND_Delay(30);
    __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
    uint8_t temp = IO_READ();
    __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备

    return temp;
}
void NAND_page_read(uint32_t page_addr, uint16_t byte_addr, uint8_t buffer[], uint16_t length)
{
    __GPIO_OUT(CE_PORT, CE_Pin, 0); // 选中

    /*命令*/
    __SEND_COMMAND(0);
    /*地址*/
    __SEND_ADDRESS(page_addr, byte_addr);
    /*命令*/
    __SEND_COMMAND(0x30);

    __IO_IN_MODE(); // 切换为输入

    NAND_Delay(25);

    while (!((GPIOD->IDR >> 6) & 1))
        ;

    for (uint16_t i = 0; i < length; i++)
    {
        __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
        buffer[i] = IO_READ();
        __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备
    }

    __IO_OUT_MODE();                // 切换为输出模式
    __GPIO_OUT(CE_PORT, CE_Pin, 1); // 未选中状态,结束通讯
}

// 返回1代表出错
uint8_t NAND_page_write(uint32_t page_addr, uint16_t byte_addr, uint8_t buffer[], uint16_t length)
{
    __GPIO_OUT(CE_PORT, CE_Pin, 0); // 选中
    NAND_Delay(5);
    /*命令*/
    __GPIO_OUT(CLE_PORT, CLE_Pin, 1); // 准备发送命令

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(0x80);                 // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(CLE_PORT, CLE_Pin, 0); // 发送命令完毕

    /*地址*/
    __GPIO_OUT(ALE_PORT, ALE_Pin, 1); // 准备发送地址

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(byte_addr);            // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(byte_addr >> 8);       // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(page_addr);            // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(page_addr >> 8);       // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(page_addr >> 16);      // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(ALE_PORT, ALE_Pin, 0); // 发送地址结束

    NAND_Delay(70);

    /*数据*/
    for (uint16_t i = 0; i < length; i++)
    {
        __GPIO_OUT(WE_PORT, WE_Pin, 0); //
        __IO_SET(buffer[i]);            // 摆放数据
        NAND_Delay(10);                 // 延时
        __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据
    }

    /*命令*/
    __GPIO_OUT(CLE_PORT, CLE_Pin, 1); // 准备发送命令
    __GPIO_OUT(WE_PORT, WE_Pin, 0);   //
    __IO_SET(0x10);                   // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1);   // 上升沿发送数据
    __GPIO_OUT(CLE_PORT, CLE_Pin, 0); // 发送命令完毕

    NAND_Delay(30);                                                    // 延时
    while ((!((NAND_read_status() >> 6)&1)) || (!((GPIOD->IDR >> 6) & 1))) // 等待忙
        ;

    uint8_t temp = NAND_read_status() & 1;
    __GPIO_OUT(CE_PORT, CE_Pin, 1); // 未选中状态,结束通讯

    return temp;
}
uint8_t NAND_block_erase(uint16_t page_addr)
{
    __GPIO_OUT(CE_PORT, CE_Pin, 0); // 选中

    /*命令*/
    __GPIO_OUT(CLE_PORT, CLE_Pin, 1); // 准备发送命令
    __GPIO_OUT(WE_PORT, WE_Pin, 0);   //
    __IO_SET(0x60);                   // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1);   // 上升沿发送数据
    __GPIO_OUT(CLE_PORT, CLE_Pin, 0); // 发送命令完毕

    /*地址*/
    __GPIO_OUT(ALE_PORT, ALE_Pin, 1); // 准备发送地址

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(page_addr);            // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(page_addr >> 8);       // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(WE_PORT, WE_Pin, 0); //
    __IO_SET(page_addr >> 16);      // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1); // 上升沿发送数据

    __GPIO_OUT(ALE_PORT, ALE_Pin, 0); // 发送地址结束

    /*命令*/
    __GPIO_OUT(CLE_PORT, CLE_Pin, 1); // 准备发送命令
    __GPIO_OUT(WE_PORT, WE_Pin, 0);   //
    __IO_SET(0xd0);                   // 摆放数据
    __GPIO_OUT(WE_PORT, WE_Pin, 1);   // 上升沿发送数据
    __GPIO_OUT(CLE_PORT, CLE_Pin, 0); // 发送命令完毕

    NAND_Delay(30);                                                    // 延时
    while ((!((NAND_read_status() >> 6)&1)) || (!((GPIOD->IDR >> 6) & 1))) // 等待忙
        ;

    uint8_t temp = NAND_read_status() & 1;
    __GPIO_OUT(CE_PORT, CE_Pin, 1); // 未选中状态,结束通讯
    return temp;
}
/**
 * @brief 检测坏块
 *
 * @param ptr 那些块坏了
 */
void NAND_test(uint16_t *ptr)
{
    uint16_t block_num = 0;         // 块号
    uint16_t ptr_i = 0;             // 发射出去的数组下标
    __GPIO_OUT(CE_PORT, CE_Pin, 0); // 选中
    for (uint32_t i = 0; i < 131072; i += 64)
    {
        /*命令*/
        __SEND_COMMAND(0);
        /*地址*/
        __SEND_ADDRESS(i, 0x801); // 页地址每次跳一个块,页内地址是读备用区第一个.
        /*命令*/
        __SEND_COMMAND(0x30);
        /*切换为输入*/
        __IO_IN_MODE();

        /*等待*/
        NAND_Delay(25);
        while (!((GPIOD->IDR >> 6) & 1))
            ;

        __GPIO_OUT(RE_PORT, RE_Pin, 0); // 下降沿读
        if (IO_READ() != 0xff)          // 如果发现备用区读出来是坏块,记录下块号
        {
            ptr[ptr_i++] = block_num;
        }
        __GPIO_OUT(RE_PORT, RE_Pin, 1); // 为下次读做准备

        __IO_OUT_MODE(); // 切换为输出模式

        block_num++; // 块号自增
    }
    __GPIO_OUT(CE_PORT, CE_Pin, 1); // 未选中状态,结束通讯
}
