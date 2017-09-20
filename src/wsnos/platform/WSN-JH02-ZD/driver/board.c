/**
 * @brief       :
 *
 * @file        : board.c
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
#include "common/lib/debug.h"
#include <stdlib.h>

pin_id_t led_red1, led_red2, led_red3, led_red4;

uint8_t debug_put_char(uint8_t ch)
{
    uart_send_char(DEBUG_PRINTF_ID, ch);
    return ch;
}

void led_init(void)
{
    GPIO_NAME_TO_PIN(&led_red1, PD_2);
    GPIO_NAME_TO_PIN(&led_red2, PD_3);
    GPIO_NAME_TO_PIN(&led_red3, PC_10);
    GPIO_NAME_TO_PIN(&led_red4, PC_11);
    
    gpio_init(led_red1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    gpio_init(led_red2, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    gpio_init(led_red3, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    gpio_init(led_red4, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    
    LED_CLOSE(RED);
    LED_CLOSE(YELLOW);
    LED_CLOSE(BLUE);
    LED_CLOSE(GREEN);

//
    clk_init(SYSCLK_8MHZ);
}

void board_reset(void)
{
    OSEL_INT_LOCK();
    WDTCTL = 0xFFFF;
}

void board_init(void)
{
//    pmap_init();
}

void board_deinit(void)
{
    ;
}

unsigned int BoardGetRandomSeed( void )
{
    return 0x00ull;
}

static uint32_t ID_01 = 0x10090002;
static uint32_t ID_02 = 0x10153216;
static uint32_t nwk_id = 0;
void BoardGetUniqueId( unsigned char *id )
{

//    uint32_t ID_03 = 0x1FF80064;
//    id[7] = ( ID_01 ) + ( ID_03 ) >> 24;
//    id[6] = ( ID_01 ) + ( ID_03 ) >> 16;
//    id[5] = ( ID_01 ) + ( ID_03 ) >> 8;
//    id[4] = ( ID_01 ) + ( ID_03 );

    id[7] = ( ID_01 ) ;
    id[6] = ( ID_01 ) >> 8;
    id[5] = ( ID_01 ) >> 16;
    id[4] = (ID_01 ) >> 24;

    id[3] = ( ID_02 ) ;
    id[2] = ( ID_02 ) >> 8;
    id[1] = ( ID_02 ) >> 16;
    id[0] = (ID_02 ) >> 24;
}

void lorawan_uid_set(unsigned char *id )
{
    uint8_t id_buf[8];
    for (uint8_t i = 0; i < 8; i++)                     //´óÐ¡¶Ë×ª»»
    {
        id_buf[i] = *(id + 7 - i);
    }
//    uint32_t id_01;
//    uint32_t id_02;
    osel_memcpy(&ID_01, id_buf, sizeof(uint32_t));
    nwk_id = ID_01;
    osel_memcpy(&ID_02, &id_buf[4], sizeof(uint32_t));
}

uint32_t lorawan_device_addr(void)
{
    return nwk_id;
}

void srand_arch(void)
{
    srand(TA0R);
}

void lpm_arch(void)
{
    LPM3;
}
