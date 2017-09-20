#ifndef __MAC_ASSOC_TABLE_H__
#define __MAC_ASSOC_TABLE_H__

#include "common/lib/lib.h"

#define ASSOC_NONE_ADDR             (0xFFFFFFFF)

#define ASSOC_TABLE_NUM             (20u)

typedef struct
{
    uint32_t node_id;
    // uint64_t unique_id; //*< big endcode
    // uint16_t direct_conn;
} assoc_node_t;

// typedef struct 
// {
//     uint64_t unique_id;
//     uint16_t direct_conn;
// } flash_node_t;

typedef enum
{
    ASSOC_TABLE_NONE,   //*< 当前关联表无效
    ASSOC_TABLE_VAILED, //*< 当前关联设备在表里
    ASSOC_TABLE_FAILED, //*< 当前关联设备部在表里
} assoc_table_result_t;


/**
 * @brief 用于检查关联表的接口，如果没有配置关联表，返回None，表示关联表无效，不
 * 进行地址校验；如果配置了关联表，需要对地址进行校验；
 * @param  node 需要进行地址校验的节点信息
 * @return      关联表校验结果
 */
assoc_table_result_t mac_assoc_table_check(assoc_node_t *node);

/**
 * @biref app set this status. when flash get node info. 
 * @param status [description]
 */
void mac_assoc_status_set(uint8_t status);

/**
 * @brief 插入需要保存的关联设备信息
 * @param node 保存关联的设备信息
 * @return True，插入成功；FALSE 插入表满；
 */
bool_t mac_assoc_table_insert(assoc_node_t *node);

/**
 * @brief 把关联表的信息保存到Flash里，并复位
 */
void mac_assoc_table_save(void);

/**
 * @brief clean assoc table of flash addr.
 */
void mac_assoc_table_del(void);

/**
 * @brief 从flash里面读取关联信息到RAM里
 */
void mac_assoc_table_init(void);

/**
 * @biref 查找某个节点是否在关联设备表里面
 * @param  node 保存关联的设备信息
 * @return TRUE 找到； FALSE 没找到
 */
bool_t mac_assoc_table_find(assoc_node_t *node);





#endif
