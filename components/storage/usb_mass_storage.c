/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* DESCRIPTION:
 * This example contains code to make ESP32-S3 based device recognizable by USB-hosts as a USB Mass Storage Device.
 * It either allows the embedded application i.e. example to access the partition or Host PC accesses the partition over USB MSC.
 * They can't be allowed to access the partition at the same time.
 * For different scenarios and behaviour, Refer to README of this example.
 */
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include "esp_check.h"
#include "driver/gpio.h"
#include "tinyusb.h"
#include "tusb_msc_storage.h"
#include "usb_mass_storage.h"
#include "storage.h"

#include "diskio_impl.h"
#include "diskio_sdmmc.h"

static const char *TAG = "usb_mass_storage";

#define BASE_PATH "/data" // base path to mount the partition

uint8_t usb_initd = 0;
static sdmmc_card_t *usb_card = NULL;

extern void dcd_init(uint8_t rhport);
extern void dcd_int_enable (uint8_t rhport);

#define EPNUM_MSC       1
#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN)

enum {
    ITF_NUM_MSC = 0,
    ITF_NUM_TOTAL
};

enum {
    EDPT_CTRL_OUT = 0x00,
    EDPT_CTRL_IN  = 0x80,

    EDPT_MSC_OUT  = 0x01,
    EDPT_MSC_IN   = 0x81,
};

static uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EDPT_MSC_OUT, EDPT_MSC_IN, TUD_OPT_HIGH_SPEED ? 512 : 64),
};

static tusb_desc_device_t descriptor_config = {
    .bLength = sizeof(descriptor_config),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x303A, // This is Espressif VID. This needs to be changed according to Users / Customers
    .idProduct = 0x4002,
    .bcdDevice = 0x100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

static char const *string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
    "SENTIENT",                     // 1: Manufacturer
    "Barometer Device",             // 2: Product
    "666666",                       // 3: Serials
    "MSC",                          // 4. MSC
};

#include "esp_console.h"
const esp_console_cmd_t cmds[] = {
    {
        .command = "start",
        .help = "read BASE_PATH/README.MD and print its contents",
        .hint = NULL,
        .func = &usb_mount_start,
    },
    {
        .command = "stop",
        .help = "create file BASE_PATH/README.MD if it does not exist",
        .hint = NULL,
        .func = &usb_mount_stop,
    },
    {
        .command = "write",
        .help = "create file BASE_PATH/README.MD if it does not exist",
        .hint = NULL,
        .func = &record_file_create,
    }
};

void usb_mass_storage_card_init(void)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Initializing storage...");

    ret = storage_usb_init_sdmmc(&usb_card);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "storage init fail...");
        return ;
    }

    const tinyusb_msc_sdmmc_config_t config_sdmmc = {
        .card = usb_card,
    };

    ESP_ERROR_CHECK(tinyusb_msc_storage_init_sdmmc(&config_sdmmc));

    tinyusb_msc_storage_mount(BASE_PATH);

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    repl_config.prompt = CONFIG_IDF_TARGET ">";
    repl_config.max_cmdline_length = 64;
    esp_console_register_help_command();
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    for (int count = 0; count < sizeof(cmds) / sizeof(esp_console_cmd_t); count++) {
        ESP_ERROR_CHECK( esp_console_cmd_register(&cmds[count]) );
    }
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

void usb_mass_storage_driver_init(void)
{
    ESP_LOGI(TAG, "USB MSC initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = &descriptor_config,
        .string_descriptor = string_desc_arr,
        .string_descriptor_count = sizeof(string_desc_arr) / sizeof(string_desc_arr[0]),
        .external_phy = false,
        .configuration_descriptor = desc_configuration,
    };
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB MSC initialization DONE");
}
void usb_mass_storage_driver_deinit(void)
{
    extern esp_err_t tusb_stop_task(void);
    tinyusb_driver_uninstall();
    tusb_stop_task();
    tinyusb_msc_storage_unmount();
    tinyusb_msc_storage_mount(BASE_PATH);
    ESP_LOGI(TAG, "USB MSC DeInit DONE");
}

//usb挂载调用这个
void usb_mount_start(void)
{
    if(usb_initd == 0)
    {
        usb_initd = 1;
        usb_mass_storage_driver_init();
        dcd_init(0);
        dcd_int_enable(0);
    }
}

//usb解挂调用这个
void usb_mount_stop(void)
{
    if(usb_initd == 1)
    {
        usb_initd = 0;
        usb_mass_storage_driver_deinit();
    }
}
