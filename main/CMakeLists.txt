idf_component_register(
    SRCS "main.c" "lvgl_port.c" "lvgl_demo.c" 
    REQUIRES lvgl_esp32_drivers lvgl_ui storage peripheral my_lvgl device_nvs
                    INCLUDE_DIRS "")
nvs_create_partition_image(nvs ../nvs_data.csv FLASH_IN_PROJECT)
