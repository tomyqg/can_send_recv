/**
 * @brief       : this
 * @file        : sx1278_arch.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-01-14
 * change logs  :
 * Date       Version     Author        Note
 * 2016-01-14  v0.0.1  gang.cheng    first version
 */

#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "driver.h"
#include "sx1278_arch.h"

void pe42421_power_on(void)
{
    // P4.7
    //set UNB pin to 1
    P1SEL &= ~BIT0;
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    delay_us(5);
}

void pe42421_power_off(void)
{
    // P4.7
    //set UNB pin to 0
    P1OUT &= ~BIT0;
}

void sx127x_power_on(void)
{
    P9SEL &= ~BIT0;
    P9DIR |= BIT0;
    P9OUT |= BIT0;
}

void sx127x_power_off(void)
{
    P9SEL &= ~BIT0;
    P9DIR |= BIT0;
    P9OUT &= ~BIT0;
}

void sx127x_reset(void)
{
     //P8.7->SX_RXTX -- 输入内部下拉处理
    P8SEL &= ~BIT7;//0->IO pin
    P8DIR &= ~BIT7;//0->input
    P8REN |= BIT7;
    P8OUT &= ~BIT7;

    sx127x_power_on();
    pe42421_power_on();
    
    // P1.6
    //set reset pin to 0
    P8SEL &= ~BIT0;
    P8DIR |= BIT0;
    P8OUT &= ~BIT0;

    delay_ms(1);
    P8OUT |= BIT0;
    delay_ms(11);

    //set reset pin as input
    P8DIR &= ~BIT0;

}

void sx127x_spi_init(void)
{
    //P8.1->SPI1_SCK;P8.2->SPI1_MOSI;P8.3->SPI1_MISO;P8.4->SPI1_CS;
    //P8.4->SPI1_SCK;P8.5->SPI1_MOSI;P8.6->SPI1_MISO;P8.1->SPI1_CS;
    P8SEL |= (BIT1 + BIT2 + BIT3);		
    P8SEL &= ~BIT4;	
    
    P8DIR |= (BIT1 + BIT2 + BIT4);//1->output
    P8DIR &= ~BIT3;//0->input
    
    UCA1CTL1 = UCSWRST;
    /*! SCLK常态低电平，上升沿采样，MSB在前，8位数据，主模式，3-pin SPI，同步模式*/
//    UCA1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
//    UCA1CTL0 = UCCKPL + UCMSB + UCMST + UCMODE_0 + UCSYNC;
    UCA1CTL0 = UCMSB + UCMST + UCMODE_0 + UCSYNC;
    UCA1CTL1 |= UCSSEL_2;               //  SMCLK
 
    UCA1BR0 = 8;
    UCA1BR1 = 0;						//  SCLK频率是SMCLK / 2
    UCA1IE &= ~(UCRXIE + UCTXIE);	  	//  禁止SPI中断
    UCA1CTL1 &= ~UCSWRST;
    
    P8OUT &= ~BIT4;
}


void sx127x_dio_irq_init(void)
{
    //DIO0->IT1/P1.7;DIO1->IT2//P1.6;DIO2->IT3//P1.5;DIO3->IT3//P1.4;
    P1SEL &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);//0->IO pin
    P1DIR &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);//0->input
    P1REN |=  (BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    P1OUT &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
    
    P1IES &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);        // P1IFG flag is set with a high-to-low transition.下降沿触发, ;
    P1IFG &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);        //P1IES的切换可能使P1IFG置位，需清除
    P1IE |= (BIT3 + BIT4 + BIT5 + BIT6 + BIT7);          // Corresponding port interrupt enabled
}

void sx127x_dio_irq_enable(void)
{
    P1IE |= (BIT3 + BIT4 + BIT5 + BIT6 + BIT7);         // Corresponding port interrupt enabled
    P1IFG &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);       //P1IES的切换可能使P1IFG置位，需清除
}
void sx127x_dio_irq_disable(void)
{
    P1IE &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);      // Corresponding port interrupt disable
    P1IFG &= ~(BIT3 + BIT4 + BIT5 + BIT6 + BIT7);     //P1IES的切换可能使P1IFG置位，需清除
}

uint8_t sx127x_spi_write_read(uint8_t val)
{
	SX127X_SPI_SEND_CHAR(val);

	return SX127X_SPI_RECEIVE_CHAR();
}

void sx127x_ant_sw_init(void)
{
	;
}

void sx127x_ant_sw_deinit(void)
{
	;
}

void sx127x_ant_sw_set(uint8_t rx_tx)
{
	if(rx_tx == 1)	//1: tx, 0: rx
	{
		;
	}
	else
	{
		;
	}
}

void rf_int_handler(uint16_t time)
{

}
#ifdef USE_SSN
#pragma vector = PORT1_VECTOR
__interrupt void port1_int_handle(void)
{
    OSEL_ISR_ENTRY();
    
//    extern void sx127x_int_handler(uint16_t time);
//    sx127x_int_handler(TA0R);
    
    if( (P1IFG & BIT7) == BIT7)
    {
        P1IFG &= ~BIT7;
        extern void sx127x_dio0_irq(uint16_t time);
        sx127x_dio0_irq(TA0R);
    }
    
    if( (P1IFG & BIT6) == BIT6)
    {
        P1IFG &= ~BIT6;
        extern void sx127x_dio1_irq(uint16_t time);
        sx127x_dio1_irq(TA0R);
    }
    
    if( (P1IFG & BIT5) == BIT5)
    {
        P1IFG &= ~BIT5;
        extern void sx127x_dio2_irq(uint16_t time);
        sx127x_dio2_irq(TA0R);
    }
    if( (P1IFG & BIT4) == BIT4)
    {
        P1IFG &= ~BIT4;
        extern void sx127x_dio3_irq(uint16_t time);
        sx127x_dio3_irq(TA0R);
    }
    
    if( (P1IFG & BIT3) == BIT3)
    {
        P1IFG &= ~BIT3;
        extern void sx127x_dio4_irq(uint16_t time);
        sx127x_dio4_irq(TA0R);
    }
    
    OSEL_ISR_EXIT();
    LPM3_EXIT;
}
#endif
