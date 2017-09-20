/**
 * @brief       : 实现一个内存池，里面存放固定空间的内存块，用户从这里申请，并
 * 保存数据，处理释放。
 * @file        : mpool.c
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2017/2/22
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2017/2/22   v0.0.1      gang.cheng    first version
 */
#include "lib.h"
#include "mpool.h"

#define MPOOL_DATA_SIZE(mpool)              (mpool->end - mpool->head)


int8_t mpool_init(mpool_manage_t *manage, uint16_t p_num, uint16_t p_size)
{
    DBG_ASSERT(manage != NULL __DBG_LINE);
    if(manage == NULL )
        return -1;

    uint8_t *mblock_ptr;

    list_init(&(manage->free_list));
#if MPOOL_DBG_EN > 0
    list_init(&(manage->used_list));
#endif

    manage->p_size = OSEL_ALIGN(p_size, OSEL_MEM_ALIGNMENT);
    manage->m_size = manage->p_size + OSEL_ALIGN(sizeof(mpool_t), OSEL_MEM_ALIGNMENT);
    mblock_ptr = osel_mem_alloc(manage->p_size * p_num);
    DBG_ASSERT(NULL != mblock_ptr __DBG_LINE);
    if (mblock_ptr == NULL)
        return -1;

    manage->p_num  = p_num;
    manage->p_free = p_num;
    manage->p_used = 0;

    for (register uint16_t i = 0; i < p_num; i++)
    {
        list_add_to_tail(&((mpool_t *)(mblock_ptr + i * manage->m_size))->list,
                         &(manage->free_list));
        ((mpool_t *)(mblock_ptr + i * manage->m_size))->parent = (void *)manage;
#if MPOOL_DBG_EN > 0
        ((mpool_t *)(mblock_ptr + i * manage->m_size))->alloc_line = 0;
        ((mpool_t *)(mblock_ptr + i * manage->m_size))->free_line  = 0;
#endif
    }

    manage->m_ptr = mblock_ptr;

    return 0;
}


void *mpool_alloc(mpool_manage_t *manage, uint16_t size _MLINE_)
{
    if (size > manage->p_size) 
    {
        return NULL;
    }

    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    mpool_t *mpool = list_entry_decap(&(manage->free_list), mpool_t, list);
    OSEL_EXIT_CRITICAL(s);

    if (mpool != NULL)
    {
#if MPOOL_DBG_EN > 0
        OSEL_ENTER_CRITICAL(s);
        list_add_to_tail(&mpool->list, &manage->used_list);
        mpool->alloc_line = line;
        mpool->free_line  = 0;
        OSEL_EXIT_CRITICAL(s);
#endif

        manage->p_free--;
        manage->p_used++;

        return (void *)((uint8_t *)mpool + sizeof(mpool_t));   //*< 这里用uint32_t修饰mpool会导致64位指针被修改
    }

    return NULL;
}

void mpool_free(void **ptr _MLINE_)
{
    if(*ptr == NULL)
        return;
    
    mpool_t *mpool = (mpool_t *)((ptr_t)*ptr - sizeof(mpool_t));
    mpool_manage_t *manage = (mpool_manage_t *)mpool->parent;
    
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
#if MPOOL_DBG_EN > 0
    list_del(&mpool->list);
    mpool->free_line = line;
#endif
    manage->p_used--;
    manage->p_free++;

    list_init(&mpool->list);
    list_add_to_tail(&(mpool->list), &manage->free_list);
    OSEL_EXIT_CRITICAL(s);

    osel_memset(*ptr, 0x00, manage->p_size);
    *ptr = NULL;
}

