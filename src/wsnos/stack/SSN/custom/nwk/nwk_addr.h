/**
 * @brief       : 
 *
 * @file        : nwk_addr.h
 * @author      : WangJifang
 * @version     : v0.0.1
 * @date        : 2015/12/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/12/7    v0.0.1      WangJifang    some notes
 */
#include "common/lib/lib.h"
#include "nwk_frames.h"
#define ROUTE_NUM                                           (4u)
#define DETEC_NUM                                           (15)


#ifndef PF_CORE_POSIX
#define UNDEFINE_NWK_ADDR       0xFFFFu
#define UNDEFINE_NUI            0xFFFFFFFFFFFFFFFFull
#else
#define UNDEFINE_NWK_ADDR       0xFFFFu
#define UNDEFINE_NUI            0xFFFFFFFFFFFFFFFFull
#endif

/**
 * @brief 地址表初始化
 * @param[in] void
 * @return void
 */
void nwk_addr_table_init(void);

/**
 * @brief 地址表删除对应的地址，回收资源
 * @param[in] nwk_addr 要回收的网络地址
 * @return 更新成功或失败
 *  - true 回收成功
 *  - false 回收失败
 */
bool_t nwk_addr_del(uint16_t nwk_addr);

/**
 * @brief 地址表添加对应的地址
 * @param[in] nwk_addr 要添加的设备信息
 * @return 更新成功或失败
 *  - true节点更新成功
 *  - false 更新失败
 */
bool_t nwk_addr_table_add(nwk_join_req_t nwk_addr);

/**
 * @brief 查找地址表对应的网络地址
 * @param[in] nwk_join_req 要查找的设备信息
 * @return 查找结果
 *  - UNDEFINE_NWK_ADDR 查找失败
 */
uint16_t nwk_addr_short_get(nwk_join_req_t *nwk_join_req);

bool_t nwk_addr_short_find(uint64_t nui, uint16_t *nwk_addr);

/**
 * @brief find long id from nwk addr.
 * @param  nwk_addr [description]
 * @return          [description]
 */
bool_t nwk_addr_long_get(uint16_t nwk_addr, uint64_t *nui);


void nwk_addr_table_del(void);

/**
 * @brief save 
 * @return  [description]
 */
void nwk_addr_table_save(void);
