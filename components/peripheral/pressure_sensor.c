
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "pressure_sensor.h"
#include "my_lvgl.h"

static const char *TAG = "pressure_sensor";

#define I2C_MASTER_SCL_IO GPIO_NUM_41 /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_42 /*!< gpio number for I2C master data  */
#define I2C_PRESSURE_SENSOR_NUM 1     /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000     /*!< I2C master clock frequency */

#define DATA_MSB 0X06
#define DATA_CSB 0X07
#define DATA_LSB 0X08 // 压力值
#define TEMP_MSB 0X09
#define TEMP_LSB 0X0A   // 温度值
#define CMD 0X30        // 命令寄存器
#define SYS_CONFIG 0XA5 //
#define P_CONFIG 0XA6
#define I2C_ADDR 0X6D // 气压传感器设备地址

#define I2C_Battery_ADDR 0X6B // 电池芯片设备地址

int16_t pressure_value = 0;
int16_t powerSTATE = 0;

static int read_pressure(void)
{
    uint8_t data[3] = {0};
    uint8_t reg_addr = DATA_MSB;

    i2c_master_write_read_device(I2C_PRESSURE_SENSOR_NUM, I2C_ADDR, &reg_addr, 1, data, 3, 1000);
    // printf("data0=%x,  data1=%x,  data2=%x \n",data[0] , data[1] , data[2]);
    return (data[0] << 16) | (data[1] << 8) | data[2]; // 单位为 Pa
}

void XGZP6897D_Wait(void)
{
    uint8_t recv_data;

    uint8_t send_buf[3];
    uint32_t value = 0;
    send_buf[0] = CMD;
    send_buf[1] = TEMP_LSB;
    send_buf[2] = 1;
    i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_ADDR, send_buf, 3, 1000);
    i2c_master_write_read_device(I2C_PRESSURE_SENSOR_NUM, I2C_ADDR, send_buf, 1, &recv_data, 1, 1000);
    if (recv_data & 0x80)
    {
        ESP_LOGI(TAG, "error pressure\r\n");
    }
    else
    {

        value = read_pressure();
        // printf("V=%ld\r\n",value);
        if (value >= 8388608) // 0x800000   1048576 16711680
        {
            pressure_value = (value - 16777216) / 512;
        }
        else
        {
            pressure_value = value / 512;
        }
        // printf("v:%ld \n",  value );
        // printf("pd:%d \n",  pressure_value);
    }
}

void PowerOFF()
{
    uint8_t send_buf[3];
    uint8_t recv_data1[2];
    send_buf[0] = 0x07;
    send_buf[1] = 0x64;
    i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 2, 1000);
    printf("ok:off \n");
    send_buf[0] = 0x07;
    // i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 1, 1000);
    i2c_master_write_read_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 1, &recv_data1, 1, 1000);
}
uint8_t PowerState_I2C(void)
{
    uint8_t recv_data1[2];
    uint8_t send_buf[3];
    send_buf[0] = 0x08;
    // i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 1, 1000);
    i2c_master_write_read_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 1, &recv_data1, 2, 1000);
    // printf("bt:%x   %x\n", recv_data1[0], recv_data1[1]);
    //  send_buf[0]=0x01;send_buf[1]=0x5A;
    //  i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 2, 1000);
    return recv_data1[0];
}

uint8_t Power_VSYS_MIN_I2C_init(void)
{
    uint8_t send_buf[2];
    uint8_t recv_data1[4];
    send_buf[0] = 0X07;
    send_buf[1] = 0x40;
    // i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 2, 1000);
    i2c_master_write_read_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 1, &recv_data1, 1, 1000);
    if(recv_data1[0] == 0x40){
        return 0;
    }else if(recv_data1[0] == 0x64){
        i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 2, 1000);
        return 1;
    }else {
        i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 2, 1000);
        return 0;
    }
    // printf("VSYS_MIN:%x \n", recv_data1[0]);
    //  send_buf[0]=0x01;send_buf[1]=0x5A;
    //  i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 2, 1000);
    // return recv_data1[0];
}

void set_power_i2c(uint8_t data){
    uint8_t send_buf[2];
    send_buf[0] = 0x07;
    send_buf[1] = data;
    i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 2, 1000);
}

void ChargingDetection(void)
{
    uint8_t data;
    data = 0x18 & PowerState_I2C();
    if (data == 0x00)
    {
        my_charge_set_status(0); // 没有充电
    }
    else if (data == 0x10)
    {
        my_charge_set_status(1); // 快速充电
    }
    else if (data == 0x08)
    {
        my_charge_set_status(2); // 涓流充电
    }
    else if(data == 0x18)
    {
        my_charge_set_status(3); // 充满电
    }
}

static void sensor_read_task(void *arg)
{
    while (1)
    {
        // POWER_STATE(powerSTATE);
        XGZP6897D_Wait();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int16_t pressure_sensor_value_read(void)
{
    return pressure_value;
}

void pressure_sensor_init(void)
{
    uint8_t recv_data;
    uint8_t send_buf[3];
    int i2c_master_port = I2C_PRESSURE_SENSOR_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);

    send_buf[0] = CMD;
    send_buf[1] = TEMP_LSB;
    send_buf[2] = 1;
    i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_ADDR, send_buf, 3, 1000);
    i2c_master_write_read_device(I2C_PRESSURE_SENSOR_NUM, I2C_ADDR, send_buf, 1, &recv_data, 1, 1000);
    if (recv_data & 0x80)
    {
        ESP_LOGI(TAG, "error pressure\r\n");
    }
    else
    {
        ESP_LOGI(TAG, "I2C initialized successfully\r\n");
    }

    uint8_t recv_data1[2];
    send_buf[0] = 0x08;
    i2c_master_write_to_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 1, 1000);
    i2c_master_write_read_device(I2C_PRESSURE_SENSOR_NUM, I2C_Battery_ADDR, send_buf, 1, &recv_data1, 2, 1000);
    printf("bt:%x   %x\n", recv_data1[0], recv_data1[1]);

    xTaskCreatePinnedToCore(sensor_read_task, "sensor_read_task", 2048, NULL, 3, NULL, tskNO_AFFINITY);
}
