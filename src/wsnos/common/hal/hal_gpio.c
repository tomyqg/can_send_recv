/**
 * @brief       : 
 *
 * @file        : hal_gpio.c
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */
#include "common/lib/lib.h"
#include "platform/platform.h"
#include "common/hal/hal.h"
#include <node_cfg.h>

void hal_gpio_set(hal_pin_id_t pin_id)
{
    gpio_set(pin_id);
}

void hal_gpio_clr(hal_pin_id_t pin_id)
{
    gpio_clr(pin_id);
}

void hal_gpio_toggle(hal_pin_id_t pin_id)
{
    gpio_toggle(pin_id);
}

void hal_gpio_make_input(hal_pin_id_t pin_id)
{
    gpio_make_input(pin_id);
}

bool_t hal_gpio_is_input(hal_pin_id_t pin_id)
{
    return gpio_is_input(pin_id);
}

void hal_gpio_make_output(hal_pin_id_t pin_id)
{
    gpio_make_output(pin_id);
}

bool_t hal_gpio_is_output(hal_pin_id_t pin_id)
{
    return gpio_is_output(pin_id);
}

void hal_gpio_sel(hal_pin_id_t pin_id)
{
    gpio_sel(pin_id);
}

bool_t hal_gpio_get(hal_pin_id_t pin_id)
{
    return  gpio_get(pin_id);
}

void hal_gpio_interrupt_enable(hal_pin_id_t pin_id)
{
    gpio_interrupt_enable(pin_id);
}

void hal_gpio_interrupt_disable(hal_pin_id_t pin_id)
{
    gpio_interrupt_disable(pin_id);
}

void hal_gpio_interrupt_edge(hal_pin_id_t pin_id, uint8_t edge)
{
    gpio_interrupt_edge(pin_id, edge);
}

void hal_gpio_interrupt_clear(hal_pin_id_t pin_id)
{
    gpio_interrupt_clear(pin_id);
}

bool_t hal_gpio_init(hal_pin_id_t *pin_id,
                     hal_pin_name_t name,
                     hal_pin_mode_t mode,
                     hal_pin_conf_t config,
                     hal_pin_type_t type,
                     uint32_t value)
{
    DBG_ASSERT(pin_id != NULL __DBG_LINE);
    pin_id->pin = name >> 4;
    pin_id->bit = 0x01 << (name & 0x0F);
    gpio_init(*pin_id, mode, config, type, value);

    return TRUE;
}

