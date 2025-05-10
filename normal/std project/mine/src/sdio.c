#include "sdio.h"
#include "stdio.h"
#include "gd32f4xx_gpio.h"
#include "gd32f4xx_sdio.h"
#include "drv_sdcard.h"

//Ê×ÏÈ
//D0       ~  C8
//D1       ~  C9
//D2       ~  C10
//D3       ~  C11
//CLK      ~  C12
//CMD      ~  D2

sd_card_info_struct sd_cardinfo;/* information of SD card */

sd_error_enum sd_config(void)
{
    sd_error_enum status = SD_OK;
    uint32_t cardstate = 0;
    /* initialize the card */
    status = sd_init();
    if(SD_OK == status) {
        status = sd_card_information_get(&sd_cardinfo);
    }
    printf("\r\ncard_blocksize:%d\r\n",sd_cardinfo.card_blocksize);
//    printf("card_capacity:%d\r\n",sd_cardinfo.card_capacity);
    printf("card_type:%d\r\n",sd_cardinfo.card_type);
    if(SD_OK == status) {
        status = sd_card_select_deselect(sd_cardinfo.card_rca);
    }
    status = sd_cardstatus_get(&cardstate);
    if(cardstate & 0x02000000) {
        printf("\r\n The card is locked!");
#if 0
        /* unlock the card if necessary */
        status = sd_lock_unlock(SD_UNLOCK);
        if(SD_OK != status) {
            printf("\r\n Unlock failed!");
            while(1) {
            }
        } else {
            printf("\r\n The card is unlocked! Please reset MCU!");
        }
#endif
        while(1) {
        }
    }
    if((SD_OK == status) && (!(cardstate & 0x02000000))) {
        /* set bus mode */
        status = sd_bus_mode_config(SDIO_BUSMODE_4BIT);
//        status = sd_bus_mode_config( SDIO_BUSMODE_1BIT );
    }
    if(SD_OK == status) {
        /* set data transfer mode */
//        status = sd_transfer_mode_config( SD_DMA_MODE );
        status = sd_transfer_mode_config(SD_POLLING_MODE);
    }

    return status;
}
