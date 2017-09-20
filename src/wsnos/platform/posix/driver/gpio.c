/**
 * @brief       : this
 * @file        : gpio.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-10-15
 * change logs  :
 * Date       Version     Author        Note
 * 2015-10-15  v0.0.1  gang.cheng    first version
 */
#include "driver.h"
#include "sys_arch/osel_arch.h"

void gpio_set(pin_id_t pin_id)
{
	printf("%fs:P%d_%d open\n", osel_ticks_get()*1.0/OSEL_TICK_RATE_HZ, pin_id.pin, pin_id.bit);
}

void gpio_clr(pin_id_t pin_id)
{
	printf("%fs:P%d_%d close\n", osel_ticks_get()*1.0/OSEL_TICK_RATE_HZ, pin_id.pin, pin_id.bit);
}

void gpio_toggle(pin_id_t pin_id)
{
	
}

void gpio_make_input(pin_id_t pin_id)
{
	;
}

bool_t gpio_is_input(pin_id_t pin_id)
{
	return TRUE;
}

void gpio_make_output(pin_id_t pin_id)
{
	;
}

bool_t gpio_is_output(pin_id_t pin_id)
{
	return TRUE;
}

void gpio_sel(pin_id_t pin_id)
{
	;
}

bool_t gpio_get(pin_id_t pin_id)
{
	return  TRUE;
}

void gpio_interrupt_enable(pin_id_t pin_id)
{
	;
}

void gpio_interrupt_disable(pin_id_t pin_id)
{
	;
}

void gpio_interrupt_edge(pin_id_t pin_id, uint8_t edge)
{
	;
}

void gpio_interrupt_clear(pin_id_t pin_id)
{
	;
}

bool_t gpio_init(pin_id_t pin_id, pin_mode_e mode, pin_config_e config, pin_type_e type, uint32_t value)
{
	return TRUE;
}

void gpio_deinit(pin_id_t pin_id)
{
	;
}