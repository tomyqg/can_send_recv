/**
* @brief       :
*
* @file        : hal_board.c
* @author      : gang.cheng
*
* Version      : v0.0.1
* Date         : 2015/5/7
* Change Logs  :
*
* Date        Version      Author      Notes
* 2015/5/7    v0.0.1      gang.cheng    first version
*/
#include "common/hal/hal.h"
#include "common/dev/dev.h"
#include "platform/platform.h"

#define DEVICE_OPERATE_MODE_FLAG_ADDR           DEVICE_INFO_ADDR


device_info_t device_info;
static bool_t delay_save_flag = FALSE;

void hal_led_init(void)
{
    led_init();
}
void hal_led_open(uint8_t color)
{
    switch (color)
    {
    case HAL_LED_BLUE:
        LED_OPEN(BLUE);
        break;
    case HAL_LED_RED:
        LED_OPEN(RED);
        break;
    case HAL_LED_GREEN:
        LED_OPEN(GREEN);
        break;
    case HAL_LED_POWER:
        LED_OPEN(RED);
        break;
    case HAL_LED_ERROR:
        LED_OPEN(BLUE);
        break;
    default :
        break;
    }
}

void hal_led_close(uint8_t color)
{
    switch (color)
    {
    case HAL_LED_BLUE:
        LED_CLOSE(BLUE);
        break;
    case HAL_LED_RED:
        LED_CLOSE(RED);
        break;
    case HAL_LED_GREEN:
        LED_CLOSE(GREEN);
        break;
    case HAL_LED_POWER:
        LED_CLOSE(RED);
        break;
    case HAL_LED_ERROR:
        LED_CLOSE(BLUE);
        break;
    default :
        break;
    }
}

void hal_led_toggle(uint8_t color)
{
    switch (color)
    {
    case HAL_LED_BLUE:
        LED_TOGGLE(BLUE);
        break;
    case HAL_LED_RED:
        LED_TOGGLE(RED);
        break;
    case HAL_LED_GREEN:
        LED_TOGGLE(GREEN);
        break;
    case HAL_LED_POWER:
        LED_TOGGLE(RED);
        break;
    case HAL_LED_ERROR:
        LED_TOGGLE(BLUE);
        break;
    default :
        break;
    }
}

void hal_board_reset(void)
{
    board_reset();
}

void hal_board_info_init(void)
{
    hal_nvmem_read((uint8_t *)DEVICE_INFO_ADDR,
                   (uint8_t *)&device_info, sizeof(device_info));
    
    lorawan_uid_set((uint8_t *)&device_info.unique_id);
    
//    device_info.noload_ad = 100;
//    hal_board_info_save(&device_info, TRUE);
}

void hal_board_deinit(void)
{
    board_deinit();
}

bool_t hal_hex_to_ascii(uint8_t *buf, uint8_t dat)
{
    uint8_t dat_buff;

    dat_buff = dat;
    dat = dat & 0x0f;
    if (dat <= 9)
    {
        dat += 0x30;
    }
    else
    {
        dat += 0x37;
    }

    buf[1] = dat;

    dat = dat_buff;
    dat >>= 4;
    dat = dat & 0x0f;
    if (dat <= 9)
    {
        dat += 0x30;
    }
    else
    {
        dat += 0x37;
    }

    buf[0] = dat;

    return TRUE;
}

bool_t hal_ascii_to_hex(uint8_t hi, uint8_t lo, uint8_t *hex)
{
    *hex = 0;
    if ((hi >= 0x30) && (hi <= 0x39))
    {
        hi -= 0x30;
    }
    else if ((hi >= 0x41) && (hi <= 0x46))
    {
        hi -= 0x37;
    }
    else
    {
        return FALSE;
    }
    *hex |= (hi << 4);

    if ((lo >= 0x30) && (lo <= 0x39))
    {
        lo -= 0x30;
    }
    else if ((lo >= 0x41) && (lo <= 0x46))
    {
        lo -= 0x37;
    }
    else
    {
        return FALSE;
    }
    *hex |= lo;

    return TRUE;
}

void hal_board_info_write(void  *p_info)
{
    osel_memcpy((uint8_t *)&device_info, p_info, sizeof(device_info_t));
}


bool_t hal_board_info_save(device_info_t *p_info, bool_t flag)
{
    osel_memcpy((uint8_t *)&device_info, (uint8_t *)p_info, sizeof(device_info_t));

    if (flag)
    {
        hal_nvmem_erase((uint8_t *)DEVICE_INFO_ADDR, 0);
        hal_nvmem_write((uint8_t *)DEVICE_INFO_ADDR,
                        (uint8_t *)p_info, sizeof(device_info_t));
    }
    else
    {
        delay_save_flag = TRUE;
    }

    return TRUE;
}

bool_t hal_board_info_delay_save(void)
{
    if (delay_save_flag)
    {
        delay_save_flag = FALSE;
        hal_nvmem_erase((uint8_t *)DEVICE_INFO_ADDR, 0);
        hal_nvmem_write((uint8_t *)DEVICE_INFO_ADDR,
                        (uint8_t *)&device_info, sizeof(device_info_t));
    }

    return TRUE;
}

device_info_t hal_board_info_get(void)
{
    hal_nvmem_read((uint8_t *)DEVICE_INFO_ADDR,
                   (uint8_t *)&device_info, sizeof(device_info));
    
    return device_info;
}

device_info_t hal_board_info_look(void)
{
    return device_info;
}

void operate_mode_flag_save(uint32_t flag)
{
    hal_nvmem_erase((uint8_t *)DEVICE_OPERATE_MODE_FLAG_ADDR, 0);
    hal_nvmem_write((uint8_t *)DEVICE_OPERATE_MODE_FLAG_ADDR, (uint8_t *)&flag, sizeof(uint32_t));
}


uint32_t operate_mode_flag_get(void)
{
    uint32_t flag = 0;
    hal_nvmem_read((uint8_t *)DEVICE_OPERATE_MODE_FLAG_ADDR,
                   (uint8_t *)&flag, sizeof(uint32_t));
    
    return flag;
}

void operate_mode_flag_clear(void)
{
    uint32_t flag = DEVICE_FLAG_CLEAR;
    hal_nvmem_write((uint8_t *)DEVICE_OPERATE_MODE_FLAG_ADDR, (uint8_t *)&flag, sizeof(uint32_t));
   
    uint8_t i = 0;
    while((operate_mode_flag_get() != DEVICE_FLAG_CLEAR))
    { 
        hal_nvmem_write((uint8_t *)DEVICE_OPERATE_MODE_FLAG_ADDR, (uint8_t *)&flag, sizeof(uint32_t));
        if (i++ >100)
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }
    }                 
}

void gpio_lowpower_init(void)
{
    ;
}

void hal_board_init(void)
{
#ifdef PF_CORE_POSIX
    board_init(0, NULL);
#else
    board_init();
    hal_board_info_init();
    gpio_lowpower_init();
    hal_led_init();
    hal_led_open(BLUE);
    hal_led_close(BLUE);
//    hal_wdt_clear(16000);
    hal_timer_init();

#if NODE_TYPE != NODE_TYPE_GATEWAY
    hal_uart_init(HAL_UART_1, 115200);
    hal_uart_init(HAL_UART_2, 115200);
    //hal_uart_init(HAL_UART_3, 38400);
#else
//    wdt_extern_clear();
    hal_uart_init(HAL_UART_1, 115200);
#endif
#endif

     debug_init(DBG_LEVEL_INFO | DBG_LEVEL_ORIGIN);
}
