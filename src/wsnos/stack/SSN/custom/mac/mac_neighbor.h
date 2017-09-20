#ifndef __MAC_NEIGHBOR_H__
#define __MAC_NEIGHBOR_H__

#define NEIGHBOR_NONE_ADDR      (0xFFFFFFFF)

typedef struct
{
    uint32_t addr;
    int8_t rssi;
    uint8_t hops;
} neighbor_node_t;


#define NBR_TABLE_NUM               (20u)



/**
 * @brief 初始化邻居模块
 */
void mac_neighbor_init(void);

/**
 * @biref 获取最新的邻居节点数
 * @return 当前有多少邻居给出应答
 */
uint8_t mac_neighbor_count(void);

/**
 * @brief 从邻居节点里挑选一个最好的节点：跳数、rssi、容量综合考虑
 * @return  最好节点的ID，如果0xFFFF表示没有获取到
 */
neighbor_node_t mac_neighbor_get_best(uint8_t hops);

/**
 * @brief 插入一个邻居节点信息到邻居表里
 * @param  node 要插入的邻居节点信息
 * @return      插入是否成功
 *   - 0 表示插入成功
 *   - 1 表示邻居表满，插入节点信息被丢弃
 *   - 2 表示邻居表满，插入节点信息较好，覆盖最差的邻居信息
 */
int8_t mac_neighbor_insert(neighbor_node_t node);


/**
 * @brief 邻居表模块启动
 */
void mac_neighbor_start(void);

/*
 *@brief 邻居表模块停止
 */
void mac_neighbor_stop(void);

#endif
