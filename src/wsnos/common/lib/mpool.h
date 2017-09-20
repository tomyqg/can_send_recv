/**
 * @brief       : create a buffer segment to temporarily store data that will
 *                sending & recieving. user can used it to store application data.
 * @file        : mpool.h
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2017/2/20
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2017/2/20   v0.0.1      gang.cheng    first version
 */
#ifndef __MPOOL_H__
#define __MPOOL_H__

#include "list.h"

#define MPOOL_DBG_EN                        (0u)

#if MPOOL_DBG_EN > 0
/*形参*/
#define _MLINE_  ,uint16_t line
/*实参*/
#define __MLINE  ,__LINE__

#else

#define _MLINE_
#define __MLINE

#endif

/**
 * @brief 用于存储stack的数据结构，需要单独模块实现
 */
typedef struct
{
    struct list_head list;
    void *parent;
#if MPOOL_DBG_EN > 0
    uint16_t alloc_line;
    uint16_t free_line;
#endif
} mpool_t;

typedef struct
{
    list_head_t free_list;      //*< 内存池空闲链表头

#if MPOOL_DBG_EN > 0
    list_head_t used_list;      //*< 内存池使用链表头
#endif

    uint8_t *m_ptr;             //*< 内存池申请空间起始地址

    uint16_t p_num;             //*< 内存池的最大内存块数
    uint16_t p_free;            //*< 内存池当前剩余空闲内存块
    uint16_t p_used;            //*< 内存池当前使用内存块
    
    uint16_t p_size;            //*< 内存池里单个内存块大小
    uint16_t m_size;            //*< 内存池里内存块以及控制块的大小
} mpool_manage_t;

/**
 * @biref 初始化内存池
 *
 * @param[in] manage 保存内存池的控制块
 * @param[in] p_num 内存池有多少内存块
 * @param[in] p_size 每个内存块可用内存空间大小
 *
 * @return 返回内存池初始化结果  TRUE 成功，FALSE 失败
 */
int8_t mpool_init(mpool_manage_t *manage, uint16_t p_num, uint16_t p_size);

/**
 * @brief 从内存池中申请一块空闲内存块
 *
 * @param[in] manage 指定申请内存快的内存池
 * @param[in] size 申请的内存块空间大小
 * @param[in]_MLINE_ 调用mpool_alloc()位置的行号，调试模式调用时传入实参形式__MLINE
 *
 * @return 返回申请的内存块地址
 *   - NULL 内存块申请不到，申请空间超过每个内存块上限或者内存块已经占用满
 *   - !NULL 申请到的内存地址空间
 */
void *mpool_alloc(mpool_manage_t *manage, uint16_t size _MLINE_);

/**
 * @brief 释放已经使用完的pool
 *
 * @param[in] ptr 指向需要操作的指针的指针
 * @param[in] _MLINE_ 调用mpool_free()位置的行号，调试模式调用时传入实参形式__MLINE
 */
void mpool_free(void **ptr _MLINE_);





#endif
