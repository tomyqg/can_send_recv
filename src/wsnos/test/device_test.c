/**
 * @brief       : this
 * @file        : device_test.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-05-05
 * change logs  :
 * Date       Version     Author        Note
 * 2015-05-05  v0.0.1  gang.cheng    first version
 */
#include "gz_sdk.h"

DBG_THIS_MODULE("device_test");

static osel_device_t dev_one;
static osel_device_t dev_two;

#define DEV_ONE_NAME        "devOne"
#define DEV_TWO_NAME        "devTwo"

TEST_GROUP(device);

static osel_int8_t dev_init(osel_device_t *dev)
{
    PRINTF("\tdev: %s init.\r\n", dev->name);
    return 0;
}

static osel_int8_t dev_open(osel_device_t *dev, osel_int16_t oflag)
{
    PRINTF("\tdev: %s open, oflag is 0x%04x.\r\n", dev->name, oflag);
    return 0;
}

static osel_int8_t dev_close(osel_device_t *dev)
{
    PRINTF("\tdev: %s close.\r\n", dev->name);
    return 0;
}

static osel_int8_t dev_rx_ind(osel_device_t *dev, osel_uint32_t size)
{
    PRINTF("\tdev: %s rx ind.\r\n", dev->name);

    return 0;
}

static osel_int8_t dev_tx_done(osel_device_t *dev, void *buffer)
{
    PRINTF("\tdev: %s tx done.\r\n", dev->name);
    return 0;
}

TEST_SETUP(device)
{
    dev_one.init = dev_init;
    dev_one.open = dev_open;
    dev_one.close = dev_close;

    dev_two.init = dev_init;
    dev_two.open = dev_open;
    dev_two.close = dev_close;
}

TEST_TEAR_DOWN(device)
{
    osel_device_unregister(&dev_one);
    osel_device_unregister(&dev_two);
}

TEST(device, register_single)
{
    osel_device_register(&dev_one, DEV_ONE_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_t *dev = osel_device_find(DEV_ONE_NAME);
    TEST_ASSERT_EQUAL(&dev_one, dev);
}


TEST(device, register_multi)
{
    osel_device_register(&dev_one, DEV_ONE_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_t *dev1 = osel_device_find(DEV_ONE_NAME);
    TEST_ASSERT_EQUAL(&dev_one, dev1);

    osel_device_register(&dev_two, DEV_TWO_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_t *dev2 = osel_device_find(DEV_TWO_NAME);
    TEST_ASSERT_EQUAL(&dev_two, dev2);
}

TEST(device, unregister)
{
    osel_device_register(&dev_one, DEV_ONE_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_t *dev1 = osel_device_find(DEV_ONE_NAME);
    TEST_ASSERT_EQUAL(&dev_one, dev1);
    osel_device_unregister(&dev_one);
    dev1 = osel_device_find(DEV_ONE_NAME);
    TEST_ASSERT_EQUAL(NULL, dev1);

    osel_device_register(&dev_two, DEV_TWO_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_t *dev2 = osel_device_find(DEV_TWO_NAME);
    TEST_ASSERT_EQUAL(&dev_two, dev2);
    osel_device_unregister(&dev_two);
    dev2 = osel_device_find(DEV_TWO_NAME);
    TEST_ASSERT_EQUAL(NULL, dev2);
}


TEST(device, init_single)
{
    osel_device_register(&dev_one, DEV_ONE_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_init(&dev_one);
}

TEST(device, init_multi)
{
    osel_device_register(&dev_one, DEV_ONE_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_register(&dev_two, DEV_TWO_NAME, OSEL_DEVICE_FLAG_RDWR);
    osel_device_init_all();
}

TEST(device, open)
{
    int8_t res = osel_device_register(&dev_one, DEV_ONE_NAME, OSEL_DEVICE_FLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_open(&dev_one, OSEL_DEVICE_OFLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_close(&dev_one);
    TEST_ASSERT_EQUAL(0, res);
}

TEST(device, open_multi)
{
    int8_t res = osel_device_register(&dev_one, DEV_ONE_NAME, OSEL_DEVICE_FLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_open(&dev_one, OSEL_DEVICE_OFLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_open(&dev_one, OSEL_DEVICE_OFLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_close(&dev_one);
    TEST_ASSERT_EQUAL(0, res);
}

TEST(device, close_right)
{
    int8_t res = osel_device_register(&dev_two, DEV_TWO_NAME, OSEL_DEVICE_FLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_open(&dev_two, OSEL_DEVICE_OFLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_close(&dev_two);
    TEST_ASSERT_EQUAL(0, res);
}

TEST(device, close_error)
{
    int8_t res = osel_device_register(&dev_two, DEV_TWO_NAME, OSEL_DEVICE_FLAG_RDWR);
    TEST_ASSERT_EQUAL(0, res);

    res = osel_device_close(&dev_two);
    TEST_ASSERT_EQUAL(-1, res);
}

TEST(device, set_rx_indicate)
{
    osel_device_set_rx_indicate(&dev_one, dev_rx_ind);

    TEST_ASSERT_NOT_NULL(dev_one.rx_indicate);
    if (dev_one.rx_indicate != NULL)
        dev_one.rx_indicate(&dev_one, 0);
}

TEST(device, set_tx_complete)
{
    osel_device_set_tx_complete(&dev_one, dev_tx_done);

    TEST_ASSERT_NOT_NULL(dev_one.tx_complate);
    if (dev_one.tx_complate != NULL)
        dev_one.tx_complate(&dev_one, NULL);
}






