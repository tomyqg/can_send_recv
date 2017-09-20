/**
* @brief       :
*
* @file        : stack.c
* @author      : gang.cheng
* @version     : v0.0.1
* @date        : 2015/5/7
*
* Change Logs  :
*
* Date        Version      Author      Notes
* 2015/5/7    v0.0.1      gang.cheng    first version
*/

#include "common/lib/lib.h"
#include "common/hal/hal.h"
#include "common/dev/dev.h"

#include "stack/stack.h"

#include "./common/pbuf.h"
#include "./common/sbuf.h"
#include "./common/prim.h"

#include "./module/mac_module/mac_module.h"

#include "./custom/mac/mac.h"
#include "./custom/nwk/nwk_interface.h"

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

    osel_idle_hook(sys_enter_lpm_handler);

    mac_pib_t mac_pib_temp;
    //读取当前配置信息
    device_info_t dev_info = hal_board_info_look();

    if (dev_info.device_id == 0xFFFFFFFF)//*<读取不成功包括：设备ID未配置和入参为NULL
    {
//        return;
    }
    else
    {
        mac_pib_temp.rf_channel    = dev_info.rf_channel;
        mac_pib_temp.rf_power      = dev_info.rf_power;
        mac_pib_temp.mac_src_addr  = dev_info.device_id;
        mac_pib_temp.mac_self_addr = dev_info.unique_id;
    }
    mac_pib_temp.mac_seq_num = 0;
    mac_pib_temp.mac_hops    = 0;

    SSN_RADIO.init();

    SSN_RADIO.set_value(RF_CHANNEL, mac_pib_temp.rf_channel);
    SSN_RADIO.set_value(RF_POWER, mac_pib_temp.rf_power);
    //@TODO 添加mac注册、启动、关闭接口

    mac_init(&mac_pib_temp);

    nwk_init();

    mac_run();
    nwk_run();
}
