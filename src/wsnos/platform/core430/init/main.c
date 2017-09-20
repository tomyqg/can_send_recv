/**
* @brief       : 
*
* @file        : main.c
* @author      : gang.cheng
* @version     : v0.0.1
* @date        : 2015/5/7
*
* Change Logs  :
*
* Date        Version      Author      Notes
* 2015/5/7    v0.0.1      gang.cheng    first version
*/

#include "common/lib/lib.h"
#include "common/hal/hal.h"
#include "sys_arch/osel_arch.h"
#include "stack/stack.h"

static uint8_t osel_heap_buf[OSEL_HEAP_SIZE];


int16_t main(void)
{
    /*开启了看门狗52s后复位*/
#ifdef NDEBUG
    extern void bootloader_init(void);
    bootloader_init();
#endif
    
    osel_env_init(osel_heap_buf, OSEL_HEAP_SIZE, OSEL_MAX_PRIO);
    
    hal_board_init();
    
//    stack_init();
    
    extern void app_task_init(void);
    app_task_init();
    
    osel_run();
    
    return 0;
}
