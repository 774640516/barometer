#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "device_nvs.h"

#define STORAGE_NAMESPACE "storage"

esp_err_t storage_save_key_blob(char* key,uint8_t* value,uint32_t length)       //写入字符串
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_set_blob(my_handle, key, value, length);
    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);

    return ESP_OK;
}

esp_err_t storage_save_key_value(char* key,uint32_t value)              //写入数据
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_set_u32(my_handle, key, value);
    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);

    return ESP_OK;
}

esp_err_t storage_read_key_blob(char* key,uint8_t* value,uint32_t *length)         //读取字符串
{
    nvs_handle_t my_handle;
    esp_err_t err;
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_get_blob(my_handle, key, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) 
        return err;

    if (required_size == 0) 
    {
        printf("storage_read_app_key empty!\n");
        err = ESP_ERR_NVS_NOT_FOUND;
    } 
    else 
    {
        err = nvs_get_blob(my_handle, key, value, &required_size);
        if (err != ESP_OK) {
            return err;
        }
        *length = required_size;
    }
    // Close
    nvs_close(my_handle);

    return err;
}

esp_err_t storage_read_key_value(char* key,uint32_t *value)         //读取
{
    esp_err_t err;
    nvs_handle_t my_handle;

    uint32_t value_temp = 0;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_get_u32(my_handle, key, &value_temp);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) 
        return err;

    *value = value_temp;

    // Close
    nvs_close(my_handle);

    return err;
}
// static const char *TAG = "example";
uint32_t Test_ID = 99999;
char Device_ID_String[128] = "機身編號: 99999";
esp_err_t storage_nvs_init(void)        //初始化
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    printf("Opening Non-Volatile Storage (NVS) handle\r\n");
    nvs_handle_t my_handle;
    err = nvs_open_from_partition("nvs", "storage", NVS_READONLY, &my_handle);
    // if (err != ESP_OK) {
    //     printf(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    //     // return;
    // }
    // printf(TAG, "The NVS handle successfully opened");

    // // Read values
    // printf(TAG, "Reading values from NVS");

    
    err = nvs_get_u32(my_handle, "Test_ID", &Test_ID);
    // ESP_ERROR_CHECK(err);
    sprintf(Device_ID_String,"機身編號: %05ld",Test_ID);
    // printf("%"PRIu8"\n", Test_ID);
    // assert(Test_ID == 255);

    // int8_t i8_val = 0;
    // err = nvs_get_i8(my_handle, "i8_key", &i8_val);
    // ESP_ERROR_CHECK(err);
    // printf("%"PRIi8"\n", i8_val);
    // assert(i8_val == -128);

    // uint16_t u16_val = 0;
    // err = nvs_get_u16(my_handle, "u16_key", &u16_val);
    // ESP_ERROR_CHECK(err);
    // printf("%"PRIu16"\n", u16_val);
    // assert(u16_val == 65535);

    // uint32_t u32_val = 0;
    // err = nvs_get_u32(my_handle, "u32_key", &u32_val);
    // ESP_ERROR_CHECK(err);
    // printf("%"PRIu32"\n", u32_val);
    // assert(u32_val == 4294967295);

    // int32_t i32_val = 0;
    // err = nvs_get_i32(my_handle, "i32_key", &i32_val);
    // ESP_ERROR_CHECK(err);
    // printf("%"PRIi32"\n", i32_val);
    // assert(i32_val == -2147483648);

    // size_t str_len = 0;
    // err = nvs_get_str(my_handle, "str_key", NULL, &str_len);
    // ESP_ERROR_CHECK(err);
    // assert(str_len == 222);

    // char* str_val = (char*) malloc(str_len);
    // err = nvs_get_str(my_handle, "str_key", str_val, &str_len);
    // ESP_ERROR_CHECK(err);
    // printf("%s\n", str_val);
    // assert(str_val[0] == 'L');
    // free(str_val);

    // // Close
    // nvs_close(my_handle);
    // ESP_LOGI(TAG, "Reading values from NVS done - all OK");

    return err;
}

char *my_nvs_Get_Device_ID()
{
    printf("Device_ID_String = %s\r\n",Device_ID_String);
    return Device_ID_String;
}
uint32_t my_nvs_Get_Device_ID_number()
{
    return Test_ID;
}