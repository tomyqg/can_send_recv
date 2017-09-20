/**
 * @brief       : 配置模块的处理
 *
 * @file        : app_cfg.c
 * @author      : zhangzhan
 * @version     : v0.0.1
 * @date        : 2016/4/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2016/4/7    v0.0.1      zhangzhan    some notes
 */
#include "common/lib/lib.h"
#include "common/hal/hal.h"
#include "sys_arch/osel_arch.h"
#include "app_module_cfg.h"
     
static app_cfg_info_t device_local_info;

DBG_THIS_MODULE("app_module_cfg");

/**
 * @brief 从ram获取配置信息
*  @param[out] 配置参数ID、信道、功率等
 */
bool_t config_module_read(app_cfg_info_t *device_info)
{
    bool_t result = TRUE;

    if (NULL == device_info)
    {
        result = FALSE;
        DBG_LOG(DBG_LEVEL_ERROR, "app_cfg read args is NULL! \r\n");
    }
    else
    {  
        osel_memcpy(device_info, &device_local_info, sizeof(app_cfg_info_t));
        if (device_info->unique_id == 0xFFFFFFFFFFFFFFFF)
        {
            result = FALSE;
            DBG_LOG(DBG_LEVEL_ERROR, "device id  is 0xfffffffff! \r\n");
        }
    }

    return result;
}


/**
 * @brief 配置信息写入到ram
*  @param[in] type 配置参数ID、信道、功率等
 */
bool_t config_module_write(const app_cfg_info_t *device_info)
{
    bool_t result = TRUE;

    if (NULL == device_info)
    {
        result = FALSE;
        DBG_LOG(DBG_LEVEL_ERROR, "app_cfg wrtite args is NULL! \r\n");
    }
    else
    {
        result = TRUE;
        osel_memcpy(&device_local_info, device_info, sizeof(app_cfg_info_t));
    }

    return result;
}

/**
 * @brief 把ram的缓存数据写入到flash
*  @param[in] 空
 */
void config_module_save(void)
{
    osel_uint32_t seg_addr = FLASH_PAGE_ADDR_DEV_INFO;
    uint8_t *buffer = (uint8_t *)&device_local_info;
    hal_nvmem_erase((uint8_t *)seg_addr, FLASH_SEG_ERASE);
    hal_nvmem_write((uint8_t *)seg_addr, buffer, sizeof(app_cfg_info_t));
}

/**
 * @brief 配置模块初始化，从芯片的flash里读取信息到ram里
 *  @param[in] 空
 */
void config_module_init(void)
{
    hal_nvmem_read((uint8_t *)FLASH_PAGE_ADDR_DEV_INFO,
                   (uint8_t *)&device_local_info, sizeof(app_cfg_info_t));
}