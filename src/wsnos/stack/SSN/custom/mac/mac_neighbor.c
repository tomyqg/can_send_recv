/**
 * @brief       : this
 * @file        : mac_neighbor.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-24
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-24  v0.0.1  gang.cheng    first version
 */
#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "node_cfg.h"
#include "mac_neighbor.h"

static neighbor_node_t g_nbr_table[NBR_TABLE_NUM];


void mac_neighbor_init(void)
{
    osel_memset(g_nbr_table, 0xFF,
                NBR_TABLE_NUM * sizeof(neighbor_node_t));
    for (uint8_t i = 0; i < NBR_TABLE_NUM; i++)
        g_nbr_table[i].rssi = -127;
}

/**
 * @biref 获取最新的邻居节点数
 * @return 当前有多少邻居给出应答
 */
uint8_t mac_neighbor_count(void)
{
    return 0;
}

/**
 * @brief 从邻居节点里挑选一个最好的节点：跳数、rssi、容量综合考虑
 * @return  最好节点的ID，如果0xFFFF表示没有获取到
 */
neighbor_node_t mac_neighbor_get_best(uint8_t hops)
{
    neighbor_node_t node;
    node.rssi = -127;

    uint8_t index = 0;
    int8_t rssi_max = -127;
    uint8_t hops_min = 8;

    for (uint8_t i = 0; i < NBR_TABLE_NUM; i++)
    {
        if (g_nbr_table[i].hops < hops)
            continue;
        if (g_nbr_table[i].rssi > rssi_max)
        {
            rssi_max = g_nbr_table[i].rssi;
            index = i;
        }
    }

    if (rssi_max > RSSI_QUERY_THRESHOLD)
    {
        for (uint8_t i = 0; i < NBR_TABLE_NUM; i++)
        {
            if (g_nbr_table[i].hops < hops)
                continue;
            
            if(g_nbr_table[i].rssi <= RSSI_QUERY_THRESHOLD)
                continue;
            
            if(g_nbr_table[i].hops < hops_min)
            {
                hops_min = g_nbr_table[i].hops;
                index = i;
            }
        }
    }
    else if (rssi_max > -127)
    {
        node.addr = g_nbr_table[index].addr;
        node.hops = g_nbr_table[index].hops;
    }

    node.addr = g_nbr_table[index].addr;
    node.hops = g_nbr_table[index].hops;

    return node;
}

/**
 * @brief 插入一个邻居节点信息到邻居表里
 * @param  node 要插入的邻居节点信息
 * @return      插入是否成功
 *   - 0 表示插入成功
 *   - 1 表示邻居表满，插入节点信息被丢弃
 *   - 2 表示邻居表满，插入节点信息较好，覆盖最差的邻居信息
 */
int8_t mac_neighbor_insert(neighbor_node_t node)
{
    for (uint8_t i = 0; i < NBR_TABLE_NUM; i++)
    {
        if (g_nbr_table[i].addr == node.addr)
        {
            g_nbr_table[i].rssi = node.rssi;
            g_nbr_table[i].hops = node.hops;
            return 0;
        }
    }

    for (uint8_t i = 0; i < NBR_TABLE_NUM; i++)
    {
        if (g_nbr_table[i].addr == 0xFFFFFFFFu)
        {
            g_nbr_table[i].addr = node.addr;
            g_nbr_table[i].rssi = node.rssi;
            g_nbr_table[i].hops = node.hops;
            return 2;
        }
    }

    return 1;
}


void mac_neighbor_start(void)
{
    ;
}

/*
 *@brief 邻居表模块停止
 */
void mac_neighbor_stop(void)
{
    mac_neighbor_init();
}

