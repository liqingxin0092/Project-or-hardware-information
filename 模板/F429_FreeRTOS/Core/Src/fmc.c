#include "fmc.h"

/*nand flash 映射*/
/*
PE7   ------> FMC_D4
PE8   ------> FMC_D5
PE9   ------> FMC_D6
PE10   ------> FMC_D7
PD11   ------> FMC_CLE (T) (A16)
PD12   ------> FMC_ALE (T) (A17)
PD14   ------> FMC_D0
PD15   ------> FMC_D1
PD0   ------> FMC_D2
PD1   ------> FMC_D3
PD4   ------> FMC_NOE (RE)
PD5   ------> FMC_NWE (WE)
PG9   ------> FMC_NCE3 (CE)
PD6   ------> RB
*/

/*接口*/
// 选择的是BANK3,映射的地址是0x8000 0000
//*(volatile uint8_t*) 0x80000000 = data;
//*(volatile uint8_t*) 0x80000000|(1<<16) = command;
//*(volatile uint8_t*) 0x80000000|(1<<17) = address;

NAND_HandleTypeDef hnand1;

/*SDRAM*/
SDRAM_HandleTypeDef hsdram1;
void MX_FMC_Init(void)
{
      /*NAND FLASH初始化*/
  FMC_NAND_PCC_TimingTypeDef ComSpaceTiming = {0};
  FMC_NAND_PCC_TimingTypeDef AttSpaceTiming = {0};
  hnand1.Instance = FMC_NAND_DEVICE;
  /* hnand1.Init */
  hnand1.Init.NandBank = FMC_NAND_BANK3;
  hnand1.Init.Waitfeature = FMC_NAND_PCC_WAIT_FEATURE_DISABLE;
  hnand1.Init.MemoryDataWidth = FMC_NAND_PCC_MEM_BUS_WIDTH_8;
  hnand1.Init.EccComputation = FMC_NAND_ECC_DISABLE;
  hnand1.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_512BYTE;
  hnand1.Init.TCLRSetupTime = 4;
  hnand1.Init.TARSetupTime = 4;
  /* hnand1.Config */
  hnand1.Config.PageSize = 2048;
  hnand1.Config.SpareAreaSize = 64;
  hnand1.Config.BlockSize = 64;
  hnand1.Config.BlockNbr = 2048;
  hnand1.Config.PlaneNbr = 2;
  hnand1.Config.PlaneSize = 1024;
  hnand1.Config.ExtraCommandEnable = DISABLE;
  /* ComSpaceTiming */
  ComSpaceTiming.SetupTime = 9;
  ComSpaceTiming.WaitSetupTime = 9;
  ComSpaceTiming.HoldSetupTime = 10;
  ComSpaceTiming.HiZSetupTime = 9;
  /* AttSpaceTiming */
  AttSpaceTiming.SetupTime = 9;
  AttSpaceTiming.WaitSetupTime = 9;
  AttSpaceTiming.HoldSetupTime = 10;
  AttSpaceTiming.HiZSetupTime = 9;

  if (HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming) != HAL_OK)
  {
    Error_Handler( );
  }
  
  /*SDRAM初始化*/
  FMC_SDRAM_TimingTypeDef SdramTiming = {0};
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 8;
  SdramTiming.SelfRefreshTime = 6;
  SdramTiming.RowCycleDelay = 6;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  HAL_SDRAM_Init(&hsdram1, &SdramTiming);
  
}

static uint32_t FMC_Initialized = 0;

static void HAL_SDRAM_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (FMC_Initialized)
  {
    return;
  }
  FMC_Initialized = 1;

  __HAL_RCC_FMC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0
                          |GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


}
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* sdramHandle)
{
  HAL_SDRAM_IO_Init();
}

//static void HAL_NAND_IO_Init(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if (FMC_Initialized) 
//  {
//    return;
//  }
//  FMC_Initialized = 1;
//  __HAL_RCC_FMC_CLK_ENABLE();
//  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

//  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

//  /* GPIO_InitStruct */
//  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15
//                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

//  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

//  /* GPIO_InitStruct */
//  GPIO_InitStruct.Pin = GPIO_PIN_9;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

//  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

//}
void HAL_NAND_MspInit(NAND_HandleTypeDef* nandHandle)
{
HAL_SDRAM_IO_Init();
}
