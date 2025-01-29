#include "sdmmc_sdcard.h"
#include "string.h"
#include "delay.h"
#include "stdio.h"

SD_HandleTypeDef        SDCARD_Handler;             //SD�����
HAL_SD_CardInfoTypeDef  SDCardInfo;                 //SD����Ϣ�ṹ��
DMA_HandleTypeDef SDTxDMAHandler,SDRxDMAHandler;    //SD��DMA���ͺͽ��վ��

//SD_ReadDisk/SD_WriteDisk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
//��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
__align(4) u8 SDIO_DATA_BUFFER[512];

//SD����ʼ��
//����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
u8 SD_Init(void)
{
    //��ʼ��ʱ��ʱ�Ӳ��ܴ���400KHZ 
    SDCARD_Handler.Instance=SDMMC1;
    SDCARD_Handler.Init.ClockEdge=SDMMC_CLOCK_EDGE_RISING;              //������     
    SDCARD_Handler.Init.ClockPowerSave=SDMMC_CLOCK_POWER_SAVE_DISABLE;  //����ʱ���ر�ʱ�ӵ�Դ
    SDCARD_Handler.Init.BusWide=SDMMC_BUS_WIDE_1B;                      //1λ������
    SDCARD_Handler.Init.HardwareFlowControl=SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;//�ر�Ӳ������
    SDCARD_Handler.Init.ClockDiv=6;                //SD����ʱ��Ƶ�����25MHZ
    
    if(HAL_SD_Init(&SDCARD_Handler)!=HAL_OK)
        return 1;
    if(HAL_SD_ConfigWideBusOperation(&SDCARD_Handler,SDMMC_BUS_WIDE_4B)!=HAL_OK)
        return 2;
    show_sdcard_info();
    return 0;
}

//SDMMC�ײ�������ʱ��ʹ�ܣ��������ã�DMA����
//�˺����ᱻHAL_SD_Init()����
//hsd:SD�����
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_SDMMC1_CLK_ENABLE();  //ʹ��SDMMC1ʱ��
    
    //PC8,9,10,11,12
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF12_SDMMC1;  //����ΪSDIO
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��
    
    //PD2
    GPIO_Initure.Pin=GPIO_PIN_2;            
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��

#if (SD_DMA_MODE==1)                        //ʹ��DMAģʽ
    HAL_NVIC_SetPriority(SDMMC1_IRQn,2,0);  //����SDMMC1�жϣ���ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);        //ʹ��SDMMC1�ж�
    
    //���÷���DMA
    SDRxDMAHandler.Instance=DMA2_Stream3;
    SDRxDMAHandler.Init.Channel=DMA_CHANNEL_4;
    SDRxDMAHandler.Init.Direction=DMA_PERIPH_TO_MEMORY;
    SDRxDMAHandler.Init.PeriphInc=DMA_PINC_DISABLE;
    SDRxDMAHandler.Init.MemInc=DMA_MINC_ENABLE;
    SDRxDMAHandler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
    SDRxDMAHandler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
    SDRxDMAHandler.Init.Mode=DMA_PFCTRL;
    SDRxDMAHandler.Init.Priority=DMA_PRIORITY_VERY_HIGH;
    SDRxDMAHandler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;
    SDRxDMAHandler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    SDRxDMAHandler.Init.MemBurst=DMA_MBURST_INC4;
    SDRxDMAHandler.Init.PeriphBurst=DMA_PBURST_INC4;

    __HAL_LINKDMA(hsd, hdmarx, SDRxDMAHandler); //������DMA��SD���ķ���DMA��������
    HAL_DMA_DeInit(&SDRxDMAHandler);
    HAL_DMA_Init(&SDRxDMAHandler);              //��ʼ������DMA
    
    //���ý���DMA 
    SDTxDMAHandler.Instance=DMA2_Stream6;
    SDTxDMAHandler.Init.Channel=DMA_CHANNEL_4;
    SDTxDMAHandler.Init.Direction=DMA_MEMORY_TO_PERIPH;
    SDTxDMAHandler.Init.PeriphInc=DMA_PINC_DISABLE;
    SDTxDMAHandler.Init.MemInc=DMA_MINC_ENABLE;
    SDTxDMAHandler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
    SDTxDMAHandler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
    SDTxDMAHandler.Init.Mode=DMA_PFCTRL;
    SDTxDMAHandler.Init.Priority=DMA_PRIORITY_VERY_HIGH;
    SDTxDMAHandler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;
    SDTxDMAHandler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    SDTxDMAHandler.Init.MemBurst=DMA_MBURST_INC4;
    SDTxDMAHandler.Init.PeriphBurst=DMA_PBURST_INC4;
    
    __HAL_LINKDMA(hsd, hdmatx, SDTxDMAHandler);//������DMA��SD���ķ���DMA��������
    HAL_DMA_DeInit(&SDTxDMAHandler);
    HAL_DMA_Init(&SDTxDMAHandler);              //��ʼ������DMA 
  

    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 3, 0);  //����DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 3, 0);  //����DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
#endif
}


#if (SD_DMA_MODE==1)        //DMAģʽ

//ͨ��DMA��ȡSD��һ������
//buf:�����ݻ�����
//sector:������ַ
//blocksize:������С(һ�㶼��512�ֽ�)
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;
u8 SD_ReadBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt)
{
    u8 err=0;
    if(Get_DCahceSta())SCB_CleanInvalidateDCache(); //���������D_Cache��һ��Ҫ���建��!!!!!!
    err=HAL_SD_ReadBlocks_DMA(&SDCARD_Handler,buf,sector,blocksize,cnt);//ͨ��DMA��ȡSD��һ������
    if(err==0)//��ȡ�ɹ�
    {
        //�ȴ���ȡ���
        err=HAL_SD_CheckReadOperation(&SDCARD_Handler,(uint32_t)SD_TIMEOUT);
    }
    if(Get_DCahceSta())SCB_CleanInvalidateDCache(); //DMA��ɣ��建��
    return err;
}

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//blocksize:������С(һ�㶼��512�ֽ�)
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;	
u8 SD_WriteBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt)
{
    u8 err=0; 
    if(Get_DCahceSta())SCB_CleanInvalidateDCache(); //���������D_Cache��һ��Ҫ���建��!!!!!!
    err=HAL_SD_WriteBlocks_DMA(&SDCARD_Handler,buf,sector,blocksize,cnt);//ͨ��DMAдSD��һ������
    if(err==0)//д�ɹ�
    {
        //�ȴ���ȡ���/
       err=HAL_SD_CheckWriteOperation(&SDCARD_Handler,(uint32_t)SD_TIMEOUT);
    }
    if(Get_DCahceSta())SCB_CleanInvalidateDCache(); //DMA��ɣ��建��
    return err;
}

//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;
u8 SD_ReadDisk(u8* buf,u32 sector,u8 cnt)
{
    u8 sta=SD_OK;
    long long lsector=sector;
    u8 n;
    if(SDCardInfo.CardType!=STD_CAPACITY_SD_CARD_V1_1)lsector<<=9;
    if((u32)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            sta=SD_ReadBlocks_DMA((uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);
            memcpy(buf,SDIO_DATA_BUFFER,512);
            buf+=512;
        }
    }else
    {
        sta=SD_ReadBlocks_DMA((uint32_t*)buf,lsector, 512,cnt);
    }
    return sta;
}  

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;	
u8 SD_WriteDisk(u8 *buf,u32 sector,u8 cnt)
{   
    u8 sta=SD_OK;
    long long lsector=sector;
    u8 n;
    if(SDCardInfo.CardType!=STD_CAPACITY_SD_CARD_V1_1)lsector<<=9;
    if((u32)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            memcpy(SDIO_DATA_BUFFER,buf,512);
            sta=SD_WriteBlocks_DMA((uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector��д����
            buf+=512;
        }
    }else
    {
        sta=SD_WriteBlocks_DMA((uint32_t*)buf,lsector,512,cnt);//���sector��д����
    }
    return sta;
} 

//SDMMC1�жϷ�����
void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&SDCARD_Handler);
}

//DMA2������6�жϷ�����
void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmatx);
}

//DMA2������3�жϷ�����
void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmarx);
}
#else                                   //��ѵģʽ

//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;
u8 SD_ReadDisk(u8* buf,u32 sector,u8 cnt)
{
    u8 sta=0;
    long long lsector=sector;
    u8 n;
    lsector<<=9;
    __disable_irq(); //�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    if((u32)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            sta=HAL_SD_ReadBlocks(&SDCARD_Handler,SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector�Ķ�����
            memcpy(buf,SDIO_DATA_BUFFER,512);
            buf+=512;
        }
    }else
    {
        sta=HAL_SD_ReadBlocks(&SDCARD_Handler,buf,lsector,512,cnt);//����sector�Ķ�����
    }
    __enable_irq();//�������ж�
    return sta;
}  

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;	
u8 SD_WriteDisk(u8 *buf,u32 sector,u8 cnt)
{   
    u8 sta=0;
    long long lsector=sector;
    u8 n;
    lsector<<=9;
    __disable_irq();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    if((u32)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            memcpy(SDIO_DATA_BUFFER,buf,512);
            sta=HAL_SD_WriteBlocks(&SDCARD_Handler,SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector��д����
            buf+=512;
        }
    }else
    {
        sta=HAL_SD_WriteBlocks(&SDCARD_Handler,buf,lsector,512,cnt);//���sector��д����
    }
    __enable_irq();//�������ж�
    return sta;
} 

/**
 * @brief       ͨ�����ڴ�ӡSD�������Ϣ
 * @param       ��
 * @retval      ��
 */
void show_sdcard_info(void)
{
    HAL_SD_CardCIDTypeDef sd_card_cid;
    HAL_SD_GetCardCID(&SDCARD_Handler, &sd_card_cid);             /*��ȡCID */
    HAL_SD_GetCardInfo(&SDCARD_Handler, &SDCardInfo);  /*��ȡSD����Ϣ*/

    switch (SDCardInfo.CardType)
    {
    case CARD_SDSC:
    {
        if (SDCardInfo.CardVersion == CARD_V1_X)
        {
            printf("Card Type:SDSC V1\r\n");
        }
        else if (SDCardInfo.CardVersion == CARD_V2_X)
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

    printf("Card ManufacturerID:%d\r\n", sd_card_cid.ManufacturerID);                   /* ������ID */
    printf("Card RCA:%d\r\n", SDCardInfo.RelCardAdd);                        /* ����Ե�ַ */
    printf("LogBlockNbr:%d \r\n", (uint32_t)(SDCardInfo.LogBlockNbr));       /* ��ʾ�߼������� */
    printf("LogBlockSize:%d \r\n", (uint32_t)(SDCardInfo.LogBlockSize));     /* ��ʾ�߼����С */
    printf("Card Capacity:%d MB\r\n", (uint32_t)SD_TOTAL_SIZE_MB(&SDCARD_Handler));   /* ��ʾ���� */
    printf("Card BlockSize:%d\r\n\r\n", SDCardInfo.BlockSize);               /* ��ʾ���С */
}

#endif




