/**
 * @brief       : provides an abstraction for peripheral device.
 *
 * @file        : board.h
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2015/5/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "node_cfg.h"

#define BLUE              (1u)
#define RED               (2u)
#define GREEN             (3u)
#include "gpio.h"
extern pin_id_t led_red;

#define LED_INIT(RED)     P9SEL &= ~BIT7; P9DIR |= BIT7;

#define LED_OPEN(color)   ((color) == RED) ? gpio_clr(led_red):gpio_clr(led_red)
#define LED_CLOSE(color)  ((color) == RED) ? gpio_set(led_red):gpio_set(led_red)
#define LED_TOGGLE(color) ((color) == RED) ? gpio_toggle(led_red):gpio_toggle(led_red)
void led_init(void);

/**
 * reset system
 */
void board_reset(void);

/**
 * Initializes mcu clock, peripheral device and enable globle interrupt
 */
void board_init(void);

void board_deinit(void);

void srand_arch(void);

void lpm_arch(void);

unsigned int BoardGetRandomSeed( void );
void BoardGetUniqueId( unsigned char *id );

uint32_t lorawan_device_addr(void);
void lorawan_uid_set(unsigned char *id );
#endif

/**
 * @}
 */

