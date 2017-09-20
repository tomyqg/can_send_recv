/***************************************************************************
* File        : app_module_alg.h
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/11/4
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/11/4         v0.1            wangjifang        first version
***************************************************************************/

#ifndef __APP_MODULE_ALG_H
#define __APP_MODULE_ALG_H

#pragma pack(1)

/**
 * @brief 加速度原始数据结构体
 */
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} acc_data_t;

typedef struct _feature_event_t_
{
    uint16_t energy;
    uint16_t ez_max0;
    uint16_t ez_max8;
    uint16_t ez_max16;
    uint16_t ez_max32;
    uint16_t ez_max64;
    uint16_t ez_max128;
    uint16_t ez_max256;
    uint16_t ez_max512;
    uint16_t res1;
    uint16_t res2;
} feature_event_t;

  
typedef struct _alg_detect_event_t_
{
    uint8_t type;                       //报警类型 0无报警，1有报警
    feature_event_t var_val;            //特征值
} alg_event_t;

typedef void (*alg_event_callback_t)(alg_event_t *motion_detect_event);

/**
 * @brief 算法的初始化接口
 * @param alg_event_cb 算法事件响应回调函数
 */
void alg_init(alg_event_callback_t alg_event_cb);

/**
 * @brief 算法的参数配置接口
 * @param feature_para_cfg 算法参数 
 */
void alg_para_cfg(feature_event_t feature_para_cfg);

/**
 * @brief 算法的数据输入接口，周期输入加速度原始数据
 * @param acc_origin_data 原始数据的结构体
 */
void alg_origin_data_input(acc_data_t acc_origin_data);
#pragma pack()
#endif

