/***************************************************************************
* File        : re46c191_arch.h
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/31
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/31         v0.1            wangjifang        first version
***************************************************************************/
#include "common/dev/dev.h"

#ifndef __RE46C191_ARCH_H
#define __RE46C191_ARCH_H

//TEST2 数据线定义
#define TEST2OUT    (P3DIR |= BIT7)
#define TEST2IN     (P3DIR &= ~BIT7)
#define TEST2H      (P3OUT |= BIT7)
#define TEST2L      (P3OUT &= ~BIT7)
#define TEST2SEL0   (P3SEL &= ~BIT7)
#define TEST2SEL1   (P3SEL |= BIT7)

//TESTBAT 数据线定义
#define TESTBATOUT       (P4DIR |= BIT1)
#define TESTBATIN        (P4DIR &= ~BIT1)
#define TESTBATH         (P4OUT |= BIT1)
#define TESTBATL         (P4OUT &= ~BIT1)
#define TESTBATSEL0      (P4SEL &= ~BIT1)
#define TESTBATSEL1      (P4SEL |= BIT1)

//TEST5V 数据线定义
#define TEST5VOUT       (P4DIR |= BIT2)
#define TEST5VIN        (P4DIR &= ~BIT2)
#define TEST5VH         (P4OUT |= BIT2)
#define TEST5VL         (P4OUT &= ~BIT2)
#define TEST5VSEL0      (P4SEL &= ~BIT2)
#define TEST5VSEL1      (P4SEL |= BIT2)

//FEED 数据线定义
#define FEEDOUT     (P4DIR |= BIT3)
#define FEEDIN      (P4DIR &= ~BIT3)
#define FEEDH       (P4OUT |= BIT3)
#define FEEDL       (P4OUT &= ~BIT3)
#define FEEDSEL0    (P4SEL &= ~BIT3)
#define FEEDSEL1    (P4SEL |= BIT3)

//IO  数据线定义
#define IOOUT       (P3DIR |= BIT6)
#define IOIN        (P3DIR &= ~BIT6)
#define IOH         (P3OUT |= BIT6)
#define IOL         (P3OUT &= ~BIT6)
#define IOSEL0      (P3SEL &= ~BIT6)
#define IOSEL1      (P3SEL |= BIT6)

//RGLEDCTL 数据线定义
#define RGLEDCTLOUT       (P2DIR |= BIT0)
#define RGLEDCTLIN        (P2DIR &= ~BIT0)
#define RGLEDCTLH         (P2OUT |= BIT0)
#define RGLEDCTLL         (P2OUT &= ~BIT0)
#define RGLEDCTLSEL0      (P2SEL &= ~BIT0)
#define RGLEDCTLSEL1      (P2SEL |= BIT0)

//IRCAP 数据线定义
#define IRCAPOUT        (P3DIR |= BIT1)
#define IRCAPIN         (P3DIR &= ~BIT1)
#define IRCAPH          (P3OUT |= BIT1)
#define IRCAPL          (P3OUT &= ~BIT1)
#define IRCAPSEL0       (P3SEL &= ~BIT1)
#define IRCAPSEL1       (P3SEL |= BIT1)

//VBSTCTL 数据线定义
#define VBSTCTLOUT       (P3DIR |= BIT2)
#define VBSTCTLIN        (P3DIR &= ~BIT2)
#define VBSTCTLH         (P3OUT |= BIT2)
#define VBSTCTLL         (P3OUT &= ~BIT2)
#define VBSTCTLSEL0      (P3SEL &= ~BIT2)
#define VBSTCTLSEL1      (P3SEL |= BIT2)

void re46c191_init(void);
void re46c191_deinit(void);
void re46c191_config_start(void);
void re46c191_data_save(void);
void re46c191_enter_mode(uint8_t mode_numb);
void re46c191_send_char(uint8_t data, uint8_t data_bit_numb);
void re46c191_sel_vbat(void);
void re46c191_config_stop(void);
void re46c191_test_pin_operar(void);
#endif