/**
 * @brief       : 
 *
 * @file        : platform.h
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

/** 需要定义平台
	PF_CORE_M3
	PF_ITCS_AP
	PF_ITS_RP
	PF_SEN_COMM
    PF_CORE_POSIX
    PF_CORE_STM32F103
    PF_WSPR_SPV1
**/

#ifdef PF_CORE_M3
#include "platform/stm32l/drivers/driver.h"
#elif PF_CORE_430
#include "platform/core430/driver/driver.h"
#elif PF_CORE_POSIX
#include "platform/posix/driver/driver.h"
#elif PF_WSPR_SPV1
#include "platform/SensingPalletV1.0.0/drivers/driver.h"
#elif PF_CORE_STM32F103
#include "platform/stm32f103/drivers/driver.h"
#elif PF_CORE_PF_CORE_WSN_JH02_ZD
#include "platform/WSN-JH02-ZD/driver/driver.h"
#else
#error "need define NODE_PF in proj defined symbols"
#endif



#endif
