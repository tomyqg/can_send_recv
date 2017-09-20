/**
 * @brief       : this
 * @file        : radio.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-01-09
 * change logs  :
 * Date       Version     Author        Note
 * 2016-01-09  v0.0.1  gang.cheng    first version
 */
#include <sys/socket.h>     //*< sendmsg, recvmsg
#include <sys/types.h>
#include <arpa/inet.h>      //*< inet_addr()
#include <netinet/in.h>     //*< struct sockaddr_in,
#include <netinet/ip.h>
#include <errno.h>

#include "common/lib/lib.h"
#include "common/hal/hal.h"
#include "sys_arch/osel_arch.h"

#include "../radio_defs.h"
#include "radio.h"

#include "platform/platform.h"

DBG_THIS_MODULE("SimRadio");

#define DEBUG                   (1)

#define MCAST_PORT              9000
#define MCAST_ADDR              "224.0.0.88"
#define MAX_SIZE                256

typedef struct
{
    uint32_t dev_id;
    uint16_t socket_port;
    uint32_t x; //*< Screen pointer for x axis
    uint32_t y; //*< Screen pointer for y axis
} sim_radio_node_info_t;

#define  TIMEOUT(i)                                 \
    i = 0;                                          \
    do                                              \
    {                                               \
        if((i)++ > 70000)                           \
        {                                           \
            DBG_ASSERT(FALSE __DBG_LINE);           \
        }                                           \
    } while(__LINE__ == -1)

static rf_int_reg_t rf_int_reg[RF_INT_MAX_NUM];

static pthread_t sim_radio_thread;
static int sock_fd;
static fd_set sim_fd;
static bool_t sim_radio_is_trigger = FALSE;

static uint8_t recv_tmp_buf[MAX_SIZE];
static uint8_t recv_tmp_len = 0;
static uint8_t recv_tmp_index = 0;

uint8_t sim_radio_get_rxfifo_cnt(void);

static void *sim_radio_thread_routine(void *p)
{
    struct sockaddr_in local_addr;
    int addr_len = 0;
    char buf[256];
    int n = 0;

    while (1)
    {
        FD_ZERO(&sim_fd);
        FD_SET(sock_fd, &sim_fd);
        int32_t ret = select(sock_fd + 1, &sim_fd, NULL, NULL, NULL);
        if (ret < 0)
            DBG_LOG(DBG_LEVEL_INFO, "sim radio interrupt select error\n");
        else if (ret == 0)
            continue;
        else
        {
            if (FD_ISSET(sock_fd, &sim_fd))
            {
                sim_radio_is_trigger = TRUE;
                sem_post(&sem_id);
            }
        }
    }

    return NULL;
}

static bool_t sim_radio_init(void)
{
    DBG_LOG(DBG_LEVEL_INFO, "sim_raido_init\n");

    struct sockaddr_in local_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1)
    {
        DBG_LOG(DBG_LEVEL_ERROR, "socket(SOCK_DGRAM) failed\n");
        return 0;
    }

    //*< 设置复用端口，接收可以bind到同一个端口
    int on = 1;
#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if (sock_fd < 0)
    {
        DBG_LOG(DBG_LEVEL_ERROR, "setsockopt(SO_REUSEPORT) failed\n");
        return 0;
    }
//*< 初始化地址
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(MCAST_PORT);
//*< 绑定socket
    int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
    if (err < 0)
    {
        DBG_LOG(DBG_LEVEL_ERROR, "bind()\n");
        return 0;
    }

    //*< 设置回环许可
    int loop = 1;   //*< 1,允许本地的回环接口
    err = setsockopt(sock_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (err < 0)
    {
        printf("setsockopt():IP_MULTICAST_LOOP\n");
        return -3;
    }

    struct ip_mreq mreq;    //*< 广播组结构体
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);          //*< 广播地址
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);          //*< 网络接口默认

    //*< 将本机加入广播组
    err = setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (err < 0)
    {
        printf("setsockopt():IP_ADD_MEMBERSHIP\n");
        return -4;
    }

    if (hardware_int_init(&sim_radio_thread, sim_radio_thread_routine, NULL) != 0)
        return 0;

    return 1;
}

static int8_t sim_radio_prepare(uint8_t const *p_data, uint8_t count)
{
    struct sockaddr_in dst_addr;
    struct msghdr msg;
    struct iovec iov;

    memset(&msg, 0x00, sizeof(msg));

    dst_addr.sin_family = AF_INET;
    // dst_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dst_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);
    dst_addr.sin_port = htons(MCAST_PORT);

    msg.msg_name = &dst_addr;
    msg.msg_namelen = sizeof(dst_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_iov->iov_base = (void *)p_data;
    msg.msg_iov->iov_len = count;
    msg.msg_control = 0;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;
    sendmsg(sock_fd, &msg, 0);
#if DEBUG   //*< debug for send frame

    DBG_LOG(DBG_LEVEL_INFO, "pack send: ");
    for (uint8_t i = 0; i < count; i++)
        DBG_LOG(DBG_LEVEL_ORIGIN, "0x%02x ", p_data[i]);
    DBG_LOG(DBG_LEVEL_ORIGIN, "\n");
#endif

    return count;
}

int8_t sim_radio_transmit(uint8_t len)
{
    if (rf_int_reg[TX_OK_INT] != NULL)
        ( *(rf_int_reg[TX_OK_INT]) )((uint16_t)(hal_timer_now().w));

    return len;
}

int8_t sim_radio_send(uint8_t const *p_data, uint8_t count)
{
    sim_radio_prepare(p_data, count);
    sim_radio_transmit(count);
    return count;
}

int8_t sim_radio_recv(uint8_t *p_data, uint8_t count)
{
    uint8_t len = 0;
    if (count > recv_tmp_len)
    {
        len = recv_tmp_len;
    }
    else
    {
        len = count;
    }

    memcpy(p_data, &recv_tmp_buf[recv_tmp_index], len);
    recv_tmp_index = len;

    return len;
}

uint8_t sim_radio_get_rxfifo_cnt(void)
{
    struct sockaddr_in src_addr;
    struct msghdr msg;
    struct iovec iov;

    memset(&msg, 0x00, sizeof(msg));

    recv_tmp_len = 0;
    recv_tmp_index = 0;
    memset(recv_tmp_buf, 0x00, MAX_SIZE);

    msg.msg_name = &src_addr;
    msg.msg_namelen = sizeof(src_addr);

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_iov->iov_base = recv_tmp_buf;
    msg.msg_iov->iov_len = MAX_SIZE; //*< MAX size

    recv_tmp_len = recvmsg(sock_fd, &msg, MSG_DONTWAIT);
    return recv_tmp_len;
}

rf_result_t sim_radio_set_value(rf_cmd_t cmd, uint8_t value)
{
    rf_result_t ret = RF_RESULT_OK;

    return ret;
}

rf_result_t sim_radio_get_value(rf_cmd_t cmd, void *value)
{
    rf_result_t ret = RF_RESULT_OK;
    if (cmd == RF_RXFIFO_CNT)
    {
        *(uint8_t *)value = sim_radio_get_rxfifo_cnt();
    }

    return ret;
}

static bool_t sim_radio_int_cfg(rf_int_t state,
                                rf_int_reg_t int_cb,
                                uint8_t type)
{
    (type == INT_ENABLE) ? ((int_cb != NULL) ? (rf_int_reg[state] = int_cb) : (NULL)) : (NULL);

    return TRUE;
}

const struct radio_driver sim_radio_driver =
{
    sim_radio_init,

    sim_radio_prepare,
    sim_radio_transmit,

    sim_radio_send,
    sim_radio_recv,

    sim_radio_get_value,
    sim_radio_set_value,

    sim_radio_int_cfg,
};

void sim_radio_handle(void)
{
    if (sim_radio_is_trigger)
    {
        sim_radio_is_trigger = FALSE;

        if (rf_int_reg[RX_SFD_INT] != NULL)
            ( *(rf_int_reg[RX_SFD_INT]) )((uint16_t)(hal_timer_now().w));

        if (rf_int_reg[RX_OK_INT] != NULL)
        {
            ( *(rf_int_reg[RX_OK_INT]) )((uint16_t)(hal_timer_now().w));
        }
        else
        {
            static uint8_t cnt = 0;
            uint8_t read_len = 0;
            uint8_t read_buf[256];
            memset(read_buf, 0x00, 256);
            sim_radio_get_value(RF_RXFIFO_CNT, (void *)&read_len);
            if ((read_len != 0 ) && (read_len != 255))
            {
                sim_radio_recv(read_buf, read_len);
                printf("recv %d, %d, %s\n", cnt++, read_len, recv_tmp_buf);
            }
        }
    }
}


#undef DEBUG




























