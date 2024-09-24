#include <stdio.h>
#include <string.h>
#include <time.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "ds3231.h"

#define I2C_DS3231_NUM 1                        /*!< I2C port number for master dev */

static const char *TAG = "ds3231";

#define     DS3231_ARRD         0x68    /* slave address */

#define     REG_SEC             0x00
#define     REG_MIN             0x01
#define     REG_HOUR            0x02
#define     REG_DAY             0x03
#define     REG_WEEK            0x04
#define     REG_MON             0x05
#define     REG_YEAR            0x06
#define     REG_ALM1_SEC        0x07
#define     REG_ALM1_MIN        0x08
#define     REG_ALM1_HOUR       0x09
#define     REG_ALM1_DAY_DATE   0x0A
#define     REG_ALM2_MIN        0x0B
#define     REG_ALM2_HOUR       0x0C
#define     REG_ALM2_DAY_DATE   0x0D
#define     REG_CONTROL         0x0E
#define     REG_STATUS          0x0F
#define     REG_AGING_OFFSET    0x10
#define     REG_TEMP_MSB        0x11
#define     REG_TEMP_LSB        0x12

static unsigned char bcd_to_hex(unsigned char data)
{
    unsigned char temp;

    temp = ((data>>4)*10 + (data&0x0f));
    return temp;
}

static unsigned char hex_to_bcd(unsigned char data)
{
    unsigned char temp;

    temp = (((data/10)<<4) + (data%10));
    return temp;
}

static esp_err_t ds3231_read_reg(uint8_t reg,uint8_t *data,uint8_t data_size)
{
    esp_err_t ret = ESP_OK;

    ret = i2c_master_write_read_device(I2C_DS3231_NUM, DS3231_ARRD, &reg, 1, data , data_size, 1000);

    return ret;
}

static esp_err_t ds3231_write_reg(uint8_t reg, uint8_t *data, uint8_t data_size)
{
    esp_err_t ret = ESP_OK;

    uint8_t *send_buf = malloc(data_size + 1);
    memcpy(&send_buf[1],data,data_size);
    send_buf[0] = reg;
    i2c_master_write_to_device(I2C_DS3231_NUM, DS3231_ARRD, send_buf , data_size + 1, 1000);
    free(send_buf);

    return ret;
}

esp_err_t ds3231_init_register(void)
{
    uint8_t data = 0x04;    /* close clock out */
    return ds3231_write_reg(REG_CONTROL, &data, 1);
}

time_t ds3231_get_time(void)
{
    time_t      time;
    struct tm   time_temp;
    uint8_t  buff[7];
    esp_err_t ret = ESP_OK;

    ret = ds3231_read_reg(REG_SEC,buff,7);
    if(ret == ESP_OK)
    {
        time_temp.tm_year  = bcd_to_hex(buff[6]) + 2000 - 1900;
        time_temp.tm_mon   = bcd_to_hex(buff[5]&0x7f) - 1;
        time_temp.tm_mday  = bcd_to_hex(buff[4]);
        time_temp.tm_hour  = bcd_to_hex(buff[2]);
        time_temp.tm_min   = bcd_to_hex(buff[1]);
        time_temp.tm_sec   = bcd_to_hex(buff[0]);
        time = mktime(&time_temp);
        return time;
    }
    else
    {
        return 0;
    }
}

void ds3231_set_time(uint8_t  buff[7])
{
    // time_t      *time;
    // struct tm *time_new;
    // uint8_t  buff[7];
    esp_err_t ret = ESP_OK;

    // time =&time_set;
    // time_new = localtime(time);
    
    buff[6] = hex_to_bcd(buff[6]);
    buff[5] = hex_to_bcd(buff[5]);
    buff[4] = hex_to_bcd(buff[4]);
    buff[3] = hex_to_bcd(buff[3]);
    buff[2] = hex_to_bcd(buff[2]);
    buff[1] = hex_to_bcd(buff[1]);
    buff[0] = hex_to_bcd(buff[0]);
    // printf("Setting Time = %d %d %d   %d %d %d\r\n", time_new->tm_year, time_new->tm_mon + 1, time_new->tm_mday, time_new->tm_hour, time_new->tm_min, time_new->tm_sec);
    for(int i=0;i<7;i++){
        printf("%x  ",buff[6- i]);
    }
    printf("Setting Time \r\n");
    ret = ds3231_write_reg(REG_SEC, buff, 7);
    if(ret == ESP_OK)
    {
        ESP_LOGI(TAG,"time set to success\r\n");
    }
    else
    {
        ESP_LOGE(TAG,"time set failed\r\n");
    }
}

float ds3231_get_temperature(void)
{
    int8_t buff[2];
    float temp = 0.0f;
    esp_err_t ret = ESP_OK;

    ret = ds3231_read_reg(REG_TEMP_MSB, (uint8_t*)buff, 2);
    if(ret == ESP_OK)
    {
        if(buff[0] & 0x80)
        {/* negative temperature */
            temp = buff[0];
            temp -= (buff[1]>>6)*0.25;  /* 0.25C resolution */
        }
        else
        {/* positive temperature */
            temp = buff[0];
            temp += ((buff[1]>>6)&0x03)*0.25;
        }

        return temp;
    }

    return 0;
}

void ds3231_init(void)
{
    if(ds3231_init_register() == ESP_OK)
    {
        printf("the rtc of ds3231 init succeed!\r\n");
    }
    else
    {
        printf("the rtc of ds3231 init failed!\r\n");
    }
}