/**
 * @brief       :
 *
 * @file        : stack.h
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 * 2017/2/15   v0.0.2      gang.cheng   modify interface.
 */

#ifndef __STACK_H
#define __STACK_H


typedef enum
{
    MSG_UNCONFIRMED,        //*< 不需要确认
    MSG_CONFIRMED,          //*< 需要确认
    MSG_MULTICAST,          //*< 广播，暂不支持
    MSG_PROPRIETARY,        //*< 优先级高，可以插入发送队列前，暂不支持
} msg_flag_t;

/**
 * @brief 用于存储stack的数据结构，需要单独模块实现
 */
typedef struct
{
    list_head_t list;

    uint32_t seq;
    uint8_t port;
    msg_flag_t flag;

    uint8_t *data_p;
    uint8_t *head;
    uint8_t *end;
    uint8_t data_len;
} mbuf_t;

/**
 * @brief 协议栈的回调数据结构，用于用户与协议栈之间进行双向交互
 */
typedef struct
{
    //!< 发送数据后返回确认的回调函数
    void (*tx_confirm) (bool_t res, uint32_t seq, void *buffer, uint8_t len);

    //!< 收到数据后通知APP的回调函数
    void (*rx_indicate) (const void *buffer, uint8_t len);

    //*< 入网状态指示
    void (*join_confirm)(bool_t res);
} stack_callback_t;

/**
 * @biref 当前协议栈初始化，由main调用，用户不需要初始化这部分
 */
void stack_init(void);

/**
 * @brief 启动当前协议栈
 */
void stack_run(void);

/**
 * @brief 停止当前协议栈
 */
void stack_stop(void);

/**
 * @brief 定义协议栈的回调接口
 * @param callbacks 注册的回调实体
 *
 * @code
 * void app_init(void)
 * {
 *     ...
 *     stack_callback_t app_cbs;
 *     app_cbs.tx_confirm   = &app_tx_confirm;
 *     app_cbs.rx_indicate  = &app_rx_indicate;
 *     app_cbs.join_confirm = &app_join_confirm;
 *     stack_dependent_cfg(&app_cbs);
 *     ...
 * }
 * @endcode
 */
void stack_dependent_cfg(const stack_callback_t *callbacks);

/**
 * @brief 无线协议栈通用发送数据接口
 * @param flag   定义数据发送类型
 * @param seq    当前发送数据的序列号，作为数据记录，不放入载荷
 * @param buffer 要发送数据的地址
 * @param len    要发送数据的长度
 * @return
 *    - 0   数据写入缓冲队列
 *    - -1  数据超过发送最大长度
 *    - -2  当前网络未入网
 *    - -3  当前正在发送数据，无法发送
 *
 * @code
 * ...
 * static int8_t send_res = 0;
 * send_res = stack_send(MSG_UNCONFIRMED, &app_mbuf);
 * if(send_res == -1)
 * {
 *     ;   //!< 数据超过发送最大长度
 * }
 * else if(send_res == -2)
 * {
 *     ;   //!< 当前网络还未入网
 * }
 * else if(send_res == -3)
 * {
 *     ;   //!< 当前正在发送数据，无法发送，请主动退避
 * }
 * ...
 * 
 * @endcode
 */
int8_t stack_send(msg_flag_t flag, mbuf_t *mbuf);

/**
 * @biref 设置无线协议栈应用层数据发送、接收端口
 * @param port 应用端口号
 */
void stack_set_port(uint8_t port);

/**
 * @}
 */
#endif
