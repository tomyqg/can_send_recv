#include "common/lib/lib.h"
#include "stdlib.h"
DBG_THIS_MODULE("demo_clist")

CLIST(my_list);

typedef union
{
    struct clist *next;
    uint8_t value;
    struct
    {
        uint8_t Tx              : 1;
        uint8_t Rx              : 1;
        uint8_t Type          : 1;
    } Bits;
} FieldFlags_t;

typedef struct
{
    FieldFlags_t Field;
    uint8_t index;
} node_t;

/**
 * [demo_list_clear 链表数据清除]
 */
void demo_list_clear(void)
{
    int num = clist_length(my_list);
    for (int i = 0; i < num; i++)
    {
        node_t  *node = clist_remove_head(my_list);
        free(node);     //这里为了方便使用系统函数
    }
}

/**
 * [demo_list_add_head 往链表尾添加测试数据]
 * @param num [测试数据个数]
 */
void demo_list_add_tail(uint8_t num)
{
    for (int i = 0; i < num; i++)
    {
        node_t *node = (node_t* )malloc(sizeof(node_t));  //可以使用mpool_alloc中来申请内存，这里为了方便使用系统函数
        node->index = i;
        clist_add_tail(my_list, node);
    }
    //验证下队列中数据个数
    DBG_LOG(DBG_LEVEL_INFO, "list length = %d\r\n", clist_length(my_list));
}

void demo_list_foreach(void)
{
    node_t *node = NULL;
    CLIST_FOR_EACH(my_list, node)
    {
        if (node != NULL)
        {
            DBG_LOG(DBG_LEVEL_INFO, "%d\r\n", node->index);
        }
    }
}

void demo_list_remove()
{
    node_t * node = NULL;
    //从链表头移除一个对象
    node = clist_remove_head(my_list);
    free(node);     //这里为了方便使用系统函数
    demo_list_foreach();    //打印链表中剩余对象

    //从链表尾移除一个对象
    node = clist_remove_tail(my_list);
    free(node);     //这里为了方便使用系统函数
    demo_list_foreach();    //打印链表中剩余对象
}

void simple_clist(void)
{
    clist_init(my_list);

    demo_list_add_tail(10);

    demo_list_foreach();

    demo_list_remove();

    demo_list_clear();
}