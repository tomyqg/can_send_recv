/**
 * @brief       :
 *
 * @file        : dev.h
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2015/10/20
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/10/20    v0.0.1      gang.cheng    first version
 */
#ifndef __DEV_H__
#define __DEV_H__

#include "common/lib/lib.h"

#ifdef SILABS_RADIO_SI443X
#include "si4432/radio.h"
#elif SILABS_RADIO_SI446X
#include "si4463/radio.h"
#elif LORA_RADIO_SX127X
#ifdef USE_SSN
#include "sx1278/radio.h"
#endif
#elif SIM_RADIO
#include "sim_dev/radio.h"
#else
#error "must define radio_type in node_cfg.h"
#endif

#ifndef PF_CORE_POSIX
#include "sht21/sht21.h"
#include "adxl345/adxl345.h"
#include "mamx845x/mamx845x.h"
#include "re46c191/re46c191.h"
#include "bq32000/bq32000.h"
#include "mx35lf1g/exter_flash.h"
#include "re46c191/re46c191.h"
#include "rm3000/rm3000.h"
//#include "mxc400x/mxc400x.h"
//#include "bma253/bma253.h"
#else
#include "sim_dev/rtc.h"
#endif

typedef enum 
{
    MPL_PRESSURE,
    MPL_ALTITUDE,
    MPL_TEMPERATURE,
} mpl_cmd_type_e;

#include "radio_defs.h"
#endif

