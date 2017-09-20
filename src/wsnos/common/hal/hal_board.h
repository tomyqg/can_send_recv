/**
* @brief       :
*
* @file        : hal_board.h
* @author      : gang.cheng
*
* Version      : v0.0.1
* Date         : 2015/5/7
* Change Logs  :
*
* Date        Version      Author      Notes
* 2015/5/7    v0.0.1      gang.cheng    first version
*/


#ifndef __HAL_BOARD_H
#define __HAL_BOARD_H
#include "common/lib/data_type_def.h"
#include "platform/platform.h"

#define HAL_LED_BLUE          0x01
#define HAL_LED_RED           0x02
#define HAL_LED_GREEN         0x03
#define HAL_LED_POWER         0x04
#define HAL_LED_ERROR         0x05

#define DEVICE_MODE_WORK      0x00
#define DEVICE_MODE_TEST      0xDEADBEEF
#define DEVICE_FLAG_CLEAR     0xFFFFFFFF

typedef struct
{
    uint64_t    unique_id;              //*< 唯一的设备标识ID
    uint32_t    device_id ;            //*< 设备 ID
    uint8_t     rf_channel;            //*< rf信道
    uint8_t     rf_power;              //*< rf功率

    uint16_t    heart_cycle;
    uint16_t    th_dcycle;
    uint16_t    th_scycle;

    int8_t      temp_lower;
    int8_t      temp_higher;

    int8_t      hum_lower;
    int8_t      hum_higher;

    uint16_t    sm_scycle;
    uint8_t     smoke_threshold;

    uint16_t    acc_scycle;
    uint32_t    shock_threshold;
    uint32_t    impact_threshold;
    uint32_t    tilt_threshold;

    bool_t      th_enable;
    bool_t      smoke_enable;
    bool_t      acc_enable;

    uint8_t     operate_mode;
    uint8_t     offset_z;
    uint32_t    noload_ad;
    uint32_t    standard_ad;
    uint16_t    standard_weight;   
} device_info_t;

extern device_info_t device_info;


/**
* 获取设备短地址
*
* @param: 无
*
* @return: 设备短地址
*/
uint16_t hal_board_get_device_short_addr(void);


/**
* 系统重启
*
* @param: 无
*
* @return: 无
*/
void hal_board_reset(void);

/**
* LED初始化管脚配置
*
* @param: 无
*
* @return: 无
*/
void hal_led_init(void);

/**
* 点亮LED
*
* @param color: 需要点亮的LED的颜色
*
* @return: 无
*/
void hal_led_open(uint8_t color);

/**
* 熄灭LED
*
* @param color: 需要熄灭的LED的颜色
*
* @return: 无
*/
void hal_led_close(uint8_t color);

/**
* LED反转(亮-->灭  或  灭-->亮)
*
* @param color: 需要反转的LED的颜色
*
* @return: 无
*/
void hal_led_toggle(uint8_t color);

/**
* 查看设备信息
*
* @param: 无
*
* @return: 设备信息
*/
device_info_t hal_board_info_look(void);

/**
* 保存设备信息
*
* @param: p_info: 新的设备信息的地址
*
* @return: 是否保存成功
*/
bool_t hal_board_save_device_info(uint8_t len, uint8_t *p_info);

/**
* 板级初始化，初始化系统时钟，外围设备等
*
* @param: 无
*
* @return: 无
*/
void hal_board_init(void);

/**
* 板级反初始化，反初始化系统时钟，外围设备等
*
* @param: 无
*
* @return: 无
*/
void hal_board_deinit(void);

/**
* 写入设备信息
*
* @param: p_info: 新的设备信息的地址
*
* @return: 无
*/
void hal_board_info_write(void  *p_info);

/**
* 存储设备信息
*
* @param: p_info: 新的设备信息的地址
* @param: flag: TRUE，立即写入flash；FALSE，延时写入flash
* @return: 是否保存成功
*/
bool_t hal_board_info_save(device_info_t *p_info, bool_t flag);

/**
* 延时存储设备信息
*
* @param: 无
* 
* @return: 是否保存成功
*/
bool_t hal_board_info_delay_save(void);

/**
* 获取存储设备信息
*
* @param: 无
* 
* @return: 设备信息
*/
device_info_t hal_board_info_get(void);

/**
* ascii转换为hex
*
* @param: hi: ascii高字节
* @param: lo: ascii低字节
* @param: hex: 转换为hex的指针
* @return: 转换是否成功
*/
bool_t hal_ascii_to_hex(uint8_t hi, uint8_t lo, uint8_t *hex);

/**
* hex转换为ascii
*
* @param: buf: 转换之后的数据的指针
* @param: dat: 需要转换的数据
* @return: 转换是否成功
*/
bool_t hal_hex_to_ascii(uint8_t *buf, uint8_t dat);

/**
* 存储操作模式标志
*
* @param: flag: 操作模式标志
* 
* @return: 无
*/
void operate_mode_flag_save(uint32_t flag);

/**
* 获取操作模式标志
*
* @param: 无
* 
* @return: 操作模式标志
*/
uint32_t operate_mode_flag_get(void);

/**
* 清除操作模式标志
*
* @param: 无
* 
* @return: 无
*/
void operate_mode_flag_clear(void);
#endif
