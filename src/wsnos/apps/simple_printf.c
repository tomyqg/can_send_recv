#include "common/hal/hal.h"
#include "common/lib/lib.h"
#define APP_UART                           (SERIAL_1)

// static void putc ( void* p, char c)
// {
//     while (!SERIAL_PORT_EMPTY) ;
//     SERIAL_PORT_TX_REGISTER = c; //寄存器输出
// }
//

static void _putc ( void* p, char c)
{
    uint8_t *t = &c;
    serial_write(APP_UART, t, 1);   //串口打印结果
}

void simple_printf(void)
{
    serial_fsm_init(APP_UART);
    wsnos_init_printf(NULL, _putc);

    uint8_t buf[100];
    uint8_t buf1[10] = "hello";
    uint8_t buf2[10] = "world";
    wsnos_printf("%s\r\n", buf1);   //输出：hello

    wsnos_sprintf(buf, "%s %s", buf1, buf2);
    wsnos_printf("%s\r\n", buf);                     //输出：hello world
}