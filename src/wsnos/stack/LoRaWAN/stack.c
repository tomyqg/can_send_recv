/**
 * @brief       : this
 * @file        : stack.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017/2/15
 * change logs  :
 * Date       Version     Author        Note
 * 2017/2/15  v0.0.1  gang.cheng    first version
 */
#include "platform/platform.h"

#include "stack/stack.h"
#include "./common/pbuf.h"
#include "./common/sbuf.h"
#include "./common/prim.h"

#include "./mac/LoRaMac_classA.h"
#include "./mac/LoRaMac_classC.h"

#include "sys_arch/osel_arch.h"

static void sys_enter_lpm_handler(void *p)
{
    debug_info_printf(); // 延时打印日志信息
    lpm_arch();
}

void stack_init(void)
{
    pbuf_init();
    bool_t res = sbuf_init();
    DBG_ASSERT(res != FALSE __DBG_LINE);

#ifndef PF_CORE_POSIX
    osel_idle_hook(sys_enter_lpm_handler);
#endif

    mac_init(NULL);
}

void stack_run(void)
{
    mac_class_run();
}

void stack_stop(void)
{
    mac_class_stop();
}

void stack_dependent_cfg(const stack_callback_t *callbacks)
{
    mac_class_dependent_cfg(callbacks);
}

/**
 * @brief 无线协议栈通用发送数据接口
 * @param flag   定义数据发送类型
 * @param seq    当前发送数据的序列号，作为数据记录，不放入载荷
 * @param buffer 要发送数据的地址
 * @param len    要发送数据的长度
 */
int8_t stack_send(msg_flag_t flag, mbuf_t *mbuf)
{
    int8_t res = 0;

    res = mac_class_send(flag, mbuf);

    return res;
}
