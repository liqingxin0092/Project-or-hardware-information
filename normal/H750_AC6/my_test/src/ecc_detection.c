#include "ecc_detection.h"
#include "h7_core.h"

#if UES_RAM_ECC==1

RAMECC_HandleTypeDef hramecc;

void ram_ecc_enable(void)
{
     hramecc.Instance=RAMECC1_Monitor5;  //监控ITCM RAM:64KBytes
     HAL_RAMECC_Init(&hramecc);
     __HAL_RAMECC_ENABLE_IT(&hramecc, RAMECC_IT_MONITOR_ALL);//使能全部中断
     __set_priority(ECC_IRQn,4,0);
     __enable_IRQ(ECC_IRQn);    
    hramecc.Instance->CR |= RAMECC_CR_ECCELEN;   //enable RAMECC monitor 
    __HAL_RAMECC_CLEAR_FLAG(&hramecc,RAMECC_FLAGS_ALL);
}

void ECC_IRQHandler(void)
{
    HAL_RAMECC_IRQHandler(&hramecc);
}

void HAL_RAMECC_DetectErrorCallback(RAMECC_HandleTypeDef *hramecc)
{
    printf("问题地址:%#x\r\n",HAL_RAMECC_GetFailingAddress(hramecc));
    printf("问题数据低位:%#x\r\n",HAL_RAMECC_GetFailingDataLow(hramecc));
    printf("问题数据高位:%#x\r\n",HAL_RAMECC_GetFailingDataHigh(hramecc));
    printf("海明码:%#x\r\n",HAL_RAMECC_GetHammingErrorCode(hramecc));
    if(HAL_RAMECC_IsECCSingleErrorDetected(hramecc))
        printf("发生了单bit错误\r\n");
    if(HAL_RAMECC_IsECCDoubleErrorDetected(hramecc))
        printf("发生了双bit错误\r\n");
}


#endif



