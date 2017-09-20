/**
 * @brief       : 
 *
 * @file        : wsnos.h
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2015/9/30
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/9/30    v0.0.1      gang.cheng    first version
 */

#ifndef __WSNOS_H__
#define __WSNOS_H__

#include "wsnos_config.h"

#if defined(PF_CORE_M3) || defined(PF_WSPR_SPV1)
#include "sys_arch/ports/stm32l/wsnos_port.h"
#elif defined(PF_CORE_STM32F103)
#include "sys_arch/ports/stm32f1/wsnos_port.h"
#elif PF_CORE_POSIX
#include "sys_arch/ports/posix/wsnos_port.h"
#elif PF_CORE_430
#include "sys_arch/ports/msp430/wsnos_port.h"
#elif PF_CORE_PF_CORE_WSN_JH02_ZD
#include "sys_arch/ports/msp430/wsnos_port.h"
#else
#error "please define PF in wsnos.h"
#endif

#include "sys_arch/wsnos/wsnos_default.h"


#include "sys_arch/wsnos/wsnos_equeue.h"
#include "sys_arch/wsnos/wsnos_mem.h"
#include "sys_arch/wsnos/wsnos_task.h"
#include "sys_arch/wsnos/wsnos_pthread.h"
#include "sys_arch/wsnos/wsnos_etimer.h"
#include "sys_arch/wsnos/wsnos_sched.h"
#include "sys_arch/wsnos/wsnos_device.h"

/**
 * @brief 用于os内部的事件定时器、设备驱动注册线程框架使用
 */
extern osel_task_t *os_task_tcb; 

/** 
 * @brief 该接口实现了os的环境初始化，定义了系统堆空间，系统支持的最大优先级数；
 * @param[in] buf 系统堆空间的起始地址
 * @param[in] size 系统堆空间的大小
 * @param[in] max_prio 系统支持的最大优先级数
 * @code
 * static uint8_t osel_heap_buf[4096];
 * ...
 * int main(void)
 * {
 *     osel_env_init(osel_heap_buf, 4096, 8);
 *     ...
 *     osel_run();
 * } 
 * @endcode
 */
void osel_env_init(osel_uint8_t *buf, 
                   osel_uint16_t size,
                   osel_uint8_t max_prio);
 
#endif

