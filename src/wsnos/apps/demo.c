#include "demo.h"
#include "common/lib/lib.h"

static uint8_t *alloc_ptr = NULL;

void app_init(void)
{
    // simple_task_init();
    // simple_pthread_init();
    // multi_pthread();

    //*< if you want to test demo4,5. you should close stack_init(). because Demo4
    //,5 prio is duplicate for MAC,NWK prio.
    // simple_etimer_init();
    // simple_osel_delay_init();

    // simple_aes();
    // simple_cmac();
    // simple_clist();
    // simple_sqqueue();
    // simple_serial();
    simple_lora_init();
    // simple_printf();
}
