#include <stdio.h>
#include "lvgl_helpers.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lvgl_demo.h"

SemaphoreHandle_t xGuiSemaphore;
static lv_disp_draw_buf_t disp_buf;

static void increase_lvgl_tick(void *arg)
{
    lv_tick_inc(1);
}

void lvgl_task(void *pvParameter)
{
    lv_init();                               // lvgl初始化
    lvgl_driver_init();                      // 初始化液晶驱动

    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_DMA);
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_DMA);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);
    // 添加并注册触摸驱动
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = 480;
    disp_drv.ver_res = 800;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    disp_drv.sw_rotate = 0;   // add for rotation
    disp_drv.rotated = LV_DISP_ROT_NONE;   // add for rotation
    lv_disp_drv_register(&disp_drv);

#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif
    lv_demo_init();
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10)); /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_timer_handler();            // 处理LVGL任务
            xSemaphoreGive(xGuiSemaphore); // 释放信号量
        }
    }
}

void display_init(void)
{
    gpio_set_level(6, 0);                    // 开启背光

    const esp_timer_create_args_t lvgl_tick_timer_args = 
    {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1 * 1000));
    xGuiSemaphore = xSemaphoreCreateMutex(); // 创建GUI信号量

    xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 1024 * 8, NULL, 5, NULL, 1);
}
