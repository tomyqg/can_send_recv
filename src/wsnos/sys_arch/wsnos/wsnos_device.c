/**
 * @brief       : 添加设备管理功能，参考rtthread的device驱动
 * @file        : wsnos_device.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-02-08
 * change logs  :
 * Date       Version     Author        Note
 * 2017-02-08  v0.0.1  gang.cheng    first version
 */
#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"

static list_head_t device_container_head = {&device_container_head,
                                            &device_container_head
                                           };

osel_int8_t osel_device_register(osel_device_t *dev,
                                 const char *name,
                                 osel_uint16_t flags)
{
    if (dev == NULL)
        return -1;

    if (osel_device_find(name) != NULL)
        return -2;

    osel_strncpy(dev->name, name, OSEL_NAME_MAX);
    dev->flag = flags;
    dev->ref_count = 0;
    dev->magicflag = 0xdeadbeef;

    //*< add to list
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    list_add_to_tail(&dev->node, &device_container_head);
    OSEL_EXIT_CRITICAL(s);

    return 0;
}

osel_int8_t osel_device_unregister(osel_device_t *dev)
{
    if (dev == NULL)
        return -1;

    if (dev->magicflag != 0xdeadbeef)
        return -3;

    osel_int_status_t s;

    OSEL_ENTER_CRITICAL(s);
    list_del(&dev->node);
    dev->magicflag = 0x00ul;
    OSEL_EXIT_CRITICAL(s);

    return 0;
}


osel_int8_t osel_device_init_all(void)
{
    osel_device_t *dev;
    list_head_t *node;
    register osel_int8_t result;

    for (node = device_container_head.next;
            ((node != &device_container_head) && (node != NULL));
            node = node->next)
    {
        dev = list_entry_addr_find(node, osel_device_t, node);
        if ((dev->init != NULL) &&
                (dev->flag != OSEL_DEVICE_FLAG_ACTIVATED))
        {
            result = dev->init(dev);
            if ( result != 0)
            {
                PRINTF("osel device init %s failed.\r\n", dev->name);
            }
            else
            {
                dev->flag |= OSEL_DEVICE_FLAG_ACTIVATED;
            }
        }
    }

    return 0;
}


osel_device_t *osel_device_find(const char *name)
{
    osel_device_t *device;
    list_head_t *node;

    osel_int_status_t s;
    if ( PROCESS_CURRENT() != NULL)
        OSEL_ENTER_CRITICAL(s);

    for (node = device_container_head.next;
            ((node != &device_container_head) && (node != NULL));
            node = node->next)
    {
        device = list_entry_addr_find(node, osel_device_t, node);

        if (osel_strncmp(device->name, name, OSEL_NAME_MAX) == 0)
        {
            if ( PROCESS_CURRENT() != NULL)
                OSEL_EXIT_CRITICAL(s);

            return device;
        }
    }

    if ( PROCESS_CURRENT() != NULL)
        OSEL_EXIT_CRITICAL(s);

    return NULL;
}

osel_int8_t osel_device_init(osel_device_t *dev)
{
    osel_int8_t result = 0;

    DBG_ASSERT(dev != NULL __DBG_LINE);

    if (dev->init != NULL)
    {
        if (!(dev->flag & OSEL_DEVICE_FLAG_ACTIVATED))
        {
            result = dev->init(dev);
            if ( result != 0)
            {
                PRINTF("osel device init %s failed.\r\n", dev->name);
            }
            else
            {
                dev->flag |= OSEL_DEVICE_FLAG_ACTIVATED;
            }
        }
    }

    return result;
}

osel_int8_t osel_device_open(osel_device_t *dev, osel_uint16_t oflag)
{
    osel_int8_t result = 0;

    DBG_ASSERT(dev != NULL __DBG_LINE);

    if (dev->init != NULL)
    {
        if (!(dev->flag & OSEL_DEVICE_FLAG_ACTIVATED))
        {
            result = dev->init(dev);
            if ( result != 0)
            {
                PRINTF("osel device open %s failed.\r\n", dev->name);
            }
            else
            {
                dev->flag |= OSEL_DEVICE_FLAG_ACTIVATED;
            }
        }
    }
    else
    {
        return -1;
    }

    if (dev->oflag & OSEL_DEVICE_OFLAG_OPEN)
        return 0;

    dev->ref_count++;
    DBG_ASSERT(dev->ref_count != 0 __DBG_LINE);

    if (dev->open != NULL)
        result = dev->open(dev, oflag);

    if (result == 0)
        dev->oflag = oflag | OSEL_DEVICE_OFLAG_OPEN;

    return result;
}

osel_int8_t osel_device_close(osel_device_t *dev)
{
    osel_int8_t result = 0;
    DBG_ASSERT(dev != NULL __DBG_LINE);

    if (dev->ref_count == 0)
        return -1;

    dev->ref_count--;

    if (dev->ref_count != 0)
        return 0;

    if (dev->close != NULL)
    {
        result = dev->close(dev);
    }

    if (result == 0)
        dev->oflag = OSEL_DEVICE_OFLAG_CLOSE;

    return result;
}

osel_uint32_t osel_device_read(osel_device_t *dev,
                               osel_uint32_t  pos,
                               void          *buffer,
                               osel_uint32_t  size)
{
    DBG_ASSERT(dev != NULL __DBG_LINE);

    if (dev->ref_count == 0)
    {
        PRINTF("dev %s is not open\r\n", dev->name);
        return 0;
    }

    if ( dev->read != NULL)
    {
        return dev->read(dev, pos, buffer, size);
    }

    return 0;
}


osel_uint32_t osel_device_write(osel_device_t *dev,
                                osel_uint32_t  pos,
                                const void    *buffer,
                                osel_uint32_t  size)
{
    DBG_ASSERT(dev != NULL __DBG_LINE);

    if (dev->ref_count == 0)
    {
        PRINTF("dev %s is not open\r\n", dev->name);
        return 0;
    }

    if ( dev->write != NULL)
    {
        return dev->write(dev, pos, buffer, size);
    }

    return 0;
}

osel_uint32_t  osel_device_control(osel_device_t *dev, osel_uint8_t cmd, void *arg)
{
    DBG_ASSERT(dev != NULL __DBG_LINE);

    if (dev->control != NULL)
    {
        return dev->control(dev, cmd, arg);
    }

    return 0;
}

osel_int8_t
osel_device_set_rx_indicate(osel_device_t *dev,
                            osel_int8_t (*rx_ind)(osel_device_t *dev, osel_uint32_t size))
{
    DBG_ASSERT(dev != NULL __DBG_LINE);
    dev->rx_indicate = rx_ind;
    return 0;
}

osel_int8_t
osel_device_set_tx_complete(osel_device_t *dev,
                            osel_int8_t (*tx_done)(osel_device_t *dev, void *buffer))
{
    DBG_ASSERT(dev != NULL __DBG_LINE);
    dev->tx_complate = tx_done;
    return 0;
}







