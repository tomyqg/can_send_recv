/**
 * @brief       : 
 *
 * @file        : timer.c
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */

#include "driver.h"
#include "sys_arch/osel_arch.h"

void HAL_Delay(uint32_t Delay)
{
    for(uint32_t i=0;i<Delay;i++)
    {
        delay_ms(1);
    }
}

extern volatile uint16_t backvalue_wdt_cnt;
extern volatile uint16_t data_send_wdt_cnt;

void htimer_int_handler(void);
#pragma vector = TIMER0_A1_VECTOR
__interrupt void timer0_a1_isr(void)
{
    OSEL_ISR_ENTRY();

    switch (__even_in_range(TA0IV, 14))
    {
    case TA0IV_TA0CCR1:
        if(backvalue_wdt_cnt-- == 0)
        {
            TA0CCTL1 &= ~CCIE; DBG_ASSERT(FALSE __DBG_LINE);
        }            
        TA0CCR1 += 0x8000; // 1s
        break;

    case TA0IV_TA0CCR2:
        if (data_send_wdt_cnt-- == 0)
        {
            TA0CCTL2 &= ~CCIE; DBG_ASSERT(FALSE __DBG_LINE);
        }
        TA0CCR2 += 0x8000; // 1s
        break;

    case TA0IV_TA0CCR3:
        TA0CCR3 = TA0R + 0x50a; // 40ms - 0x50a
        wsnos_ticks();
        break;

    case TA0IV_TA0CCR4:
        htimer_int_handler();
        break;

    case TA0IV_TA0IFG:
        break;

    default:
        break;
    }

    OSEL_ISR_EXIT();
    LPM3_EXIT;
}


