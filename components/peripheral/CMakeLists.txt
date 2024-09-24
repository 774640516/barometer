idf_component_register(SRCS "ds3231.c" "pressure_sensor.c" "peripheral.c"
                REQUIRES driver my_lvgl
                INCLUDE_DIRS "include")
