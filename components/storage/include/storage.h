#include <esp_err.h>
#include "driver/sdmmc_host.h"

void storage_init(void);
esp_err_t storage_usb_init_sdmmc(sdmmc_card_t **card);
uint8_t record_file_create(char *CustomID);
void write_record_to_file(uint32_t machine_id,char *staff_id,uint8_t mode,uint8_t stage,uint16_t t_cnt,uint16_t s_cnt,double pressure,uint8_t PressureUnit);
void usb_mount_start(void);
void usb_mount_stop(void);
esp_err_t sd_card_mount(void);
esp_err_t sd_card_unmount(void); 