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

void spi_com_set(Spi_t *spi, uint8_t com)
{
    spi->Spi = com;
}

void spi_init(Spi_t *spi,
              pin_name_e mosi,
              pin_name_e miso,
              pin_name_e sclk,
              pin_name_e nss)
{
    
}

void spi_deinit(Spi_t *spi)
{
    ;
}

void spi_disable(Spi_t *spi)
{
    if (spi->Spi == SPI1)
    {
        UCB1CTL1 |= UCSWRST;    //*< put eUSCI_B in reset state
    }
}

void spi_enable(Spi_t *spi)
{
    if (spi->Spi == SPI1)
    {
        UCB1IE &= ~(UCRXIE + UCTXIE);   //*< stop spi interrupt
        UCB1CTL1 &= ~UCSWRST;
    }
}

void spi_format(Spi_t *spi, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave)
{
    uint8_t set_value = 0;
    set_value |= UCMSB + UCSYNC;
    
    if (bits == 1)
    {
        set_value |= UC7BIT;
    }
    
    if (cpol)
    {
        set_value |= UCCKPL;
    }
    if (cpha)
    {
        set_value |= UCCKPH;
    }
    if (slave == 0)
    {
        set_value |= UCMST;
    }
    
    if (spi->Spi == SPI1)
    {
        UCB1CTL1 |= UCSSEL_2;   //*< SMCLK
        UCB1CTL0 = set_value;
    }
}

void spi_frequency( Spi_t *spi, uint32_t hz )
{
    uint16_t baud = get_system_clock() / hz;
    uint8_t baud_h = baud >> 8;
    uint8_t baud_l = (uint8_t)(baud & 0x00FF);
    
    if (spi->Spi == SPI1)
    {
        UCB0BR0 = baud_l;
        UCB0BR1 = baud_h;
    }
}

uint16_t spi_inout( Spi_t *spi, uint16_t out_data )
{
    if (spi->Spi == SPI1)
    {
        while (!(UCB1IFG&UCTXIFG));
        UCB1TXBUF = out_data;
        while (!(UCB1IFG&UCRXIFG));
        return (uint16_t)UCB1RXBUF;
    }
    else
    {
        return 0;
    }
}