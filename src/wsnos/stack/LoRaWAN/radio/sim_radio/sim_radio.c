/**
 * @brief       : this
 * @file        : sim_radio.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-03-22
 * change logs  :
 * Date       Version     Author        Note
 * 2017-03-22  v0.0.1  gang.cheng    first version
 */
#include <sys/socket.h>     //*< sendmsg, recvmsg
#include <sys/types.h>
#include <arpa/inet.h>      //*< inet_addr()
#include <netinet/in.h>     //*< struct sockaddr_in,
#include <netinet/ip.h>
#include <errno.h>

#include <time.h>           //*< time
#include <stdlib.h>         //*< srand, random
#include <math.h>

#include "platform/platform.h"
#include "common/lib/lib.h"
#include "common/hal/hal.h"
#include "sys_arch/osel_arch.h"

#include "sim_radio.h"


#include "../../mac/LoRaMac_arch.h"
#include "../../mac/LoRaMac.h"

DBG_THIS_MODULE("SimRadio");

#define DEBUG                   (1)

/*!
 * Hardware DIO IRQ callback initialization
 */
DioIrqHandler *DioIrq[] = { NULL, NULL,
                            NULL, NULL,
                            NULL, NULL
                          };

/*!
 * Tx and Rx timers
 */
TimerEvent_t TxTimeoutTimer;
TimerEvent_t RxTimeoutTimer;
TimerEvent_t RxTimeoutSyncWord;

RadioSettings_t SimRadioSetting;
extern LoRaMacFlags_t LoRaMacFlags;
/*!
 * Reception buffer
 */
static uint8_t RxTxBuffer[RX_BUFFER_SIZE];


static RadioEvents_t *RadioEvents;

void SimRadioSetModem( RadioModems_t modem );
void SimRadioOnIrq( void );
static int8_t sim_radio_send(uint8_t const *p_data, uint8_t count);

void SimOnTimeoutIrq( void )
{
    printf( "SimOnTimeoutIrq\r\n" );
}

static void OnRadioRxTimeout( void )
{
    Radio.Sleep( );
    LoRaMacFlags.Bits.MacDone = 1;
}

void SimRadioInit(RadioEvents_t *events)
{
    RadioEvents = events;

    TimerInit( &TxTimeoutTimer, SimOnTimeoutIrq );
    TimerInit( &RxTimeoutTimer, OnRadioRxTimeout );
    TimerInit( &RxTimeoutSyncWord, SimOnTimeoutIrq );

    SimRadioSetModem(MODEM_FSK);
    SimRadioSetting.State = RF_IDLE;
    LoRaMacFlags.Value = 0;
    l_srand( BoardGetRandomSeed() );
}

void SimRadioSetOpMode(uint8_t mode)
{
    switch (mode)
    {
    case RF_OPMODE_TRANSMITTER:     //发送数据
        SimRadioOnIrq();
        break;
    }
}

void SimRadioWriteFifo(uint8_t *buf, uint8_t size)
{
    sim_radio_send(buf, size);
}

RadioState_t SimRadioGetStatus( void )
{
    return SimRadioSetting.State;
}

void SimRadioSetModem( RadioModems_t modem )
{
    if (SimRadioSetting.Modem == modem)
        return;

    SimRadioSetting.Modem = modem;
}

void SimRadioSetChannel( uint32_t freq )
{
    SimRadioSetting.Channel = freq;
}

bool SimRadioIsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh )
{
    SimRadioSetModem(modem);
    SimRadioSetChannel(freq);

    //*< enter receiver

    int16_t rssi = 0;

    return TRUE;
}

uint32_t SimRadioRandom(void)
{
    return (uint32_t)randr( 0x0, 0x7FFFFFFF );
}

void SimRadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                          uint32_t datarate, uint8_t coderate,
                          uint32_t bandwidthAfc, uint16_t preambleLen,
                          uint16_t symbTimeout, bool fixLen,
                          uint8_t payloadLen,
                          bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                          bool iqInverted, bool rxContinuous )
{
    SimRadioSetModem( modem );

    switch ( modem )
    {
    case MODEM_FSK:
    {
        SimRadioSetting.Fsk.Bandwidth    = bandwidth;
        SimRadioSetting.Fsk.Datarate     = datarate;
        SimRadioSetting.Fsk.BandwidthAfc = bandwidthAfc;
        SimRadioSetting.Fsk.FixLen       = fixLen;
        SimRadioSetting.Fsk.PayloadLen   = payloadLen;
        SimRadioSetting.Fsk.CrcOn        = crcOn;
        SimRadioSetting.Fsk.IqInverted   = iqInverted;
        SimRadioSetting.Fsk.RxContinuous = rxContinuous;
        SimRadioSetting.Fsk.PreambleLen  = preambleLen;

        datarate = ( uint16_t )( ( double )XTAL_FREQ / ( double )datarate );
    }
    break;
    case MODEM_LORA:
    {
        if ( bandwidth > 2 )
        {
            // Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
            while ( 1 );
        }
        bandwidth += 7;
        SimRadioSetting.LoRa.Bandwidth    = bandwidth;
        SimRadioSetting.LoRa.Datarate     = datarate;
        SimRadioSetting.LoRa.Coderate     = coderate;
        SimRadioSetting.LoRa.PreambleLen  = preambleLen;
        SimRadioSetting.LoRa.FixLen       = fixLen;
        SimRadioSetting.LoRa.PayloadLen   = payloadLen;
        SimRadioSetting.LoRa.CrcOn        = crcOn;
        SimRadioSetting.LoRa.FreqHopOn    = freqHopOn;
        SimRadioSetting.LoRa.HopPeriod    = hopPeriod;
        SimRadioSetting.LoRa.IqInverted   = iqInverted;
        SimRadioSetting.LoRa.RxContinuous = rxContinuous;

        if ( datarate > 12 )
        {
            datarate = 12;
        }
        else if ( datarate < 6 )
        {
            datarate = 6;
        }

        if ( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
                ( ( bandwidth == 8 ) && ( datarate == 12 ) ) )
        {
            SimRadioSetting.LoRa.LowDatarateOptimize = 0x01;
        }
        else
        {
            SimRadioSetting.LoRa.LowDatarateOptimize = 0x00;
        }
    }
    break;
    }

}

void SimRadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                          uint32_t bandwidth, uint32_t datarate,
                          uint8_t coderate, uint16_t preambleLen,
                          bool fixLen, bool crcOn, bool freqHopOn,
                          uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{
    SimRadioSetModem( modem );

    switch ( modem )
    {
    case MODEM_FSK:
    {
        SimRadioSetting.Fsk.Power       = power;
        SimRadioSetting.Fsk.Fdev        = fdev;
        SimRadioSetting.Fsk.Bandwidth   = bandwidth;
        SimRadioSetting.Fsk.Datarate    = datarate;
        SimRadioSetting.Fsk.PreambleLen = preambleLen;
        SimRadioSetting.Fsk.FixLen      = fixLen;
        SimRadioSetting.Fsk.CrcOn       = crcOn;
        SimRadioSetting.Fsk.IqInverted  = iqInverted;
        SimRadioSetting.Fsk.TxTimeout   = timeout;
    }
    break;
    case MODEM_LORA:
    {
        SimRadioSetting.LoRa.Power = power;
        if ( bandwidth > 2 )
        {
            // Fatal error: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
            while ( 1 );
        }
        bandwidth += 7;
        SimRadioSetting.LoRa.Bandwidth   = bandwidth;
        SimRadioSetting.LoRa.Datarate    = datarate;
        SimRadioSetting.LoRa.Coderate    = coderate;
        SimRadioSetting.LoRa.PreambleLen = preambleLen;
        SimRadioSetting.LoRa.FixLen      = fixLen;
        SimRadioSetting.LoRa.FreqHopOn   = freqHopOn;
        SimRadioSetting.LoRa.HopPeriod   = hopPeriod;
        SimRadioSetting.LoRa.CrcOn       = crcOn;
        SimRadioSetting.LoRa.IqInverted  = iqInverted;
        SimRadioSetting.LoRa.TxTimeout   = timeout;

        if ( datarate > 12 )
        {
            datarate = 12;
        }
        else if ( datarate < 6 )
        {
            datarate = 6;
        }
        if ( ( ( bandwidth == 7 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
                ( ( bandwidth == 8 ) && ( datarate == 12 ) ) )
        {
            SimRadioSetting.LoRa.LowDatarateOptimize = 0x01;
        }
        else
        {
            SimRadioSetting.LoRa.LowDatarateOptimize = 0x00;
        }
    }
    break;
    }
}

bool SimRadioCheckRfFrequency( uint32_t frequency )
{
    return TRUE;
}

uint32_t SimRadioGetTimeOnAir( RadioModems_t modem, uint8_t pktLen )
{
    uint32_t airTime = 0;

    switch ( modem )
    {
    case MODEM_FSK:
    {
        airTime = round( ( 8 * ( SimRadioSetting.Fsk.PreambleLen +
                                 ( 4 + 1 ) +    //*< 4 sync sizes
                                 ( ( SimRadioSetting.Fsk.FixLen == 0x01 ) ? 0.0 : 1.0 ) +
                                 ( 1 ) + //*< REG_PACKETCONFIG1 1 sizes
                                 pktLen +
                                 ( ( SimRadioSetting.Fsk.CrcOn == 0x01 ) ? 2.0 : 0 ) ) /
                           SimRadioSetting.Fsk.Datarate ) * 1e3 );
    }
    break;
    case MODEM_LORA:
    {
        double bw = 0.0;
        // REMARK: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
        switch ( SimRadioSetting.LoRa.Bandwidth )
        {
        //case 0: // 7.8 kHz
        //    bw = 78e2;
        //    break;
        //case 1: // 10.4 kHz
        //    bw = 104e2;
        //    break;
        //case 2: // 15.6 kHz
        //    bw = 156e2;
        //    break;
        //case 3: // 20.8 kHz
        //    bw = 208e2;
        //    break;
        //case 4: // 31.2 kHz
        //    bw = 312e2;
        //    break;
        //case 5: // 41.4 kHz
        //    bw = 414e2;
        //    break;
        //case 6: // 62.5 kHz
        //    bw = 625e2;
        //    break;
        case 7: // 125 kHz
            bw = 125e3;
            break;
        case 8: // 250 kHz
            bw = 250e3;
            break;
        case 9: // 500 kHz
            bw = 500e3;
            break;
        }

        // Symbol rate : time for one symbol (secs)
        double rs = bw / ( 1 << SimRadioSetting.LoRa.Datarate );
        double ts = 1 / rs;
        // time of preamble
        double tPreamble = ( SimRadioSetting.LoRa.PreambleLen + 4.25 ) * ts;
        // Symbol length of payload and time
        double tmp = ceil( ( 8 * pktLen - 4 * SimRadioSetting.LoRa.Datarate +
                             28 + 16 * SimRadioSetting.LoRa.CrcOn -
                             ( SimRadioSetting.LoRa.FixLen ? 20 : 0 ) ) /
                           ( double )( 4 * SimRadioSetting.LoRa.Datarate -
                                       ( ( SimRadioSetting.LoRa.LowDatarateOptimize > 0 ) ? 2 : 0 ) ) ) *
                     ( SimRadioSetting.LoRa.Coderate + 4 );
        double nPayload = 8 + ( ( tmp > 0 ) ? tmp : 0 );
        double tPayload = nPayload * ts;
        // Time on air
        double tOnAir = tPreamble + tPayload;
        // return us secs
        airTime = floor( tOnAir * 1e3 + 0.999 );
    }
    break;
    }
    return airTime;
}

static void SimRadioSetTx( uint32_t timeout )
{
    TimerSetValue( &TxTimeoutTimer, timeout );
    switch (SimRadioSetting.Modem)
    {
    case MODEM_FSK:
        SimRadioSetting.FskPacketHandler.FifoThresh = 0x3F;
        break;

    case MODEM_LORA:

        break;
    }

    SimRadioSetting.State = RF_TX_RUNNING;
    TimerStart( &TxTimeoutTimer );
    SimRadioSetOpMode( RF_OPMODE_TRANSMITTER );
}

void SimRadioSend( uint8_t *buffer, uint8_t size )
{
    uint32_t txTimeout = 0;
    switch (SimRadioSetting.Modem)
    {
    case MODEM_FSK:
        SimRadioSetting.FskPacketHandler.NbBytes = 0;
        SimRadioSetting.FskPacketHandler.Size = size;

        if (SimRadioSetting.Fsk.FixLen == FALSE)
        {
            //*< write size to fifo
        }
        else
        {
            //*< write size to payload length register
        }

        if ( ( size > 0 ) && ( size <= 64 ) )
        {
            SimRadioSetting.FskPacketHandler.ChunkSize = size;
        }
        else
        {
            memcpy1( RxTxBuffer, buffer, size );
            SimRadioSetting.FskPacketHandler.ChunkSize = 32;
        }

        // Write payload buffer
        // SimRadioWriteFifo( buffer, SimRadioSetting.FskPacketHandler.ChunkSize );
        SimRadioSetting.FskPacketHandler.NbBytes += SimRadioSetting.FskPacketHandler.ChunkSize;
        txTimeout = SimRadioSetting.Fsk.TxTimeout;
        break;

    case MODEM_LORA:
        SimRadioSetting.LoRaPacketHandler.Size = size;
        // Write payload buffer
        // ***

        SimRadioWriteFifo(buffer, size);
        txTimeout = SimRadioSetting.LoRa.TxTimeout;
        break;
    }

    SimRadioSetTx( txTimeout );
}

void SimRadioSetSleep(void)
{
    TimerStop( &RxTimeoutTimer );
    TimerStop( &TxTimeoutTimer );

    // SimRadioSetOpMode( RF_OPMODE_SLEEP );
    SimRadioSetting.State = RF_IDLE;
}

void SimRadioSetStby( void )
{
    TimerStop( &RxTimeoutTimer );
    TimerStop( &TxTimeoutTimer );

    // SimRadioSetOpMode( RF_OPMODE_STANDBY );
    SimRadioSetting.State = RF_IDLE;
}

void SimRadioSetRx( uint32_t timeout )
{
    bool_t rxContinuous = FALSE;

    switch (SimRadioSetting.Modem)
    {
    case MODEM_FSK:
        rxContinuous = SimRadioSetting.Fsk.RxContinuous;
        SimRadioSetting.FskPacketHandler.PreambleDetected = false;
        SimRadioSetting.FskPacketHandler.SyncWordDetected = false;
        SimRadioSetting.FskPacketHandler.NbBytes = 0;
        SimRadioSetting.FskPacketHandler.Size = 0;
        break;

    case MODEM_LORA:
        // ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal
        if ( SimRadioSetting.LoRa.Bandwidth < 9 )
        {
            switch ( SimRadioSetting.LoRa.Bandwidth )
            {
            case 0: // 7.8 kHz
                SimRadioSetChannel(SimRadioSetting.Channel + 7.81e3 );
                break;
            case 1: // 10.4 kHz
                SimRadioSetChannel(SimRadioSetting.Channel + 10.42e3 );
                break;
            case 2: // 15.6 kHz
                SimRadioSetChannel(SimRadioSetting.Channel + 15.62e3 );
                break;
            case 3: // 20.8 kHz
                SimRadioSetChannel(SimRadioSetting.Channel + 20.83e3 );
                break;
            case 4: // 31.2 kHz
                SimRadioSetChannel(SimRadioSetting.Channel + 31.25e3 );
                break;
            case 5: // 41.4 kHz
                SimRadioSetChannel(SimRadioSetting.Channel + 41.67e3 );
                break;
            case 6: // 62.5 kHz
                break;
            case 7: // 125 kHz
                break;
            case 8: // 250 kHz
                break;
            }
        }

        rxContinuous = SimRadioSetting.LoRa.RxContinuous;
        break;
    }

    memset( RxTxBuffer, 0, ( size_t )RX_BUFFER_SIZE );

    SimRadioSetting.State = RF_RX_RUNNING;
    if ( timeout != 0 )
    {
        TimerSetValue( &RxTimeoutTimer, timeout );
        TimerStart( &RxTimeoutTimer );
    }

    if ( SimRadioSetting.Modem == MODEM_FSK )
    {
        // SimRadioSetOpMode( RF_OPMODE_RECEIVER );
        if ( rxContinuous == false )
        {
            TimerSetValue( &RxTimeoutSyncWord, ceil( ( 8.0 * ( SimRadioSetting.Fsk.PreambleLen +
                           ( 4 + 1 ) +    //*< 4 sync sizes
                           + 10.0 ) /
                           ( double )SimRadioSetting.Fsk.Datarate ) * 1e3 ) + 4 );
            TimerStart( &RxTimeoutSyncWord );
        }
    }
    else
    {
        if ( rxContinuous == true )
        {
            // SimRadioSetOpMode( RFLR_OPMODE_RECEIVER );
        }
        else
        {
            // SimRadioSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
        }
    }
}

static void SimRadioStartCad(void)
{
    switch ( SimRadioSetting.Modem )
    {
    case MODEM_FSK:
        break;

    case MODEM_LORA:
        SimRadioSetting.State = RF_CAD;
        break;
    default:
        break;
    }
}

void SimRadioSetTxContinuousWave(uint32_t freq, int8_t power, uint16_t time )
{
    uint32_t timeout = ( uint32_t )( time * 1e3 );

    SimRadioSetChannel( freq );

    SimRadioSetTxConfig( MODEM_FSK, power, 0, 0, 4800, 0, 5, false, false, 0, 0, 0, timeout );

    // Disable radio interrupts

    TimerSetValue( &TxTimeoutTimer, timeout );

    SimRadioSetting.State = RF_TX_RUNNING;
    TimerStart( &TxTimeoutTimer );
    SimRadioSetOpMode( RF_OPMODE_TRANSMITTER );
}

int16_t SimRadioReadRssi(RadioModems_t modem)
{
    return -1;
}

void SimRadioWrite(uint8_t addr, uint8_t data)
{
    ;
}

uint8_t SimRadioRead(uint8_t addr)
{
    return 0;
}

void SimRadioWriteBuffer(uint8_t addr, uint8_t *buffer, uint8_t size)
{
    ;
}

void SimRadioReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    ;
}

void SimRadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    SimRadioSetModem(modem);

    //*< set max length
}

void SimRadioSetPublicNetwork(bool_t enable)
{
    SimRadioSetModem( MODEM_LORA );
    SimRadioSetting.LoRa.PublicNetwork = enable;
    if( enable == true )
    {
        // Change LoRa modem SyncWord
    }
    else
    {
        // Change LoRa modem SyncWord
    }
}

void SimRadioOnIrq( void )
{
    switch ( SimRadioSetting.State )
    {
    case RF_RX_RUNNING:
        break;

    case RF_TX_RUNNING:
        TimerStop( &TxTimeoutTimer );
        switch ( SimRadioSetting.Modem )
        {
        case MODEM_LORA:
        // Clear Irq
        // Intentional fall through
        case MODEM_FSK:
        default:
            SimRadioSetting.State = RF_IDLE;
            if ( ( RadioEvents != NULL ) && ( RadioEvents->TxDone != NULL ) )
            {
                RadioEvents->TxDone( );
            }
            break;
        }
        break;
    default:
        break;
    }
}

const struct Radio_s Radio =
{
    SimRadioInit,
    SimRadioGetStatus,
    SimRadioSetModem,
    SimRadioSetChannel,
    SimRadioIsChannelFree,
    SimRadioRandom,
    SimRadioSetRxConfig,
    SimRadioSetTxConfig,
    SimRadioCheckRfFrequency,
    SimRadioGetTimeOnAir,
    SimRadioSend,
    SimRadioSetSleep,
    SimRadioSetStby,
    SimRadioSetRx,
    SimRadioStartCad,
    SimRadioSetTxContinuousWave,
    SimRadioReadRssi,
    SimRadioWrite,
    SimRadioRead,
    SimRadioWriteBuffer,
    SimRadioReadBuffer,
    SimRadioSetMaxPayloadLength,
    SimRadioSetPublicNetwork
};

//模拟网络
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>         //*< srand, random

//接收
#define RSELF_PORT              9001
#define RSELF_ADDR              "224.0.0.91"
//发送
#define SMCAST_PORT              9000
#define SMCAST_ADDR              "224.0.0.88"
#define SSELF_PORT              9001
#define SSELF_ADDR              "224.0.0.89"

#define MAXBUF                200

typedef struct
{
    int len;
    uint8_t buf[MAXBUF];
} element_t;

static sqqueue_ctrl_t queue;   //创建队列对象
static int ssockfd;
static struct sockaddr_in speeraddr;
static int8_t sim_radio_send(uint8_t const *p_data, uint8_t count)
{
    if (sendto(ssockfd, p_data, count, 0, (struct sockaddr *) &speeraddr,
               sizeof(struct sockaddr_in)) < 0) {
        printf("sendto error!");
        exit(3);
    }
    printf("[redio send]:");
    printf_string((uint8_t *)p_data, count);
    return count;
}

void sim_radio_send_init(void)
{
    struct sockaddr_in myaddr;
    unsigned int socklen;

    /* 创建 socket 用于UDP通讯 */
    ssockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ssockfd < 0) {
        perror("socket creating error");
        exit(EXIT_FAILURE);
    }

    socklen = sizeof(struct sockaddr_in);

    /* 设置对方的端口和IP信息 */
    memset(&speeraddr, 0, socklen);
    speeraddr.sin_family = AF_INET;
    speeraddr.sin_port = htons(SMCAST_PORT);

    /* 注意这里设置的对方地址是指组播地址，而不是对方的实际IP地址 */
    if (inet_pton(AF_INET, SMCAST_ADDR, &speeraddr.sin_addr) <= 0) {
        perror("wrong group address");
        exit(EXIT_FAILURE);
    }

    /* 设置自己的端口和IP信息 */
    memset(&myaddr, 0, socklen);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(SSELF_PORT);

    if (inet_pton(AF_INET, SSELF_ADDR, &myaddr.sin_addr) <= 0) {
        perror("self ip address error!");
        exit(EXIT_FAILURE);
    }
    /* 绑定自己的端口和IP信息到socket上 */
    if (bind(ssockfd, (struct sockaddr *) &myaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }
}

static void thread_udp_broadcast(void)
{
    int rsockfd;
    struct sockaddr_in rpeeraddr;
    struct in_addr ia;
    unsigned int socklen;
    struct hostent *group;
    struct ip_mreq mreq;
    /* 创建 socket 用于UDP通讯 */
    rsockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (rsockfd < 0) {
        perror("socket creating err in udptalk\n");
        exit(EXIT_FAILURE);
    }

    /* 设置要加入组播的地址 */
    bzero(&mreq, sizeof(struct ip_mreq));
    if ((group = gethostbyname(RSELF_ADDR)) == (struct hostent *) 0) {
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    bcopy((void *) group->h_addr, (void *) &ia, group->h_length);
    /* 设置组地址 */
    bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));

    /* 设置发送组播消息的源主机的地址信息 */
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    /* 把本机加入组播地址，即本机网卡作为组播成员，只有加入组才能收到组播消息 */
    if (setsockopt
            (rsockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
             sizeof(struct ip_mreq)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    socklen = sizeof(struct sockaddr_in);
    memset(&rpeeraddr, 0, socklen);
    rpeeraddr.sin_family = AF_INET;
    rpeeraddr.sin_port = htons(RSELF_PORT);

    if (inet_pton(AF_INET, RSELF_ADDR, &rpeeraddr.sin_addr) <= 0) {
        perror("Wrong dest IP address");
        exit(EXIT_FAILURE);
    }

    /* 绑定自己的端口和IP信息到socket上 */
    if (bind(rsockfd, (struct sockaddr *) &rpeeraddr, sizeof(struct sockaddr_in)) == -1) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    /* 循环接收网络上来的组播消息 */
    element_t element;
    for (;;) {
        bzero(element.buf, MAXBUF);
        element.len = recvfrom(rsockfd, element.buf, MAXBUF, 0,
                               (struct sockaddr *) &rpeeraddr, &socklen);
        if ( element.len < 0) {
            perror("recvfrom err in udptalk!");
            exit(4);
        } else {
            /* 成功接收到数据报 */
            // printf("队列长度:%d\r\n",  queue.get_len(&queue));
            if ((queue.sqq.sqq_len - 1 - queue.get_len(&queue)) != 0)
            {
                queue.enter(&queue, &element);  //将成员插入到队列中
            }
        }
    }
}

static void wait_ms(unsigned long a) {
    struct timespec dly;
    struct timespec rem;

    dly.tv_sec = a / 1000;
    dly.tv_nsec = ((long)a % 1000) * 1000000;

    if ((dly.tv_sec > 0) || ((dly.tv_sec == 0) && (dly.tv_nsec > 100000))) {
        clock_nanosleep(CLOCK_MONOTONIC, 0, &dly, &rem);
    }
    return;
}

static uint8_t queue_read(uint8_t *pload)
{
    int len = 0;
    if ( queue.get_len(&queue) != 0)
    {
        element_t *element = queue.del(&queue);
        memcpy(pload, element->buf, element->len);
        len = element->len;
    }
    return len;
}

static void SimOnDio0Irq(void)
{
    while (1)
    {
        wait_ms(100);
        uint8_t RxBuffer[MAXBUF];
        int len = queue_read(RxBuffer);
        if (len != 0)
        {
            printf("[redio recv]:");
            printf_string(RxBuffer, len);
            if ( ( RadioEvents != NULL ) && ( RadioEvents->RxDone != NULL ) )
            {
                RadioEvents->RxDone( RxBuffer, len, 0, 0 );
            }
        }
    }
}

int8_t SimRadioHwInit(void)
{
    DBG_LOG(DBG_LEVEL_INFO, "sim_raido_init\n");

    int size = 11;
    if (FALSE == sqqueue_ctrl_init(&queue, sizeof(element_t), size))    //队列长度要比实际创建的长度-1
    {
        printf("queue error\r\n");
        return -1;
    }

    sim_radio_send_init();
    pthread_t thrid_udp, simIrq;
    if (pthread_create( &thrid_udp, NULL, (void * (*)(void *))thread_udp_broadcast, NULL) != 0) {
        return -1;
    }

    if (pthread_create( &simIrq, NULL, (void * (*)(void *))SimOnDio0Irq, NULL) != 0) {
        return -1;
    }
    return 1;
}
