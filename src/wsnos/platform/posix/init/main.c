/*
 * main.c
 *
 *  Created on: 2015年7月10日
 *      Author: uto
 */

#include "gz_sdk.h"

#include "stdio.h"

static uint8_t osel_heap_buf[OSEL_HEAP_SIZE];

static void osel_info(void)
{
    extern osel_uint32_t next_free_byte;
    printf("free heap : %ld\n", OSEL_HEAP_SIZE -  next_free_byte);
}


int main(int argc, char* argv[])
{
    osel_env_init(osel_heap_buf, OSEL_HEAP_SIZE, OSEL_MAX_PRIO);

    board_init(argc, argv);

#if defined (STACK_SSN) || defined(STACK_LORAWAN)
    stack_init();
#else
#warning "not defined STACK_LORAWAN or STACK_SSN"
#endif

    extern void app_init(void);
    app_init();

    osel_info();

    osel_run();

    return 0;
}

