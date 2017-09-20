---
title: LoRaWAN协议之添加频段支持
date: 2017-02-10 15:52:21
tags: LoRaWAN,
---

## 0 前言
最新的LoRaWAN的V4版本已经不支持470MHz频段，如果需要支持的话，需要对源代码进行修改。(更新到V4.3.2以后支持470频段，但是还是需要修改内容)

<!-- more -->
## 1 定义宏参数
在LoRaWAN协议定义了不同频段，查看LoRaMac-definitions.h文件，我们可以发现：
```
#if defined( USE_BAND_433)
...
#elif defined( USE_BAND_780 )
...
#elif defined( USE_BAND_868 )
...
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
...
#endif
```

我们需要新增**USE_BAND_470**。在USE_BAND_433与USE_BAND_780之间新增：
```
#elif defined( USE_BAND_470 )
/*!
 * LoRaMac maximum number of channels
 */
#define LORA_MAX_NB_CHANNELS                        16

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_TX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_TX_MAX_DATARATE                     DR_5

/*!
 * Minimal datarate that can be used by the node
 */
#define LORAMAC_RX_MIN_DATARATE                     DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define LORAMAC_RX_MAX_DATARATE                     DR_5

/*!
 * Default datarate used by the node
 */
#define LORAMAC_DEFAULT_DATARATE                    DR_0

/*!
 * Minimal Rx1 receive datarate offset
 */
#define LORAMAC_MIN_RX1_DR_OFFSET                   0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define LORAMAC_MAX_RX1_DR_OFFSET                   5

/*!
 * Minimal Tx output power that can be used by the node
 */
#define LORAMAC_MIN_TX_POWER                        TX_POWER_2_DBM

/*!
 * Maximal Tx output power that can be used by the node
 */
#define LORAMAC_MAX_TX_POWER                        TX_POWER_17_DBM

/*!
 * Default Tx output power used by the node
 */
#define LORAMAC_DEFAULT_TX_POWER                    TX_POWER_14_DBM

/*!
 * LoRaMac TxPower definition
 */
#define TX_POWER_17_DBM                             0
#define TX_POWER_16_DBM                             1
#define TX_POWER_14_DBM                             2
#define TX_POWER_12_DBM                             3
#define TX_POWER_10_DBM                             4
#define TX_POWER_7_DBM                              5
#define TX_POWER_5_DBM                              6
#define TX_POWER_2_DBM                              7

/*!
 * LoRaMac datarates definition
 */
#define DR_0                                        0  // SF12 - BW125
#define DR_1                                        1  // SF11 - BW125
#define DR_2                                        2  // SF10 - BW125
#define DR_3                                        3  // SF9  - BW125
#define DR_4                                        4  // SF8  - BW125
#define DR_5                                        5  // SF7  - BW125

/*!
 * Second reception window channel definition. 定义接收窗口使用的信道、速率
 */
// Channel = { Frequency [Hz], Datarate }
#define RX_WND_2_CHANNEL                                  { 502300000, DR_0 }

/*!
 * LoRaMac maximum number of bands
 */
#define LORA_MAX_NB_BANDS                           1

// Band = { DutyCycle, TxMaxPower, LastTxDoneTime, TimeOff }
#define BAND0              { 1, TX_POWER_17_DBM, 0,  0 } //  100 %

/*!
 * LoRaMac default channels：这里需要定义不同信道、速率
 */
// Channel = { Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
#define LC1                { 471500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC2                { 471700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC3                { 471900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC4                { 472100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 472300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 472500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 472700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 472900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!
 * LoRaMac duty cycle for the back-off procedure
 */
#define BACKOFF_DC_1_HOUR       100
#define BACKOFF_DC_10_HOURS     1000
#define BACKOFF_DC_24_HOURS     10000

#define BACKOFF_RND_OFFSET      600000

/*!
 * LoRaMac channels which are allowed for the join procedure
 */
#define JOIN_CHANNELS      ( uint16_t )(LC(1) | LC(2) | LC(3) | LC(4) | LC(5) | LC(6) | LC(7) | LC(8))

```

这里的参数基本与433频段一致，主要是信道、速率与433有所区别。

## 2 定义速率、功率配置表
在LoRaMac.c源文件，与第一部分一样，添加宏编译，内容参考USB_BAND_433。
```
#elif defined( USE_BAND_470 )
/*!
 * Data rates table definition
 */
const uint8_t Datarates[]  = { 12, 11, 10,  9,  8,  7,  7, 50 };

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
const uint8_t MaxPayloadOfDatarate[] = { 59, 59, 59, 123, 250, 250, 250, 250 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
const uint8_t MaxPayloadOfDatarateRepeater[] = { 59, 59, 59, 123, 230, 230, 230, 230 };

/*!
 * Tx output powers table definition
 */
const int8_t TxPowers[]    = { 20, 17, 14, 11,  8,  5,  2 };

/*!
 * LoRaMac bands
 */
static Band_t Bands[LORA_MAX_NB_BANDS] =
{
    BAND0,
};

/*!
 * LoRaMAC channels
 */
static ChannelParams_t Channels[LORA_MAX_NB_CHANNELS] =
{
    LC1,
    LC2,
    LC3,
    LC4,
    LC5,
    LC6,
    LC7,
    LC8,
};

```

在TxPowers[]里面添加17选项。

## 3 信道修改
从上面的配置信息我们也可以看出来，470相对433的通道有8个，433只有3个。所以代码里面需要在修改一下。对应LoRaMac.c源文件，搜索所有USB_BAND_433，首先修改信道掩码3处：
```
/*!
 * Defines the first channel for RX window 1 for CN470 band
 */
#define LORAMAC_FIRST_RX1_CHANNEL           ( (uint32_t) 471.5e6 ) //*< 500.3e6

/*!
 * Defines the last channel for RX window 1 for CN470 band
 */
#define LORAMAC_LAST_RX1_CHANNEL            ( (uint32_t) 472.9e6 ) //*< 509.7e6


LoRaMacStatus_t LoRaMacInitialization( LoRaMacPrimitives_t *primitives, LoRaMacCallback_t *callbacks )
{
    ...
 #elif defined( USE_BAND_470 )
     // 125 kHz channels
//    for( uint8_t i = 0; i < LORA_MAX_NB_CHANNELS; i++ )
//    {
//        Channels[i].Frequency = 470.3e6 + i * 200e3;
//        Channels[i].DrRange.Value = ( DR_5 << 4 ) | DR_0;
//        Channels[i].Band = 0;
//    }
 #endif

    ...
}

```

## 4 头文件修改
### 4.1 LoRaMacCrypto.h
```
#include <stdlib.h>
#include <stdint.h>
// #include "utilities.h"

// #include "aes.h"
// #include "cmac.h"
#include "common/lib/lib.h"    # 新增
#include "LoRaMacCrypto.h"
```

### 4.2 LoRaMac.c
```
#include <math.h>
// #include "board.h"
#include "LoRaMac_arch.h"    # 新增
#include "LoRaMacCrypto.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
```

这样就完成了LoRaWAN版本对470频段支持。





