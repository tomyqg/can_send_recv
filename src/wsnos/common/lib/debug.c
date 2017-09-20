/**
 * @brief       :
 *
 * @file        : debug.c
 * @author      : gang.cheng
 *
 * Version      : v0.0.1
 * Date         : 2015/5/7
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 * 2016/09/08  v0.0.2      gang.cheng   add debug_put_char() to reload print
 */

#include "common/lib/debug.h"
#include "common/lib/sqqueue.h"
#include "common/hal/hal.h"
#include "platform/platform.h"

#define DEBUG_PREFIX_LEN                (128u)      // log日志的前缀
#define DEBUG_BUF_SQQ_LEN               (512u)

typedef uint8_t debug_entry_t;
static sqqueue_ctrl_t debug_buf_sqq;

uint8_t global_debug_level;

static OSEL_NO_INIT uint16_t dbg_line;
static OSEL_NO_INIT uint16_t dbg_flag;

//static uint8_t dbg_prefix_buf[DEBUG_PREFIX_LEN];

void debug_init(uint8_t debug_lev)
{
    osel_memset((uint8_t *)&debug_buf_sqq, 0x00, sizeof(sqqueue_ctrl_t));

    if (sqqueue_ctrl_init(&debug_buf_sqq,
                          sizeof(debug_entry_t),
                          DEBUG_BUF_SQQ_LEN)
            == FALSE)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }

    global_debug_level = debug_lev;
}

static bool_t debug_info_enter_sqqueue(uint8_t *string, uint8_t len)
{
    hal_int_state_t s = 0;
    bool_t res = FALSE;

    HAL_ENTER_CRITICAL(s);
    // 用string_enter不用enter会减少一半的时间
    res = debug_buf_sqq.string_enter(&debug_buf_sqq, string, len);
    HAL_EXIT_CRITICAL(s);

    return res;
}

void debug_log(uint8_t dbg_lev, const char *fn, uint16_t line, ...)
{
    if (!(global_debug_level & dbg_lev))
    {
        return;
    }

    uint8_t dbg_prefix_buf[DEBUG_PREFIX_LEN];
    char *pbuf = (char *)&dbg_prefix_buf[0];
    (void)osel_memset(dbg_prefix_buf, 0, DEBUG_PREFIX_LEN);

    switch (dbg_lev)
    {
    case DBG_LEVEL_TRACE:
        (void)tfp_sprintf(pbuf, "[%s, %u][TRACE] ", fn, line);
        break;

    case DBG_LEVEL_INFO:
        (void)tfp_sprintf(pbuf, "[%s, %u][INFO ] ", fn, line);
        break;

    case DBG_LEVEL_WARNING:
        (void)tfp_sprintf(pbuf, "[%s, %u][WARN ] ", fn, line);
        break;

    case DBG_LEVEL_ERROR:
        (void)tfp_sprintf(pbuf, "[%s, %u][ERROR] ", fn, line);
        break;

    default:
        break;
    }

    pbuf += strlen((char_t *)dbg_prefix_buf);

    va_list args;
    va_start(args, line);
    const char_t *fmt = va_arg(args, const char_t*);
    tfp_vsprintf(pbuf, (char *)fmt, args);
    va_end(args);
    debug_info_enter_sqqueue((uint8_t *)dbg_prefix_buf, strlen((char_t *)dbg_prefix_buf));

#if DEBUG_INFO_PRINT_EN == 1
    uint8_t ch = 0x00;
    if (debug_buf_sqq.sqq.base == NULL)
    {
        return;
    }

    while (debug_buf_sqq.get_len(&debug_buf_sqq))
    {
        ch = *((debug_entry_t *)debug_buf_sqq.del(&debug_buf_sqq));
        debug_put_char(ch);
    }
#endif
}

void debug_info_printf(void)
{
#if ((DEBUG_INFO_PRINT_EN == 2) || (DEBUG_INFO_PRINT_EN == 0))
    uint8_t ch = 0x00;
    if (debug_buf_sqq.sqq.base == NULL)
    {
        return;
    }

    while (debug_buf_sqq.get_len(&debug_buf_sqq))
    {
        ch = *((debug_entry_t *)debug_buf_sqq.del(&debug_buf_sqq));
        debug_put_char(ch);
    }
#endif
}

uint16_t debug_info_get(void)
{
    uint16_t line;
    if (dbg_flag == 0xA5)
    {
        line = dbg_line;
        dbg_line = 0xFFFF;
    }
    else
    {
        dbg_flag = 0xA5;
        dbg_line = 0xFFFF;
        line = 0xFFFF;
    }
    return line;
}

void debug_info_clr(void)
{
    dbg_line = 0xFFFF;
}

#if DEBUG_INFO_PRINT_EN > 0
void DBG_ASSERT(bool_t cond _DBG_LINE_)
{
    do
    {
        if ((cond) == FALSE)
        {
            OSEL_INT_LOCK();
            hal_led_open(BLUE);
            dbg_line = line;
#ifdef PF_CORE_POSIX
            printf("assert line:%d\r\n", dbg_line);
#endif
            while (1);
        }
    }
    while (__LINE__ == -1);
}

#else
void DBG_ASSERT(bool_t cond _DBG_LINE_)
{
    extern void board_reset(void);
    do
    {
        if ((cond) == FALSE)
        {
            dbg_line = line;
            board_reset();
        }
    }
    while (__LINE__ == -1);
}

#endif
