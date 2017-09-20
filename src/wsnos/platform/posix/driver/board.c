/**
 * @brief       : this
 * @file        : board.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-05-05
 * change logs  :
 * Date       Version     Author        Note
 * 2015-05-05  v0.0.1  gang.cheng    first version
 */

#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include "common/hal/hal_board.h"   //*< for

#include "driver.h"

#include <stdlib.h>

DBG_THIS_MODULE("board");


#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN   16000
#endif

static struct termios l_tsav;
sem_t sem_id;

static pthread_t tick_thread;
static bool_t tick_is_trigger = FALSE;

static const char *board_opts = "i:I:c:p:h";
static void show_help(void)
{
    char *help =
        "board info set config.\r\n"        \
        "usage: ./xxx.out [option] ...\r\n" \
        ;
    printf("%s\n", help);
}

uint8_t debug_put_char(uint8_t ch)
{
    putchar(ch);
    return ch;
}

static void board_info_config(int argc, char *argv[])
{
    int cmd_opt = 0;
    device_info_t device_info = hal_board_info_get();

    if (argc <= 1)
        return;

    while ((cmd_opt = getopt(argc, argv, board_opts)) != -1)
    {
        switch (cmd_opt)
        {
        case 'i':   //*< device id
            device_info.device_id = strtol(optarg, NULL, 16);
            printf("device id is 0x%x\n", device_info.device_id);
            hal_board_info_save(&device_info, TRUE);
            break;

        case 'I':
        {
            uint64_t dev_id = strtol(optarg, NULL, 16);
            printf("unique id is 0x%lx\n", dev_id);
            uint8_t id_buf[8];  //*< big end store unique_id
            uint32_t ul = (uint32_t)dev_id;
            uint32_t uh = (uint32_t)(dev_id >> 32);
            id_buf[0] = HI_1_UINT32(uh);
            id_buf[1] = HI_2_UINT32(uh);
            id_buf[2] = HI_3_UINT32(uh);
            id_buf[3] = HI_4_UINT32(uh);
            id_buf[4] = HI_1_UINT32(ul);
            id_buf[5] = HI_2_UINT32(ul);
            id_buf[6] = HI_3_UINT32(ul);
            id_buf[7] = HI_4_UINT32(ul);
            if (!id_general(id_buf, 8, &device_info.device_id))
            {
                printf("unique id is not vailed id in id_rules.\r\n");
            }
            osel_memcpy((uint8_t *) & (device_info.unique_id),
                        id_buf, 8);
            hal_board_info_save(&device_info, TRUE);
            break;
        }
        case 'c':   //*< rf channel
            device_info.rf_channel = strtol(optarg, NULL, 16);
            printf("channel is %x\n", device_info.rf_channel);
            hal_board_info_save(&device_info, TRUE);
            break;

        case 'p':   //*< rf power
            device_info.rf_power = strtol(optarg, NULL, 16);
            printf("power is %x\n", device_info.rf_power);
            hal_board_info_save(&device_info, TRUE);
            break;

        case '?':
        case 'h':
        default:
            show_help();
            break;
        }
    }

    if (argc > optind)
    {
        int i = 0;
        for (i = optind; i < argc; i++)
            fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
    }
}



int hardware_int_init(  pthread_t *thread,
                        thread_routine_t routine,
                        void *arg)
{
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, (size_t)PTHREAD_STACK_MIN);

    if (pthread_create(thread, &attr, routine, arg) != 0)
    {
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        param.sched_priority = 0;
        pthread_attr_setschedparam(&attr, &param);
        if (pthread_create(thread, &attr, routine, arg) != 0)
        {
            printf("software int simulator init failed\r\n");
            return -1;
        }
    }

    return 0;
}

static void* tick_thread_routine(void *arg)
{
    while (1)
    {
        struct timeval timeout = {0};
        timeout.tv_usec = MS_FOR_ONETICK * 1000;    // 10MS
        select(0, 0, 0, 0, &timeout);
        tick_is_trigger = TRUE;
        sem_post(&sem_id);
    }

    return NULL;
}

static void board_idle_handle(void *p)
{
    sem_wait(&sem_id);
    if (tick_is_trigger)
    {
        tick_is_trigger = FALSE;
        wsnos_ticks();
    }

    //*< 这里是所有外部中断的处理函数
    //linux_terminal_irq_handle();
    //linux_rfid_irq_handle();
    //
    extern void rtimer_int_handle(void);
    rtimer_int_handle();

    extern void sim_radio_handle(void);
    sim_radio_handle();

    extern void uart2_int_handle(void);
    uart2_int_handle();

    osel_schedule();
    debug_info_printf();
}


void board_init(int argc, char *argv[])
{
    struct termios tio;
    tcgetattr(0, &l_tsav);
    tcgetattr(0, &tio);

    tio.c_cflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &tio);

    sem_init(&sem_id, 0, 0);
    osel_idle_hook(board_idle_handle);
    hardware_int_init(&tick_thread, tick_thread_routine, NULL);

    debug_init(DBG_LEVEL_ERROR | DBG_LEVEL_TRACE | DBG_LEVEL_ORIGIN | DBG_LEVEL_INFO | DBG_LEVEL_WARNING);

    extern void hal_board_info_init(void);
    hal_board_info_init();
    board_info_config(argc, argv);

    extern void hal_timer_init(void);
    hal_timer_init();


#if (NODE_TYPE == NODE_TYPE_GATEWAY)
uart_init(UART_2, 9600);
#endif

    extern device_info_t hal_board_info_look(void);
    device_info_t device_info = hal_board_info_look();
    printf("device_info.device_id  =0x%08x\n", device_info.device_id);
    printf("device_info.unique_id  =0x%08lx\n", device_info.unique_id);
    printf("device_info.rf_channel =%02d\n", device_info.rf_channel);
    printf("device_info.rf_power   =%02d\n", device_info.rf_power);
}

void led_init(void)
{
    ;
}

void board_deinit(void)
{
    ;
}

void board_reset(void)
{
    DBG_LOG(DBG_LEVEL_INFO, "board reset\r\n");
    sleep(1);
    exit(0);
}

void srand_arch(void)
{

}

void lpm_arch(void)
{
    board_idle_handle(NULL);
}

unsigned int BoardGetRandomSeed( void )
{
    return 0x00ull;
}


static uint32_t ID_01 = 0x10090001;
static uint32_t ID_02 = 0x10153216;
static uint32_t nwk_id = 0;

void BoardGetUniqueId( unsigned char *id )
{
    id[7] = ( ID_01 ) ;
    id[6] = ( ID_01 ) >> 8;
    id[5] = ( ID_01 ) >> 16;
    id[4] = (ID_01 ) >> 24;
    nwk_id = ID_01;
    id[3] = ( ID_02 ) ;
    id[2] = ( ID_02 ) >> 8;
    id[1] = ( ID_02 ) >> 16;
    id[0] = (ID_02 ) >> 24;
    printf("ID:");
    printf_string(id, 8);
}

unsigned int lorawan_device_addr(void)
{
    return nwk_id;
}

void lorawan_uid_set(unsigned char *id)
{
    uint8_t id_buf[8];
    for (uint8_t i = 0; i < 8; i++)
    {
        id_buf[i] = *(id + 7 - i);
    }

    // osel_memcpy(&ID_01, id_buf, sizeof(uint32_t));
    nwk_id = ID_01;
    // osel_memcpy(&ID_02, &id_buf[4], sizeof(uint32_t));
}


void printf_string(unsigned char *s, unsigned char length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02x ", s[i] );
    }
    printf("\n");
}

void printf_time(void)
{
    time_t timep;
    time(&timep);
    printf("%s", asctime(gmtime(&timep)));
}
