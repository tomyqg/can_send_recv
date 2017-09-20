#ifndef __DEMO_H__
#define __DEMO_H__

typedef enum
{
    DEMO1_PRIO = 1,
    DEMO2_PRIO,
    DEMO3_1_PRIO,
    DEMO3_2_PRIO,
    DEMO4_PRIO,
    DEMO5_PRIO,
} demo_prio_t;


void simple_task_init(void);

void simple_pthread_init(void);

void multi_pthread_init(void);

void simple_etimer_init(void);

void simple_osel_delay_init(void);

void simple_lora_init(void);

void simple_aes(void);

void simple_cmac(void);

void simple_clist(void);

void simple_sqqueue(void);

void simple_device_init(void);

void simple_serial(void);

void simple_printf(void);


#endif
