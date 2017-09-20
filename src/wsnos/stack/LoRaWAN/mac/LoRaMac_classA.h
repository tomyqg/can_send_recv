#ifndef __LORAMAC_CLASS_A_H__
#define __LORAMAC_CLASS_A_H__

#include "stack/stack.h"
#include "../common/prim.h"
/**
 * [MAC_TASK_PRIO  loraWAN mac task prior]
 */
//#define MAC_TASK_PRIO           (5u)


bool_t mac_init(void *mac_pib);

void mac_class_run(void);

void mac_class_stop(void);

void mac_class_dependent_cfg(const stack_callback_t *callbacks);

int8_t mac_class_send(msg_flag_t flag, mbuf_t *mbuf);

#endif
