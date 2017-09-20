#ifndef __MAC_QUEUE_H__
#define __MAC_QUEUE_H__


#include "../../common/sbuf.h"

void mac_queue_init(void);

void mac_queue_insert(sbuf_t *sbuf);

sbuf_t *mac_queue_get(uint8_t immedi_wait);

sbuf_t *mac_queue_find(uint8_t immedi_wait, uint32_t addr);

void mac_queue_start();

void mac_queue_stop();

#endif
