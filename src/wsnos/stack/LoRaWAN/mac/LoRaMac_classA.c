
#include "platform/platform.h"
#include "LoRaMac_arch.h"

#include "LoRaMac.h"
#include "lora_comissioning.h"
#include "LoRaMac_classA.h"

#include "sys_arch/osel_arch.h"

/*!
 * Defines the application data transmission duty cycle. 5s, value in [ms].
 */
#define APP_TX_DUTYCYCLE                            5000

/*!
 * Defines a random delay for application data transmission duty cycle. 1s,
 * value in [ms].
 */
#define APP_TX_DUTYCYCLE_RND                        1000

/*!
 * Default datarate
 */
#define LORAWAN_DEFAULT_DATARATE                    DR_0

/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG_ON                    FALSE

/*!
 * LoRaWAN Adaptive Data Rate
 *
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_ADR_ON                              1


#if defined( USE_BAND_868 )

#include "LoRaMacTest.h"

/*!
 * LoRaWAN ETSI duty cycle control enable/disable
 *
 * \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
 */
#define LORAWAN_DUTYCYCLE_ON                        true

#define USE_SEMTECH_DEFAULT_CHANNEL_LINEUP          1

#if( USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1 )

#define LC4                { 867100000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 867300000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 867500000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 867700000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 867900000, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC9                { 868800000, { ( ( DR_7 << 4 ) | DR_7 ) }, 2 }
#define LC10               { 868300000, { ( ( DR_6 << 4 ) | DR_6 ) }, 1 }

#endif

#endif

/*!
 * LoRaWAN application port
 */
#define LORAWAN_APP_PORT                            2

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

#if( OVER_THE_AIR_ACTIVATION == 0 )

static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;

/*!
 * Device address
 */
static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

#endif

/*!
 * Indicates if the MAC layer has already joined a network.
 */
static bool IsNetworkJoined = FALSE;

/*!
 * Application port
 */
static uint8_t AppPort = LORAWAN_APP_PORT;

/*!
 * User application data size
 */
static uint8_t AppDataSize = 0;

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_MAX_SIZE                           64

/*!
 * User application data
 */
static uint8_t AppData[LORAWAN_APP_DATA_MAX_SIZE];

/*!
 * Indicates if the node is sending confirmed or unconfirmed messages
 */
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*!
 * Defines the application data transmission duty cycle
 */
static uint32_t TxDutyCycleTime;

/*!
 * Indicates if a new packet can be sent
 */
static bool NextTx = true;

/*!
 * Device states
 */
typedef enum eDevicState
{
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_SEND,
    DEVICE_STATE_SLEEP,
} DeviceState_t;

/*!
 * LoRaWAN compliance tests support data
 */
struct ComplianceTest_s
{
    bool Running;
    uint8_t State;
    bool IsTxConfirmed;
    uint8_t AppPort;
    uint8_t AppDataSize;
    uint8_t *AppDataBuffer;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
} ComplianceTest;

static DeviceState_t DeviceState;


static LoRaMacPrimitives_t LoRaMacPrimitives;
static LoRaMacCallback_t LoRaMacCallbacks;
#define MAC_EVENT_MAX               (10u)
static osel_task_t *mac_task_tcb = NULL;
static osel_event_t mac_event_store[MAC_EVENT_MAX];

static stack_callback_t classA_callbacks;

static uint32_t lora_classa_send_seq = 0;
static mbuf_t *l_mbuf = NULL;

PROCESS_NAME(loramac_classa_init_process);
PROCESS_NAME(loramac_classa_join_process);
PROCESS_NAME(loramac_classa_send_process);

/*!
 * \brief   Prepares the payload of the frame
 */
static void PrepareTxFrame( uint8_t port )
{
    switch (port)
    {
    case 2:
    {
#if defined( USE_BAND_433 ) || defined( USE_BAND_470 ) || defined( USE_BAND_780 ) || defined( USE_BAND_868 )
        IsTxConfirmed = l_mbuf->flag;
        if (LORAWAN_APP_DATA_MAX_SIZE > l_mbuf->data_len)
            AppDataSize = l_mbuf->data_len;
        else
            AppDataSize = LORAWAN_APP_DATA_MAX_SIZE;

        osel_memcpy(AppData, l_mbuf->head, AppDataSize);
#elif defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )
        //*< AppData[0~10]
#endif
    }
    break;

    case 224:
        if ( ComplianceTest.LinkCheck == true )
        {
            ComplianceTest.LinkCheck = false;
            AppDataSize = 3;
            AppData[0] = 5;
            AppData[1] = ComplianceTest.DemodMargin;
            AppData[2] = ComplianceTest.NbGateways;
            ComplianceTest.State = 1;
        }
        else
        {
            switch ( ComplianceTest.State )
            {
            case 4:
                ComplianceTest.State = 1;
                break;
            case 1:
                AppDataSize = 2;
                AppData[0] = ComplianceTest.DownLinkCounter >> 8;
                AppData[1] = ComplianceTest.DownLinkCounter;
                break;
            }
        }
        break;

    default:
        break;
    }
}

/*!
 * \brief   Prepares the payload of the frame
 *
 * \retval  [0: frame could be send, 1: error]
 */
static bool SendFrame( void )
{
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if ( LoRaMacQueryTxPossible( AppDataSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
    }
    else
    {
        if ( IsTxConfirmed == false )
        {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = AppPort;
            mcpsReq.Req.Unconfirmed.fBuffer = AppData;
            mcpsReq.Req.Unconfirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
        else
        {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = AppPort;
            mcpsReq.Req.Confirmed.fBuffer = AppData;
            mcpsReq.Req.Confirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Confirmed.NbTrials = 3;
            mcpsReq.Req.Confirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
    }

    if ( LoRaMacMcpsRequest( &mcpsReq ) == LORAMAC_STATUS_OK )
    {
        return false;
    }
    return true;
}

/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    if (mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
    {
        switch (mcpsConfirm->McpsRequest)
        {
        case MCPS_UNCONFIRMED:
            // check datareate
            // check txpower
            NextTx = TRUE;
            if (classA_callbacks.tx_confirm != NULL)
                classA_callbacks.tx_confirm(TRUE, l_mbuf->seq, l_mbuf->head, l_mbuf->data_len);
            break;

        case MCPS_CONFIRMED:
            // check datarate
            // check txpower
            // check ack-received
            // check nbtrials
            NextTx = TRUE;
            if (classA_callbacks.tx_confirm != NULL)
                if (mcpsConfirm->AckReceived == TRUE)
                {
                    classA_callbacks.tx_confirm(TRUE, l_mbuf->seq, l_mbuf->head, l_mbuf->data_len);
                }
                else
                {
                    classA_callbacks.tx_confirm(FALSE, l_mbuf->seq, l_mbuf->head, l_mbuf->data_len);
                }
            break;

        case MCPS_PROPRIETARY:
            break;

        default:
            break;
        }
    }
    else
    {
        switch (mcpsConfirm->McpsRequest)
        {
        case MCPS_UNCONFIRMED:
            NextTx = TRUE;
            if (classA_callbacks.tx_confirm != NULL)
                classA_callbacks.tx_confirm(FALSE, l_mbuf->seq, l_mbuf->head, l_mbuf->data_len);
            break;

        case MCPS_CONFIRMED:
            NextTx = TRUE;
            if (classA_callbacks.tx_confirm != NULL)
                classA_callbacks.tx_confirm(FALSE, l_mbuf->seq, l_mbuf->head, l_mbuf->data_len);
            break;

        case MCPS_PROPRIETARY:
            break;

        default:
            break;
        }
    }
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication( McpsIndication_t *mcpsIndication )
{
    if ( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }

    switch ( mcpsIndication->McpsIndication )
    {
    case MCPS_UNCONFIRMED:
    {
        break;
    }
    case MCPS_CONFIRMED:
    {
        break;
    }
    case MCPS_PROPRIETARY:
    {
        break;
    }
    case MCPS_MULTICAST:
    {
        break;
    }
    default:
        break;
    }

    // Check Multicast
    // Check Port
    // Check Datarate
    // Check FramePending
    // Check Buffer
    // Check BufferSize
    // Check Rssi
    // Check Snr
    // Check RxSlot

    if ( ComplianceTest.Running == true )
    {
        ComplianceTest.DownLinkCounter++;
    }

    if ( mcpsIndication->RxData == true )
    {
        switch ( mcpsIndication->Port )
        {
        case 1: // The application LED can be controlled on port 1 or 2
        case 2:
            if (classA_callbacks.rx_indicate != NULL)
                classA_callbacks.rx_indicate(mcpsIndication->Buffer,
                                             mcpsIndication->BufferSize);
            break;
        case 224:
            if ( ComplianceTest.Running == false )
            {
                // Check compliance test enable command (i)
                if ( ( mcpsIndication->BufferSize == 4 ) &&
                        ( mcpsIndication->Buffer[0] == 0x01 ) &&
                        ( mcpsIndication->Buffer[1] == 0x01 ) &&
                        ( mcpsIndication->Buffer[2] == 0x01 ) &&
                        ( mcpsIndication->Buffer[3] == 0x01 ) )
                {
                    IsTxConfirmed = false;
                    AppPort = 224;
                    AppDataSize = 2;
                    ComplianceTest.DownLinkCounter = 0;
                    ComplianceTest.LinkCheck = false;
                    ComplianceTest.DemodMargin = 0;
                    ComplianceTest.NbGateways = 0;
                    ComplianceTest.Running = true;
                    ComplianceTest.State = 1;

                    MibRequestConfirm_t mibReq;
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = true;
                    LoRaMacMibSetRequestConfirm( &mibReq );

#if defined( USE_BAND_868 )
                    LoRaMacTestSetDutyCycleOn( false );
#endif
                    // GpsStop( );
                }
            }
            else
            {
                ComplianceTest.State = mcpsIndication->Buffer[0];
                switch ( ComplianceTest.State )
                {
                case 0: // Check compliance test disable command (ii)
                    IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;
                    AppPort = LORAWAN_APP_PORT;
                    AppDataSize = LORAWAN_APP_DATA_MAX_SIZE;
                    ComplianceTest.DownLinkCounter = 0;
                    ComplianceTest.Running = false;

                    MibRequestConfirm_t mibReq;
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
                    LoRaMacMibSetRequestConfirm( &mibReq );
#if defined( USE_BAND_868 )
                    LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif
                    // GpsStart( );
                    break;
                case 1: // (iii, iv)
                    AppDataSize = 2;
                    break;
                case 2: // Enable confirmed messages (v)
                    IsTxConfirmed = true;
                    ComplianceTest.State = 1;
                    break;
                case 3:  // Disable confirmed messages (vi)
                    IsTxConfirmed = false;
                    ComplianceTest.State = 1;
                    break;
                case 4: // (vii)
                    AppDataSize = mcpsIndication->BufferSize;

                    AppData[0] = 4;
                    for ( uint8_t i = 1; i < AppDataSize; i++ )
                    {
                        AppData[i] = mcpsIndication->Buffer[i] + 1;
                    }
                    break;
                case 5: // (viii)
                {
                    MlmeReq_t mlmeReq;
                    mlmeReq.Type = MLME_LINK_CHECK;
                    LoRaMacMlmeRequest( &mlmeReq );
                }
                break;
                case 6: // (ix)
                {
                    MlmeReq_t mlmeReq;

                    mlmeReq.Type = MLME_JOIN;

                    mlmeReq.Req.Join.DevEui = DevEui;
                    mlmeReq.Req.Join.AppEui = AppEui;
                    mlmeReq.Req.Join.AppKey = AppKey;

                    LoRaMacMlmeRequest( &mlmeReq );
                }
                break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] mlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    switch ( mlmeConfirm->MlmeRequest )
    {
    case MLME_JOIN:
    {
        if ( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
        {
            // Status is OK, node has joined the network
            IsNetworkJoined = TRUE;
            osel_int_status_t s;
            OSEL_ENTER_CRITICAL(s);
            DeviceState = DEVICE_STATE_SEND;
            OSEL_EXIT_CRITICAL(s);
            NextTx = true;
            osel_pthread_create(mac_task_tcb, &loramac_classa_send_process, PROCESS_CURRENT());
            if (classA_callbacks.join_confirm != NULL)
                classA_callbacks.join_confirm(TRUE);
        }
        else
        {
            // Join was not successful. Try to join again
            osel_int_status_t s;
            OSEL_ENTER_CRITICAL(s);
            DeviceState = DEVICE_STATE_JOIN;
            OSEL_EXIT_CRITICAL(s);
            osel_event_t event;
            event.sig = PROCESS_EVENT_MSG;
            event.param = NULL;
            osel_post(NULL, &loramac_classa_join_process, &event);
        }
        break;
    }
    case MLME_LINK_CHECK:
    {
        // Check DemodMargin
        // Check NbGateways
        if ( ComplianceTest.Running == true )
        {
            ComplianceTest.LinkCheck = true;
            ComplianceTest.DemodMargin = mlmeConfirm->DemodMargin;
            ComplianceTest.NbGateways = mlmeConfirm->NbGateways;
        }
        break;
    }
    default:
        break;
    }

    NextTx = true;
}

static uint8_t BoardGetBatteryLevel(void)
{
    return 255; //*<
}

/**
 * @brief lorawan state init.
 */
static void lorawan_state_init_handle(void)
{
    MibRequestConfirm_t mibReq;

    LoRaMacPrimitives.MacMcpsConfirm    = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm    = MlmeConfirm;
    LoRaMacCallbacks.GetBatteryLevel    = BoardGetBatteryLevel;
    LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks);

    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
    LoRaMacMibSetRequestConfirm(&mibReq);

    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
    LoRaMacMibSetRequestConfirm(&mibReq);

#if defined( USE_BAND_868 )
    LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );

#if( USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1 )
    LoRaMacChannelAdd( 3, ( ChannelParams_t )LC4 );
    LoRaMacChannelAdd( 4, ( ChannelParams_t )LC5 );
    LoRaMacChannelAdd( 5, ( ChannelParams_t )LC6 );
    LoRaMacChannelAdd( 6, ( ChannelParams_t )LC7 );
    LoRaMacChannelAdd( 7, ( ChannelParams_t )LC8 );
    LoRaMacChannelAdd( 8, ( ChannelParams_t )LC9 );
    LoRaMacChannelAdd( 9, ( ChannelParams_t )LC10 );
#endif

#endif

    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    DeviceState = DEVICE_STATE_JOIN;
    OSEL_EXIT_CRITICAL(s);

    osel_pthread_create(mac_task_tcb, &loramac_classa_join_process, PROCESS_CURRENT());
}

static void lorawan_state_join_handle(void)
{
#if( OVER_THE_AIR_ACTIVATION != 0 )
    MlmeReq_t mlmeReq;

    // Initialize LoRaMac device unique ID
    BoardGetUniqueId( DevEui );

    mlmeReq.Type = MLME_JOIN;

    mlmeReq.Req.Join.DevEui = DevEui;
    mlmeReq.Req.Join.AppEui = AppEui;
    mlmeReq.Req.Join.AppKey = AppKey;
    mlmeReq.Req.Join.NbTrials = 3;      //TOADD
    if ( NextTx == true )
    {
        LoRaMacMlmeRequest( &mlmeReq );
    }

    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    DeviceState = DEVICE_STATE_SLEEP;
    OSEL_EXIT_CRITICAL(s);
#else
    BoardGetUniqueId( DevEui );
    // Choose a random device address if not already defined in Comissioning.h
    if ( DevAddr == 0 )
    {
#if 1
        //Random seed initialization
        l_srand( BoardGetRandomSeed() );

        //Choose a random device address
        DevAddr = randr( 0, 0x01FFFFFF );
#else
        DevAddr = lorawan_device_addr();
#endif
    }


    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NET_ID;
    mibReq.Param.NetID = LORAWAN_NETWORK_ID;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_DEV_ADDR;
    mibReq.Param.DevAddr = DevAddr;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_NWK_SKEY;
    mibReq.Param.NwkSKey = NwkSKey;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_APP_SKEY;
    mibReq.Param.AppSKey = AppSKey;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_NETWORK_JOINED;
    mibReq.Param.IsNetworkJoined = true;
    LoRaMacMibSetRequestConfirm( &mibReq );

    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    DeviceState = DEVICE_STATE_SEND;
    OSEL_EXIT_CRITICAL(s);

    NextTx = true;
    if (classA_callbacks.join_confirm != NULL)
        classA_callbacks.join_confirm(TRUE);

    osel_pthread_create(mac_task_tcb, &loramac_classa_send_process, PROCESS_CURRENT());
#endif
}


static void lorawan_state_send_handle(process_event_t ev, process_data_t data)
{
    if ( NextTx == true )
    {
        if (l_mbuf != NULL)
        {
            AppPort = l_mbuf->port;
            PrepareTxFrame( AppPort );
            NextTx = SendFrame();
        }
    }

    if ( ComplianceTest.Running == true )
    {
        // Schedule next packet transmission
        TxDutyCycleTime = 5000; // 5000 ms
    }
    else
    {
        // Schedule next packet transmission
        TxDutyCycleTime = APP_TX_DUTYCYCLE + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
    }
}

PROCESS(loramac_classa_init_process, "loRaMAC classA init thread");
PROCESS_THREAD(loramac_classa_init_process, ev, data)
{
    PROCESS_BEGIN();

    static osel_etimer_t delay_timer;

    DeviceState = DEVICE_STATE_INIT;
    //*< board, radio, device init

    LORAWAN_RADIO_HW_INIT();

    //*< delay 5 ticks ,wait os start up.
    OSEL_ETIMER_DELAY(&delay_timer, 5);

    lorawan_state_init_handle();

    PROCESS_END();
}


PROCESS(loramac_classa_join_process, "loRaMAC classA join thread");
PROCESS_THREAD(loramac_classa_join_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t join_cycle_timer;

    while (1)
    {
        if (ev == PROCESS_EVENT_INIT)
        {
            lorawan_state_join_handle();
        }
        else if (ev == PROCESS_EVENT_MSG)
        {
            OSEL_ETIMER_DELAY(&join_cycle_timer, 200 * OSEL_TICK_RATE_HZ);
            lorawan_state_join_handle();
        }

        PROCESS_YIELD();
    }

    PROCESS_END();
}

PROCESS(loramac_classa_send_process, "loRaMAC classA send thread");
PROCESS_THREAD(loramac_classa_send_process, ev, data)
{
    PROCESS_BEGIN();

    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_MSG)
        {
            lorawan_state_send_handle(ev, data);
        }
        else if (ev == PROCESS_EVENT_EXIT)
        {
            ;
        }
    }

    PROCESS_END();
}


bool_t mac_init(void *mac_pib)
{

    mac_task_tcb = osel_task_create(NULL, MAC_TASK_PRIO, mac_event_store, MAC_EVENT_MAX);
    DBG_ASSERT(NULL != mac_task_tcb __DBG_LINE);

    classA_callbacks.tx_confirm   = NULL;
    classA_callbacks.rx_indicate  = NULL;
    classA_callbacks.join_confirm = NULL;

    osel_pthread_create(mac_task_tcb, &loramac_timercallback_process, NULL);

    return TRUE;
}

void mac_class_run(void)
{
    osel_pthread_create(mac_task_tcb, &loramac_classa_init_process, NULL);
}


void mac_class_stop(void)
{
    osel_pthread_exit(mac_task_tcb, &loramac_classa_init_process, PROCESS_CURRENT());
    osel_pthread_exit(mac_task_tcb, &loramac_classa_join_process, PROCESS_CURRENT());
    osel_pthread_exit(mac_task_tcb, &loramac_classa_send_process, PROCESS_CURRENT());
}

void mac_class_dependent_cfg(const stack_callback_t *callbacks)
{
    DBG_ASSERT(NULL != callbacks __DBG_LINE);

    classA_callbacks.tx_confirm   = callbacks->tx_confirm;
    classA_callbacks.rx_indicate  = callbacks->rx_indicate;
    classA_callbacks.join_confirm = callbacks->join_confirm;
}

int8_t mac_class_send(msg_flag_t flag, mbuf_t *mbuf)
{
    DBG_ASSERT(NULL != mbuf __DBG_LINE);

    if (NextTx == FALSE)
        return -3;

    if ( mbuf->data_len > LORAWAN_APP_DATA_MAX_SIZE)
        return -1;

    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;
    mibReq.Type = MIB_NETWORK_JOINED;
    osel_int8_t s = 0;
    s = osel_mutex_lock(OSEL_MAX_PRIO);
    status = LoRaMacMibGetRequestConfirm( &mibReq );
    osel_mutex_unlock(s);
    if ( status == LORAMAC_STATUS_OK )
    {
        if ( mibReq.Param.IsNetworkJoined == FALSE )
            return -2;
    }
    else
        return -2;

    l_mbuf = mbuf;
    l_mbuf->seq  = lora_classa_send_seq++;
    l_mbuf->flag = flag;
    l_mbuf->port = LORAWAN_APP_PORT;

    osel_event_t event;
    event.sig = PROCESS_EVENT_MSG;
    event.param = mbuf;
    osel_post(NULL, &loramac_classa_send_process, &event);

    return 0;
}