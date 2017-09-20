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
#include "platform/WSN-JH02-ZD/driver/board.h"
#include "platform/WSN-JH02-ZD/driver/clock.h"
#include "platform/WSN-JH02-ZD/driver/gpio.h"
#include "platform/WSN-JH02-ZD/driver/timer.h"
#include "platform/WSN-JH02-ZD/driver/uart.h"
#include "platform/WSN-JH02-ZD/driver/energy.h"
#include "platform/WSN-JH02-ZD/driver/flash.h"
#include "platform/WSN-JH02-ZD/driver/spi.h"
#include "platform/WSN-JH02-ZD/driver/rtimer_arch.h"
#include "platform/WSN-JH02-ZD/driver/sx1278_arch.h"
#include "platform/WSN-JH02-ZD/driver/adxl345_arch.h"
#include "platform/WSN-JH02-ZD/driver/re46c191_arch.h"
#include "platform/WSN-JH02-ZD/driver/sht21_arch.h"
#include "platform/WSN-JH02-ZD/driver/sx1276_arch.h"
extern uint8_t volatile sst_int_nest;
#endif

