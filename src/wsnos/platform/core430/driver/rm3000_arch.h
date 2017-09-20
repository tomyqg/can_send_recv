/**
 * @brief       : this
 * @file        : rm3000_arch.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-04-13
 * change logs  :
 * Date       Version     Author        Note
 * 2016-04-13  v0.0.1  gang.cheng    first version
 */
#ifndef __RM3000_ARCH_H__
#define __RM3000_ARCH_H__

#define RM3000_SPI_BEGIN()          P9OUT |= BIT3;delay_us(1);P9OUT &= ~BIT3;
#define RM3000_SPI_END()            P9OUT |= BIT3;delay_us(1);P9OUT &= ~BIT3;

#define RM3000_SPI_SEND_CHAR(x)                     \
    do                                              \
    {                                               \
        while (!(UCA2IFG&UCTXIFG));                 \
        UCA2TXBUF = (x);                            \
        while (!(UCA2IFG&UCTXIFG));                 \
        while (!(UCA2IFG&UCRXIFG));                 \
    } while(__LINE__ == -1)

#define RM3000_SPI_RECEIVE_CHAR()   (UCA2RXBUF)


void rm3000_spi_init(void);

void rm3000_int_init(void);

void rm3000_clear_init(void);

void rm3000_clear_set(void);

uint8_t rm3000_spi_write_read(uint8_t val);

#endif
