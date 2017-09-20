#include "common/lib/lib.h"
DBG_THIS_MODULE("simple_sqqueue")

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t buf[40];
} element_t;

sqqueue_ctrl_t queue;   //创建队列对象

void traverse_cb(const void *e)
{
    element_t *p = (element_t* )e;
    DBG_LOG(DBG_LEVEL_INFO, "%d,%d\n",     p->x, p->y);
}

/**
 * [demo_traverse 遍历队列打印]
 */
void demo_traverse()
{
    queue.traverse(&queue, traverse_cb);
}

/**
 * [demo_enter 往队列里添加对象]
 * @param num [队列长度]
 */
void demo_enter(uint8_t num)
{
    DBG_LOG(DBG_LEVEL_INFO, "创建长度:%d\n",  num);
    for (int i = 1; i <= num; i++)
    {
        element_t element;
        element.x = i * 10;
        element.y = i * 10;
        queue.enter(&queue, &element);  //将成员插入到队列中
    }
    DBG_LOG(DBG_LEVEL_INFO, "队列长度:%d，队列满：%d\n",  queue.get_len(&queue), queue.full(&queue));
    DBG_LOG(DBG_LEVEL_INFO, "队列剩余空间:%d\n",    queue.sqq.sqq_len - 1 - queue.get_len(&queue) );
}

/**
 * [demo_remove_element 测试队列删除功能]
 */
void demo_remove_element(void)
{
    DBG_LOG(DBG_LEVEL_INFO, "队列长度:%d,测试移除首元素\n",  queue.get_len(&queue));
    queue.del(&queue);
    demo_traverse();
    DBG_LOG(DBG_LEVEL_INFO, "队列剩余空间:%d\n",    queue.sqq.sqq_len - 1 - queue.get_len(&queue) );

    DBG_LOG(DBG_LEVEL_INFO, "队列长度:%d,测试移除尾元素\n",  queue.get_len(&queue));
    queue.revoke(&queue);
    demo_traverse();
    DBG_LOG(DBG_LEVEL_INFO, "队列剩余空间:%d\n",    queue.sqq.sqq_len - 1 - queue.get_len(&queue) );

    DBG_LOG(DBG_LEVEL_INFO, "队列长度:%d,删除相对队头指定偏移位置的元素：3\n",  queue.get_len(&queue));
    queue.remove(&queue, 3);
    demo_traverse();
    DBG_LOG(DBG_LEVEL_INFO, "队列剩余空间:%d\n",    queue.sqq.sqq_len - 1 - queue.get_len(&queue) );
}

void simple_sqqueue(void)
{
    int size = 10;
    if (FALSE == sqqueue_ctrl_init(&queue, sizeof(element_t), size))    //队列长度要比实际创建的长度-1
    {
        DBG_LOG(DBG_LEVEL_INFO, "队列初始化失败\r\n");
        return;
    }
    else
        DBG_LOG(DBG_LEVEL_INFO, "队列初始化成功，容量:%d\r\n", size);

    demo_enter(11);
    demo_traverse();
    queue.clear_sqq(&queue);

    demo_enter(8);
    demo_traverse();

    demo_remove_element();
}
