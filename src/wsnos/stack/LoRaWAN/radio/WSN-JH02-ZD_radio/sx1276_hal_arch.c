#include "sx1276_hal_arch.h"
#include "platform/platform.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    SX1276Init,
    SX1276GetStatus,
    SX1276SetModem,
    SX1276SetChannel,
    SX1276IsChannelFree,
    SX1276Random,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
    SX1276SetStby,
    SX1276SetRx,
    SX1276StartCad,
    SX1276SetTxContinuousWave,
    SX1276ReadRssi,
    SX1276Write,
    SX1276Read,
    SX1276WriteBuffer,
    SX1276ReadBuffer,
    SX1276SetMaxPayloadLength,
    SX1276SetPublicNetwork
};

/*!
 * Antenna switch GPIO pins objects
 */
Gpio_t AntSwitchLf;
Gpio_t AntSwitchHf;
Gpio_t RadioPower;
Gpio_t RadioSw;
Gpio_t RadioRxtx;

void SpiDeInit( Spi_t *obj );

void SX1276IoInit( void )
{
//    GpioInit(&RadioRxtx, RADIO_RXTX, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0 );
    //*< For Power
    GpioInit(&RadioPower, RADIO_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
//    GpioInit(&RadioSw, RADIO_SW, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    GpioInit( &SX1276.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );

    GpioInit( &SX1276.DIO0, RADIO_DIO_0, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0 );
    GpioInit( &SX1276.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0 );
    GpioInit( &SX1276.DIO2, RADIO_DIO_2, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0 );
    GpioInit( &SX1276.DIO3, RADIO_DIO_3, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0 );
    GpioInit( &SX1276.DIO4, RADIO_DIO_4, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0 );
    GpioInit( &SX1276.DIO5, RADIO_DIO_5, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0 );

    SpiInit( &SX1276.Spi, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC );
}

void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
    GpioSetInterrupt( &SX1276.DIO0, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[0] );
    GpioSetInterrupt( &SX1276.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[1] );
    GpioSetInterrupt( &SX1276.DIO2, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[2] );
    GpioSetInterrupt( &SX1276.DIO3, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[3] );
    GpioSetInterrupt( &SX1276.DIO4, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[4] );
//    GpioSetInterrupt( &SX1276.DIO5, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[5] );
}

void SX1276IoDeInit( void )
{
    GpioInit(&RadioPower, PB_8, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);

    GpioInit( &SX1276.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );

    GpioInit( &SX1276.DIO0, RADIO_DIO_0, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX1276.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX1276.DIO2, RADIO_DIO_2, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX1276.DIO3, RADIO_DIO_3, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX1276.DIO4, RADIO_DIO_4, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &SX1276.DIO5, RADIO_DIO_5, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );

    SpiDeInit(&SX1276.Spi);
}

uint8_t SX1276GetPaSelect( uint32_t channel )
{
    if ( channel < RF_MID_BAND_THRESH )
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    else
    {
        return RF_PACONFIG_PASELECT_RFO;
    }
}

void SX1276SetAntSwLowPower( bool status )
{
    if ( RadioIsActive != status )
    {
        RadioIsActive = status;

        if ( status == false )
        {
            SX1276AntSwInit( );
        }
        else
        {
            SX1276AntSwDeInit( );
        }
    }
}

void SX1276AntSwInit( void )
{
//    GpioInit( &AntSwitchLf, RADIO_ANT_SWITCH_LF, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
//    GpioInit( &AntSwitchHf, RADIO_ANT_SWITCH_HF, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
}

void SX1276AntSwDeInit( void )
{
//    GpioInit( &AntSwitchLf, RADIO_ANT_SWITCH_LF, PIN_OUTPUT, PIN_OPEN_DRAIN, PIN_NO_PULL, 0 );
//    GpioInit( &AntSwitchHf, RADIO_ANT_SWITCH_HF, PIN_OUTPUT, PIN_OPEN_DRAIN, PIN_NO_PULL, 0 );
}

void SX1276SetAntSw( uint8_t rxTx )
{
    if ( rxTx != 0 ) // 1: TX, 0: RX
    {
//        GpioWrite( &AntSwitchLf, 0 );
//        GpioWrite( &AntSwitchHf, 1 );
    }
    else
    {
//        GpioWrite( &AntSwitchLf, 1 );
//        GpioWrite( &AntSwitchHf, 0 );
    }
}

bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}


/***************************** GPIO init *******************************/

void GpioInit( Gpio_t *obj, PinNames pin, PinModes mode,  PinConfigs config, PinTypes type, uint32_t value )
{
    GPIO_NAME_TO_PIN(obj, pin);
    gpio_init(*obj, mode, config, type, value);
}

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    gpio_interrupt_set(*obj, irqMode, irqPriority, irqHandler);
}

void GpioRemoveInterrupt( Gpio_t *obj )
{
    gpio_interrupt_clear(*obj);
}

void GpioWrite( Gpio_t *obj, uint32_t value )
{
    (value == 0) ? gpio_clr(*obj) : gpio_set(*obj);
}

void GpioToggle( Gpio_t *obj )
{
    gpio_toggle(*obj);
}

uint32_t GpioRead( Gpio_t *obj )
{
    return gpio_get(*obj);
}

/***************************** SPI init *******************************/


void SpiInit( Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss )
{
    spi_com_set(obj, SPI1);
    spi_disable(obj);
    spi_init(obj, mosi, miso, sclk, nss);

    GpioInit( &obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, GPIO_AF5_SPI1 );
    GpioInit( &obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, GPIO_AF5_SPI1 );
    GpioInit( &obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, GPIO_AF5_SPI1 );
    
    if ( nss != NC )
    {
        GpioInit( &obj->Nss, nss, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, GPIO_AF5_SPI1 );
    }
    else
    {
#ifdef PF_CORE_M3
        obj->Spi.Init.NSS = SPI_NSS_SOFT;
#endif
    }

    if ( nss == NC )
    {
        spi_format( obj, 0, 0, 0, 0 );
    }
    else
    {
        spi_format( obj, 0, 0, 0, 1 );
    }
    spi_frequency( obj, 10000000 );

    spi_enable(obj);
}

void SpiDeInit( Spi_t *obj )
{
    spi_deinit(obj);

    GpioInit( &obj->Mosi, (PinNames)obj->Mosi.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &obj->Miso, (PinNames)obj->Miso.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0 );
    GpioInit( &obj->Sclk, (PinNames)obj->Sclk.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &obj->Nss, (PinNames)obj->Nss.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
}

uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
    return spi_inout(obj, outData);
}





