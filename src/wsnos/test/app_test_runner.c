/**
 * @brief       : this
 * @file        : app_test_runner.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-03-06
 * change logs  :
 * Date       Version     Author        Note
 * 2017-03-06  v0.0.1  gang.cheng    first version
 */
#include "gz_sdk.h"

TEST_GROUP_RUNNER(mpool)
{
    RUN_TEST_CASE(mpool, init);
    RUN_TEST_CASE(mpool, alloc_single_right);
    RUN_TEST_CASE(mpool, alloc_single_over);

    RUN_TEST_CASE(mpool, alloc_continuous);
    RUN_TEST_CASE(mpool, alloc_empty);
    RUN_TEST_CASE(mpool, free_single);
    RUN_TEST_CASE(mpool, free_empty);
}

TEST_GROUP_RUNNER(device)
{
    RUN_TEST_CASE(device, register_single);
    RUN_TEST_CASE(device, register_multi);
    RUN_TEST_CASE(device, unregister);
    RUN_TEST_CASE(device, init_single);
    RUN_TEST_CASE(device, init_multi);
    RUN_TEST_CASE(device, open);
    RUN_TEST_CASE(device, open_multi);
    RUN_TEST_CASE(device, close_right);
    RUN_TEST_CASE(device, close_error);
    RUN_TEST_CASE(device, set_rx_indicate);
    RUN_TEST_CASE(device, set_tx_complete);
}