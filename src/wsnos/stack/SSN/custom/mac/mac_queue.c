/**
 * @brief       : this
 * @file        : mac_queue.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-10-26
 * change logs  :
 * Date       Version     Author        Note
 * 2016-10-26  v0.0.1  gang.cheng    first version
 */
#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include "mac_queue.h"

static list_head_t queue_immedilink_head;
static list_head_t queue_waitlink_head;

void mac_queue_init(void)
{
    list_init(&queue_immedilink_head);
    list_init(&queue_waitlink_head);
}


void mac_queue_insert(sbuf_t *sbuf)
{
    osel_int8_t s = 0;
    s = osel_mutex_lock(OSEL_MAX_PRIO);

    DBG_ASSERT(sbuf != NULL __DBG_LINE);
    if (sbuf->up_down_link == IMMEDI_LINK)
    {
        list_add_to_tail((list_head_t *)sbuf, &queue_immedilink_head);
    }
    else
    {
        list_add_to_tail((list_head_t *)sbuf, &queue_waitlink_head);
    }

    osel_mutex_unlock(s);
}

sbuf_t *mac_queue_get(uint8_t immedi_wait)
{
    sbuf_t *sbuf = NULL;
    osel_int8_t s = 0;

    s = osel_mutex_lock(OSEL_MAX_PRIO);
    if (immedi_wait == IMMEDI_LINK)
    {
        if (!list_empty(&queue_immedilink_head))
        {
            sbuf = (sbuf_t *)list_next_elem_get(&queue_immedilink_head);
        }
    }
    else
    {
        if (!list_empty(&queue_waitlink_head))
        {
            sbuf = (sbuf_t *)list_next_elem_get(&queue_waitlink_head);
        }
    }
    osel_mutex_unlock(s);

    return sbuf;
}

sbuf_t *mac_queue_find(uint8_t immedi_wait, uint32_t addr)
{
    sbuf_t *sbuf = NULL;
    osel_int8_t s = 0;

    s = osel_mutex_lock(OSEL_MAX_PRIO);
    if (immedi_wait == IMMEDI_LINK)
    {
        osel_mutex_unlock(s);
        return NULL; //*< don`t need find in immedi data queue.
    }
    else
    {
        if (!list_empty(&queue_waitlink_head))
        {
            list_entry_for_each(sbuf, &queue_waitlink_head, sbuf_t, list)
            {
                mac_prim_arg_t *mac_prim_arg =
                    &(sbuf->primargs.prim_arg.mac_prim_arg);
                if (mac_prim_arg->dst_addr == addr)
                {
                    osel_mutex_unlock(s);
                    return sbuf;
                }
            }
        }
    }

    osel_mutex_unlock(s);
    return NULL;
}

void mac_queue_start()
{

}

void mac_queue_stop()
{

}