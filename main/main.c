/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "storage.h"
#include "peripheral.h"
#include "lvgl_port.h"
#include <string.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "device_nvs.h"
#include "my_lvgl.h"
#include "peripheral.h"
#include "pressure_sensor.h"
#include "driver/adc.h"
#include "driver/uart.h"

#define ADC1_TEST_CHANNEL ADC1_CHANNEL_0

#define MAX_ADC_VALUE 4095.0    // 12位ADC的最大值
#define MAX_BATTERY_VOLTAGE 4.2 // 锂电池的最大电压
#define MIN_BATTERY_VOLTAGE 3.0 // 锂电池的最小电压

#define R1 2000000.0 // 分压电阻R1，单位：欧姆
#define R2 1000000.0 // 分压电阻R2，单位：欧姆

void GT911_RST()
{
    // RTS IO4 and INT IO17

    // 设置4号和17号引脚为输出模式
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);

    // 低电平复位
    gpio_set_level(4, 0);
    gpio_set_level(5, 0);

    // 延迟150毫秒，这里为避免时序过短，增加50毫秒
    vTaskDelay(pdMS_TO_TICKS(15));

    // RTS拉高
    gpio_set_level(4, 1);

    // 延迟5ms
    vTaskDelay(pdMS_TO_TICKS(50));

    // 将INT转为悬浮输入态
    gpio_set_direction(5, (GPIO_MODE_INPUT) | (GPIO_MODE_DEF_OD));
}

// int ADCGet(void)
// {
//     int read_raw = adc1_get_raw(ADC1_TEST_CHANNEL); // 采集ADC
//     return read_raw;
// }
// static void ADC_task(void *arg)
// {
//     // float sum = 0;
//     // int count = 0;
//     // int batteryPercentage;
//     // uint8_t frist = 0;
//     uint8_t data[1024]={0};
//     while (1)
//     {
//         int get_adc_data =  adc1_get_raw(ADC1_TEST_CHANNEL);
//         int adc_vcc = get_adc_data * 1253 / 4096;
//         printf("get adc %d  %d\r\n", get_adc_data,adc_vcc);
//         ChargingDetection();

//         // scanf
//         vTaskDelay(50);
//     }
// }

void app_main(void)
{
    storage_nvs_init();
    GT911_RST();
    display_init(); // LVGL初始化

    gpio_init(); // GPIO初始化        阀门控制GPIO40  阀门使能GPIO02  检测电源芯片开关GPIO44
    // adc_Init();     // 采集电源电压
    storage_init(); // SD卡存储初始化
    peripheral_init();

    
    my_lvgl_data_init();
    my_bat_adc_init();
    my_Test_Handle_init();
}
