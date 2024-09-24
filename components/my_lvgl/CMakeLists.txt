idf_component_register(SRCS "my_lvgl.c"  
        REQUIRES lvgl_esp32_drivers lvgl_ui storage peripheral device_nvs nvs_flash
                    INCLUDE_DIRS "include")
