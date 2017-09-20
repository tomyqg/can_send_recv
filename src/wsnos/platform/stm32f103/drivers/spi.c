/**
 * @brief       : this
 * @file        : spi.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-04-24
 * change logs  :
 * Date       Version     Author        Note
 * 2017-04-24  v0.0.1  gang.cheng    first version
 */
#include "driver.h"
#include "common/lib/lib.h"


void spi_com_set(Spi_t *spi, SPI_TypeDef *com)
{
    spi->Spi.Instance = ( SPI_TypeDef *) com;
}

void spi_init(Spi_t *spi,
              pin_name_e mosi,
              pin_name_e miso,
              pin_name_e sclk,
              pin_name_e nss)
{
    __HAL_RCC_SPI1_CLK_ENABLE( );
}

void spi_deinit(Spi_t *spi)
{
    HAL_SPI_DeInit( &spi->Spi );
}

void spi_disable(Spi_t *spi)
{
    __HAL_RCC_SPI1_FORCE_RESET( );
    __HAL_RCC_SPI1_RELEASE_RESET( );
}

void spi_enable(Spi_t *spi)
{
    HAL_SPI_Init( &spi->Spi );
}


void spi_format( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave )
{
    obj->Spi.Init.Direction = SPI_DIRECTION_2LINES;
    if ( bits == SPI_DATASIZE_8BIT )
    {
        obj->Spi.Init.DataSize = SPI_DATASIZE_8BIT;
    }
    else
    {
        obj->Spi.Init.DataSize = SPI_DATASIZE_16BIT;
    }
    obj->Spi.Init.CLKPolarity = cpol;
    obj->Spi.Init.CLKPhase = cpha;
    obj->Spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    obj->Spi.Init.TIMode = SPI_TIMODE_DISABLE;
    obj->Spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    obj->Spi.Init.CRCPolynomial = 7;

    if ( slave == 0 )
    {
        obj->Spi.Init.Mode = SPI_MODE_MASTER;
    }
    else
    {
        obj->Spi.Init.Mode = SPI_MODE_SLAVE;
    }
}

__STATIC_INLINE uint8_t __ffs( uint32_t value )
{
    return ( uint32_t )( 32 - __CLZ( value & ( -value ) ) );
}

void spi_frequency( Spi_t *obj, uint32_t hz )
{
    uint32_t divisor;

    divisor = SystemCoreClock / hz;

    // Find the nearest power-of-2
    divisor = divisor > 0 ? divisor - 1 : 0;
    divisor |= divisor >> 1;
    divisor |= divisor >> 2;
    divisor |= divisor >> 4;
    divisor |= divisor >> 8;
    divisor |= divisor >> 16;
    divisor++;

    divisor = __ffs( divisor ) - 1;

    divisor = ( divisor > 0x07 ) ? 0x07 : divisor;

    obj->Spi.Init.BaudRatePrescaler = divisor << 3;
}

FlagStatus SpiGetFlag( Spi_t *obj, uint16_t flag )
{
    FlagStatus bitstatus = RESET;

    // Check the status of the specified SPI flag
    if ( ( obj->Spi.Instance->SR & flag ) != ( uint16_t )RESET )
    {
        // SPI_I2S_FLAG is set
        bitstatus = SET;
    }
    else
    {
        // SPI_I2S_FLAG is reset
        bitstatus = RESET;
    }
    // Return the SPI_I2S_FLAG status
    return  bitstatus;
}

uint16_t spi_inout( Spi_t *obj, uint16_t out_data )
{
    uint8_t rxData = 0;

    if ( ( obj == NULL ) || ( obj->Spi.Instance ) == NULL )
    {
        assert_param( FAIL );
    }

    __HAL_SPI_ENABLE( &obj->Spi );

    while ( SpiGetFlag( obj, SPI_FLAG_TXE ) == RESET );
    obj->Spi.Instance->DR = ( uint16_t ) ( out_data & 0xFF );

    while ( SpiGetFlag( obj, SPI_FLAG_RXNE ) == RESET );
    rxData = ( uint16_t ) obj->Spi.Instance->DR;

    return ( rxData );
}