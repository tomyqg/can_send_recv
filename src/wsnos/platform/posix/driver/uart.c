/**
 * @brief       : this
 * @file        : uart.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-01-10
 * change logs  :
 * Date       Version     Author        Note
 * 2016-01-10  v0.0.1  gang.cheng    first version
 */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "errno.h"
#include "fcntl.h"
#include "unistd.h"
#include "termios.h"

#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include "driver.h"

DBG_THIS_MODULE("uart");

static int32_t uart2_fd = -1;
static pthread_t uart2_thread;
static bool_t uart2_is_trigger = FALSE;

static uart_interupt_cb_t uart_interrupt_cb = NULL;

static int32_t set_opt(int32_t fd, int32_t nSpeed, int32_t nBits, char_t nEvent, int32_t nStop)
{
    struct termios newtio, oldtio;
    if ( tcgetattr( fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }

    osel_memset(&newtio, 0x00, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    switch ( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    switch ( nEvent )
    {
    case 'O':
    case 'o':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'e':
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'n':
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }
    switch ( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 19200:
        cfsetispeed(&newtio, B19200);
        cfsetospeed(&newtio, B19200);
        break;
    case 38400:
        cfsetispeed(&newtio, B38400);
        cfsetospeed(&newtio, B38400);
        break;
    case 57600:
        cfsetispeed(&newtio, B57600);
        cfsetospeed(&newtio, B57600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    if ( nStop == 1 )
        newtio.c_cflag &= ~CSTOPB;
    else if ( nStop == 2 )
        newtio.c_cflag |= CSTOPB;
    newtio.c_cc[VTIME] = 1; // critical parameters, 0.1 Second
    newtio.c_cc[VMIN] = 12; // critical parameters
    tcflush(fd, TCIFLUSH);
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }

    // DBG_LOG(DBG_LEVEL_INFO, "opt set done!\r\n");
    return 0;
}

static void *uart2_thread_routine(void *p)
{
    fd_set rdfds;
    uint8_t buf[128];
    uint32_t len = 0;

    while (1)
    {
        FD_ZERO(&rdfds);
        FD_SET(uart2_fd, &rdfds);
        int32_t ret = select(uart2_fd + 1, &rdfds, NULL, NULL, NULL);
        if (ret < 0)
            DBG_LOG(DBG_LEVEL_INFO, "uart interrupt select error\n");
        else if (ret == 0)
            continue;
        else
        {
            if (FD_ISSET(uart2_fd, &rdfds))
            {
                //*< 接收完整的串口数组才产生中断
                uart2_is_trigger = TRUE;
                sem_post(&sem_id);
            }
        }
    }
}

void uart_init(uint8_t uart_id, uint32_t baud_rate)
{
    // char_t *dev = "/dev/ttyUSB0";
    char_t *dev = "/dev/ttyS2";

    switch (uart_id)
    {
    case UART_1:
        break;

    case UART_2:
        uart2_fd = open(dev, O_RDWR | O_NOCTTY);
        if (uart2_fd == -1)
        {
            DBG_LOG(DBG_LEVEL_ERROR, "uart dev open failed\n");
            return;
        }

        if (fcntl(uart2_fd, F_SETFL, 0) < 0)
        {
            DBG_LOG(DBG_LEVEL_ERROR, "fcntl failed!\n");
            close(uart2_fd);
            return;
        }

        if (isatty(STDIN_FILENO) == 0)
        {
            DBG_LOG(DBG_LEVEL_ERROR, "standard input is not a terminal device\n");
            close(uart2_fd);
            return;
        }

        if (set_opt(uart2_fd, baud_rate, 8, 'N', 1) < 0)
        {
            DBG_LOG(DBG_LEVEL_INFO, "set_opt error");
            return;
        }

        if (hardware_int_init(&uart2_thread, uart2_thread_routine, NULL) != 0)
        {
            close(uart2_fd);
            DBG_LOG(DBG_LEVEL_ERROR, "uart2 thread init failed\n");
            return;
        }
        break;

    case UART_3:

        break;

    default:
        break;
    }
}

void uart_send_char(uint8_t id, uint8_t value)
{
    write(uart2_fd, &value, 1);
}

void uart_send_string(uint8_t id, uint8_t *string, uint16_t length)
{
    switch (id)
    {
    case UART_1:
        for (uint16_t i = 0; i < length; i++)
        {
            printf("%c", string[i]);
        }
        break;

    case UART_2:
        if (uart2_fd != -1)
        {
            write(uart2_fd, string, length);
        }
        else
        {
            DBG_LOG(DBG_LEVEL_ERROR, "uart2_fd-open failed\n");
        }
        break;

    case UART_3:
        for (uint16_t i = 0; i < length; i++)
        {
            printf("%c", string[i]);
        }
        break;

    default:
        break;
    }


}

bool_t uart_enter_q(uint8_t id, uint8_t c)
{
    return TRUE;
}

bool_t uart_string_enter_q(uint8_t id, uint8_t *string, uint16_t length)
{
    bool_t ret = FALSE;

    return ret;
}

bool_t uart_del_q(uint8_t id, uint8_t *c_p)
{
    return FALSE;
}

void uart_clear_rxbuf(uint8_t id)
{
    ;
}


void uart_recv_enable(uint8_t uart_id)
{
    ;
}

void uart_recv_disable(uint8_t uart_id)
{
    ;
}

void uart_power_open(bool_t res)
{
    ;
}


void uart_int_cb_reg(uart_interupt_cb_t cb)
{
    if (cb != NULL)
    {
        uart_interrupt_cb = cb;
    }
}

static void uart_int_cb_handle(uint8_t id, uint8_t ch)
{
    if (uart_interrupt_cb != NULL)
    {
        uart_interrupt_cb(id, ch);
    }
}

void uart2_int_handle(void)
{
    if (uart2_is_trigger)
    {
        uart2_is_trigger = FALSE;

        uint16_t len = 0;
        uint8_t buf[256];
        osel_memset(buf, 0x00, 256);
        len = read(uart2_fd, buf, 256);
#if 0
        DBG_LOG(DBG_LEVEL_INFO, "uart recv: %d, buf[]:", len);
        for (uint8_t i = 0; i < len; i++)
            DBG_LOG(DBG_LEVEL_ORIGIN, "0x%02x ", buf[i]);
        DBG_LOG(DBG_LEVEL_ORIGIN, "\n");
#endif

        for (int16_t i = 0; i < len; i++)
        {
            uart_int_cb_handle(UART_2, buf[i]);
        }
        uart2_is_trigger = FALSE;
    }
}
