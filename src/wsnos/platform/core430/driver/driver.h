/**
 * @brief       : 
 *
 * @file        : driver.h
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2015/5/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */

#ifndef __DRIVER_H
#define __DRIVER_H

#include "node_cfg.h"

#define SMCLK           8000000

#define ACLK            32768

#define CPU_F           ((double)SMCLK)
#define delay_us(x)     __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x)     __delay_cycles((long)(CPU_F*(double)x/1000.0))

#include <msp430.h>
#include "common/lib/lib.h"
#include "platform/core430/driver/board.h"
#include "platform/core430/driver/clock.h"
#include "platform/core430/driver/gpio.h"
#include "platform/core430/driver/timer.h"
#include "platform/core430/driver/uart.h"
#include "platform/core430/driver/sensor.h"
#include "platform/core430/driver/energy.h"
#include "platform/core430/driver/flash.h"
#include "platform/core430/driver/spi.h"
#include "platform/core430/driver/rtimer_arch.h"
#include "platform/core430/driver/rm3000_arch.h"
#include "platform/core430/driver/sx1276_arch.h"
#include "platform/core430/driver/adxl345_arch.h"

extern uint8_t volatile sst_int_nest;
#endif

