#include "QSPI.h"

/**
 * @brief 配置QSPI外设时钟
 *
 */
void QSPI_RCC_Init(void)
{
    RCC_PeriphCLKInitTypeDef QSPI_InitStruct = {0};

    /*以下配置的到250MHz QSPI时钟频率,QSPI需要配置2分频得到125MHz时钟频率*/
    QSPI_InitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;   // 选择外设时钟
    QSPI_InitStruct.PLL2.PLL2M = 25;                             // 晶振分频系数25(注：LXB750M1-VBT6的晶振为25M)
    QSPI_InitStruct.PLL2.PLL2N = 500;                            // 将经过预分频后的晶振时钟进行500倍倍频，得到500M时钟
    QSPI_InitStruct.PLL2.PLL2P = 2;                              // 这个时钟无关QSPI，用户可自由配置和使用
    QSPI_InitStruct.PLL2.PLL2Q = 2;                              // 这个时钟无关QSPI，用户可自由配置和使用
    QSPI_InitStruct.PLL2.PLL2R = 2;                              // 进行2分频，得到 250M 的 pll2_r_ck 时钟
    QSPI_InitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;           // PLL输入范围,25/25=1
    QSPI_InitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;           // PLL输出范围,1*500=500
    QSPI_InitStruct.PLL2.PLL2FRACN = 0;                          // 分频的小数部分
    QSPI_InitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2; // 选择PLL2作为QSPI时钟

    HAL_RCCEx_PeriphCLKConfig(&QSPI_InitStruct); // 设置寄存器
}
/**
 * @brief 初始化IO
 *  @note ***************************************************
 *           PB2     ------> QUADSPI_CLK
 *           PB6     ------> QUADSPI_BK1_NCS
 *           PD11    ------> QUADSPI_BK1_IO0
 *           PD12    ------> QUADSPI_BK1_IO1
 *           PE2     ------> QUADSPI_BK1_IO2
 *           PD13    ------> QUADSPI_BK1_IO3
 *       ****************************************************
 **/
void QSPI_IO_Init(void)
{
    __HAL_RCC_QSPI_CLK_ENABLE();  // 使能QSPI时钟
    __HAL_RCC_QSPI_FORCE_RESET(); // 复位QSPI
    __HAL_RCC_QSPI_RELEASE_RESET();

    __HAL_RCC_GPIOB_CLK_ENABLE(); // 开启IO时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;            // 复用推挽输出模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;                // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 超高速IO口速度

    GPIO_InitStruct.Pin = QUADSPI_CLK_PIN;             // QUADSPI_CLK 引脚
    GPIO_InitStruct.Alternate = QUADSPI_CLK_AF;        // QUADSPI_CLK 复用
    HAL_GPIO_Init(QUADSPI_CLK_PORT, &GPIO_InitStruct); // 初始化 QUADSPI_CLK 引脚

    GPIO_InitStruct.Pin = QUADSPI_BK1_NCS_PIN;             // QUADSPI_BK1_NCS 引脚
    GPIO_InitStruct.Alternate = QUADSPI_BK1_NCS_AF;        // QUADSPI_BK1_NCS 复用
    HAL_GPIO_Init(QUADSPI_BK1_NCS_PORT, &GPIO_InitStruct); // 初始化 QUADSPI_BK1_NCS 引脚

    GPIO_InitStruct.Pin = QUADSPI_BK1_IO0_PIN;             // QUADSPI_BK1_IO0 引脚
    GPIO_InitStruct.Alternate = QUADSPI_BK1_IO0_AF;        // QUADSPI_BK1_IO0 复用
    HAL_GPIO_Init(QUADSPI_BK1_IO0_PORT, &GPIO_InitStruct); // 初始化 QUADSPI_BK1_IO0 引脚

    GPIO_InitStruct.Pin = QUADSPI_BK1_IO1_PIN;             // QUADSPI_BK1_IO1 引脚
    GPIO_InitStruct.Alternate = QUADSPI_BK1_IO1_AF;        // QUADSPI_BK1_IO1 复用
    HAL_GPIO_Init(QUADSPI_BK1_IO1_PORT, &GPIO_InitStruct); // 初始化 QUADSPI_BK1_IO1 引脚

    GPIO_InitStruct.Pin = QUADSPI_BK1_IO2_PIN;             // QUADSPI_BK1_IO2 引脚
    GPIO_InitStruct.Alternate = QUADSPI_BK1_IO2_AF;        // QUADSPI_BK1_IO2 复用
    HAL_GPIO_Init(QUADSPI_BK1_IO2_PORT, &GPIO_InitStruct); // 初始化 QUADSPI_BK1_IO2 引脚

    GPIO_InitStruct.Pin = QUADSPI_BK1_IO3_PIN;             // QUADSPI_BK1_IO3 引脚
    GPIO_InitStruct.Alternate = QUADSPI_BK1_IO3_AF;        // QUADSPI_BK1_IO3 复用
    HAL_GPIO_Init(QUADSPI_BK1_IO3_PORT, &GPIO_InitStruct); // 初始化 QUADSPI_BK1_IO3 引脚
}

void QSPI_Peripheral_Init(void)
{
    /*在某些场合，例如用作下载算法时，需要手动复位QSPI寄存器配置，否则会工作不正常*/

    uint32_t i;
    uint32_t *p;
    p = (uint32_t *)QUADSPI;
    for (i = 0; i < sizeof(QUADSPI); i++)
    {
        *p++ = 0;
    }

    /*初始化QSPI,这里涉及两个寄存器CR和DCR*/

    uint32_t QSPI_Register = 0;
    QSPI_Register |= 1 << 24; // QSPI2分频得到125MHz时钟
    QSPI_Register |= 31 << 8; // FIFO阈值32
    // 选择FLASH1
    // 禁用双闪存
    QSPI_Register |= 1 << 4;     // 延迟半周期读数
    QUADSPI->CR = QSPI_Register; // 赋值寄存器CR

    QSPI_Register = 0;
    QSPI_Register |= 22 << 16; // Flash大小
    // 片选高电平时间1个CLK
    QSPI_Register |= 1;           // SPI模式3
    QUADSPI->DCR = QSPI_Register; // 赋值寄存器DCR

    QUADSPI->CR |= 1; // QSPI使能
}
void W25Q64_Init(void)
{
    uint32_t QSPI_Register = 0;
    /*发送复位使能命令*/

    /*发送复位命令*/
}
void QSPI_and_NorFlash_Init(void)
{
    QSPI_RCC_Init();        // 时钟配置
    QSPI_IO_Init();         // 引脚配置
    QSPI_Peripheral_Init(); // QSPI外设初始化
    W25Q64_Init();          // 外设初始化
}

uint32_t W25Q64_read_ID(void)
{
    uint32_t QSPI_Register = 0;
    QSPI_Register |= 1 << 24;     // 数据线一根
    QSPI_Register |= 1 << 8;      // 指令一根线
    QSPI_Register |= 0x9f;        // 读ID指令
    QUADSPI->DLR |= 2;            // 接收3个数据
    QUADSPI->CCR = QSPI_Register; // 赋值

    while ((QUADSPI->SR >> 5) & 1) // 等待发送
        ;

    QUADSPI->CCR |= 1 << 26; // 接收模式
    uint32_t temp = QUADSPI->DR;
    char *p = (char *)&temp;

    QUADSPI->CCR &= ~(1 << 26); // 默认间接写模式
    return (p[0] << 16) | (p[1] << 8) | p[2];
}
