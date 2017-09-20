/**
 * @brief       : provides an abstraction for peripheral device.
 *
 * @file        : hal_gpio.h
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2015/5/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */

#ifndef __HAL_GPIO_H
#define __HAL_GPIO_H

#include "common/lib/lib.h"

typedef pin_id_t                hal_pin_id_t;
typedef pin_name_e              hal_pin_name_t;
typedef pin_mode_e              hal_pin_mode_t;
typedef pin_type_e              hal_pin_type_t;
typedef pin_config_e            hal_pin_conf_t;
typedef irq_mode_e              hal_irq_mode_t;
typedef irq_prioritie_e         hal_irq_prio_t;

/**
 * pullup the corresponding bit of corresponding output pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_set(hal_pin_id_t pin_id);

/**
 * pulldown the corresponding bit of corresponding output pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_clr(hal_pin_id_t pin_id);

/**
 * toggle the corresponding bit of corresponding output pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_toggle(hal_pin_id_t pin_id);

/**
 * switch to input direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_make_input(hal_pin_id_t pin_id);

/**
 * judge corresponding I/O pin's direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 *
 * @return TRUE or FALSE
 */
bool_t hal_gpio_is_input(hal_pin_id_t pin_id);

/**
 * switch to output direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_make_output(hal_pin_id_t pin_id);

/**
 * judge corresponding I/O pin's direction
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 *
 * @return TRUE or FALSE
 */
bool_t hal_gpio_is_output(hal_pin_id_t pin_id);

/**
 * Port Select
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_sel(hal_pin_id_t pin_id);

/**
 * get the value of corresponding bit of input register
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 *
 * @return value of the bit
 */
bool_t hal_gpio_get(hal_pin_id_t pin_id);

/**
 * enable interrupt
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_interrupt_enable(hal_pin_id_t pin_id);

/**
 * disable interrupt
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_interrupt_disable(hal_pin_id_t pin_id);

/**
 * selects the interrupt edge for the corresponding I/O pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_interrupt_edge(hal_pin_id_t pin_id, uint8_t edge);

/**
 * clear the interrupt flag for the corresponding I/O pin
 *
 * @param pin_id Pin id is a double-digit,10-digit is pin, single-digit is
 * pin's bit, .e.g 12, 1 is pin and 2 is pin's bit
 */
void hal_gpio_interrupt_clear(hal_pin_id_t pin_id);

/**
 * @brief  init the gpio
 * @param  pin_id 
 * @param  name   board gpio pin names
 * @param  mode   operation mode for the gpio
 * @param  config define the gpio as push-pull type or open-drain
 * @param  type   add a pull-up, pull-down or nothing on the gpio line
 * @param  value  the initilize value of init gpio
 * @return        init ok or false
 */
bool_t hal_gpio_init(hal_pin_id_t *pin_id,
                     hal_pin_name_t name,
                     hal_pin_mode_t mode,
                     hal_pin_conf_t config,
                     hal_pin_type_t type,
                     uint32_t value);
    

#endif