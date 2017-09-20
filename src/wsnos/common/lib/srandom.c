/**
 * @brief       : 
 *
 * @file        : srandom.c
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
#include "common/lib/debug.h"
#include <stdlib.h>

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t l_rand( void )
{
    return ( ( next = next * 1103515245L + 12345L ) % RAND_LOCAL_MAX );
}

void l_srand( uint32_t seed )
{
    next = seed;
}
// Standard random functions redefinition end

int32_t randr( int32_t min, int32_t max )
{
    DBG_ASSERT(max >= min __DBG_LINE);
    return ( int32_t )l_rand() % ( max - min + 1 ) + min;
}
