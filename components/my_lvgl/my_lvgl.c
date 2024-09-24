#include <stdio.h>
#include "my_lvgl.h"
#include "ui.h"
#include "ui_helpers.h"
#include "device_nvs.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "time.h"
#include "ds3231.h"
#include "driver/gpio.h"
#include "pressure_sensor.h"
#include "storage.h"

#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "driver/adc.h"

#define ADC1_TEST_CHANNEL ADC1_CHANNEL_0 // adc测试通道

// 对应需要读取设置ui控件的ID        无密码登录用户

#define user_neihou_Get_SPTime ui_SN5PNL9PNL1TXT1       // 内喉稳压时间
#define user_neihou_Get_TestTime ui_SN5PNL9PNL2TXT1     // 内喉试漏测试时间
#define user_neihou_Get_RPTime ui_SN5PNL9PNL3TXT1       // 内喉降压时间
#define user_neihou_Get_IntervalTime ui_SN5PNL9PNL4TXT1 // 内喉间隔时间
#define user_neihou_Get_Time ui_SN5PNL9PNL5TXT1         // 内喉试过刮
#define user_neihou_Get_Button ui_SN5PNL9BTN2LAB1       // 内喉确认按钮lab

#define user_waihou_Get_SPTime ui_SN5PNL10PNL1TXT1       // 街吼稳压时间
#define user_waihou_Get_TestTime ui_SN5PNL10PNL2TXT1     // 街吼试漏测试时间
#define user_waihou_Get_RPTime ui_SN5PNL10PNL3TXT1       // 街吼降压时间
#define user_waihou_Get_IntervalTime ui_SN5PNL10PNL4TXT1 // 街吼间隔时间
#define user_waihou_Get_Time ui_SN5PNL10PNL5TXT1         // 街吼试过刮
#define user_waihou_Get_Button ui_SN5PNL10BTN2LAB1       // 街吼确认按钮lab

#define user_neihou_Set_SPTime ui_SN5PNL2PNL1LAB2       // 内喉稳压时间
#define user_neihou_Set_TestTime ui_SN5PNL2PNL2LAB2     // 内喉试漏测试时间
#define user_neihou_Set_RPTime ui_SN5PNL2PNL3LAB2       // 内喉降压时间
#define user_neihou_Set_IntervalTime ui_SN5PNL2PNL4LAB2 // 内喉间隔时间
#define user_neihou_Set_Time ui_SN5PNL2PNL5LAB2         // 内喉试过刮

#define user_waihou_Set_SPTime ui_SN5PNL4PNL1LAB2       // 街吼稳压时间
#define user_waihou_Set_TestTime ui_SN5PNL4PNL2LAB2     // 街吼试漏测试时间
#define user_waihou_Set_RPTime ui_SN5PNL4PNL3LAB2       // 街吼降压时间
#define user_waihou_Set_IntervalTime ui_SN5PNL4PNL4LAB2 // 街吼间隔时间
#define user_waihou_Set_Time ui_SN5PNL4PNL5LAB2         // 街吼试过刮

#define user_Custom_Get_SPTime ui_SN5PNL7PNL1TXT1       // 稳压时间
#define user_Custom_Get_TestTime ui_SN5PNL7PNL2TXT1     // 试漏测试时间
#define user_Custom_Get_RPTime ui_SN5PNL7PNL3TXT1       // 降压时间
#define user_Custom_Get_IntervalTime ui_SN5PNL7PNL4TXT1 // 间隔时间
#define user_Custom_Get_Time ui_SN5PNL7PNL5TXT1         // 试过刮
#define user_Custom_Get_Button ui_SN5PNL7BTN2LAB1       // 自定确认按钮lab

#define user_Custom_Set_SPTime ui_SN5PNL6PNL1LAB2       // 稳压时间
#define user_Custom_Set_TestTime ui_SN5PNL6PNL2LAB2     // 试漏测试时间
#define user_Custom_Set_RPTime ui_SN5PNL6PNL3LAB2       // 降压时间
#define user_Custom_Set_IntervalTime ui_SN5PNL6PNL4LAB2 // 间隔时间
#define user_Custom_Set_Time ui_SN5PNL6PNL5LAB2         // 试过刮

// 对应需要读取设置ui控件的ID        密码登录管理员
#define admin_neihou_Get_SPTime ui_SN4PNL14PNL1TXT1       // 内喉稳压时间
#define admin_neihou_Get_TestTime ui_SN4PNL14PNL2TXT1     // 内喉试漏测试时间
#define admin_neihou_Get_RPTime ui_SN4PNL14PNL3TXT1       // 内喉降压时间
#define admin_neihou_Get_IntervalTime ui_SN4PNL14PNL4TXT1 // 内喉间隔时间
#define admin_neihou_Get_Time ui_SN4PNL14PNL5TXT1         // 内喉试挂过时间
#define admin_neihou_Get_Button ui_SN4PNL14BTN1LAB1       // 内喉确认按钮lab

#define admin_waihou_Get_SPTime ui_SN4PNL13PNL1TXT1       // 街吼稳压时间
#define admin_waihou_Get_TestTime ui_SN4PNL13PNL2TXT1     // 街吼试漏测试时间
#define admin_waihou_Get_RPTime ui_SN4PNL13PNL3TXT1       // 街吼降压时间
#define admin_waihou_Get_IntervalTime ui_SN4PNL13PNL4TXT1 // 街吼间隔时间
#define admin_waihou_Get_Time ui_SN4PNL13PNL5TXT1         // 街吼试挂过时间
#define admin_waihou_Get_Button ui_SN4PNL13BTN1LAB1       // 街吼确认按钮lab

#define admin_neihou_Set_SPTime ui_SN4PNL2PNL1LAB2       // 内喉稳压时间
#define admin_neihou_Set_TestTime ui_SN4PNL2PNL2LAB2     // 内喉试漏测试时间
#define admin_neihou_Set_RPTime ui_SN4PNL2PNL3LAB2       // 内喉降压时间
#define admin_neihou_Set_IntervalTime ui_SN4PNL2PNL4LAB2 // 内喉间隔时间
#define admin_neihou_Set_Time ui_SN4PNL2PNL5LAB2         // 内喉试挂过时间

#define admin_waihou_Set_SPTime ui_SN4PNL4PNL1LAB2       // 街吼稳压时间
#define admin_waihou_Set_TestTime ui_SN4PNL4PNL2LAB2     // 街吼试漏测试时间
#define admin_waihou_Set_RPTime ui_SN4PNL4PNL3LAB2       // 街吼降压时间
#define admin_waihou_Set_IntervalTime ui_SN4PNL4PNL4LAB2 // 街吼间隔时间
#define admin_waihou_Set_Time ui_SN4PNL4PNL5LAB2         // 街吼试挂过时间

#define admin_Custom_Get_SPTime ui_SN4PNL7PNL1TXT1       // 稳压时间
#define admin_Custom_Get_TestTime ui_SN4PNL7PNL2TXT1     // 试漏测试时间
#define admin_Custom_Get_RPTime ui_SN4PNL7PNL3TXT1       // 降压时间
#define admin_Custom_Get_IntervalTime ui_SN4PNL7PNL4TXT1 // 间隔时间
#define admin_Custom_Get_Time ui_SN4PNL7PNL5TXT1         // 试挂过时间
#define admin_Custom_Get_Button ui_SN4PNL7BTN1LAB1       // 自定确认按钮lab

#define admin_Custom_Set_SPTime ui_SN4PNL6PNL1LAB2       // 稳压时间
#define admin_Custom_Set_TestTime ui_SN4PNL6PNL2LAB2     // 试漏测试时间
#define admin_Custom_Set_RPTime ui_SN4PNL6PNL3LAB2       // 降压时间
#define admin_Custom_Set_IntervalTime ui_SN4PNL6PNL4LAB2 // 间隔时间
#define admin_Custom_Set_Time ui_SN4PNL6PNL5LAB2         // 试挂过时间

#define admin_Change_Pass_1 ui_SN4PNL8TXT1       // 新密码
#define admin_Change_Pass_2 ui_SN4PNL8TXT2       // 确认新密码
#define admin_Change_Pass_error ui_SN4PNL8LAB1   // 更新密码错误显示
#define admin_Change_Pass_Lab ui_SN4PNL8BTN2LAB1 // 保存完成按键lab

#define admin_set_Me ui_SN4PNL9DPDN1           // 设置测量单位
#define admin_set_Me_Button ui_SN4PNL9BTN2LAB1 // 设置测量单位确认按键

// 对应需要读取设置ui控件的ID        管理员密码
#define admin_PassWord ui_SN3PNL5TXT1       // 管理员密码
#define Firmware_Name ui_SN3PNL4LAB1        // 升级软件名称
#define Old_Versions ui_SN3PNL4LAB2         // 当前版本
#define New_Versions ui_SN3PNL4LAB4         // 新版本
#define Progress_Bar ui_SN3PNL4BAR1         // 进度条
#define Progress_Bar_Data ui_SN3PNL4LAB6    // 进度条百分比显示
#define admin_PassWord_Error ui_SN3PNL5LAB2 // 登录密码错误显示

// 对应需要读取设置ui控件的ID        选择测试内容或是否进入测试界面
#define Employee_ID ui_SN2PNL1TXT2   // 员工编号
#define Customer_Data ui_SN2PNL1TXT1 // 客户资料
#define Test_Mode ui_SN2PNL1DPDN1    // 测试模式

// 测试界面需要读取设置ui控件的ID
#define Test_TIME1_HOUR ui_SN6BAR2LAB1   // 第一个圆球测试时间（小时）
#define Test_TIME1_Symbol ui_SN6BAR2LAB2 // 第一个圆球测试时间（冒号）
#define Test_TIME1_MIN ui_SN6BAR2LAB3    // 第一个圆球测试时间（分钟）

#define Test_TIME2_HOUR ui_SN6BAR3LAB1   // 第二个圆球测试时间（小时）
#define Test_TIME2_Symbol ui_SN6BAR3LAB2 // 第二个圆球测试时间（冒号）
#define Test_TIME2_MIN ui_SN6BAR3LAB3    // 第二个圆球测试时间（分钟）

#define Test_TIME3_HOUR ui_SN6BAR4LAB1   // 第二个圆球测试时间（小时）
#define Test_TIME3_Symbol ui_SN6BAR4LAB2 // 第二个圆球测试时间（冒号）
#define Test_TIME3_MIN ui_SN6BAR4LAB3    // 第二个圆球测试时间（分钟）

#define Test_Low_Error_Lab ui_SN6PNL7LAB2                 // 测试界面 气压下降（刮过）lab
#define Test_Low_Barometric_drop_Lab ui_SN6PNL7LAB1       // 测试界面 气压下降显示值
#define Test_Low_Barometric_drop_unit1 ui_SN6PNL7PNL1LAB1 // 测试界面 气压单位值第一部分
#define Test_Low_Barometric_drop_unit2 ui_SN6PNL7PNL1LAB2 // 测试界面 气压单位值第二部分
#define Test_Low_Barometric_drop_unit3 ui_SN6PNL7PNL1LAB2 // 测试界面 气压单位值第三部分

#define Test_RealTime_Barometric_unit1 ui_SN6PNL3PNL1LAB1 // 测试界面实时显示当前气压单位值第一部分
#define Test_RealTime_Barometric_unit2 ui_SN6PNL3PNL1LAB2 // 测试界面实时显示当前气压单位值第二部分
#define Test_RealTime_Barometric_unit3 ui_SN6PNL3PNL1LAB3 // 测试界面实时显示当前气压单位值第三部分
#define Test_RealTime_Barometric_Lab ui_SN6PNL3LAB1       // 测试界面实时显示当前气压值

#define Test_LowLeft_test_ID ui_SN6PNL2LAB3   // 测试界面左下角 显示客户的号码
#define Test_LowLeft_test_Mode ui_SN6PNL2LAB4 // 测试界面左下角 显示测试模式

#define Test_Low_test_ID ui_SN6PNL5LAB3   // 测试界面中下方 显示客户的号码
#define Test_Low_test_Mode ui_SN6PNL5LAB4 // 测试界面中下方 显示测试模式

#define Test_Status_Lab1 ui_SN6PNL4LAB1 // 测试界面当前状态  供氣閥測試中...
#define Test_Status_Lab2 ui_SN6PNL4LAB2 // 测试界面当前状态  降壓中...
#define Test_Status_Lab3 ui_SN6PNL4LAB3 // 测试界面当前状态  測試合格
#define Test_Status_Lab4 ui_SN6PNL4LAB4 // 测试界面当前状态  壓力測試中...
#define Test_Status_Lab5 ui_SN6PNL4LAB5 // 测试界面当前状态  試漏失敗

#define Test_LowLeft_ID_Mode_Sreen ui_SN6PNL2 // 测试界面左下角显示客户号码+测试模式
#define Test_LowLeft_return_Sreen ui_SN6PNL8  // 测试界面左下角显示返回主界面

#define Test_take_a_picture_Sreen ui_SN6PNL6 // 测试界面中下方 请拍照记录界面

#define Test_Barometric_drop_Sreen ui_SN6PNL7 // 测试界面中下方 显示气压下降数值

#define Test_Low_Start_Sreen ui_SN6PNL9 // 测试界面中下方 显示开始按键+归零按键 以及Lab（請接駁喉管及關掣）
#define Test_Low_End_Sreen ui_SN6PNL10  // 测试界面中下方 显示重新测试按键 + 完成按键
#define Test_Low_test_Sreen ui_SN6PNL5  // 测试界面中下方 显示客户号码，测试模式（加大加粗）

#define Test_Status_Sreen ui_SN6PNL4 // 测试界面显示当前状态

#define Set_Time_Button ui_SN4IM1BTN1        // 进入设置时间长按按键
#define Return_Time_Button ui_SN4PNL11BTN1   // 退出设置时间按键
#define Setting_Time_Button ui_SN4PNL11BTN2  // 保存设置时间的按键
#define Setting_Time_Lab ui_SN4PNL11BTN2LAB1 // 显示保存或者完成的lab

#define Setting_Time_year ui_SN4PNL11TXT1  // 设置时间 年
#define Setting_Time_month ui_SN4PNL11TXT2 // 设置时间 月
#define Setting_Time_day ui_SN4PNL11TXT3   // 设置时间 日
#define Setting_Time_hour ui_SN4PNL11TXT4  // 设置时间 时
#define Setting_Time_min ui_SN4PNL11TXT6   // 设置时间 分
#define Setting_Time_sec ui_SN4PNL11TXT5   // 设置时间 秒

#define SC2_Bat_icon ui_SN2Bar1 // 界面2电池图标
#define SC2_Bat_data ui_SN2LAB1 // 界面2电量百分比
#define SC3_Bat_icon ui_SN3BAR1 // 界面3电池图标
#define SC3_Bat_data ui_SN3LAB1 // 界面3电量百分比
#define SC4_Bat_icon ui_SN4BAR1 // 界面4电池图标
#define SC4_Bat_data ui_SN4LAB1 // 界面4电量百分比
#define SC5_Bat_icon ui_SN5BAR1 // 界面5电池图标
#define SC5_Bat_data ui_SN5LAB1 // 界面5电量百分比
#define SC6_Bat_icon ui_SN6BAR1 // 界面6电池图标
#define SC6_Bat_data ui_SN6LAB1 // 界面6电量百分比

#define GPIO_OUTPUT_IO_0 GPIO_NUM_40
#define GPIO_OUTPUT_IO_1 GPIO_NUM_2
#define GPIO_OUTPUT_IO_2 GPIO_NUM_44

struct Test_Data Neihou = {60, 120, 15, 5, 60};   // 内喉参数
struct Test_Data Waihou = {120, 180, 15, 5, 120}; // 街喉参数
struct Test_Data Custom = {60, 180, 15, 5, 120};  // 自订参数

struct Test_Data *test;    // 测试指向的参数
uint32_t test_delay_time1; // 左侧圆球测试时间
uint32_t test_delay_time2; // 中间连接测试时间
uint32_t test_delay_time3; // 右侧圆球测试时间

uint8_t test_status = 0; // 测试的状态

uint16_t delay_1s = 0;    // 延迟1s
uint16_t delay_500ms = 0; // 延迟500ms
uint16_t delay_100ms = 0;
uint32_t delay_time; // 定时器计数值

uint16_t time_show_status = 0; // 时间显示标志位

uint16_t unit = 0; // 测量单位选择标志位

char admin_password_buff[20]; // 管理员登录密码

char device_id[5];      // 机身ID
char *Customer_buff;    // 客户资料
char *Employee_id_buff; // 员工编号

time_t tick;   // 读取的时间
struct tm *tm; // 时间

int16_t get_pressure;                   // 读取到的压力值
int16_t get_pressure_calibration;       // 压力值的标定
int16_t start_get_pressure_calibration; // 压力值的标定
double pressure_data;                   // 读取的压力值 - 压力值的标定
double pressure;                        // 换算的压力值

int16_t pressure_up;        // 压力值上升的判断标准值
int16_t pressure_down;      // 压力值下降的判断标准值
int16_t pressure_deviation; // 气压下降值或者气压上升值
int16_t pressure_test;

uint16_t count;      // 总计数
uint16_t test_count; // 单次计数

char get_Employee_id[10]; // 员工ID String->uint32
uint8_t get_test_mode;    // 员工测试模式

uint16_t valve_open_time = 0;

char get_nvs_user_id[20];   // 内部存储的用户ID
uint16_t get_nvs_test_mode; // 内部存储的测试模式

const char *Mode_TXT[3] = {"內喉", "街喉", "自訂"};

uint8_t power_set_time = 0; // 关机按键时间计数
uint8_t power_status = 0;   // 关机按键状态

uint8_t power_start_flag = 0;
uint8_t power_select = 0; // 关机界面选择
uint8_t power_flag = 0;   // 关机状态

uint8_t create_sd = 0; // 关机界面选择

uint8_t showwring = 0; // 关机界面选择

uint8_t charge_status = 0;      // 充电实时状态
uint32_t bat = 0;               // 总剩余电量
uint8_t lvgl_charge_status = 0; // lvgl显示充电状态标志位
uint8_t lvgl_bat = 100;         // lvgl显示电量
uint8_t lvgl_show_bat = 101;
uint16_t bat_high;       // 读取电量高16位
uint16_t bat_low;        // 读取电量低16位
uint16_t bat_status = 0; // 读取电量标志位(防止异常关机等状态)
uint8_t bat_delay_1s = 0;
uint8_t bat_down_status = 0;

uint8_t xp_flag = 0; // 熄屏标志位

uint16_t lvgl_show_time = 0; // lvgl显示计时（熄屏，关机）
uint8_t lvgl_show_1s = 0;
#define DORMANCY_TIME 300   // 5*60s
#define POWER_OFF_TIME 1200 // 20*60s

uint16_t bat_nvs_time = 0;

uint8_t retest_Test_flag = 0;

#define BATTERY_CAPACITY 27360000

// struct VoltagePercentage batteryLookupTable[]={
// {4.20,100},
// {4.10,95},
// {4.60,90},
// {3.95,85},
// {3.90,80},
// {3.80,75},
// {3.70,60},
// {3.60,40},
// {3.50,20},
// {3.40,10},
// {3.30,5},
// {3.20, 0}

uint16_t bat_adc_valve[12] = {
    4200, 4100, 4000, 3950, 3900, 3800, 3700, 3600, 3500, 3400, 3300, 3200};
uint8_t bat_show_valve[12] = {
    100, 95, 90, 85, 75, 65, 45, 25, 20, 10, 5, 0};

uint8_t get_nvs_valve(char *key, uint16_t *data, uint16_t default_valve)
{
    uint32_t get_data;
    esp_err_t err = storage_read_key_value(key, &get_data);
    if (err == ESP_OK)
    {
        *data = (uint16_t)get_data;
        return 0;
    }
    else if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        storage_save_key_value(key, default_valve);
        *data = default_valve;
        return 1;
    }
    else
    {
        *data = default_valve;
    }
    return 2;
}
uint8_t get_nvs_buff(char *key, char *buff, char *default_buff, uint16_t default_len)
{
    uint32_t buff_len;
    uint8_t buffzzz[20];
    esp_err_t err = storage_read_key_blob(key, (uint8_t *)buff, &buff_len);
    if (err == ESP_OK)
    {
        return 0;
    }
    else if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        storage_save_key_blob(key, (uint8_t *)default_buff, default_len);
        strcpy(buff, default_buff);
        return 1;
    }
    else
    {
        strcpy(buff, default_buff);
    }
    return 2;
}
// uint16_t u8_key,i8_key,u16_key;
uint16_t test_id;
void my_lvgl_data_init() // 参数初始化
{
    get_nvs_valve("Neihou.WYTime", &Neihou.WYTime, 60);
    get_nvs_valve("Neihou.SLTime", &Neihou.SLTime, 120);
    get_nvs_valve("Neihou.JYTime", &Neihou.JYTime, 15);
    get_nvs_valve("Neihou.JGTime", &Neihou.JGTime, 5);
    get_nvs_valve("Neihou.Time", &Neihou.Time, 60);

    get_nvs_valve("Waihou.WYTime", &Waihou.WYTime, 120);
    get_nvs_valve("Waihou.SLTime", &Waihou.SLTime, 180);
    get_nvs_valve("Waihou.JYTime", &Waihou.JYTime, 15);
    get_nvs_valve("Waihou.JGTime", &Waihou.JGTime, 5);
    get_nvs_valve("Waihou.Time", &Waihou.Time, 120);

    get_nvs_valve("Custom.WYTime", &Custom.WYTime, 60);
    get_nvs_valve("Custom.SLTime", &Custom.SLTime, 180);
    get_nvs_valve("Custom.JYTime", &Custom.JYTime, 15);
    get_nvs_valve("Custom.JGTime", &Custom.JGTime, 5);
    get_nvs_valve("Custom.Time", &Custom.Time, 120);

    get_nvs_buff("admin_password", admin_password_buff, "123456", 6);
    get_nvs_valve("unit", &unit, 0);

    get_nvs_valve("bat_status", &bat_status, 0);
    get_nvs_valve("bat_high", &bat_high, 0);
    get_nvs_valve("bat_low", &bat_low, 0);

    storage_save_key_value("bat_status", 0);

    get_nvs_buff("user_id", get_nvs_user_id, "123456", 6);
    get_nvs_valve("test_mode", &get_nvs_test_mode, 0);
    test_status = 0;
}
void my_main_interface_init()
{ // 主界面数据初始化（用户编号，测试模式）
    // Employee_ID
    // Test_Mode
    lv_textarea_set_text(Employee_ID, get_nvs_user_id);
    // lv_dropdown_get_selected
    lv_dropdown_set_selected(Test_Mode, get_nvs_test_mode);
    // lv_textarea_get_text
}
void my_test_next_nvs_save() // 主界面点击下一步后，员工ID和测试模式保存记录
{
    if (strcmp(Employee_id_buff, get_nvs_user_id))
    {
        // 当前员工编号和记录的员工编号不同时，保存新的员工编号
        storage_save_key_blob("user_id", (uint8_t *)Employee_id_buff, strlen(Employee_id_buff));
        strncpy(get_nvs_user_id, Employee_id_buff, 6);
        get_nvs_user_id[sizeof(get_nvs_user_id) - 1] = '\0'; // 确保以空字符结尾，避免溢出
        printf("user_id\r\n");
    }
    if (get_test_mode != get_nvs_test_mode)
    {
        storage_save_key_value("test_mode", get_test_mode);
        get_nvs_test_mode = get_test_mode;
        printf("test_mode\r\n");
    }
}

void my_lvgl_set_bat_color() // 设置充电状态颜色
{
    switch (charge_status)
    {
    case 0:
        lv_obj_set_style_bg_color(SC2_Bat_icon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC3_Bat_icon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC4_Bat_icon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC5_Bat_icon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC6_Bat_icon, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        _ui_flag_modify(ui_SN2IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN3IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN4IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN5IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN6IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        break;
    case 1:
    case 2:
    case 3:
        lv_obj_set_style_bg_color(SC2_Bat_icon, lv_color_hex(0x17FC1F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC3_Bat_icon, lv_color_hex(0x17FC1F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC4_Bat_icon, lv_color_hex(0x17FC1F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC5_Bat_icon, lv_color_hex(0x17FC1F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC6_Bat_icon, lv_color_hex(0x17FC1F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        _ui_flag_modify(ui_SN2IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_SN3IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_SN4IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_SN5IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_SN6IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        break;
    case 4:
        lv_obj_set_style_bg_color(SC2_Bat_icon, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC3_Bat_icon, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC4_Bat_icon, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC5_Bat_icon, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(SC6_Bat_icon, lv_color_hex(0xFF0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        _ui_flag_modify(ui_SN2IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN3IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN4IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN5IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_SN6IM6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        break;
    }
}

void my_lvgl_time_show() // 时间显示
{
    static uint8_t get_min = 100;
    int get_year;
    // printf("Time = %d %d %d   %d %d %d\r\n", tm->tm_year, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    char time[20], time1[5], time2[5], time3[5], time4[5];
    uint8_t show_flag = 0;
    if (time_show_status == 0)
    {
        time_show_status = 1;
        power_select = 1;
        get_min = tm->tm_min;
        show_flag = 1;
    }
    if (get_min != tm->tm_min)
    {
        get_min = tm->tm_min;
        show_flag = 1;
    }
    if (show_flag)
    {
        get_year = tm->tm_year;
        get_year += 1900;
        sprintf(time, "%04d", get_year);
        sprintf(time1, "%02d", (uint8_t)tm->tm_mon + 1);
        sprintf(time2, "%02d", (uint8_t)tm->tm_mday);
        sprintf(time3, "%02d", (uint8_t)tm->tm_hour);
        sprintf(time4, "%02d", (uint8_t)tm->tm_min);
        lv_label_set_text(ui_SN2LAB3, time3);
        lv_label_set_text(ui_SN2LAB4, time4);
        lv_label_set_text(ui_SN2LAB6, time2);
        lv_label_set_text(ui_SN2LAB8, time1);
        lv_label_set_text(ui_SN2LAB10, time);

        lv_label_set_text(ui_SN3LAB3, time3);
        lv_label_set_text(ui_SN3LAB4, time4);
        lv_label_set_text(ui_SN3LAB6, time2);
        lv_label_set_text(ui_SN3LAB8, time1);
        lv_label_set_text(ui_SN3LAB10, time);

        lv_label_set_text(ui_SN4LAB3, time3);
        lv_label_set_text(ui_SN4LAB4, time4);
        lv_label_set_text(ui_SN4LAB6, time2);
        lv_label_set_text(ui_SN4LAB8, time1);
        lv_label_set_text(ui_SN4LAB10, time);

        lv_label_set_text(ui_SN5LAB3, time3);
        lv_label_set_text(ui_SN5LAB4, time4);
        lv_label_set_text(ui_SN5LAB6, time2);
        lv_label_set_text(ui_SN5LAB8, time1);
        lv_label_set_text(ui_SN5LAB10, time);

        lv_label_set_text(ui_SN6LAB3, time3);
        lv_label_set_text(ui_SN6LAB4, time4);
        lv_label_set_text(ui_SN6LAB6, time2);
        lv_label_set_text(ui_SN6LAB8, time1);
        lv_label_set_text(ui_SN6LAB10, time);
    }
}
void my_lvgl_bat_show() // 电量显示
{
    char txt[12];
    uint8_t data;
    // printf("%d  %d  %d  %d \r\n",lvgl_show_bat,lvgl_bat,lvgl_charge_status,charge_status);
    if (lvgl_show_bat != lvgl_bat)
    {
        lvgl_show_bat = lvgl_bat;
        sprintf(txt, "%d", lvgl_show_bat);
        lv_label_set_text(SC2_Bat_data, txt);
        lv_label_set_text(SC3_Bat_data, txt);
        lv_label_set_text(SC4_Bat_data, txt);
        lv_label_set_text(SC5_Bat_data, txt);
        lv_label_set_text(SC6_Bat_data, txt);

        if (lvgl_show_bat < 10)
            data = 10;
        else
            data = lvgl_show_bat;

        lv_bar_set_value(SC2_Bat_icon, data, LV_ANIM_ON); // 增加进度条
        lv_bar_set_value(SC3_Bat_icon, data, LV_ANIM_ON); // 增加进度条
        lv_bar_set_value(SC4_Bat_icon, data, LV_ANIM_ON); // 增加进度条
        lv_bar_set_value(SC5_Bat_icon, data, LV_ANIM_ON); // 增加进度条
        lv_bar_set_value(SC6_Bat_icon, data, LV_ANIM_ON); // 增加进度条
    }
    if (lvgl_charge_status != charge_status)
    {
        if (lvgl_charge_status == 0 || lvgl_charge_status == 4)
        {
            my_lvgl_set_bat_color();
        }
        else if (charge_status == 0 || charge_status == 4)
        {
            my_lvgl_set_bat_color();
        }

        lvgl_charge_status = charge_status;
    }
}

void my_lvgl_pressure_show() // lvgl 气压刷新显示
{
    get_pressure = pressure_sensor_value_read();
    pressure_data = get_pressure - get_pressure_calibration;
    if (unit)
    {
        pressure = pressure_data / 1000;
    }
    else
    {
        pressure = pressure_data / 249;
    }
    char P_TEXT[20];
    if (pressure >= 0.01 || pressure <= (-0.01))
    {
        sprintf(P_TEXT, "%.2f", pressure);
    }
    else
    {
        sprintf(P_TEXT, "0.0");
    }

    // printf("P_TEXT = %s   %d\r\n", P_TEXT, get_pressure);
    if (test_status)
    {
        lv_label_set_text(Test_RealTime_Barometric_Lab, P_TEXT); // 修改按键文本
    }
}

void my_lvgl_on_Parameter_setting() // 进入参数设置界面 点击事件
{
    lv_textarea_set_text(admin_PassWord, "");
    _ui_flag_modify(admin_PassWord_Error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
}

void my_lvgl_text_noll_set(lv_obj_t *obj1, uint16_t data)
{ // 当输入框位空的时候，设置为data值
    char set_test[10];
    char *get_text = lv_textarea_get_text(obj1);
    if (strlen(get_text) == 0)
    {
        sprintf(set_test, "%d", data);
        lv_textarea_set_text(obj1, set_test);
    }
    else
    {
        int value = atoi(get_text); // Convert text to integer
        if (value == 0)
        {
            sprintf(set_test, "%d", data);
            lv_textarea_set_text(obj1, set_test);
        }
    }
}
void my_lvgl_text_noll_set_wy(lv_obj_t *obj1, uint16_t data)
{ // 当输入框位空的时候，设置为data值
    char set_test[10];
    char *get_text = lv_textarea_get_text(obj1);
    if (strlen(get_text) == 0)
    {
        sprintf(set_test, "%d", data);
        lv_textarea_set_text(obj1, set_test);
    }
    else
    {
        int value = atoi(get_text); // Convert text to integer
        if (value < 1 || value > 120)
        {
            sprintf(set_test, "%d", data);
            lv_textarea_set_text(obj1, set_test);
            showwring = 1;
        }
    }
}
void my_lvgl_add(lv_obj_t *obj1, lv_obj_t *obj2)
{
    char *text = lv_textarea_get_text(obj1);
    int value = atoi(text); // Convert text to integer
    if (value < 999)
        value++;
    char new_text[32];                    // Buffer to hold the new text
    sprintf(new_text, "%d", value);       // Convert integer to text
    lv_textarea_set_text(obj1, new_text); // Set the new text to the label
    lv_label_set_text(obj2, "確認");
}
void my_lvgl_subtract(lv_obj_t *obj1, lv_obj_t *obj2)
{
    char *text = lv_textarea_get_text(obj1);
    int value = atoi(text); // Convert text to integer
    if (value > 1)
        value--;                          // Increment the integer value
    char new_text[32];                    // Buffer to hold the new text
    sprintf(new_text, "%d", value);       // Convert integer to text
    lv_textarea_set_text(obj1, new_text); // Set the new text to the label
    lv_label_set_text(obj2, "確認");
}

void my_lvgl_add_wy(lv_obj_t *obj1, lv_obj_t *obj2)
{
    char *text = lv_textarea_get_text(obj1);
    int value = atoi(text); // Convert text to integer
    if (value < 120)
        value++;
    else
    {
        showwring = 1;
    }
    char new_text[32];                    // Buffer to hold the new text
    sprintf(new_text, "%d", value);       // Convert integer to text
    lv_textarea_set_text(obj1, new_text); // Set the new text to the label
    lv_label_set_text(obj2, "確認");
}
void my_lvgl_subtract_wy(lv_obj_t *obj1, lv_obj_t *obj2)
{
    char *text = lv_textarea_get_text(obj1);
    int value = atoi(text); // Convert text to integer
    if (value > 1 && value <= 120)
        value--; // Increment the integer value
    else
    {
        showwring = 1;
    }
    char new_text[32];                    // Buffer to hold the new text
    sprintf(new_text, "%d", value);       // Convert integer to text
    lv_textarea_set_text(obj1, new_text); // Set the new text to the label
    lv_label_set_text(obj2, "確認");
}

void my_lvgl_user_on_neihou() // 用户模式进入内喉参数设置 点击事件
{
    char test[10];
    sprintf(test, "%d", Neihou.WYTime);
    lv_label_set_text(user_neihou_Set_SPTime, test);
    sprintf(test, "%d", Neihou.SLTime);
    lv_label_set_text(user_neihou_Set_TestTime, test);
    sprintf(test, "%d", Neihou.JYTime);
    lv_label_set_text(user_neihou_Set_RPTime, test);
    sprintf(test, "%d", Neihou.JGTime);
    lv_label_set_text(user_neihou_Set_IntervalTime, test);
    sprintf(test, "%d", Neihou.Time);
    lv_label_set_text(user_neihou_Set_Time, test);
}
void my_lvgl_user_on_neihou_Settings()
{ // 用户模式进入内喉参数设置(更改参数) 点击事件
    char test[10];

    sprintf(test, "%d", Neihou.WYTime);
    lv_textarea_set_text(user_neihou_Get_SPTime, test);
    sprintf(test, "%d", Neihou.SLTime);
    lv_textarea_set_text(user_neihou_Get_TestTime, test);
    sprintf(test, "%d", Neihou.JYTime);
    lv_textarea_set_text(user_neihou_Get_RPTime, test);
    sprintf(test, "%d", Neihou.JGTime);
    lv_textarea_set_text(user_neihou_Get_IntervalTime, test);
    sprintf(test, "%d", Neihou.Time);
    lv_textarea_set_text(user_neihou_Get_Time, test);
    lv_label_set_text(user_neihou_Get_Button, "確認");
}
void my_lvgl_user_on_neihou_Change_Settings()
{ // 用户模式在内喉参数设置中确认更改参数  点击事件

    char *text = lv_textarea_get_text(user_neihou_Get_SPTime);
    uint16_t value = atoi(text); // Convert text to integer
    Neihou.WYTime = value;
    text = lv_textarea_get_text(user_neihou_Get_TestTime);
    value = atoi(text); // Convert text to integer
    Neihou.SLTime = value;
    text = lv_textarea_get_text(user_neihou_Get_RPTime);
    value = atoi(text); // Convert text to integer
    Neihou.JYTime = value;
    text = lv_textarea_get_text(user_neihou_Get_IntervalTime);
    value = atoi(text); // Convert text to integer
    Neihou.JGTime = value;
    text = lv_textarea_get_text(user_neihou_Get_Time);
    value = atoi(text); // Convert text to integer
    Neihou.Time = value;
    char *text2 = lv_label_get_text(user_neihou_Get_Button);

    if (strcmp(text2, "確認") == 0)
    {
        storage_save_key_value("Neihou.WYTime", Neihou.WYTime);
        storage_save_key_value("Neihou.SLTime", Neihou.SLTime);
        storage_save_key_value("Neihou.JYTime", Neihou.JYTime);
        storage_save_key_value("Neihou.JGTime", Neihou.JGTime);
        storage_save_key_value("Neihou.Time", Neihou.Time);
    }
    lv_label_set_text(user_neihou_Get_Button, "完成");
    my_lvgl_user_on_neihou();
}

void my_lvgl_user_neihou_wy_add() // 用户模式内喉点击稳压时间 +号 点击事件
{
    my_lvgl_add_wy(user_neihou_Get_SPTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_wy_subtract() // 用户模式内喉点击稳压时间 -号 点击事件
{
    my_lvgl_subtract_wy(user_neihou_Get_SPTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_sl_add() // 用户模式内喉点击试漏测试时间 +号 点击事件
{
    my_lvgl_add(user_neihou_Get_TestTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_sl_subtract() // 用户模式内喉点击试漏测试时间 -号 点击事件
{
    my_lvgl_subtract(user_neihou_Get_TestTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_jy_add() // 用户模式内喉点击降压时长 +号 点击事件
{
    my_lvgl_add(user_neihou_Get_RPTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_jy_subtract() // 用户模式内喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract(user_neihou_Get_RPTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_jg_add() // 用户模式内喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(user_neihou_Get_IntervalTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_jg_subtract() // 用户模式内喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(user_neihou_Get_IntervalTime, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_cs_add() // 用户模式内喉点击试过刮 +号 点击事件
{
    my_lvgl_add(user_neihou_Get_Time, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_cs_subtract() // 用户模式内喉点试过刮 -号 点击事件
{
    my_lvgl_subtract(user_neihou_Get_Time, user_neihou_Get_Button);
}
void my_lvgl_user_neihou_set_KeyBoard() // 用户模式内喉打开键盘
{
    lv_label_set_text(user_neihou_Get_Button, "確認");
}

void my_lvgl_user_on_waihou() // 用户模式进入街喉参数设置 点击事件
{
    char test[10];
    sprintf(test, "%d", Waihou.WYTime);
    lv_label_set_text(user_waihou_Set_SPTime, test);
    sprintf(test, "%d", Waihou.SLTime);
    lv_label_set_text(user_waihou_Set_TestTime, test);
    sprintf(test, "%d", Waihou.JYTime);
    lv_label_set_text(user_waihou_Set_RPTime, test);
    sprintf(test, "%d", Waihou.JGTime);
    lv_label_set_text(user_waihou_Set_IntervalTime, test);
    sprintf(test, "%d", Waihou.Time);
    lv_label_set_text(user_waihou_Set_Time, test);
}
void my_lvgl_user_on_waihou_Settings()
{ // 用户模式进入街喉参数设置(更改参数) 点击事件
    char test[10];

    sprintf(test, "%d", Waihou.WYTime);
    lv_textarea_set_text(user_waihou_Get_SPTime, test);
    sprintf(test, "%d", Waihou.SLTime);
    lv_textarea_set_text(user_waihou_Get_TestTime, test);
    sprintf(test, "%d", Waihou.JYTime);
    lv_textarea_set_text(user_waihou_Get_RPTime, test);
    sprintf(test, "%d", Waihou.JGTime);
    lv_textarea_set_text(user_waihou_Get_IntervalTime, test);
    sprintf(test, "%d", Waihou.Time);
    lv_textarea_set_text(user_waihou_Get_Time, test);
    lv_label_set_text(user_waihou_Get_Button, "確認");
}
void my_lvgl_user_on_waihou_Change_Settings()
{ // 用户模式在街喉参数设置中确认更改参数  点击事件

    char *text = lv_textarea_get_text(user_waihou_Get_SPTime);
    uint16_t value = atoi(text); // Convert text to integer
    Waihou.WYTime = value;
    text = lv_textarea_get_text(user_waihou_Get_TestTime);
    value = atoi(text); // Convert text to integer
    Waihou.SLTime = value;
    text = lv_textarea_get_text(user_waihou_Get_RPTime);
    value = atoi(text); // Convert text to integer
    Waihou.JYTime = value;
    text = lv_textarea_get_text(user_waihou_Get_IntervalTime);
    value = atoi(text); // Convert text to integer
    Waihou.JGTime = value;
    text = lv_textarea_get_text(user_waihou_Get_Time);
    value = atoi(text); // Convert text to integer
    Waihou.Time = value;
    char *text2 = lv_label_get_text(user_waihou_Get_Button);
    if (strcmp(text2, "確認") == 0)
    {
        storage_save_key_value("Waihou.WYTime", Waihou.WYTime);
        storage_save_key_value("Waihou.SLTime", Waihou.SLTime);
        storage_save_key_value("Waihou.JYTime", Waihou.JYTime);
        storage_save_key_value("Waihou.JGTime", Waihou.JGTime);
        storage_save_key_value("Waihou.Time", Waihou.Time);
    }
    lv_label_set_text(user_waihou_Get_Button, "完成");
    my_lvgl_user_on_waihou();
}

void my_lvgl_user_waihou_wy_add() // 用户模式街喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(user_waihou_Get_SPTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_wy_subtract() // 用户模式街喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(user_waihou_Get_SPTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_sl_add() // 用户模式街喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(user_waihou_Get_TestTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_sl_subtract() // 用户模式街喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(user_waihou_Get_TestTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_jy_add() // 用户模式街喉点击降压时长 +号 点击事件
{
    my_lvgl_add(user_waihou_Get_RPTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_jy_subtract() // 用户模式街喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract(user_waihou_Get_RPTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_jg_add() // 用户模式街喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(user_waihou_Get_IntervalTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_jg_subtract() // 用户模式街喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(user_waihou_Get_IntervalTime, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_cs_add() // 用户模式街喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(user_waihou_Get_Time, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_cs_subtract() // 用户模式街喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(user_waihou_Get_Time, user_waihou_Get_Button);
}
void my_lvgl_user_waihou_set_KeyBoard() // 用户模式街喉打开键盘
{
    lv_label_set_text(user_waihou_Get_Button, "確認");
}

void my_lvgl_user_on_Custom() // 用户模式进入自订参数设置 点击事件
{
    char test[10];
    sprintf(test, "%d", Custom.WYTime);
    lv_label_set_text(user_Custom_Set_SPTime, test);
    sprintf(test, "%d", Custom.SLTime);
    lv_label_set_text(user_Custom_Set_TestTime, test);
    sprintf(test, "%d", Custom.JYTime);
    lv_label_set_text(user_Custom_Set_RPTime, test);
    sprintf(test, "%d", Custom.JGTime);
    lv_label_set_text(user_Custom_Set_IntervalTime, test);
    sprintf(test, "%d", Custom.Time);
    lv_label_set_text(user_Custom_Set_Time, test);
}
void my_lvgl_user_on_Custom_Settings()
{ // 用户模式进入自订参数设置(更改参数) 点击事件
    char test[10];
    sprintf(test, "%d", Custom.WYTime);
    lv_textarea_set_text(user_Custom_Get_SPTime, test);
    sprintf(test, "%d", Custom.SLTime);
    lv_textarea_set_text(user_Custom_Get_TestTime, test);
    sprintf(test, "%d", Custom.JYTime);
    lv_textarea_set_text(user_Custom_Get_RPTime, test);
    sprintf(test, "%d", Custom.JGTime);
    lv_textarea_set_text(user_Custom_Get_IntervalTime, test);
    sprintf(test, "%d", Custom.Time);
    lv_textarea_set_text(user_Custom_Get_Time, test);
    lv_label_set_text(user_Custom_Get_Button, "確認");
}
void my_lvgl_user_on_Custom_Change_Settings()
{ // 用户模式在自订参数设置中确认更改参数  点击事件
    char *text = lv_textarea_get_text(user_Custom_Get_SPTime);
    uint16_t value = atoi(text); // Convert text to integer
    Custom.WYTime = value;
    text = lv_textarea_get_text(user_Custom_Get_TestTime);
    value = atoi(text); // Convert text to integer
    Custom.SLTime = value;
    text = lv_textarea_get_text(user_Custom_Get_RPTime);
    value = atoi(text); // Convert text to integer
    Custom.JYTime = value;
    text = lv_textarea_get_text(user_Custom_Get_IntervalTime);
    value = atoi(text); // Convert text to integer
    Custom.JGTime = value;
    text = lv_textarea_get_text(user_Custom_Get_Time);
    value = atoi(text); // Convert text to integer
    Custom.Time = value;
    char *text2 = lv_label_get_text(user_Custom_Get_Button);
    if (strcmp(text2, "確認") == 0)
    {
        storage_save_key_value("Custom.WYTime", Custom.WYTime);
        storage_save_key_value("Custom.SLTime", Custom.SLTime);
        storage_save_key_value("Custom.JYTime", Custom.JYTime);
        storage_save_key_value("Custom.JGTime", Custom.JGTime);
        storage_save_key_value("Custom.Time", Custom.Time);
    }
    lv_label_set_text(user_Custom_Get_Button, "完成");
    my_lvgl_user_on_Custom();
}
void my_lvgl_user_Custom_wy_add() // 用户模式自订点击稳压时间 +号 点击事件
{
    my_lvgl_add_wy(user_Custom_Get_SPTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_wy_subtract() // 用户模式自订点击稳压时间 -号 点击事件
{
    my_lvgl_subtract_wy(user_Custom_Get_SPTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_sl_add() // 用户模式自订点击试漏测试时间 +号 点击事件
{
    my_lvgl_add(user_Custom_Get_TestTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_sl_subtract() // 用户模式自订点击试漏测试时间 -号 点击事件
{
    my_lvgl_subtract(user_Custom_Get_TestTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_jy_add() // 用户模式自订点击降压时长 +号 点击事件
{
    my_lvgl_add(user_Custom_Get_RPTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_jy_subtract() // 用户模式自订点击降压时长 -号 点击事件
{
    my_lvgl_subtract(user_Custom_Get_RPTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_jg_add() // 用户模式自订点击间隔时长 +号 点击事件
{
    my_lvgl_add(user_Custom_Get_IntervalTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_jg_subtract() // 用户模式自订点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(user_Custom_Get_IntervalTime, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_cs_add() // 用户模式自订点击？？ +号 点击事件
{
    my_lvgl_add(user_Custom_Get_Time, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_cs_subtract() // 用户模式自订点击？？ -号 点击事件
{
    my_lvgl_subtract(user_Custom_Get_Time, user_Custom_Get_Button);
}
void my_lvgl_user_Custom_set_KeyBoard() // 用户模式自订打开键盘
{
    lv_label_set_text(user_Custom_Get_Button, "確認");
}

void my_lvgl_user_close_KeyBoard() // 用户模式关闭设置参数键盘 事件（清空输入框 保护）
{

    my_lvgl_text_noll_set_wy(user_neihou_Get_SPTime, Neihou.WYTime);    // 判断用户模式内喉稳压时间是否为空
    my_lvgl_text_noll_set(user_neihou_Get_TestTime, Neihou.SLTime);     // 判断用户模式内喉试漏时间是否为空
    my_lvgl_text_noll_set(user_neihou_Get_RPTime, Neihou.JYTime);       // 判断用户模式内喉降压时间是否为空
    my_lvgl_text_noll_set(user_neihou_Get_IntervalTime, Neihou.JGTime); // 判断用户模式内喉间隔时间是否为空
    my_lvgl_text_noll_set(user_neihou_Get_Time, Neihou.Time);           // 判断用户模式内喉时间是否为空

    my_lvgl_text_noll_set(user_waihou_Get_SPTime, Waihou.WYTime);       // 判断用户模式自订稳压时间是否为空
    my_lvgl_text_noll_set(user_waihou_Get_TestTime, Waihou.SLTime);     // 判断用户模式自订试漏时间是否为空
    my_lvgl_text_noll_set(user_waihou_Get_RPTime, Waihou.JYTime);       // 判断用户模式自订降压时间是否为空
    my_lvgl_text_noll_set(user_waihou_Get_IntervalTime, Waihou.JGTime); // 判断用户模式自订间隔时间是否为空
    my_lvgl_text_noll_set(user_waihou_Get_Time, Waihou.JGTime);         // 判断用户模式自订时间是否为空

    my_lvgl_text_noll_set(user_Custom_Get_SPTime, Custom.WYTime);       // 判断用户模式自订稳压时间是否为空
    my_lvgl_text_noll_set(user_Custom_Get_TestTime, Custom.SLTime);     // 判断用户模式自订试漏时间是否为空
    my_lvgl_text_noll_set(user_Custom_Get_RPTime, Custom.JYTime);       // 判断用户模式自订降压时间是否为空
    my_lvgl_text_noll_set(user_Custom_Get_IntervalTime, Custom.JGTime); // 判断用户模式自订间隔时间是否为空
    my_lvgl_text_noll_set(user_Custom_Get_Time, Custom.Time);           // 判断用户模式自订时间是否为空
}

void my_lvgl_admin_on_neihou() // 管理员模式进入内喉参数设置 点击事件
{
    char test[10];
    sprintf(test, "%d", Neihou.WYTime);
    lv_label_set_text(admin_neihou_Set_SPTime, test);
    sprintf(test, "%d", Neihou.SLTime);
    lv_label_set_text(admin_neihou_Set_TestTime, test);
    sprintf(test, "%d", Neihou.JYTime);
    lv_label_set_text(admin_neihou_Set_RPTime, test);
    sprintf(test, "%d", Neihou.JGTime);
    lv_label_set_text(admin_neihou_Set_IntervalTime, test);
    sprintf(test, "%d", Neihou.Time);
    lv_label_set_text(admin_neihou_Set_Time, test);
}
void my_lvgl_admin_on_neihou_Settings()
{ // 管理员模式进入内喉参数设置(更改参数) 点击事件
    char test[10];

    sprintf(test, "%d", Neihou.WYTime);
    lv_textarea_set_text(admin_neihou_Get_SPTime, test);
    sprintf(test, "%d", Neihou.SLTime);
    lv_textarea_set_text(admin_neihou_Get_TestTime, test);
    sprintf(test, "%d", Neihou.JYTime);
    lv_textarea_set_text(admin_neihou_Get_RPTime, test);
    sprintf(test, "%d", Neihou.JGTime);
    lv_textarea_set_text(admin_neihou_Get_IntervalTime, test);
    sprintf(test, "%d", Neihou.Time);
    lv_textarea_set_text(admin_neihou_Get_Time, test);
    lv_label_set_text(admin_neihou_Get_Button, "確認");
}
void my_lvgl_admin_on_neihou_Change_Settings()
{ // 管理员模式在内喉参数设置中确认更改参数  点击事件

    char *text = lv_textarea_get_text(admin_neihou_Get_SPTime);
    uint16_t value = atoi(text); // Convert text to integer
    Neihou.WYTime = value;
    text = lv_textarea_get_text(admin_neihou_Get_TestTime);
    value = atoi(text); // Convert text to integer
    Neihou.SLTime = value;
    text = lv_textarea_get_text(admin_neihou_Get_RPTime);
    value = atoi(text); // Convert text to integer
    Neihou.JYTime = value;
    text = lv_textarea_get_text(admin_neihou_Get_IntervalTime);
    value = atoi(text); // Convert text to integer
    Neihou.JGTime = value;
    text = lv_textarea_get_text(admin_neihou_Get_Time);
    value = atoi(text); // Convert text to integer
    Neihou.Time = value;
    char *text2 = lv_label_get_text(admin_neihou_Get_Button);

    if (strcmp(text2, "確認") == 0)
    {
        storage_save_key_value("Neihou.WYTime", Neihou.WYTime);
        storage_save_key_value("Neihou.SLTime", Neihou.SLTime);
        storage_save_key_value("Neihou.JYTime", Neihou.JYTime);
        storage_save_key_value("Neihou.JGTime", Neihou.JGTime);
        storage_save_key_value("Neihou.Time", Neihou.Time);
    }
    lv_label_set_text(admin_neihou_Get_Button, "完成");
    my_lvgl_admin_on_neihou();
}

void my_lvgl_admin_neihou_wy_add() // 管理员模式内喉点击降压时长 +号 点击事件
{
    my_lvgl_add_wy(admin_neihou_Get_SPTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_wy_subtract() // 管理员模式内喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract_wy(admin_neihou_Get_SPTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_sl_add() // 管理员模式内喉点击降压时长 +号 点击事件
{
    my_lvgl_add(admin_neihou_Get_TestTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_sl_subtract() // 管理员模式内喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract(admin_neihou_Get_TestTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_jy_add() // 管理员模式内喉点击降压时长 +号 点击事件
{
    my_lvgl_add(admin_neihou_Get_RPTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_jy_subtract() // 管理员模式内喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract(admin_neihou_Get_RPTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_jg_add() // 管理员模式内喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(admin_neihou_Get_IntervalTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_jg_subtract() // 管理员模式内喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(admin_neihou_Get_IntervalTime, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_cs_add() // 管理员模式内喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(admin_neihou_Get_Time, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_cs_subtract() // 管理员模式内喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(admin_neihou_Get_Time, admin_neihou_Get_Button);
}
void my_lvgl_admin_neihou_set_KeyBoard() // 管理员模式内喉打开键盘
{
    lv_label_set_text(admin_neihou_Get_Button, "確認");
}

void my_lvgl_admin_on_waihou() // 管理员模式进入街喉参数设置 点击事件
{
    char test[10];
    sprintf(test, "%d", Waihou.WYTime);
    lv_label_set_text(admin_waihou_Set_SPTime, test);
    sprintf(test, "%d", Waihou.SLTime);
    lv_label_set_text(admin_waihou_Set_TestTime, test);
    sprintf(test, "%d", Waihou.JYTime);
    lv_label_set_text(admin_waihou_Set_RPTime, test);
    sprintf(test, "%d", Waihou.JGTime);
    lv_label_set_text(admin_waihou_Set_IntervalTime, test);
    sprintf(test, "%d", Waihou.Time);
    lv_label_set_text(admin_waihou_Set_Time, test);
}
void my_lvgl_admin_on_waihou_Settings()
{ // 管理员模式进入街喉参数设置(更改参数) 点击事件
    char test[10];

    sprintf(test, "%d", Waihou.WYTime);
    lv_textarea_set_text(admin_waihou_Get_SPTime, test);
    sprintf(test, "%d", Waihou.SLTime);
    lv_textarea_set_text(admin_waihou_Get_TestTime, test);
    sprintf(test, "%d", Waihou.JYTime);
    lv_textarea_set_text(admin_waihou_Get_RPTime, test);
    sprintf(test, "%d", Waihou.JGTime);
    lv_textarea_set_text(admin_waihou_Get_IntervalTime, test);
    sprintf(test, "%d", Waihou.Time);
    lv_textarea_set_text(admin_waihou_Get_Time, test);
    lv_label_set_text(admin_waihou_Get_Button, "確認");
}
void my_lvgl_admin_on_waihou_Change_Settings()
{ // 管理员模式在街喉参数设置中确认更改参数  点击事件

    char *text = lv_textarea_get_text(admin_waihou_Get_SPTime);
    uint16_t value = atoi(text); // Convert text to integer
    Waihou.WYTime = value;
    text = lv_textarea_get_text(admin_waihou_Get_TestTime);
    value = atoi(text); // Convert text to integer
    Waihou.SLTime = value;
    text = lv_textarea_get_text(admin_waihou_Get_RPTime);
    value = atoi(text); // Convert text to integer
    Waihou.JYTime = value;
    text = lv_textarea_get_text(admin_waihou_Get_IntervalTime);
    value = atoi(text); // Convert text to integer
    Waihou.JGTime = value;
    text = lv_textarea_get_text(admin_waihou_Get_Time);
    value = atoi(text); // Convert text to integer
    Waihou.Time = value;
    char *text2 = lv_label_get_text(admin_waihou_Get_Button);

    if (strcmp(text2, "確認") == 0)
    {
        storage_save_key_value("Waihou.WYTime", Waihou.WYTime);
        storage_save_key_value("Waihou.SLTime", Waihou.SLTime);
        storage_save_key_value("Waihou.JYTime", Waihou.JYTime);
        storage_save_key_value("Waihou.JGTime", Waihou.JGTime);
        storage_save_key_value("Waihou.Time", Waihou.Time);
    }
    lv_label_set_text(admin_waihou_Get_Button, "完成");
    my_lvgl_admin_on_waihou();
}

void my_lvgl_admin_waihou_wy_add() // 管理员模式街喉点击降压时长 +号 点击事件
{
    my_lvgl_add(admin_waihou_Get_SPTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_wy_subtract() // 管理员模式街喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract(admin_waihou_Get_SPTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_sl_add() // 管理员模式街喉点击降压时长 +号 点击事件
{
    my_lvgl_add(admin_waihou_Get_TestTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_sl_subtract() // 管理员模式街喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract(admin_waihou_Get_TestTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_jy_add() // 管理员模式街喉点击降压时长 +号 点击事件
{
    my_lvgl_add(admin_waihou_Get_RPTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_jy_subtract() // 管理员模式街喉点击降压时长 -号 点击事件
{
    my_lvgl_subtract(admin_waihou_Get_RPTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_jg_add() // 管理员模式街喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(admin_waihou_Get_IntervalTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_jg_subtract() // 管理员模式街喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(admin_waihou_Get_IntervalTime, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_cs_add() // 管理员模式街喉点击间隔时长 +号 点击事件
{
    my_lvgl_add(admin_waihou_Get_Time, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_cs_subtract() // 管理员模式街喉点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(admin_waihou_Get_Time, admin_waihou_Get_Button);
}
void my_lvgl_admin_waihou_set_KeyBoard() // 管理员模式街喉打开键盘
{
    lv_label_set_text(admin_waihou_Get_Button, "確認");
}

void my_lvgl_admin_on_Custom() // 管理员模式进入自订参数设置 点击事件
{
    char test[10];
    sprintf(test, "%d", Custom.WYTime);
    lv_label_set_text(admin_Custom_Set_SPTime, test);
    sprintf(test, "%d", Custom.SLTime);
    lv_label_set_text(admin_Custom_Set_TestTime, test);
    sprintf(test, "%d", Custom.JYTime);
    lv_label_set_text(admin_Custom_Set_RPTime, test);
    sprintf(test, "%d", Custom.JGTime);
    lv_label_set_text(admin_Custom_Set_IntervalTime, test);
    sprintf(test, "%d", Custom.Time);
    lv_label_set_text(admin_Custom_Set_Time, test);
}
void my_lvgl_admin_on_Custom_Settings()
{ // 管理员模式进入自订参数设置(更改参数) 点击事件
    char test[10];
    sprintf(test, "%d", Custom.WYTime);
    lv_textarea_set_text(admin_Custom_Get_SPTime, test);
    sprintf(test, "%d", Custom.SLTime);
    lv_textarea_set_text(admin_Custom_Get_TestTime, test);
    sprintf(test, "%d", Custom.JYTime);
    lv_textarea_set_text(admin_Custom_Get_RPTime, test);
    sprintf(test, "%d", Custom.JGTime);
    lv_textarea_set_text(admin_Custom_Get_IntervalTime, test);
    sprintf(test, "%d", Custom.Time);
    lv_textarea_set_text(admin_Custom_Get_Time, test);
    lv_label_set_text(admin_Custom_Get_Button, "確認");
}
void my_lvgl_admin_on_Custom_Change_Settings()
{ // 管理员模式在自订参数设置中确认更改参数  点击事件
    char *text = lv_textarea_get_text(admin_Custom_Get_SPTime);
    uint16_t value = atoi(text); // Convert text to integer
    Custom.WYTime = value;
    text = lv_textarea_get_text(admin_Custom_Get_TestTime);
    value = atoi(text); // Convert text to integer
    Custom.SLTime = value;
    text = lv_textarea_get_text(admin_Custom_Get_RPTime);
    value = atoi(text); // Convert text to integer
    Custom.JYTime = value;
    text = lv_textarea_get_text(admin_Custom_Get_IntervalTime);
    value = atoi(text); // Convert text to integer
    Custom.JGTime = value;
    text = lv_textarea_get_text(admin_Custom_Get_Time);
    value = atoi(text); // Convert text to integer
    Custom.Time = value;
    char *text2 = lv_label_get_text(admin_Custom_Get_Button);
    if (strcmp(text2, "確認") == 0)
    {
        storage_save_key_value("Custom.WYTime", Custom.WYTime);
        storage_save_key_value("Custom.SLTime", Custom.SLTime);
        storage_save_key_value("Custom.JYTime", Custom.JYTime);
        storage_save_key_value("Custom.JGTime", Custom.JGTime);
        storage_save_key_value("Custom.Time", Custom.Time);
    }
    lv_label_set_text(admin_Custom_Get_Button, "完成");
    my_lvgl_admin_on_Custom();
}
void my_lvgl_admin_Custom_wy_add() // 管理员模式自订点击稳压时间 +号 点击事件
{
    my_lvgl_add(admin_Custom_Get_SPTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_wy_subtract() // 管理员模式自订点击稳压时间 -号 点击事件
{
    my_lvgl_subtract(admin_Custom_Get_SPTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_sl_add() // 管理员模式自订点击试漏测试时间 +号 点击事件
{
    my_lvgl_add(admin_Custom_Get_TestTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_sl_subtract() // 管理员模式自订点击试漏测试时间 -号 点击事件
{
    my_lvgl_subtract(admin_Custom_Get_TestTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_jy_add() // 管理员模式自订点击降压时长 +号 点击事件
{
    my_lvgl_add(admin_Custom_Get_RPTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_jy_subtract() // 管理员模式自订点击降压时长 -号 点击事件
{
    my_lvgl_subtract(admin_Custom_Get_RPTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_jg_add() // 管理员模式自订点击间隔时长 +号 点击事件
{
    my_lvgl_add(admin_Custom_Get_IntervalTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_jg_subtract() // 管理员模式自订点击间隔时长 -号 点击事件
{
    my_lvgl_subtract(admin_Custom_Get_IntervalTime, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_cs_add() // 管理员模式自订点击？？ +号 点击事件
{
    my_lvgl_add(admin_Custom_Get_Time, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_cs_subtract() // 管理员模式自订点击？？ -号 点击事件
{
    my_lvgl_subtract(admin_Custom_Get_Time, admin_Custom_Get_Button);
}
void my_lvgl_admin_Custom_set_KeyBoard() // 管理员模式自订打开键盘
{
    lv_label_set_text(admin_Custom_Get_Button, "確認");
}

void my_lvgl_admin_close_KeyBoard() // 管理员模式关闭设置参数键盘 事件（清空输入框 保护）
{
    my_lvgl_text_noll_set_wy(admin_neihou_Get_SPTime, Neihou.WYTime);    // 判断管理员模式内喉稳压时间是否为空
    my_lvgl_text_noll_set(admin_neihou_Get_TestTime, Neihou.SLTime);     // 判断管理员模式内喉试漏时间是否为空
    my_lvgl_text_noll_set(admin_neihou_Get_RPTime, Neihou.JYTime);       // 判断管理员模式内喉降压时间是否为空
    my_lvgl_text_noll_set(admin_neihou_Get_IntervalTime, Neihou.JGTime); // 判断管理员模式内喉间隔时间是否为空
    my_lvgl_text_noll_set(admin_neihou_Get_Time, Neihou.Time);           // 判断管理员模式内喉时间是否为空

    my_lvgl_text_noll_set(admin_waihou_Get_SPTime, Waihou.WYTime);       // 判断管理员模式街喉稳压时间是否为空
    my_lvgl_text_noll_set(admin_waihou_Get_TestTime, Waihou.SLTime);     // 判断管理员模式街喉试漏时间是否为空
    my_lvgl_text_noll_set(admin_waihou_Get_RPTime, Waihou.JYTime);       // 判断管理员模式街喉降压时间是否为空
    my_lvgl_text_noll_set(admin_waihou_Get_IntervalTime, Waihou.JGTime); // 判断管理员模式街喉间隔时间是否为空
    my_lvgl_text_noll_set(admin_waihou_Get_Time, Waihou.Time);           // 判断管理员模式街喉时间是否为空

    my_lvgl_text_noll_set(admin_Custom_Get_SPTime, Custom.WYTime);       // 判断管理员模式自订稳压时间是否为空
    my_lvgl_text_noll_set(admin_Custom_Get_TestTime, Custom.SLTime);     // 判断管理员模式自订试漏时间是否为空
    my_lvgl_text_noll_set(admin_Custom_Get_RPTime, Custom.JYTime);       // 判断管理员模式自订降压时间是否为空
    my_lvgl_text_noll_set(admin_Custom_Get_IntervalTime, Custom.JGTime); // 判断管理员模式自订间隔时间是否为空
    my_lvgl_text_noll_set(admin_Custom_Get_Time, Custom.Time);           // 判断管理员模式自订时间是否为空
}

void my_lvgl_admin_log_in() // 管理员密码登录
{
    char *txt = lv_textarea_get_text(admin_PassWord);
    _ui_flag_modify(ui_SN3KB1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    if (strcmp(txt, admin_password_buff) == 0)
    {
        _ui_screen_change(&ui_Screen4, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Screen4_screen_init);
        _ui_flag_modify(admin_PassWord_Error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        my_button_on_admin_data();
    }
    else
    {
        _ui_flag_modify(admin_PassWord_Error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
    printf("admin_PassWord = %s\r\n", admin_password_buff);
}

void my_lvgl_admin_on_change_password() // 进入更改密码界面 点击事件 （清除输入密码框）
{
    lv_textarea_set_text(admin_Change_Pass_1, "");
    lv_textarea_set_text(admin_Change_Pass_2, "");
    _ui_flag_modify(admin_Change_Pass_error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    lv_label_set_text(admin_Custom_Get_Button, "保存");
}

void my_lvgl_admin_change_password() // 确认更改密码 点击事件
{
    char *pass_1 = lv_textarea_get_text(admin_Change_Pass_1);
    char *pass_2 = lv_textarea_get_text(admin_Change_Pass_2);

    if (strcmp(pass_1, "") != 0)
    {
        if (strcmp(pass_1, pass_2) == 0)
        {
            storage_save_key_blob("admin_password", (uint8_t *)pass_1, strlen(pass_1));
            printf("%s  size = %d\r\n", pass_1, strlen(pass_1));
            strcpy(admin_password_buff, pass_1);
            _ui_flag_modify(admin_Change_Pass_error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            lv_label_set_text(admin_Change_Pass_Lab, "完成");
        }
        else
        {
            lv_label_set_text(admin_Change_Pass_error, "密碼不一致,請再輸入.");
            _ui_flag_modify(admin_Change_Pass_error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            printf("passwora1 != passwor2\r\n");
        }
    }
    else
    {
        lv_label_set_text(admin_Change_Pass_error, "密碼不能為空");
        _ui_flag_modify(admin_Change_Pass_error, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        printf("change passwora size = 0\r\n");
    }
}

void my_lvgl_admin_on_unit() // 管理员模式进入测量单位参数设置 点击事件
{
    lv_dropdown_set_selected(admin_set_Me, unit);
    lv_label_set_text(admin_set_Me_Button, "保存");
}
void my_lvgl_admin_unit_Change_Settings() // 管理员模式在测量单位参数设置中确认更改参数  点击事件
{
    int res = lv_dropdown_get_selected(admin_set_Me);
    if (res != unit)
    {
        storage_save_key_value("unit", res);
        unit = res;
    }
    lv_label_set_text(admin_set_Me_Button, "完成");
}

void my_lvgl_admin_on_Setting_Time() // 管理员模式进入设置时间界面 点击事件
{
    int get_year;
    char time[20], time1[5], time2[5], time3[5], time4[5], time5[5];

    get_year = tm->tm_year;
    get_year += 1900;
    sprintf(time, "%04d", get_year);
    sprintf(time1, "%02d", (uint8_t)tm->tm_mon + 1);
    sprintf(time2, "%02d", (uint8_t)tm->tm_mday);
    sprintf(time3, "%02d", (uint8_t)tm->tm_hour);
    sprintf(time4, "%02d", (uint8_t)tm->tm_min);
    sprintf(time5, "%02d", (uint8_t)tm->tm_sec);

    lv_textarea_set_text(Setting_Time_year, time);
    lv_textarea_set_text(Setting_Time_month, time1);
    lv_textarea_set_text(Setting_Time_day, time2);
    lv_textarea_set_text(Setting_Time_hour, time3);
    lv_textarea_set_text(Setting_Time_min, time4);
    lv_textarea_set_text(Setting_Time_sec, time5);

    lv_label_set_text(Setting_Time_Lab, "保存");
}

int my_tm_day_get(int year, int mon)
{
    year += 1900;
    if (mon == 1 || mon == 3 || mon == 5 || mon == 7 || mon == 8 || mon == 10 || mon == 12)
    {
        return 31;
    }
    else if (mon == 2)
    {
        if ((year % 100) == 0)
        {
            if ((year % 400) == 0)
            {
                return 29;
            }
        }
        else if ((year % 4) == 0)
        {
            return 29;
        }
        return 28;
    }
    else
    {
        return 30;
    }
}
void my_lvgl_admin_Setting_Time() // 点击保存按键，保存时间
{
    char *text = lv_textarea_get_text(Setting_Time_year);
    int value = atoi(text); // Convert text to integer

    uint8_t buff[7];

    if (value > 2100 || value < 2024)
    {
        lv_textarea_set_text(Setting_Time_year, "2024");
        buff[6] = 24;
    }
    else
    {
        buff[6] = value - 2000;
    }
    text = lv_textarea_get_text(Setting_Time_month);
    value = atoi(text); // Convert text to integer
    if (value > 12 || value == 0)
    {
        lv_textarea_set_text(Setting_Time_month, "01");
        buff[5] = 1;
    }
    else
    {
        buff[5] = value;
    }
    // mon = set_tm->tm_mon + 1;
    text = lv_textarea_get_text(Setting_Time_day);
    value = atoi(text); // Convert text to integer
    if (value > my_tm_day_get(buff[6] + 2000, buff[5]) || value == 0)
    {
        lv_textarea_set_text(Setting_Time_day, "01");
        buff[4] = 1;
    }
    else
    {
        buff[4] = value;
    }
    buff[3] = 1;
    text = lv_textarea_get_text(Setting_Time_hour);
    value = atoi(text); // Convert text to integer
    if (value > 23)
    {
        lv_textarea_set_text(Setting_Time_hour, "00");
        buff[2] = 0;
    }
    else
    {
        buff[2] = value;
    }
    text = lv_textarea_get_text(Setting_Time_min);
    value = atoi(text); // Convert text to integer
    if (value > 59)
    {
        lv_textarea_set_text(Setting_Time_min, "00");
        buff[1] = 0;
    }
    else
    {
        buff[1] = value;
    }
    text = lv_textarea_get_text(Setting_Time_sec);
    value = atoi(text); // Convert text to integer
    if (value > 59)
    {
        lv_textarea_set_text(Setting_Time_sec, "00");
        buff[0] = 0;
    }
    else
    {
        buff[0] = value;
    }

    ds3231_set_time(buff);

    lv_label_set_text(Setting_Time_Lab, "完成");
}

void my_lvgl_next_to_Test() // 下一步进入测试界面内容 点击事件
{
    // printf("\r\n\r\nGet Test ID = %d\r\n\r\n",test_id);
    // my_nvs_test();
    printf("%05ld\r\n",my_nvs_Get_Device_ID_number());
    Employee_id_buff = lv_textarea_get_text(Employee_ID);
    Customer_buff = lv_textarea_get_text(Customer_Data);
    get_test_mode = lv_dropdown_get_selected(Test_Mode);

    if (strlen(Employee_id_buff) == 0)
    {
        Employee_id_buff = "123456";
    }

    strncpy(get_Employee_id, Employee_id_buff, 6);
    get_Employee_id[sizeof(get_Employee_id) - 1] = '\0'; // 确保以空字符结尾，避免溢出
    if (strlen(Customer_buff) == 0)
    {
        //_ui_flag_modify(ui_SN2PNL2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_SN2PNL3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
    else
    {
        my_test_next_nvs_save();
        if (get_test_mode == 0)
        {
            test = &Neihou;
        }
        else if (get_test_mode == 1)
        {
            test = &Waihou;
        }
        else if (get_test_mode == 2)
        {
            test = &Custom;
        }
        lv_label_set_text(Test_LowLeft_test_Mode, Mode_TXT[get_test_mode]);
        lv_label_set_text(Test_Low_test_Mode, Mode_TXT[get_test_mode]);

        test_status = 1;

        lv_label_set_text(Test_LowLeft_test_ID, Customer_buff); // 100
        lv_label_set_text(Test_Low_test_ID, Customer_buff);     // 180

        if (unit)
        {
            lv_label_set_text(Test_RealTime_Barometric_unit1, "KPa");
            lv_label_set_text(Test_Low_Barometric_drop_unit1, "KPa");
            _ui_flag_modify(ui_SN6PNL3PNL1LAB2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
            _ui_flag_modify(ui_SN6PNL7PNL1LAB2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        }
        else
        {
            lv_label_set_text(Test_RealTime_Barometric_unit1, "inH O");
            lv_label_set_text(Test_Low_Barometric_drop_unit1, "inH O");
            _ui_flag_modify(ui_SN6PNL3PNL1LAB2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            _ui_flag_modify(ui_SN6PNL7PNL1LAB2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

        }

        // my_lvgl_pressure_calibration();
        get_pressure_calibration = start_get_pressure_calibration;
        _ui_screen_change(&ui_Screen6, LV_SCR_LOAD_ANIM_FADE_ON, 0, 0, &ui_Screen6_screen_init);

        create_sd = 0;

        my_button_on_Test();

        _ui_flag_modify(ui_SN6PNL1BTN1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_SN6PNL1IM1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_SN6PNL1BTN1LAB1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
}

void my_lvgl_test_time_show(uint16_t time1, uint16_t time2, uint16_t time3) // 显示两个圆球的测试时间
{
    char time_text[10];
    sprintf(time_text, "%02d", time1 / 60);
    lv_label_set_text(Test_TIME1_HOUR, time_text);
    sprintf(time_text, "%02d", time1 % 60);
    lv_label_set_text(Test_TIME1_MIN, time_text);

    sprintf(time_text, "%02d", time2 / 60);
    lv_label_set_text(Test_TIME2_HOUR, time_text);
    sprintf(time_text, "%02d", time2 % 60);
    lv_label_set_text(Test_TIME2_MIN, time_text);

    sprintf(time_text, "%02d", time3 / 60);
    lv_label_set_text(Test_TIME3_HOUR, time_text);
    sprintf(time_text, "%02d", time3 % 60);
    lv_label_set_text(Test_TIME3_MIN, time_text);
}

void my_lvgl_test_start() // 开始测试 点击事件
{
    char time_text[10];

    my_lvgl_test_time_show(test->WYTime + test->SLTime, test->Time, test->JYTime + test->JGTime);

    _ui_flag_modify(Test_TIME1_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME1_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME1_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME2_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME2_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME2_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME3_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME3_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME3_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    _ui_flag_modify(Test_Low_Start_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_Status_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_Status_Lab4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    _ui_flag_modify(ui_SN6PNL1BTN1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_SN6PNL1IM1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_SN6PNL1BTN1LAB1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    test_status = 2;

    retest_Test_flag = 0;
}

void my_lvgl_pressure_calibration() // 在测试界面点击归零 点击事件
{
    get_pressure_calibration = get_pressure;
    start_get_pressure_calibration = get_pressure_calibration;
    // my_lvgl_pressure_show();
}

void my_lvgl_Test_Error_Retest() // 测试失败后点击重新测试 点击事件
{
    indic1 = lv_meter_add_arc(green_meter, green_scale, 100, lv_color_hex(0x808991), 0);
    lv_meter_set_indicator_end_value(green_meter, indic1, 360);
    indic1 = lv_meter_add_arc(green_meter, green_scale, 100, lv_color_hex(0xFCC917), 0);
    lv_meter_set_indicator_start_value(green_meter, indic1, 0);
    lv_meter_set_indicator_end_value(green_meter, indic1, 0);

    my_lvgl_test_time_show(test->WYTime + test->SLTime, test->Time, test->JYTime + test->JGTime);

    _ui_flag_modify(Test_TIME1_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME1_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    _ui_flag_modify(Test_TIME1_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    _ui_flag_modify(Test_Status_Lab5, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_Status_Lab4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    _ui_flag_modify(Test_Barometric_drop_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_Low_End_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_LowLeft_ID_Mode_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    test_status = 2;
    retest_Test_flag = 1;
}
void my_lvgl_Test_Error_ok() // 测试失败后点击完成 点击事件
{
    _ui_flag_modify(Test_Low_End_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    lv_obj_set_style_border_color(Test_Low_test_Sreen, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    _ui_flag_modify(Test_Low_test_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    _ui_flag_modify(Test_take_a_picture_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    _ui_flag_modify(Test_LowLeft_return_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
}

void my_lvgl_Test_Success_return() // 测试完成后点击返回主菜单 点击事件
{

    _ui_flag_modify(Test_TIME1_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_TIME1_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_TIME1_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_TIME2_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_TIME2_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_TIME2_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_TIME3_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_TIME3_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_TIME3_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_LowLeft_return_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_LowLeft_ID_Mode_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    _ui_flag_modify(Test_Status_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_Status_Lab3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(Test_Status_Lab5, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_Barometric_drop_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_take_a_picture_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_Low_test_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

    _ui_flag_modify(Test_Low_Start_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

    indic1 = lv_meter_add_arc(green_meter, green_scale, 100, lv_color_hex(0x808991), 0);
    lv_meter_set_indicator_end_value(green_meter, indic1, 360);
    indic1 = lv_meter_add_arc(green_meter, green_scale, 100, lv_color_hex(0xFCC917), 0);
    lv_meter_set_indicator_start_value(green_meter, indic1, 0);
    lv_meter_set_indicator_end_value(green_meter, indic1, 0);

    lv_obj_set_style_bg_color(ui_SN6BAR4, lv_color_hex(0xFCC917), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_bar_set_value(ui_SN6BAR4, 0, LV_ANIM_ON); // 增加进度条

    indic2 = lv_meter_add_arc(red_meter, red_scale, 100, lv_color_hex(0x808991), 0);
    lv_meter_set_indicator_end_value(red_meter, indic2, 360);
    indic2 = lv_meter_add_arc(red_meter, red_scale, 100, lv_color_hex(0xFCC917), 0);
    lv_meter_set_indicator_start_value(red_meter, indic2, 0);
    lv_meter_set_indicator_end_value(red_meter, indic2, 0);

    test_status = 0;
}

void set_power_start_flag()
{
    power_start_flag = 1;
}

void my_Test_Handle() // 测试线程
{
    static uint8_t error = 0;
    double pressure_show;

    vTaskDelay(150);
    my_main_interface_init();
    gpio_set_level(6, 1);
    // power_start_flag = 1;
    start_get_pressure_calibration = pressure_sensor_value_read();
    while (1)
    {
        if (delay_500ms == 1)
        {
            delay_500ms = 0;
            tick = ds3231_get_time();
            tm = localtime(&tick);
            // printf("Time = %d %d %d   %d %d %d\r\n", tm->tm_year, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            if (time_show_status)
            {
                my_lvgl_time_show();
                my_lvgl_bat_show();
            }
            my_lvgl_pressure_show();
            if (power_flag == 1)
            {
                power_flag = 2;
                switch (power_select)
                {
                case 1:
                    _ui_flag_modify(ui_SN2PNL4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                    break;
                case 2:
                    _ui_flag_modify(ui_SN3PNL6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                    break;
                case 3:
                    _ui_flag_modify(ui_SN4PNL12PNL1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                    break;
                case 4:
                    _ui_flag_modify(ui_SN5PNL8PNL1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                    break;
                case 5:
                    _ui_flag_modify(ui_SN6PNL11, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                    break;
                }
            }
        }

        switch (test_status)
        {
        case 0: // 不在测试界面等待
            break;
        case 1: // 测试界面等待开始
            // if (create_sd == 0)
            // {
            //     create_sd = 1;
            //     record_file_create(Customer_buff);
            // }
            // printf("test_status = 1\r\n");
            break;
        case 2: // 点击开始测试
            if (create_sd == 0)
            {
                create_sd = 1;
                record_file_create(Customer_buff);
            }
            delay_time = 0;
            delay_1s = 0;
            test_status = 3;
            error = 0;

            test_delay_time1 = 0;
            test_delay_time2 = 0;
            test_delay_time3 = 0;

            count = 0;
            test_count = 0;

            pressure_deviation = 0;
            break;
        case 3:
            if (delay_100ms)
            {
                delay_100ms = 0;
                if (test_delay_time1 < test->WYTime)
                {
                    pressure_down = get_pressure;
                }
                else if (pressure_down > get_pressure && (pressure_down - get_pressure) > 25)
                {
                    if ((pressure_down - get_pressure) > pressure_deviation)
                        pressure_deviation = pressure_down - get_pressure;
                    error = 1;
                }
            }
            if (delay_1s)
            {
                delay_1s = 0;
                if (test_delay_time1 < (test->WYTime + test->SLTime))
                    test_delay_time1 += 1;

                lv_meter_set_indicator_end_value(green_meter, indic1, test_delay_time1 * 360 / (test->WYTime + test->SLTime));
                my_lvgl_test_time_show(test->WYTime + test->SLTime - test_delay_time1, test->Time, test->JYTime + test->JGTime);

                if (test_delay_time1 >= (test->WYTime + test->SLTime))
                {
                    _ui_flag_modify(Test_TIME1_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_TIME1_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_TIME1_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    if (error == 0) // 测试成功
                    {
                        test_status = 4;
                        _ui_flag_modify(Test_Status_Lab4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                        _ui_flag_modify(Test_Status_Lab2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        indic1 = lv_meter_add_arc(green_meter, green_scale, 100, lv_color_hex(0x17FC1F), 0);
                        lv_meter_set_indicator_start_value(green_meter, indic1, 0);
                        lv_meter_set_indicator_end_value(green_meter, indic1, 360);

                        valve_open_time = 500;
                        gpio_set_level(GPIO_OUTPUT_IO_1, 1);
                        bat_down_status = 1;
                    }
                    else // 测试失败
                    {
                        test_status = 10;
                        _ui_flag_modify(Test_Status_Lab4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                        _ui_flag_modify(Test_Status_Lab5, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        _ui_flag_modify(Test_LowLeft_ID_Mode_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

                        _ui_flag_modify(Test_Barometric_drop_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                        _ui_flag_modify(Test_Low_End_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        indic1 = lv_meter_add_arc(green_meter, green_scale, 100, lv_color_hex(0xff0000), 0);
                        lv_meter_set_indicator_start_value(green_meter, indic1, 0);
                        lv_meter_set_indicator_end_value(green_meter, indic1, 360);

                        if (unit)
                        {
                            pressure_show = (double)pressure_deviation / 1000;
                        }
                        else
                        {
                            pressure_show = (double)pressure_deviation / 249;
                        }

                        char P_TEXT[20];
                        sprintf(P_TEXT, "%.2f", pressure_show);

                        lv_label_set_text(Test_Low_Barometric_drop_Lab, P_TEXT);
                        lv_label_set_text(Test_Low_Error_Lab, "氣壓下降:");

                        _ui_flag_modify(ui_SN6PNL2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

                        if(retest_Test_flag == 1){
                            my_lvgl_Test_Error_ok();
                        }
                    }
                }
                if (test_delay_time1 > test->WYTime)
                {
                    count++;
                    test_count++;
                    vTaskDelay(3);
                    write_record_to_file(my_nvs_Get_Device_ID_number(), get_Employee_id, (get_test_mode + 1), 0x0a, count, test_count, pressure, unit);
                }
                // printf("test 1   %d\r\n", test_delay_time1);
            }
            break;
        case 4:
            if (delay_1s)
            {
                delay_1s = 0;
                if (test_delay_time2 < (test->JYTime + test->JGTime))
                    test_delay_time2 += 1;
                lv_bar_set_value(ui_SN6BAR4, test_delay_time2 * 100 / (test->JYTime + test->JGTime), LV_ANIM_ON);
                my_lvgl_test_time_show(0, test->Time, test->JYTime + test->JGTime - test_delay_time2);
                if (test_delay_time2 == test->JYTime)
                {
                    valve_open_time = 0;
                    gpio_set_level(GPIO_OUTPUT_IO_0, 0);
                    gpio_set_level(GPIO_OUTPUT_IO_1, 0);
                    bat_down_status = 0;
                }
                if (test_delay_time2 >= (test->JYTime + test->JGTime))
                {
                    _ui_flag_modify(Test_TIME3_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_TIME3_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_TIME3_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    test_status = 5;
                    _ui_flag_modify(Test_Status_Lab2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_Status_Lab1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                    lv_bar_set_value(ui_SN6BAR4, 100, LV_ANIM_ON); // 增加进度条
                    lv_obj_set_style_bg_color(ui_SN6BAR4, lv_color_hex(0x17FC1F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    error = 0;
                    pressure_up = get_pressure;
                    test_count = 0;
                    pressure_deviation = 0;
                }
                // printf("test 2   %d\r\n", test_delay_time2);
            }
            break;
        case 5:
            if (delay_1s)
            {
                delay_1s = 0;
                if (test_delay_time3 < test->Time)
                    test_delay_time3 += 1;
                lv_meter_set_indicator_end_value(red_meter, indic2, test_delay_time3 * 360 / (test->Time));
                my_lvgl_test_time_show(0, test->Time - test_delay_time3, 0);

                if (test_delay_time3 >= test->Time)
                {
                    if (pressure_up < get_pressure && (get_pressure - pressure_up) > 25)
                    {
                        if ((get_pressure - pressure_up) > pressure_deviation)
                            pressure_deviation = get_pressure - pressure_up;
                        error = 1;
                    }
                    _ui_flag_modify(Test_TIME2_HOUR, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_TIME2_Symbol, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_TIME2_MIN, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);

                    _ui_flag_modify(Test_LowLeft_ID_Mode_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    _ui_flag_modify(Test_LowLeft_return_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                    _ui_flag_modify(Test_take_a_picture_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
                    if (error == 0)
                    {
                        test_status = 6;
                        _ui_flag_modify(Test_Status_Lab1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                        _ui_flag_modify(Test_Status_Lab3, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        lv_obj_set_style_border_color(Test_Low_test_Sreen, lv_color_hex(0x17FC1F), LV_PART_MAIN | LV_STATE_DEFAULT);
                        _ui_flag_modify(Test_Low_test_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        indic2 = lv_meter_add_arc(red_meter, red_scale, 100, lv_color_hex(0x17fc1f), 0);
                        lv_meter_set_indicator_start_value(red_meter, indic2, 0);
                        lv_meter_set_indicator_end_value(red_meter, indic2, 360);

                        lv_textarea_set_text(Customer_Data, ""); // 测试成功清除客户资料输入框
                    }
                    else
                    {
                        test_status = 11;

                        _ui_flag_modify(Test_Status_Lab1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                        _ui_flag_modify(Test_Status_Lab5, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        _ui_flag_modify(Test_Barometric_drop_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        lv_obj_set_style_border_color(Test_Low_test_Sreen, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                        _ui_flag_modify(Test_Low_test_Sreen, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);

                        indic2 = lv_meter_add_arc(red_meter, red_scale, 100, lv_color_hex(0xff0000), 0);
                        lv_meter_set_indicator_start_value(red_meter, indic2, 0);
                        lv_meter_set_indicator_end_value(red_meter, indic2, 360);

                        if (unit)
                        {
                            pressure_show = (double)pressure_deviation / 1000;
                        }
                        else
                        {
                            pressure_show = (double)pressure_deviation / 249;
                        }

                        char P_TEXT[20];
                        sprintf(P_TEXT, "%.2f", pressure_show);

                        lv_label_set_text(Test_Low_Barometric_drop_Lab, P_TEXT);
                        lv_label_set_text(Test_Low_Error_Lab, "過刮:");

                        _ui_flag_modify(ui_SN6PNL2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
                    }
                }
                count++;
                test_count++;
                vTaskDelay(3);
                write_record_to_file(my_nvs_Get_Device_ID_number(), get_Employee_id, (get_test_mode + 1), 0x0b, count, test_count, pressure, unit);
                // printf("test 3   %d\r\n", test_delay_time3);
            }
            break;
        case 6:
            if (delay_1s)
            {
                delay_1s = 0;
                // printf("test ok\r\n");
            }
            break;
        case 10:
            break;
        }
        vTaskDelay(10);
    }
}

void test_time_1ms(void *arg)
{
    delay_time++;
    if ((delay_time % 1000) == 0)
    {
        delay_1s = 1;
        bat_delay_1s = 1;
        lvgl_show_1s = 1;
    }
    if ((delay_time % 1000) == 0)
        delay_500ms = 1;
    if ((delay_time % 100) == 0)
        delay_100ms = 1;
    if (valve_open_time > 1)
        valve_open_time--;
    else if (valve_open_time == 1)
    {
        valve_open_time = 0;
        gpio_set_level(GPIO_OUTPUT_IO_0, 1);
    }
}

void my_Test_Handle_init() // 测试线程初始化函数
{
    xTaskCreatePinnedToCore(my_Test_Handle, "my_Test_Handle", 8192, NULL, 4, NULL, tskNO_AFFINITY);

    // 创建一个esp_timer_create_args_t结构体，设置定时器参数
    esp_timer_create_args_t timer_args = {
        .callback = test_time_1ms,         // 设置回调函数
        .name = "my_timer",                // 设置定时器名称
        .dispatch_method = ESP_TIMER_TASK, // 从任务中调用回调函数
        .skip_unhandled_events = false     // 不跳过未处理的事件
    };
    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);

    // 启动定时器，每秒触发一次
    esp_timer_start_periodic(timer, 1000); // 1000微秒 = 1ms
}

void my_button_on_Setting() // 进入设置选择界面
{
    power_select = 2;
}
void my_button_Setting_to_main() // 返回主界面
{
    power_select = 1;
}

void my_button_on_admin_data() // 管理员登录设置
{
    power_select = 3;
}
void my_button_on_data() // 无密码登录设置
{
    power_select = 4;
}
void my_button_on_Test() // 进入测试界面
{
    power_select = 5;
}

static void gpio_task(void *arg)
{
    while (1)
    {

        if (power_status == 0)
        {
            if (power_start_flag)
            {
                power_status = 1;
            }

            // if (gpio_get_level(GPIO_OUTPUT_IO_2) == 1)
            // {
            //     power_set_time++;
            //     if (power_set_time > 10)
            //     {
            //         power_status = 1;
            //         power_set_time = 0;
            //     }
            // }
            // else
            // {
            //     power_set_time = 0;
            // }
        }
        else
        {
            if (gpio_get_level(GPIO_OUTPUT_IO_2) == 0)
            {
                power_set_time++;
                if (power_set_time >= 10 && power_flag == 0)
                {
                    power_set_time = 0;
                    power_flag = 1;
                    bat_nvs_time = 20000;
                    // bat_high = bat / 0x10000;
                    // bat_low = bat % 0x10000;
                    // storage_save_key_value("bat_status", 1);
                    // storage_save_key_value("bat_high", bat_high);
                    // storage_save_key_value("bat_low", bat_low);
                }
            }
            else
            {
                power_set_time = 0;
            }
        }
        if (test_status == 3 || test_status == 4 || test_status == 5 || gpio_get_level(GPIO_OUTPUT_IO_2) == 0 || gpio_get_level(5) == 0)
        {
            if (lvgl_show_time >= DORMANCY_TIME)
            {
                gpio_set_level(6, 1);
            }
            lvgl_show_time = 0;
            xp_flag = 0;
        }
        else
        {
            if (lvgl_show_1s)
            {
                lvgl_show_1s = 0;
                lvgl_show_time++;
                if (lvgl_show_time == DORMANCY_TIME)
                {
                    gpio_set_level(6, 0);
                    xp_flag = 1;
                }
                else if (lvgl_show_time >= POWER_OFF_TIME)
                {
                    printf("power off\r\n");
                    bat_high = bat / 0x10000;
                    bat_low = bat % 0x10000;
                    storage_save_key_value("bat_status", 1);
                    storage_save_key_value("bat_high", bat_high);
                    storage_save_key_value("bat_low", bat_low);

                    vTaskDelay(100);

                    my_Shutdown();
                }
            }
        }
        bat_nvs_time++;
        if (bat_nvs_time >= 18000)
        {
            bat_nvs_time = 0;
            bat_high = bat / 0x10000;
            bat_low = bat % 0x10000;
            storage_save_key_value("bat_status", 1);
            storage_save_key_value("bat_high", bat_high);
            storage_save_key_value("bat_low", bat_low);
        }

        vTaskDelay(10);
    }
}

void gpio_init(void)
{
    esp_rom_gpio_pad_select_gpio(GPIO_OUTPUT_IO_0); // 阀门控制GPIO40
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_OUTPUT_IO_0, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_OUTPUT_IO_0, 0);

    esp_rom_gpio_pad_select_gpio(GPIO_OUTPUT_IO_1); // 阀门使能GPIO02
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(GPIO_OUTPUT_IO_1, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_OUTPUT_IO_1, 0);

    esp_rom_gpio_pad_select_gpio(GPIO_OUTPUT_IO_2); // 检测电源芯片开关GPIO44
    gpio_set_direction(GPIO_OUTPUT_IO_2, GPIO_MODE_INPUT);

    esp_rom_gpio_pad_select_gpio(6); // 阀门控制GPIO40
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(6, GPIO_MODE_OUTPUT);
    gpio_set_level(6, 0);

    xTaskCreatePinnedToCore(gpio_task, "gpio_task", 4096, NULL, 5, NULL, tskNO_AFFINITY);
    printf("gpio init ok\n");
}

void my_Shutdown()
{
    printf("Shutdown\r\n");
    if (charge_status == 0 || charge_status == 4)
    {
        // bat_high = bat / 0x10000;
        // bat_low = bat % 0x10000;
        // storage_save_key_value("bat_status", 1);
        // storage_save_key_value("bat_high", bat_high);
        // storage_save_key_value("bat_low", bat_low);
        // vTaskDelay(10);
        PowerOFF();
    }
}

void my_close_Shutdown() // 关闭关机弹窗
{
    power_flag = 0;
    switch (power_select)
    {
    case 1:
        _ui_flag_modify(ui_SN2PNL4, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        break;
    case 2:
        _ui_flag_modify(ui_SN3PNL6, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        break;
    case 3:
        _ui_flag_modify(ui_SN4PNL12PNL1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        break;
    case 4:
        _ui_flag_modify(ui_SN5PNL8PNL1, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        break;
    case 5:
        _ui_flag_modify(ui_SN6PNL11, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        break;
    }
}

void my_charge_set_status(uint8_t status) // 设置充电状态
{
    if (charge_status == 0 || charge_status == 4)
    {
        if (status == 1 || status == 2 || status == 3)
        {
            charge_status = status;
            if (lvgl_show_time >= DORMANCY_TIME)
            {
                gpio_set_level(6, 1);
            }
            lvgl_show_time = 0;
        }
    }
    else
    {
        charge_status = status;
        if (status == 0)
        {
            if (lvgl_show_time >= DORMANCY_TIME)
            {
                gpio_set_level(6, 1);
            }
            lvgl_show_time = 0;
        }
        // if (status == 0)
        // {
        //     charge_status = 0;
        // }
    }
    if (charge_status == 0 && lvgl_bat <= 20)
        charge_status = 4;
}

uint16_t my_adc_bat_compute(int get_adc_vcc) // 根据获取到的ADC值，粗略换算实际电量
{
    uint16_t compute_data = 0, i;
    uint16_t data1, data2, data3;
    for (i = 0; i < 12; i++)
    {
        if (get_adc_vcc >= bat_adc_valve[i])
        {
            if (i == 0)
                compute_data = 100;
            else
            {
                data1 = get_adc_vcc - bat_adc_valve[i];
                data2 = bat_show_valve[i - 1] - bat_show_valve[i];
                data3 = bat_adc_valve[i - 1] - bat_adc_valve[i];
                compute_data = data1 * data2 / data3;
                compute_data += bat_show_valve[i];
                i = 12;
            }
        }
    }
    if (compute_data > 100)
        compute_data = 100;
    return compute_data;
    // lvgl_bat = (uint8_t)compute_data;
    // bat = lvgl_bat * BATTERY_CAPACITY / 100;
}

void my_bat_down(uint16_t down_data)
{ // 掉电
    if (bat >= down_data)
        bat -= down_data;
}
void my_bat_up(uint16_t up_data)
{ // 充电
    bat += up_data;
    // if (charge_status == 3)
    // {
    if (bat > (BATTERY_CAPACITY / 100 + BATTERY_CAPACITY - 1))
        bat = (BATTERY_CAPACITY / 100 + BATTERY_CAPACITY - 1);
    // }
    // else
    // {
    //     if (bat > (BATTERY_CAPACITY - 1))
    //         bat = BATTERY_CAPACITY - 1;
    // }
}

void my_bat_Handle() // 电池电量计算线程
{
    int get_adc_data;
    int adc_vcc;
    uint16_t get_adc_bat;
    Power_VSYS_MIN_I2C_init();
    ChargingDetection();

    get_adc_data = adc1_get_raw(ADC1_TEST_CHANNEL);
    adc_vcc = get_adc_data * 1253 / 1024;

    if (bat_status == 0)
    {
        printf("get adc %d  %d\r\n", get_adc_data, adc_vcc);
        if (charge_status == 1)
            adc_vcc -= 200;
        else if (charge_status == 2)
            adc_vcc -= 100;
        else if (charge_status == 3)
            adc_vcc = 4200;
        // my_adc_bat_compute(adc_vcc);
        lvgl_bat = (uint8_t)my_adc_bat_compute(adc_vcc);
        bat = lvgl_bat * BATTERY_CAPACITY / 100;
    }
    else
    {
        bat = bat_high * 0x10000 + bat_low;
        my_bat_down(300);
        lvgl_bat = bat * 100 / BATTERY_CAPACITY;
    }

    while (1)
    {
        if (bat_delay_1s == 1)
        {
            bat_delay_1s = 0;
            if (charge_status == 0 || charge_status == 4)
            {
                if (xp_flag == 1)
                {
                    my_bat_down(80);
                }
                else if (bat_down_status == 0)
                {
                    my_bat_down(200);
                }
                else
                {
                    my_bat_down(500);
                }
            }
            else if (charge_status == 1 || charge_status == 2 || charge_status == 3)
            {
                get_adc_data = adc1_get_raw(ADC1_TEST_CHANNEL);
                adc_vcc = get_adc_data * 1253 / 1024;
                get_adc_bat = my_adc_bat_compute(adc_vcc);
                if (charge_status == 3)
                {
                    my_bat_up(1500);
                }
                else if (lvgl_bat < get_adc_bat)
                {
                    my_bat_up(1000);
                }
                // printf("charge status %d  %d  %d  %d\r\n", charge_status, lvgl_bat, get_adc_bat, adc_vcc);
            }
            // else if (charge_status == 1)
            // {
            //     if (bat_down_status == 0)
            //     {
            //         my_bat_up(800);
            //     }
            //     else
            //     {
            //         my_bat_up(500);
            //     }
            //     printf("charge1 bat = %d \r\n", lvgl_bat);
            // }
            // else if (charge_status == 2)
            // {
            //     my_bat_up(300);
            //     printf("charge2 bat = %d\r\n", lvgl_bat);
            // }
            // else if (charge_status == 3)
            // {
            //     my_bat_up(1200);
            //     printf("stdby bat = %d\r\n", lvgl_bat);
            // }
            lvgl_bat = bat * 100 / BATTERY_CAPACITY;
        }
        vTaskDelay(50);
        // printf("500ms\r\n");
        // Power_VSYS_MIN_I2C_init();
        ChargingDetection();

        get_adc_data = adc1_get_raw(ADC1_TEST_CHANNEL);
        adc_vcc = get_adc_data * 1253 / 1024;
        if (adc_vcc < 3000)
        {
            bat_high = 0;
            bat_low = 0;
            storage_save_key_value("bat_status", 1);
            storage_save_key_value("bat_high", bat_high);
            storage_save_key_value("bat_low", bat_low);
            vTaskDelay(100);
            
            set_power_i2c(0x64);
            my_Shutdown();
            // 关机
        }
    }
}

void my_bat_adc_init()
{
    adc1_config_width(ADC_BITWIDTH_12);                             // 12位分辨率
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_DB_2_5); // 设置通道6和2.2V参考电压
    xTaskCreatePinnedToCore(my_bat_Handle, "ADC_task", 4096, NULL, 4, NULL, tskNO_AFFINITY);
}
