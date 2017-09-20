/**
 * @brief       : this
 * @file        : bma253.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-06-01
 * change logs  :
 * Date       Version     Author        Note
 * 2017-06-01  v0.0.1  gang.cheng    first version
 */
#ifndef __BMA_253_H__
#define __BMA_253_H__

#include "bma253_defs.h"
#include "common/lib/lib.h"

#define WATERMARK_COUNT             (25u)

typedef enum
{
    BMA253_GET_FRAME_COUNT,
    BMA253_CLR_FRAME_COUNT,
    BMA253_GET_TEMPERATURE,
} BMA253_CMD_E;

typedef struct _bma253_context
{
    uint8_t i2c_addr;
    // use the fifo
    bool_t use_fifo;

    // accelerationg scaling
    fp32_t acc_scale;
} bma253_context_t;

void bma253_device_init(void);

#define ACC_SENSOR_NAME             "BMA253"


#endif