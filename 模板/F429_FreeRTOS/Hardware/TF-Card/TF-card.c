#include "TF-card.h"
#include "stdio.h"

/*IO映射*/
// D0  ~ PC8
// D1  ~ PC9
// D2  ~ PC10
// D3  ~ PC11
// SCk ~ PC12
// CMD ~ PD2

SD_HandleTypeDef g_sdcard_handler;
HAL_SD_CardInfoTypeDef g_sd_card_info_handle; /* SD卡信息结构体 */

uint8_t sd_init(void)
{
    __HAL_RCC_SDIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*D0,D1,D2,D3,SCK*/
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       // 复用推挽
    GPIO_InitStruct.Pull = GPIO_PULLUP;           // 上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 50Mhz
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*CMD*/
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    g_sdcard_handler.Instance = SDIO;
    g_sdcard_handler.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;                      // 上升沿
    g_sdcard_handler.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;                 // 使能可以分频
    g_sdcard_handler.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;          // 时钟常开
    g_sdcard_handler.Init.BusWide = SDIO_BUS_WIDE_1B;                              // 1线
    g_sdcard_handler.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE; // 使能硬件流控
    g_sdcard_handler.Init.ClockDiv = 2;                        // 传输时的分频值
    if (HAL_SD_Init(&g_sdcard_handler) != HAL_OK)
        return 1;
    HAL_SD_ConfigWideBusOperation(&g_sdcard_handler, SDIO_BUS_WIDE_4B);
    
    return 0;
}

/**
 * @brief       判断SD卡是否可以传输(读写)数据
 * @param       无
 * @retval      返回值:SD_TRANSFER_OK      传输完成，可以继续下一次传输
                       SD_TRANSFER_BUSY SD 卡正忙，不可以进行下一次传输
 */
uint8_t get_sd_card_state(void)
{
    return ((HAL_SD_GetCardState(&g_sdcard_handler) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       写SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      0, 正常;  其他, 错误代码(详见SD_Error定义);
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    
    __disable_irq();                                                                        /* 关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
    sta = HAL_SD_WriteBlocks(&g_sdcard_handler, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); /* 多个sector的写操作 */

    /* 等待SD卡写完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq();     /* 开启总中断 */
    
    return sta;
}

/**
 * @brief       读SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      0, 正常;  其他, 错误代码(详见SD_Error定义);
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    
    __disable_irq();                                                                       /* 关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!) */
    sta = HAL_SD_ReadBlocks(&g_sdcard_handler, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT); /* 多个sector的读操作 */

    /* 等待SD卡读完 */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }
    __enable_irq(); /* 开启总中断 */
    
    return sta;
}

/**
 * @brief       通过串口打印SD卡相关信息
 * @param       无
 * @retval      无
 */
void show_sdcard_info(void)
{
    HAL_SD_CardCIDTypeDef sd_card_cid;
    HAL_SD_GetCardCID(&g_sdcard_handler, &sd_card_cid);             /*获取CID */
    HAL_SD_GetCardInfo(&g_sdcard_handler, &g_sd_card_info_handle);  /*获取SD卡信息*/
    printf("\r\n/******* TF_Card_Info *******/\r\n");
    switch (g_sd_card_info_handle.CardType)
    {
    case CARD_SDSC:
    {
        if (g_sd_card_info_handle.CardVersion == CARD_V1_X)
        {
            printf("Card Type:SDSC V1\r\n");
        }
        else if (g_sd_card_info_handle.CardVersion == CARD_V2_X)
        {
            printf("Card Type:SDSC V2\r\n");
        }
    }
    break;

    case CARD_SDHC_SDXC:
        printf("Card Type:SDHC\r\n");
        break;
    default: break;
    }

    printf("Card ManufacturerID:%d\r\n", sd_card_cid.ManufacturerID);                   /* 制造商ID */
    printf("Card RCA:%d\r\n", g_sd_card_info_handle.RelCardAdd);                        /* 卡相对地址 */
    printf("LogBlockNbr:%d \r\n", (uint32_t)(g_sd_card_info_handle.LogBlockNbr));       /* 显示逻辑块数量 */
    printf("LogBlockSize:%d \r\n", (uint32_t)(g_sd_card_info_handle.LogBlockSize));     /* 显示逻辑块大小 */
    printf("Card Capacity:%d MB\r\n", (uint32_t)SD_TOTAL_SIZE_MB(&g_sdcard_handler));   /* 显示容量 */
    printf("Card BlockSize:%d\r\n\r\n", g_sd_card_info_handle.BlockSize);               /* 显示块大小 */
}

