/**
 * @brief       : 配置模块中用到的接口函数和数据结构
 *
 * @file        : app_cfg.h
 * @author      : zhangzhan
 * @version     : v0.0.1
 * @date        : 2016/4/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2016/4/7    v0.0.1      zhangzhan    some notes
 */
#ifndef __APP_CFG_H_
#define __APP_CFG_H_

#define FLASH_PAGE_ADDR_DEV_INFO        (DEVICE_INFO_ADDR_1)
     
#pragma pack(1)
     
/**
 * @brief   定义了配置的数据结构
 */
typedef struct
{
    uint64_t  unique_id;            //*< 设备唯一标识ID
    uint32_t  device_id ;           //*< 设备ID
    uint8_t   rf_channel;           //*< rf信道
    uint8_t   rf_power;             //*< 信道功率
    uint8_t   speed_or_lane;        //*< 检测器的算法输出时速度还是流量
} app_cfg_info_t;

#pragma pack()

/**
 * @brief 配置类型
 */
typedef enum
{
    INFO_TYPE_ID,                       //*< 设备 ID
    INFO_TYPE_RF_CH,                    //*< rf信道
    INFO_TYPE_RF_POWER,                 //*< rf功率
    INFO_TYPE_RESERVE,                  //*< 预留
} info_type_e;

/**
 * @brief 配置模块初始化，从芯片的flash里读取信息到ram里
 *  @param[in] 空
 */
void config_module_init(void);

/**
 * @brief 从ram获取配置信息
*  @param[out] 配置参数ID、信道、功率等
 */
bool_t config_module_read(app_cfg_info_t *device_info);

/**
 * @brief 配置信息写入到ram
*  @param[in] type 配置参数ID、信道、功率等
 */
bool_t config_module_write(const app_cfg_info_t *device_info);

/**
 * @brief 把ram的缓存数据写入到flash
*  @param[in] 空
 */
void config_module_save(void);

#endif