#include <stdio.h>
#include "peripheral.h"
#include "pressure_sensor.h"
#include "ds3231.h"

#define CCT (+8)

void peripheral_init(void)
{
    pressure_sensor_init();
    ds3231_init();
    time_t tick = ds3231_get_time();
    printf("time= %lld \r\n",tick);
    struct tm *tm;
    tm = localtime(&tick);
    printf("read ok%04d-%02d-%02d %02d:%02d   \r\n", tm->tm_year + 1900 ,  tm->tm_mon + 1,tm->tm_mday, (tm->tm_hour+CCT)%24,tm->tm_min);
}
