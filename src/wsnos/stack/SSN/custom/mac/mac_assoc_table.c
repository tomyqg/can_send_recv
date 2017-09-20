/**
 * @brief       : this
 * @file        : mac_assoc_table.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-21
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-21  v0.0.1  gang.cheng    first version
 */

#include "mac_assoc_table.h"

static assoc_node_t g_aoc_table[ASSOC_TABLE_NUM];
static bool_t flash_table_flag = FALSE;

assoc_table_result_t mac_assoc_table_check(assoc_node_t *node)
{
    if (flash_table_flag == FALSE)
        return ASSOC_TABLE_NONE;

    if (node->node_id == 0xFFFFFFFFu)
        return ASSOC_TABLE_VAILED;

    if (mac_assoc_table_find(node))
        return ASSOC_TABLE_VAILED;

    return ASSOC_TABLE_FAILED;
}

void mac_assoc_status_set(uint8_t status)
{
#ifdef PF_CORE_POSIX
    printf("mac_assoc_status_set:%d\r\n", status);
#endif
    flash_table_flag = status;
}

bool_t mac_assoc_table_insert(assoc_node_t *node)
{
    osel_int8_t s = 0;
    for (uint8_t i = 0; i < ASSOC_TABLE_NUM; i++)
    {
        if (g_aoc_table[i].node_id == node->node_id)
        {
            return TRUE;
        }
    }

    for (uint8_t i = 0; i < ASSOC_TABLE_NUM; i++)
    {
        if (g_aoc_table[i].node_id == 0xFFFFFFFFu)
        {
            s = osel_mutex_lock(OSEL_MAX_PRIO);
            g_aoc_table[i].node_id = node->node_id;
            osel_mutex_unlock(s);
            return TRUE;
        }
    }

    return FALSE;
}

void mac_assoc_table_save(void)
{
    //*< save ram to flash.
}

void mac_assoc_table_del(void)
{
    ;
}

void mac_assoc_table_init(void)
{
    flash_table_flag = FALSE;
    osel_memset(g_aoc_table, 0xFF, ASSOC_TABLE_NUM * sizeof(assoc_node_t));
    //*< TODO: reload from flash to ram.
}

/**
 * @brief find node in my assoc table. if connected one step it will find.
 * or it will be false.
 * @param  node [description]
 * @return      [description]
 */
bool_t mac_assoc_table_find(assoc_node_t *node)
{
    if (flash_table_flag == FALSE)
        return TRUE;
    
    for (uint8_t i = 0; i < ASSOC_TABLE_NUM; i++)
    {
        if (g_aoc_table[i].node_id == node->node_id)
        {
            return TRUE;
        }
    }

    // return TRUE;
    return FALSE;
}