#include "esp_system.h"

esp_err_t storage_save_key_blob(char* key,uint8_t* value,uint32_t length);
esp_err_t storage_save_key_value(char* key,uint32_t value);
esp_err_t storage_read_key_blob(char* key,uint8_t* value,uint32_t *length);
esp_err_t storage_read_key_value(char* key,uint32_t *value);
esp_err_t storage_nvs_init(void);

char *my_nvs_Get_Device_ID();
uint32_t my_nvs_Get_Device_ID_number();
