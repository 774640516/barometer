idf_component_register(SRCS "usb_mass_storage.c" "storage.c"
        REQUIRES sdmmc fatfs peripheral tinyusb esp_tinyusb console
                    INCLUDE_DIRS "include")
