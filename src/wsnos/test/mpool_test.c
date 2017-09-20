/**
 * @brief       : mpool模块的白盒测试用例
 * @file        : mpool_test.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-03-06
 * change logs  :
 * Date       Version     Author        Note
 * 2017-03-06  v0.0.1  gang.cheng    first version
 */

#include "gz_sdk.h"

DBG_THIS_MODULE("mpool_test");

#define MPOOL_TEST_NUM              20
#define MPOOL_TEST_SIZE             30

static uint8_t *ptr_array[2][MPOOL_TEST_NUM + 1] = {NULL};

static mpool_manage_t mpool_manage_one; /** mpool管理的链表头 */
static mpool_manage_t mpool_manage_two;
TEST_GROUP(mpool);

TEST_SETUP(mpool)
{
    // PRINTF("\r\nmpool test setup\r\n");
}

TEST_TEAR_DOWN(mpool)
{
    for (uint8_t i = 0; i < MPOOL_TEST_NUM; i++)
    {
        if(ptr_array[0][i] != NULL)
            mpool_free((void **)&ptr_array[0][i] __MLINE);
        TEST_ASSERT_TRUE(ptr_array[0][i] == NULL);
    }

    for (uint8_t i = 0; i < MPOOL_TEST_NUM; i++)
    {
        if(ptr_array[1][i] != NULL)
            mpool_free((void **)&ptr_array[1][i] __MLINE);
        TEST_ASSERT_TRUE(ptr_array[1][i] == NULL);
    }
}

TEST(mpool, init)
{
    osel_memset(&ptr_array[0], 0x00, MPOOL_TEST_NUM * sizeof(uint8_t *));
    osel_memset(&ptr_array[1], 0x00, MPOOL_TEST_NUM * sizeof(uint8_t *));

    int8_t res = mpool_init(&mpool_manage_one, MPOOL_TEST_NUM, MPOOL_TEST_SIZE);
    TEST_ASSERT_EQUAL(0, res);

    res = mpool_init(&mpool_manage_two, MPOOL_TEST_NUM, MPOOL_TEST_SIZE);
    TEST_ASSERT_EQUAL(0, res);
}

TEST(mpool, alloc_single_right)
{
    ptr_array[0][0] = mpool_alloc(&mpool_manage_one, 10 __MLINE);
    TEST_ASSERT_TRUE(ptr_array[0][0] != NULL);
}

TEST(mpool, alloc_single_over)
{
    ptr_array[0][0] = mpool_alloc(&mpool_manage_one, 50 __MLINE);
    TEST_ASSERT_TRUE(ptr_array[0][0] == NULL);
}

TEST(mpool, alloc_continuous)
{
    for (uint8_t i = 0; i < MPOOL_TEST_NUM; i++)
    {
        ptr_array[0][i] = mpool_alloc(&mpool_manage_one, 20 __MLINE);
        TEST_ASSERT_TRUE(ptr_array[0][i] != NULL);
    }
}

TEST(mpool, alloc_empty)
{
    for (uint8_t i = 0; i < MPOOL_TEST_NUM; i++)
    {
        ptr_array[0][i] = mpool_alloc(&mpool_manage_one, 20 __MLINE);
        TEST_ASSERT_TRUE(ptr_array[0][i] != NULL);
    }

    ptr_array[0][0] = mpool_alloc(&mpool_manage_one, 20 __MLINE);
    TEST_ASSERT_TRUE(ptr_array[0][0] == NULL);
}

TEST(mpool, free_single)
{
    ptr_array[0][0] = mpool_alloc(&mpool_manage_one, 10 __MLINE);
    TEST_ASSERT_TRUE(ptr_array[0][0] != NULL);
    mpool_free((void **)&ptr_array[0][0] __MLINE);
    TEST_ASSERT_TRUE(ptr_array[0][0] == NULL);
}

TEST(mpool, free_empty)
{
    mpool_free((void **)&ptr_array[0][0] __MLINE);
    TEST_ASSERT_TRUE(ptr_array[0][0] == NULL);
}

TEST(mpool, mulit_mpool_group)
{
    for (uint8_t i = 0; i < MPOOL_TEST_NUM; i++)
    {
        ptr_array[0][i] = mpool_alloc(&mpool_manage_one, 20 __MLINE);
        TEST_ASSERT_TRUE(ptr_array[0][i] != NULL);

        mpool_free((void **)&ptr_array[0][i] __MLINE);
        TEST_ASSERT_TRUE(ptr_array[0][i] == NULL);
    }
    for (uint8_t i = 0; i < MPOOL_TEST_NUM; i++)
    {
        ptr_array[1][i] = mpool_alloc(&mpool_manage_two, 20 __MLINE);
        TEST_ASSERT_TRUE(ptr_array[1][i] != NULL);

        mpool_free((void **)&ptr_array[1][i] __MLINE);
        TEST_ASSERT_TRUE(ptr_array[1][i] == NULL);
    }
}






