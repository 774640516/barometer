#include <stdio.h>
#include "storage.h"
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "ds3231.h"
#include "fcntl.h"
#include "esp_check.h"
#include "usb_mass_storage.h"

#define EXAMPLE_MAX_CHAR_SIZE 64    

static const char *TAG = "sdcard";

int file_fd = -1;
bool host_init = false;
char record_file_name[64] = {0};

uint8_t record_file_create(char *CustomID)
{
    char file_name[64] = {0};

    time_t tick = ds3231_get_time();
    struct tm tm;
    tm = *localtime(&tick);
    char date_str[12];
    strftime(date_str, sizeof(date_str), "%Y-%m", &tm);

    sprintf(file_name, "/data/%s", date_str);
    mkdir(file_name,0777);
    // sprintf(file_name, "/data/Test/%s.csv", CustomID);
    sprintf(file_name, "/data/%s/%s.csv", date_str ,CustomID);
    if (file_fd >= 0)
    {
        close(file_fd);
        memset(record_file_name, 0, sizeof(record_file_name));
    }


    file_fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND);
    if (file_fd >= 0)
    {
        char write_buf[128];
        sprintf(write_buf, "MachineID StaffID TestMode Date Time Stage TotalCount SubCount Pressure PressureUnit\n");
        write(file_fd, write_buf, strlen(write_buf));
        close(file_fd);
        printf("file %s create success\r\n", file_name);
        memcpy(record_file_name, file_name, sizeof(file_name));
        return ESP_OK;
    }
    else
    {
        printf("file %s create failed\r\n", file_name);
        return ESP_FAIL;
    }
}

void record_file_close(void)
{
    if (file_fd >= 0)
    {
        close(file_fd);
    }
}

void write_record_to_file(uint32_t machine_id, char *staff_id, uint8_t mode, uint8_t stage, uint16_t t_cnt, uint16_t s_cnt, double pressure, uint8_t PressureUnit)
{
    time_t tick = ds3231_get_time();
    struct tm tm;
    tm = *localtime(&tick);

    char date_str[12];
    strftime(date_str, sizeof(date_str), "%Y%m%02d", &tm);
    char time_str[12];
    // strftime(time_str, sizeof(time_str), "%H:%M:%S",&tm);
    sprintf(time_str, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    char write_buffer[128] = {0};
    // sprintf(write_buffer,"%05ld %06ld %d %s %s %X %d %d %2.2f\n",machine_id,staff_id,mode,date_str,time_str,stage,t_cnt,s_cnt,pressure);
    char unit_str[] = "KPa";
    char unit1_str[] = "inH2O";
    if (PressureUnit)
    {
        sprintf(write_buffer, "%05ld %s %d %s %s %X %d %d %2.2f %s\n", machine_id, staff_id, mode, date_str, time_str, stage, t_cnt, s_cnt, pressure, unit_str);
    }
    else
    {
        sprintf(write_buffer, "%05ld %s %d %s %s %X %d %d %2.2f %s\n", machine_id, staff_id, mode, date_str, time_str, stage, t_cnt, s_cnt, pressure, unit1_str);
    }
    file_fd = open(record_file_name, O_WRONLY | O_CREAT | O_APPEND);
    if (file_fd >= 0)
    {
        write(file_fd, write_buffer, sizeof(write_buffer));
        close(file_fd);
        printf("file open and write success\r\n");
    }
    else
    {
        printf("file open failed\r\n");
    }
}

esp_err_t storage_usb_init_sdmmc(sdmmc_card_t **card)
{
    esp_err_t ret = ESP_OK;
    bool host_init = false;
    sdmmc_card_t *sd_card;

    ESP_LOGI(TAG, "Initializing SD card");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    slot_config.width = 4;
    slot_config.clk = 47;
    slot_config.cmd = 21;
    slot_config.d0 = 48;
    slot_config.d1 = 45;
    slot_config.d2 = 13;
    slot_config.d3 = 14;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    sd_card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    ESP_GOTO_ON_FALSE(sd_card, ESP_ERR_NO_MEM, clean, TAG, "could not allocate new sdmmc_card_t");

    ESP_GOTO_ON_ERROR((*host.init)(), clean, TAG, "Host Config Init fail");

    host_init = true;

    ESP_GOTO_ON_ERROR(sdmmc_host_init_slot(host.slot, (const sdmmc_slot_config_t *)&slot_config),
                      clean, TAG, "Host init slot fail");

    ESP_GOTO_ON_ERROR(sdmmc_card_init(&host, sd_card),
                      clean, TAG, "The detection pin of the slot is disconnected(Insert uSD card)");

    sdmmc_card_print_info(stdout, sd_card);
    *card = sd_card;

    return ESP_OK;
clean:
    if (host_init)
    {
        if (host.flags & SDMMC_HOST_FLAG_DEINIT_ARG)
        {
            host.deinit_p(host.slot);
        }
        else
        {
            (*host.deinit)();
        }
    }
    if (card)
    {
        free(card);
        card = NULL;
    }

    return ret;
}

void storage_init(void)
{
    usb_mass_storage_card_init();
}
