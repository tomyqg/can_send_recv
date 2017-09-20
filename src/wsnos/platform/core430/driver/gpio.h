/**
 * provides an abstraction for general-purpose I/O.
 *
 * @file hal_board.h
 * @author wanger
 *
 * @addtogroup HAL_GPIO HAL General Purpose I/O
 * @ingroup HAL
 * @{
 */

#ifndef __GPIO_H
#define __GPIO_H

#include "common/lib/data_type_def.h"

#define PIN(x)                  (*(volatile uint8_t*) ptin [(x)])
#define POUT(x)                 (*(volatile uint8_t*) ptout[(x)])
#define PDIR(x)                 (*(volatile uint8_t*) ptdir[(x)])
#define PREN(x)                 (*(volatile uint8_t*) ptren[(x)])
#define PSEL(x)                 (*(volatile uint8_t*) ptsel[(x)])
#define PIES(x)                 (*(volatile uint8_t*) pties[(x)])
#define PIE(x)                  (*(volatile uint8_t*) ptie [(x)])
#define PIFG(x)                 (*(volatile uint8_t*) ptifg[(x)])

/* gpio define */
#define P1IN_                   PAIN_
#define P1OUT_                  PAOUT_
#define P1DIR_                  PADIR_
#define P1REN_                  PAREN_
#define P1SEL_                  PASEL_
#define P1IES_                  PAIES_
#define P1IE_                   PAIE_
#define P1IFG_                  PAIFG_
#define P2IN_                   (PAIN_ +1)
#define P2OUT_                  (PAOUT_+1)
#define P2DIR_                  (PADIR_+1)
#define P2REN_                  (PAREN_+1)
#define P2SEL_                  (PASEL_+1)
#define P2IES_                  (PAIES_+1)
#define P2IE_                   (PAIE_ +1)
#define P2IFG_                  (PAIFG_+1)

#define P3IN_                   PBIN_
#define P3OUT_                  PBOUT_
#define P3DIR_                  PBDIR_
#define P3REN_                  PBREN_
#define P3SEL_                  PBSEL_
#define P3IES_                  PBIES_
#define P3IE_                   PBIE_
#define P3IFG_                  PBIFG_

#define P4IN_                   (PBIN_ +1)
#define P4OUT_                  (PBOUT_+1)
#define P4DIR_                  (PBDIR_+1)
#define P4REN_                  (PBREN_+1)
#define P4SEL_                  (PBSEL_+1)
#define P4IES_                  (PBIES_+1)
#define P4IE_                   (PBIE_ +1)
#define P4IFG_                  (PBIFG_+1)

#define P5IN_                   PCIN_
#define P5OUT_                  PCOUT_
#define P5DIR_                  PCDIR_
#define P5REN_                  PCREN_
#define P5SEL_                  PCSEL_
#define P6IN_                   (PCIN_ +1)
#define P6OUT_                  (PCOUT_+1)
#define P6DIR_                  (PCDIR_+1)
#define P6REN_                  (PCREN_+1)
#define P6SEL_                  (PCSEL_+1)

#define P7IN_                   PDIN_
#define P7OUT_                  PDOUT_
#define P7DIR_                  PDDIR_
#define P7REN_                  PDREN_
#define P7SEL_                  PDSEL_
#define P8IN_                   (PDIN_ +1)
#define P8OUT_                  (PDOUT_+1)
#define P8DIR_                  (PDDIR_+1)
#define P8REN_                  (PDREN_+1)
#define P8SEL_                  (PDSEL_+1)

#define P9IN_                   PEIN_
#define P9OUT_                  PEOUT_
#define P9DIR_                  PEDIR_
#define P9REN_                  PEREN_
#define P9SEL_                  PESEL_
#define P10IN_                  (PEIN_ + 1)
#define P10OUT_                 (PEOUT_+ 1)
#define P10DIR_                 (PEDIR_+ 1)
#define P10REN_                 (PEREN_+1)
#define P10SEL_                 (PESEL_+ 1)


/*!
 * MSP430 Pin Names
 */
//p1.0~p1.7     PA_0~PA_7
//p2.0~p2.7     PA_8~PA_15
//p3.0~p3.7     PB_0~PB_7
//p4.0~p4.7     PB_8~PB_15
//p5.0~p5.7     PC_0~PC_7
//p6.0~p6.7     PC_8~PC_15
//p7.0~p7.7     PD_0~PD_7
//p8.0~p8.7     PD_8~PD_15
//p9.0~p9.7     PE_0~PE_7
//p10.0~p10.7   PE_8~PE_15

#define MCU_PINS \
    PA_0 = 0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7, PA_8, PA_9, PA_10, PA_11, PA_12, PA_13, PA_14, PA_15, \
    PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7, PB_8, PB_9, PB_10, PB_11, PB_12, PB_13, PB_14, PB_15,     \
    PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7, PC_8, PC_9, PC_10, PC_11, PC_12, PC_13, PC_14, PC_15,     \
    PD_0, PD_1, PD_2, PD_3, PD_4, PD_5, PD_6, PD_7, PD_8, PD_9, PD_10, PD_11, PD_12, PD_13, PD_14, PD_15,     \
    PE_0, PE_1, PE_2, PE_3, PE_4, PE_5, PE_6, PE_7, PE_8, PE_9, PE_10, PE_11, PE_12, PE_13, PE_14, PE_15,     \
    PF_0, PF_1, PF_2, PF_3, PF_4, PF_5, PF_6, PF_7, PF_8, PF_9, PF_10, PF_11, PF_12, PF_13, PF_14, PF_15,     \
    PH_0, PH_1, PH_2, PH_3, PH_4, PH_5, PH_6, PH_7, PH_8, PH_9, PH_10, PH_11, PH_12, PH_13, PH_14, PH_15
    

typedef enum {
    PORT1 = 0,
    PORT2,
    PORT3,
    PORT4,
    PORT5,
    PORT6,
} port_e;

/*!
 * Board GPIO pin names
 */
typedef enum {
    MCU_PINS,
    IOE_PINS,

    // Not connected
    NC = (int)0xFFFFFFFF
} pin_name_e;

typedef struct __pin_id_t
{
    uint16_t pin;
    uint16_t bit;
    void *port;
} pin_id_t;

#define GPIO_NAME_TO_PIN(pin_id, name)              \
    (pin_id)->pin = (name >> 3) + 1;                \
    (pin_id)->bit = (name & 0x07);            

    

/*!
 * Operation Mode for the GPIO
 */
typedef enum
{
    PIN_INPUT = 0,
    PIN_OUTPUT,
    PIN_ALTERNATE_FCT,
    PIN_ANALOGIC
} pin_mode_e;

/*!
 * Add a pull-up, a pull-down or nothing on the GPIO line
 */
typedef enum
{
    PIN_NO_PULL = 0,
    PIN_PULL_UP,
    PIN_PULL_DOWN
} pin_type_e;

/*!
 * Define the GPIO as Push-pull type or Open Drain
 */
typedef enum
{
    PIN_PUSH_PULL = 0,
    PIN_OPEN_DRAIN
} pin_config_e;

/*!
 * Define the GPIO IRQ on a rising, falling or both edges
 */
typedef enum
{
    NO_IRQ = 0,
    IRQ_RISING_EDGE,
    IRQ_FALLING_EDGE,
    IRQ_RISING_FALLING_EDGE
} irq_mode_e;

/*!
 * Define the IRQ priority on the GPIO
 */
typedef enum
{
    IRQ_VERY_LOW_PRIORITY = 0,
    IRQ_LOW_PRIORITY,
    IRQ_MEDIUM_PRIORITY,
    IRQ_HIGH_PRIORITY,
    IRQ_VERY_HIGH_PRIORITY
} irq_prioritie_e;

/*!
 * GPIO IRQ handler function prototype
 */
typedef void( *gpio_irq_handler_t )( void );


/**
 * pullup the corresponding bit of corresponding output pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_set(pin_id_t pin_id);

/**
 * pulldown the corresponding bit of corresponding output pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_clr(pin_id_t pin_id);

/**
 * toggle the corresponding bit of corresponding output pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_toggle(pin_id_t pin_id);

/**
 * switch to input direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_make_input(pin_id_t pin_id);

/**
 * judge corresponding I/O pin's direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 *
 * @return TRUE or FALSE
 */
bool_t gpio_is_input(pin_id_t pin_id);

/**
 * switch to output direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_make_output(pin_id_t pin_id);

/**
 * judge corresponding I/O pin's direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 *
 * @return TRUE or FALSE
 */
bool_t gpio_is_output(pin_id_t pin_id);

/**
 * Port Select
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_sel(pin_id_t pin_id);

/**
 * get the value of corresponding bit of input register
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 *
 * @return value of the bit
 */
bool_t gpio_get(pin_id_t pin_id);

/**
 * enable interrupt
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_interrupt_enable(pin_id_t pin_id);

/**
 * disable interrupt
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_interrupt_disable(pin_id_t pin_id);

/**
 * selects the interrupt edge for the corresponding I/O pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_interrupt_edge(pin_id_t pin_id, uint8_t edge);

/**
 * clear the interrupt flag for the corresponding I/O pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void gpio_interrupt_clear(pin_id_t pin_id);

void gpio_interrupt_handle(uint8_t num);

void gpio_interrupt_set(pin_id_t pin_id, irq_mode_e irq_mode, irq_prioritie_e irq_prio, gpio_irq_handler_t irq_handler);

/*!
 * \brief Initializes the given GPIO object
 *
 * \param [IN] obj    Pointer to the GPIO object
 * \param [IN] mode   Pin mode [PIN_INPUT, PIN_OUTPUT,
 *                              PIN_ALTERNATE_FCT, PIN_ANALOGIC]
 * \param [IN] config Pin config [PIN_PUSH_PULL, PIN_OPEN_DRAIN]
 * \param [IN] type   Pin type [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 * \param [IN] value  Default output value at initialisation
 */
bool_t gpio_init(pin_id_t pin_id, pin_mode_e mode, pin_config_e config, pin_type_e type, uint32_t value);


/**
 * DeInit I/O
 */
void gpio_deinit(pin_id_t pin_id);

#endif


/**
 * @}
 */

