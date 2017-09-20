/***************************************************************************
* File        : sht21_arch.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/

#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "driver.h"
#include "sht21_arch.h"

static void sht20_i2c_lock_init(void)
{
    //P5.4 作为模拟scl，输出9个信号
    P9SEL &= ~BIT6;
	P9DIR |= BIT6;
    P9OUT |= BIT6;
    // 主设备模拟SCL，从高到低，输出9次，使得从设备释放SDA
    for(uint8_t i=0;i<9;i++)
    {
        P9OUT |= BIT6;
        delay_ms(1);
        P9OUT &= ~BIT6;
        delay_ms(1);
    }
}

void sht20_i2c_init(void)
{
    sht20_i2c_lock_init();
    
    P9SEL |= (BIT5 + BIT6);

    UCB2CTL1 |= UCSWRST;
    UCB2CTL0 = UCMST + UCMODE_3 + UCSYNC ; // I2C Master, synchronous mode
    UCB2CTL1 |= UCSSEL_2; // Use SMCLK, keep SW reset
    UCB2BR0 = 40; // fSCL = SMCLK/40 = 200kHz
    UCB2BR1 = 0;
    UCB2CTL0 &= ~UCSLA10; //7 bits
    //UCB1I2CSA = SHT20_ADDRESS;      // IIC address write when iic start
    UCB2CTL1 &= ~UCSWRST;
}

void sht20_i2c_deinit(void)
{
    P9SEL &= ~(BIT5 + BIT6);
    P9DIR &= ~(BIT5 + BIT6);
}

void sht20_pow_open(bool_t para)
{
    P4SEL &= ~BIT6;
    P4DIR |= BIT6;
    if(para == TRUE)
    {
        P4OUT |= BIT6;
    }
    else
    {
        P4OUT &= ~BIT6;
    }
}