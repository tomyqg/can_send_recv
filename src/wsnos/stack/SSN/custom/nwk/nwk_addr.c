/**
 * @brief       :
 *
 * @file        : nwk_addr.c
 * @author      : WangJifang
 * @version     : v0.0.1
 * @date        : 2015/12/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/12/7    v0.0.1      WangJifang    some notes
 */
#include "common/lib/lib.h"
#include "common/hal/hal.h"
#include "nwk_route.h"
#include "nwk_frames.h"
#include "nwk_addr.h"
#include "common/hal/hal.h"

typedef struct
{
    uint16_t dev_nwk_addr;
    uint64_t dev_nui;
} nwk_addr_t;

typedef struct
{
    nwk_addr_t nwk_addr_buf[DETEC_NUM];
} nwk_addr_buf_t;

static nwk_addr_buf_t nwk_addr_table[ROUTE_NUM];


static bool_t is_table_empty(nwk_addr_t *addr)
{
    if ((addr->dev_nwk_addr == UNDEFINE_NWK_ADDR) && (addr->dev_nui == UNDEFINE_NUI))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static uint8_t find_father_index(uint16_t nwk_addr)
{
    uint8_t i;
    for (i = 0; i < ROUTE_NUM; i++)
    {
        if (nwk_addr_table[i].nwk_addr_buf[0].dev_nwk_addr == nwk_addr)
        {
            return i;
        }
    }
    return 0xFE;
}

bool_t nwk_addr_long_get(uint16_t nwk_addr, uint64_t *nui)
{
    uint8_t i, j;
    i = find_father_index(((nwk_addr & 0xFF00) | 0x0001));
    if (i < ROUTE_NUM)
    {
        for (j = 0; j < DETEC_NUM; j++)
        {
            if (nwk_addr_table[i].nwk_addr_buf[j].dev_nwk_addr == nwk_addr)
            {
                osel_memcpy((uint8_t *)nui,
                            (uint8_t *)&nwk_addr_table[i].nwk_addr_buf[j].dev_nui,
                            sizeof(uint64_t));

                return TRUE;
            }
        }
    }

    return FALSE;
}


static void nwk_addr_table_clear(uint8_t i)
{
    osel_memset(nwk_addr_table[i].nwk_addr_buf , 0, sizeof(nwk_addr_t)*DETEC_NUM);
}

bool_t nwk_addr_del(uint16_t nwk_addr)
{
    uint8_t anchor_add = (uint8_t)(nwk_addr >> 8);
    uint8_t i = 0;
    for (i = 0; i < DETEC_NUM; i++)
    {
        if (nwk_addr_table[anchor_add].nwk_addr_buf[i].dev_nwk_addr == nwk_addr)
        {
            nwk_addr_table[anchor_add].nwk_addr_buf[i].dev_nwk_addr = UNDEFINE_NWK_ADDR;
            nwk_addr_table[anchor_add].nwk_addr_buf[i].dev_nui = UNDEFINE_NUI;
            return TRUE;
        }
    }
    return FALSE;
}

uint16_t nwk_addr_short_get(nwk_join_req_t *nwk_join_req)
{
    uint8_t i, j;
    if (nwk_join_req->device_type == NODE_TYPE_TAG)
    {
        for (i = 0; i < ROUTE_NUM; i++)
        {
            i = find_father_index(nwk_join_req->father_id);
            if (i < ROUTE_NUM)
            {
                for (j = 0; j < DETEC_NUM; j++)
                {
                    if (nwk_addr_table[i].nwk_addr_buf[j].dev_nui == nwk_join_req->nui)
                    {
                        return nwk_addr_table[i].nwk_addr_buf[j].dev_nwk_addr;
                    }
                }
            }
        }
    }
    else
    {
        for (i = 0; i < ROUTE_NUM; i++)
        {
            if (nwk_addr_table[i].nwk_addr_buf[0].dev_nui == nwk_join_req->nui)
            {
                return nwk_addr_table[i].nwk_addr_buf[0].dev_nwk_addr;
            }
        }
    }
    return UNDEFINE_NWK_ADDR;
}

bool_t nwk_addr_short_find(uint64_t nui, uint16_t *nwk_addr)
{
    uint8_t i, j;

    for (i = 0; i < ROUTE_NUM; i++)
    {
        for (j = 0; j < DETEC_NUM; j++)
        {
            if (nwk_addr_table[i].nwk_addr_buf[j].dev_nui == nui)
            {
                osel_memcpy((uint8_t *)nwk_addr,
                            (uint8_t *)&nwk_addr_table[i].nwk_addr_buf[j].dev_nwk_addr,
                            sizeof(uint16_t));
                return TRUE;
            }
        }
    }

    return FALSE;
}

bool_t nwk_addr_table_add(nwk_join_req_t nwk_addr)
{
    uint8_t i, j;
    if (nwk_addr.device_type != NODE_TYPE_TAG)
    {
        for (i = 0; i < ROUTE_NUM; i++)
        {
            if (is_table_empty(&nwk_addr_table[i].nwk_addr_buf[0]))
            {
                nwk_addr_table[i].nwk_addr_buf[0].dev_nwk_addr = (((uint16_t)i) << 8) + 1;
                nwk_addr_table[i].nwk_addr_buf[0].dev_nui = nwk_addr.nui;
                return TRUE;
            }
        }
        return FALSE;
    }
    else
    {
        for (i = 0; i < ROUTE_NUM; i++)
        {
            i = find_father_index(nwk_addr.father_id);
            if (i < ROUTE_NUM)
            {
                for (j = 0; j < DETEC_NUM; j++)
                {
                    if (is_table_empty(&nwk_addr_table[i].nwk_addr_buf[j]))
                    {
                        nwk_addr_table[i].nwk_addr_buf[j].dev_nwk_addr = (((uint16_t)i) << 8) + j + 1;
                        nwk_addr_table[i].nwk_addr_buf[j].dev_nui = nwk_addr.nui;
                        return TRUE;
                    }
                }
                break;
            }
        }
    }
    return FALSE;
}

void nwk_addr_table_init(void)
{
    uint8_t i, j;
    for (i = 0; i < ROUTE_NUM; i++)
    {
        nwk_addr_table_clear(i);
    }

    hal_nvmem_read((uint8_t *)INFO_ADDR_ADDR,
                   (uint8_t *)nwk_addr_table,
                   sizeof(nwk_addr_t)*DETEC_NUM * ROUTE_NUM);
}

void nwk_addr_table_del(void)
{
    hal_wdt_clear(16000);
    wdt_extern_clear();
    //*< 1024 字节
    hal_nvmem_erase((uint8_t *)INFO_ADDR_ADDR, FLASH_SEG_ERASE);
    hal_nvmem_erase((uint8_t *)INFO_ADDR_ADDR+512, FLASH_SEG_ERASE);
    hal_nvmem_erase((uint8_t *)INFO_ADDR_ADDR+1024, FLASH_SEG_ERASE);
    hal_nvmem_erase((uint8_t *)INFO_ADDR_ADDR+1536, FLASH_SEG_ERASE);
}

void nwk_addr_table_save(void)
{
    hal_wdt_clear(16000);
    wdt_extern_clear();
    //*< 1024 字节
    hal_nvmem_erase((uint8_t *)INFO_ADDR_ADDR, FLASH_SEG_ERASE);
    hal_wdt_clear(16000);
    wdt_extern_clear();

    hal_nvmem_erase((uint8_t *)INFO_ADDR_ADDR + 512, FLASH_SEG_ERASE);
    hal_wdt_clear(16000);
    wdt_extern_clear();

    hal_nvmem_write((uint8_t *)INFO_ADDR_ADDR,
                    (uint8_t *)nwk_addr_table,
                    sizeof(nwk_addr_t)*DETEC_NUM * ROUTE_NUM);
    hal_wdt_clear(16000);
    wdt_extern_clear();
}