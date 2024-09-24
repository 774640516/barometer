#include <time.h>

void ds3231_init(void);
time_t ds3231_get_time(void);
void ds3231_set_time(uint8_t  buff[7]);
float ds3231_get_temperature(void);