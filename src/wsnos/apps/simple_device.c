
#include "demo.h"
#include "gz_sdk.h"

#define EVENT_STORE_SIZE        (10)
static osel_task_t *simple_device_tcb = NULL;
static osel_event_t simple_device_event_store[EVENT_STORE_SIZE];
static osel_device_t *pre_sensor = NULL;

static fp32_t pre_val = 0.0;
static fp32_t temp_val = 0.0;

PROCESS_NAME(simple_device_thread_process);

PROCESS(simple_device_thread_process, "simple_device_thread_process");
PROCESS_THREAD(simple_device_thread_process, ev, data)
{
    PROCESS_BEGIN();
    static osel_etimer_t delay_timer;
    
    pre_sensor = osel_device_find(PRE_SENSOR_NAME);
    if(pre_sensor == NULL)
    {
        PRINTF("device %s don't find\r\n", PRE_SENSOR_NAME);
        PROCESS_EXIT();
    }

    osel_device_control(pre_sensor, MPL_PRESSURE, NULL);
    
    while(1)
    {
        OSEL_ETIMER_DELAY(&delay_timer, OSEL_TICK_RATE_HZ);
        osel_device_open(pre_sensor, OSEL_DEVICE_OFLAG_RDONLY);
        
        osel_device_control(pre_sensor, MPL_PRESSURE, NULL);
        osel_device_read(pre_sensor, 0, &pre_val, 4);
        
        osel_device_control(pre_sensor, MPL_TEMPERATURE, NULL);
        osel_device_read(pre_sensor, 0, &temp_val, 4);
        
        osel_device_close(pre_sensor);
    }
    
    PROCESS_END();
}

void simple_device_init(void)
{
    /* init hardware device */
    mpl3115a2_device_init();
    
    simple_device_tcb = osel_task_create(NULL, 
                                         DEMO2_PRIO, 
                                         simple_device_event_store,
                                         EVENT_STORE_SIZE);
    if(simple_device_tcb == NULL)
        DBG_ASSERT(FALSE __DBG_LINE);
    
    osel_pthread_create(simple_device_tcb, &simple_device_thread_process, NULL);
}