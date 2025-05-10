#include "ecc_detection.h"
#include "h7_core.h"

#if UES_RAM_ECC==1

RAMECC_HandleTypeDef hramecc;

void ram_ecc_enable(void)
{
     hramecc.Instance=RAMECC1_Monitor5;  //���ITCM RAM:64KBytes
     HAL_RAMECC_Init(&hramecc);
     __HAL_RAMECC_ENABLE_IT(&hramecc, RAMECC_IT_MONITOR_ALL);//ʹ��ȫ���ж�
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
    printf("�����ַ:%#x\r\n",HAL_RAMECC_GetFailingAddress(hramecc));
    printf("�������ݵ�λ:%#x\r\n",HAL_RAMECC_GetFailingDataLow(hramecc));
    printf("�������ݸ�λ:%#x\r\n",HAL_RAMECC_GetFailingDataHigh(hramecc));
    printf("������:%#x\r\n",HAL_RAMECC_GetHammingErrorCode(hramecc));
    if(HAL_RAMECC_IsECCSingleErrorDetected(hramecc))
        printf("�����˵�bit����\r\n");
    if(HAL_RAMECC_IsECCDoubleErrorDetected(hramecc))
        printf("������˫bit����\r\n");
}


#endif



