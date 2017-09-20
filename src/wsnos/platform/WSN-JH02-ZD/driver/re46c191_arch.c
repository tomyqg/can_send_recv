/***************************************************************************
* File        : re46c191_arch.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/11/1
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/11/1         v0.1            wangjifang        first version
***************************************************************************/


#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "driver.h"
#include "re46c191_arch.h"

#define TEST_DELAY                  (300u)              //min pw3 = 100us
#define FEED_DELAY                  (30u)               //min pw1 = 10us
#define IO_DELAY                    (10000u)            //min pw2 = 10ms

void re46c191_sel_vbat(void)
{
    TESTBATH;
    TEST5VL;
}

static void re46c191_sel_5v(void)
{
    TESTBATL;
    TEST5VH;
}

void re46c191_config_start(void)
{
    IRCAPH;
    RGLEDCTLH;
    VBSTCTLH;
    delay_us(TEST_DELAY);
    TEST2H;
}

void re46c191_config_stop(void)
{
    delay_us(TEST_DELAY);
    TEST2L;
}

void re46c191_data_save(void)
{
    delay_us(TEST_DELAY);
    IOH;
    delay_us(IO_DELAY);
    IOL;
}

void re46c191_enter_mode(uint8_t mode_numb)
{
    re46c191_sel_5v();
    for(uint8_t i=0; i<mode_numb; i ++)
    {
        TEST5VH;
        delay_us(TEST_DELAY);
        TEST5VL;
        delay_us(TEST_DELAY);
    }
}

void re46c191_send_char(uint8_t data, uint8_t data_bit_numb)
{
    uint8_t tmp;
    for(tmp = 0;tmp < data_bit_numb;tmp ++)
    {
        if(data & 0x01)
        {
            TESTBATH;
        }
        else
        {
            TESTBATL;
        }
        delay_us(TEST_DELAY);
        FEEDH;
        delay_us(FEED_DELAY);
        FEEDL;
        delay_us(TEST_DELAY);
        TESTBATL;
        data >>= 1;
        delay_us(TEST_DELAY*3);
    }
}

void re46c191_init(void)
{
    TEST2SEL0;
    TEST2OUT;
    TEST2L;
    
    TESTBATSEL0;
    TESTBATOUT;
    TESTBATL;
    
    TEST5VSEL0;
    TEST5VOUT;
    TEST5VL;
    
    FEEDSEL0;
    FEEDOUT;
    FEEDL;
    
    IOSEL0;
    IOOUT;
    IOL;
    
    RGLEDCTLSEL0;
    RGLEDCTLOUT;
    RGLEDCTLL;
    
    IRCAPSEL0;
    IRCAPOUT;
    IRCAPL;
    
    VBSTCTLSEL0;
    VBSTCTLOUT;
    VBSTCTLL;
}

void re46c191_deinit(void)
{
    TEST2SEL0;
    TEST2IN;

    TESTBATSEL0;
    TESTBATIN;
    
    TEST5VSEL0;
    TEST5VIN;
    
    FEEDSEL0;
    FEEDIN;
    
    IOSEL0;
    IOIN;
    
    RGLEDCTLSEL0;
    RGLEDCTLIN;
    
    IRCAPSEL0;
    IRCAPIN;
    
    VBSTCTLSEL0;
    VBSTCTLIN;
}

void re46c191_test_pin_operar(void)
{
    re46c191_sel_vbat();
    delay_ms(50);
    TESTBATL;
    TEST5VL;
}

