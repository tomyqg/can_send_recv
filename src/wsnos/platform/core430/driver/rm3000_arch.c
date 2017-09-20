/**
 * @brief       : this
 * @file        : rm3000_arch.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-04-13
 * change logs  :
 * Date       Version     Author        Note
 * 2016-04-13  v0.0.1  gang.cheng    first version
 */

#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "driver.h"
#include "rm3000_arch.h"

void rm3000_spi_init(void)
{
    //P9.0->SPI2_SCK;P9.4->SPI2_MOSI;P9.5->SPI2_MISO;P9.3->SPI2_CS;
    P9SEL |= (BIT0 + BIT4 + BIT5);
    P9SEL &= ~BIT3;

    P9DIR |= (BIT0 + BIT4 + BIT3);  //1->output
    P9DIR &= ~BIT5;                 //0->input

    UCA2CTL1 = UCSWRST;
    /*! SCLK常态低电平，上升沿采样，MSB在前，8位数据，主模式，3-pin SPI，同步模式*/
//    UCA1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
    UCA2CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
    UCA2CTL1 |= UCSSEL_2;               //  SMCLK

    UCA2BR0 = 2;
    UCA2BR1 = 0;                        //  SCLK频率是SMCLK / 8 = 8M/8=1M,
    UCA2IE &= ~(UCRXIE + UCTXIE);       //  禁止SPI中断

    UCA2CTL1 &= ~UCSWRST;

    //P9OUT |= BIT3;
    P9OUT &= ~BIT3;
}

void rm3000_int_init(void)
{
    //P1.2->INT;
    P1SEL &= ~BIT2;     //0->IO pin
    P1DIR &= ~BIT2;     //0->input

    P1IES &= ~BIT2;     // P1IFG flag is set with a low-to-hig transition.下降沿触发, ;
    P1IFG &= ~BIT2;     //P1IES的切换可能使P1IFG置位，需清除
    P1IE |= BIT2;       // Corresponding port interrupt enabled
}

void rm3000_clear_init(void)
{
    //P10.6->CLEAR;
    P10SEL &= ~BIT6;
    P10DIR |= BIT6;

    P10OUT &= ~BIT6;
}

void rm3000_clear_set(void)
{
    delay_us(1);
    P10OUT &= ~BIT6;
    delay_us(1);
    P10OUT |= BIT6;
    delay_us(2);    //*< min is 100ns
    P10OUT &= ~BIT6;
    delay_us(1);
}

uint8_t rm3000_spi_write_read(uint8_t val)
{
    RM3000_SPI_SEND_CHAR(val);

    return RM3000_SPI_RECEIVE_CHAR();
}

