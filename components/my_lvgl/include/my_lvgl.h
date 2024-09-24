

struct Test_Data {
    uint16_t WYTime;        //稳压时间
    uint16_t SLTime;      ////试漏测试时间
    uint16_t JYTime;        //降压时间
    uint16_t JGTime;  //间隔时间 
    uint16_t Time;          //试过刮
};
void my_lvgl_data_init();        // 参数初始化
void my_lvgl_time_show();   //时间显示
void my_lvgl_bat_show(); //电量显示

void my_lvgl_on_Parameter_setting(); //进入参数设置界面 点击事件

void my_lvgl_user_on_neihou();           //用户模式进入内喉参数设置 点击事件
void my_lvgl_user_on_neihou_Settings();  // 用户模式进入内喉参数设置(更改参数) 点击事件
void my_lvgl_user_on_neihou_Change_Settings();  // 用户模式在内喉参数设置中确认更改参数  点击事件
void my_lvgl_user_neihou_wy_add(); // 用户模式内喉点击稳压时间 +号 点击事件
void my_lvgl_user_neihou_wy_subtract(); // 用户模式内喉点击稳压时间 -号 点击事件
void my_lvgl_user_neihou_sl_add(); // 用户模式内喉点击试漏测试时间 +号 点击事件
void my_lvgl_user_neihou_sl_subtract(); // 用户模式内喉点击试漏测试时间 -号 点击事件
void my_lvgl_user_neihou_jy_add(); // 用户模式内喉点击降压时长 +号 点击事件
void my_lvgl_user_neihou_jy_subtract(); // 用户模式内喉点击降压时长 -号 点击事件
void my_lvgl_user_neihou_jg_add(); // 用户模式内喉点击间隔时长 +号 点击事件
void my_lvgl_user_neihou_jg_subtract(); // 用户模式内喉点击间隔时长 -号 点击事件
void my_lvgl_user_neihou_cs_add(); // 用户模式内喉点击试过刮 +号 点击事件
void my_lvgl_user_neihou_cs_subtract(); // 用户模式内喉点击试过刮 -号 点击事件
void my_lvgl_user_neihou_set_KeyBoard();     //用户模式内喉打开键盘

void my_lvgl_user_on_waihou(); // 用户模式进入街喉参数设置 点击事件
void my_lvgl_user_on_waihou_Settings(); // 用户模式进入街喉参数设置(更改参数) 点击事件
void my_lvgl_user_on_waihou_Change_Settings();  // 用户模式在街喉参数设置中确认更改参数  点击事件
void my_lvgl_user_waihou_wy_add(); // 用户模式街喉点击稳压时间 +号 点击事件
void my_lvgl_user_waihou_wy_subtract(); // 用户模式街喉点击稳压时间 -号 点击事件
void my_lvgl_user_waihou_sl_add(); // 用户模式街喉点击试漏测试时间 +号 点击事件
void my_lvgl_user_waihou_sl_subtract(); // 用户模式街喉点击试漏测试时间 -号 点击事件
void my_lvgl_user_waihou_jy_add(); // 用户模式街喉点击降压时长 +号 点击事件
void my_lvgl_user_waihou_jy_subtract(); // 用户模式街喉点击降压时长 -号 点击事件
void my_lvgl_user_waihou_jg_add(); // 用户模式街喉点击间隔时长 +号 点击事件
void my_lvgl_user_waihou_jg_subtract(); // 用户模式街喉点击间隔时长 -号 点击事件
void my_lvgl_user_waihou_cs_add(); // 用户模式街喉点击试过刮 +号 点击事件
void my_lvgl_user_waihou_cs_subtract(); // 用户模式街喉点击试过刮 -号 点击事件
void my_lvgl_user_waihou_set_KeyBoard();     //用户模式街喉打开键盘

void my_lvgl_user_on_Custom(); // 用户模式进入自订参数设置 点击事件
void my_lvgl_user_on_Custom_Settings();// 用户模式进入自订参数设置(更改参数) 点击事件
void my_lvgl_user_on_Custom_Change_Settings();// 用户模式在自订参数设置中确认更改参数  点击事件
void my_lvgl_user_Custom_wy_add(); // 用户模式自订点击稳压时间 +号 点击事件
void my_lvgl_user_Custom_wy_subtract(); // 用户模式自订点击稳压时间 -号 点击事件
void my_lvgl_user_Custom_sl_add(); // 用户模式自订点击试漏测试时间 +号 点击事件
void my_lvgl_user_Custom_sl_subtract(); // 用户模式自订点击试漏测试时间 -号 点击事件
void my_lvgl_user_Custom_jy_add(); // 用户模式自订点击降压时长 +号 点击事件
void my_lvgl_user_Custom_jy_subtract(); // 用户模式自订点击降压时长 -号 点击事件
void my_lvgl_user_Custom_jg_add(); // 用户模式自订点击间隔时长 +号 点击事件
void my_lvgl_user_Custom_jg_subtract(); // 用户模式自订点击间隔时长 -号 点击事件
void my_lvgl_user_Custom_cs_add(); // 用户模式自订点击试过刮 +号 点击事件
void my_lvgl_user_Custom_cs_subtract(); // 用户模式自订点击试过刮 -号 点击事件
void my_lvgl_user_Custom_set_KeyBoard();     //用户模式自订打开键盘

void my_lvgl_user_close_KeyBoard(); // 用户模式关闭设置参数键盘 事件（清空输入框 保护）

void my_lvgl_admin_on_neihou();           //管理员模式进入内喉参数设置 点击事件
void my_lvgl_admin_on_neihou_Settings();  // 管理员模式进入内喉参数设置(更改参数) 点击事件
void my_lvgl_admin_on_neihou_Change_Settings();  // 管理员模式在内喉参数设置中确认更改参数  点击事件
void my_lvgl_admin_neihou_wy_add(); // 管理员模式内喉点击稳压时间 +号 点击事件
void my_lvgl_admin_neihou_wy_subtract(); // 管理员模式内喉点击稳压时间 -号 点击事件
void my_lvgl_admin_neihou_sl_add(); // 管理员模式内喉点击试漏测试时间 +号 点击事件
void my_lvgl_admin_neihou_sl_subtract(); // 管理员模式内喉点试漏测试时间 -号 点击事件
void my_lvgl_admin_neihou_jy_add(); // 管理员模式内喉点击降压时长 +号 点击事件
void my_lvgl_admin_neihou_jy_subtract(); // 管理员模式内喉点击降压时长 -号 点击事件
void my_lvgl_admin_neihou_jg_add(); // 管理员模式内喉点击间隔时长 +号 点击事件
void my_lvgl_admin_neihou_jg_subtract(); // 管理员模式内喉点击间隔时长 -号 点击事件
void my_lvgl_admin_neihou_cs_add(); // 管理员模式内喉点击试过刮 +号 点击事件
void my_lvgl_admin_neihou_cs_subtract(); // 管理员模式内喉点击试过刮 -号 点击事件
void my_lvgl_admin_neihou_set_KeyBoard();     //管理员模式内喉打开键盘

void my_lvgl_admin_on_waihou(); // 管理员模式进入街喉参数设置 点击事件
void my_lvgl_admin_on_waihou_Settings(); // 管理员模式进入街喉参数设置(更改参数) 点击事件
void my_lvgl_admin_on_waihou_Change_Settings();  // 管理员模式在街喉参数设置中确认更改参数  点击事件
void my_lvgl_admin_waihou_wy_add(); // 管理员模式街喉点击稳压时间 +号 点击事件
void my_lvgl_admin_waihou_wy_subtract(); // 管理员模式街喉点击稳压时间 -号 点击事件
void my_lvgl_admin_waihou_sl_add(); // 管理员模式街喉点击试漏测试时间 +号 点击事件
void my_lvgl_admin_waihou_sl_subtract(); // 管理员模式街喉点试漏测试时间 -号 点击事件
void my_lvgl_admin_waihou_jy_add(); // 管理员模式街喉点击降压时长 +号 点击事件
void my_lvgl_admin_waihou_jy_subtract(); // 管理员模式街喉点击降压时长 -号 点击事件
void my_lvgl_admin_waihou_jg_add(); // 管理员模式街喉点击间隔时长 +号 点击事件
void my_lvgl_admin_waihou_jg_subtract(); // 管理员模式街喉点击间隔时长 -号 点击事件
void my_lvgl_admin_waihou_cs_add(); // 管理员模式街喉点击试过刮 +号 点击事件
void my_lvgl_admin_waihou_cs_subtract(); // 管理员模式街喉点击试过刮 -号 点击事件
void my_lvgl_admin_waihou_set_KeyBoard();     //管理员模式街喉打开键盘

void my_lvgl_admin_on_Custom(); // 管理员模式进入自订参数设置 点击事件
void my_lvgl_admin_on_Custom_Settings();// 管理员模式进入自订参数设置(更改参数) 点击事件
void my_lvgl_admin_on_Custom_Change_Settings();// 管理员模式在自订参数设置中确认更改参数  点击事件
void my_lvgl_admin_Custom_wy_add(); // 管理员模式自订点击稳压时间 +号 点击事件
void my_lvgl_admin_Custom_wy_subtract(); // 管理员模式自订点击稳压时间 -号 点击事件
void my_lvgl_admin_Custom_sl_add(); // 管理员模式自订点击试漏测试时间 +号 点击事件
void my_lvgl_admin_Custom_sl_subtract(); // 管理员模式自订点击试漏测试时间 -号 点击事件
void my_lvgl_admin_Custom_jy_add(); // 管理员模式自订点击降压时长 +号 点击事件
void my_lvgl_admin_Custom_jy_subtract(); // 管理员模式自订点击降压时长 -号 点击事件
void my_lvgl_admin_Custom_jg_add(); // 管理员模式自订点击间隔时长 +号 点击事件
void my_lvgl_admin_Custom_jg_subtract(); // 管理员模式自订点击间隔时长 -号 点击事件
void my_lvgl_admin_Custom_cs_add(); // 管理员模式自订点击？？ +号 点击事件
void my_lvgl_admin_Custom_cs_subtract(); // 管理员模式自订点击？？ -号 点击事件
void my_lvgl_admin_Custom_set_KeyBoard();     //管理员模式自订打开键盘

void my_lvgl_admin_on_Setting_Time();       //管理员模式进入设置时间界面 点击事件
void my_lvgl_admin_Setting_Time(); // 点击保存按键，保存时间


void my_lvgl_admin_close_KeyBoard(); // 管理员模式关闭设置参数键盘 事件（清空输入框 保护）

void my_lvgl_admin_log_in();    //管理员密码登录
void my_lvgl_admin_on_change_password();     //进入更改密码界面 点击事件 （清除输入密码框）
void my_lvgl_admin_change_password();           //确认更改密码 点击事件

void my_lvgl_admin_on_unit();    // 管理员模式进入测量单位参数设置 点击事件
void my_lvgl_admin_unit_Change_Settings();// 管理员模式在测量单位参数设置中确认更改参数  点击事件

void my_lvgl_next_to_Test(); // 下一步进入测试界面内容

void my_lvgl_test_start();           //开始测试 点击事件
void my_lvgl_pressure_calibration(); // 在测试界面点击归零 点击事件

void my_lvgl_Test_Error_Retest(); // 测试失败后点击重新测试 点击事件
void my_lvgl_Test_Error_ok(); // 测试失败后点击完成 点击事件

void my_lvgl_Test_Success_return(); // 测试完成后点击返回主菜单 点击事件


void my_Test_Handle_init();      //测试线程初始化函数

void gpio_init(void);         //关机按键线程 + 阀门控制IO初始化
void my_button_on_Setting(); //进入设置选择界面
void my_button_Setting_to_main();    //返回主界面
void my_button_on_admin_data();  //管理员登录设置
void my_button_on_data();    //无密码登录设置
void my_button_on_Test();    //进入测试界面

void my_Shutdown();     //关机
void my_close_Shutdown();   //关闭关机弹窗

void my_bat_adc_init();     //电量ADC初始化
void my_charge_set_status(uint8_t status);  //设置充电状态


void set_power_start_flag();

extern uint8_t showwring;















