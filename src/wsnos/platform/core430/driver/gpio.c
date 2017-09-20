/**
 * @brief       :
 *
 * @file        : gpio.c
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
#include "node_cfg.h"
#include "sys_arch/osel_arch.h"

static const uint16_t ptin [] = { P1IN_,  P2IN_,  P3IN_,  P4IN_,  P5IN_,
                                  P6IN_, P7IN_, P8IN_, P9IN_, P10IN_
                                };
static const uint16_t ptout[] = { P1OUT_, P2OUT_, P3OUT_, P4OUT_, P5OUT_,
                                  P6OUT_, P7OUT_, P8OUT_, P9OUT_, P10OUT_
                                };
static const uint16_t ptdir[] = { P1DIR_, P2DIR_, P3DIR_, P4DIR_, P5DIR_,
                                  P6DIR_, P7DIR_, P8DIR_, P9DIR_, P10DIR_
                                };
static const uint16_t ptren[] = { P1REN_, P2REN_, P3REN_, P4REN_, P5REN_,
                                  P6REN_, P7REN_, P8REN_, P9REN_, P10REN_
                                };
static const uint16_t ptsel[] = { P1SEL_, P2SEL_, P3SEL_, P4SEL_, P5SEL_,
                                  P6SEL_, P7SEL_, P8SEL_, P9SEL_, P10SEL_
                                };
static const uint16_t pties[] = { P1IES_, P2IES_,  };
static const uint16_t ptie [] = { P1IE_,  P2IE_ ,  };
static const uint16_t ptifg[] = { P1IFG_, P2IFG_,  };

static gpio_irq_handler_t gpio_irq_array[16];

void gpio_set(pin_id_t pin_id)
{
    POUT(pin_id.pin - 1) |= (0x01 << pin_id.bit);
}

void gpio_clr(pin_id_t pin_id)
{
    POUT(pin_id.pin - 1) &= ~(0x01 << pin_id.bit);
}

void gpio_toggle(pin_id_t pin_id)
{
    POUT(pin_id.pin - 1) ^= (0x01 << pin_id.bit);
}

void gpio_make_input(pin_id_t pin_id)
{
    PDIR(pin_id.pin - 1) &= ~(0x01 << pin_id.bit);
}

bool_t gpio_is_input(pin_id_t pin_id)
{
    return ((PDIR(pin_id.pin - 1) & (0x01 << pin_id.bit)) == 0);
}

void gpio_ren_set(pin_id_t pin_id)
{
    PREN(pin_id.pin - 1) |= (0x01 << pin_id.bit);
}

void gpio_ren_clr(pin_id_t pin_id)
{
    PREN(pin_id.pin - 1) &= ~(0x01 << pin_id.bit);
}


void gpio_make_output(pin_id_t pin_id)
{
    PDIR(pin_id.pin - 1) |= (0x01 << pin_id.bit);
}

bool_t gpio_is_output(pin_id_t pin_id)
{
    return ((PDIR(pin_id.pin - 1) & (0x01 << pin_id.bit)) != 0);
}

void gpio_sel(pin_id_t pin_id)
{
    PSEL(pin_id.pin - 1) |= (0x01 << pin_id.bit);
}

bool_t gpio_get(pin_id_t pin_id)
{
    return  ((PIN(pin_id.pin - 1)) & (0x01 << pin_id.bit));
}

void gpio_interrupt_enable(pin_id_t pin_id)
{
    PIE(pin_id.pin - 1) |= (0x01 << pin_id.bit);
}

void gpio_interrupt_disable(pin_id_t pin_id)
{
    PIE(pin_id.pin - 1) &= ~(0x01 << pin_id.bit);
}

void gpio_interrupt_edge(pin_id_t pin_id, uint8_t edge)
{
    if (edge == 0)
    {
        PIES(pin_id.pin - 1) &= ~(0x01 << pin_id.bit);
    }
    else
    {
        PIES(pin_id.pin - 1) |= (0x01 << pin_id.bit);
    }
}

void gpio_interrupt_clear(pin_id_t pin_id)
{
    PIFG(pin_id.pin - 1) &= ~(0x01 << pin_id.bit);
}

void gpio_interrupt_handle(uint8_t num)
{
    if(gpio_irq_array[num] != NULL)
        gpio_irq_array[num]();
}

void gpio_interrupt_set(pin_id_t pin_id, irq_mode_e irq_mode, irq_prioritie_e irq_prio, gpio_irq_handler_t irq_handler)
{
    if( irq_mode == IRQ_RISING_EDGE)
        gpio_interrupt_edge(pin_id, 0);
    else if(irq_mode == IRQ_FALLING_EDGE)
        gpio_interrupt_edge(pin_id, 1);

    gpio_interrupt_clear(pin_id);
    gpio_interrupt_enable(pin_id);

    uint16_t irq_num = ((pin_id.pin-1)<< 3) + pin_id.bit;
    if(irq_num <= 15)
        gpio_irq_array[irq_num] = irq_handler;
}

bool_t gpio_init(pin_id_t pin_id, pin_mode_e mode, pin_config_e config, pin_type_e type, uint32_t value)
{
    if(mode == PIN_INPUT)
    {
        gpio_make_input(pin_id);
        if(config == PIN_PUSH_PULL)
        {
            gpio_ren_set(pin_id);
            if(type == PIN_PULL_UP)
                gpio_set(pin_id);
            else if(type == PIN_PULL_DOWN)
                gpio_clr(pin_id);
        }
    }
    else if(mode == PIN_OUTPUT)
    {
        gpio_make_output(pin_id);
        
        if(value)
            gpio_set(pin_id);
        else
            gpio_clr(pin_id);
    }
    else if(mode == PIN_ALTERNATE_FCT)
    {
        gpio_sel(pin_id);
        if(value)
            gpio_make_output(pin_id);
        else
            gpio_make_input(pin_id);
    }
    
    return TRUE;
}

void gpio_deinit(pin_id_t pin_id)
{
    gpio_make_input(pin_id);
    gpio_ren_clr(pin_id);
    gpio_clr(pin_id);
}


#pragma vector =PORT1_VECTOR
__interrupt void port1_int_handle(void)
{
    OSEL_ISR_ENTRY();

    if ( (P1IFG & BIT5) == BIT5)
    {
        P1IFG &= ~BIT5;
        gpio_interrupt_handle(PA_5);
    }
    if ( (P1IFG & BIT6) == BIT6)
    {
        P1IFG &= ~BIT6;
        gpio_interrupt_handle(PA_6);
    }
    if ( (P1IFG & BIT7) == BIT7)
    {
        P1IFG &= ~BIT7;
        gpio_interrupt_handle(PA_7);
    }

    if ((P1IFG & BIT2) == BIT2)
    {
        P1IFG &= ~BIT2;
        gpio_interrupt_handle(PA_2);
    }
    OSEL_ISR_EXIT();
    LPM3_EXIT;
}

#pragma vector =PORT2_VECTOR
__interrupt void port2_int_handle(void)
{
    OSEL_ISR_ENTRY();

    extern void sx127x_int_handler(uint16_t time);
//    sx127x_int_handler(TA0R);

    if ( (P2IFG & BIT0) == BIT0)
    {
        P2IFG &= ~BIT0;
        gpio_interrupt_handle(PA_8);
    }

    if ( (P2IFG & BIT4) == BIT4)
    {
        P2IFG &= ~BIT4;
        gpio_interrupt_handle(PA_12);
    }

    OSEL_ISR_EXIT();
    LPM3_EXIT;
}

