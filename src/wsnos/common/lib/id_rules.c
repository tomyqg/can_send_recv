/**
 * @brief       : this is general ID rules.
 * @file        : id_rules.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-08-25
 * change logs  :
 * Date       Version     Author        Note
 * 2016-08-25  v0.0.1  gang.cheng    first version
 */

#include "common/lib/lib.h"
#include "common/lib/id_rules.h"

#define ITCS_VER            0x32        //3.0 == 050

static bool_t bcd_check(uint8_t val)
{
    uint8_t val_h = val >> 4;
    uint8_t val_l = val & 0x0F;

    if (val_h > 0x09)
        return FALSE;
    if (val_l > 0x09)
        return FALSE;
    return TRUE;
}

static uint32_t mpow(uint32_t x, uint32_t y)
{
    if (y == 0)
        return 1;

    if (y == 1)
        return x;

    uint32_t val = x;
    for (uint8_t i = 2; i <= y; i++)
    {
        val = val * x;
    }
    return val;
}

/**
 * @biref 把16进制的hex当做10进制数进行转换，把16进制的0x1608250546当做十进制的
 * 1608250546,转换成0x5FDBF4B2
 * @param  val [description]
 * @return 转换后的ID
 */
static uint32_t hex2int(uint64_t val)
{
    uint32_t n_val = 0;
    for (int8_t i = 0; i < 10; i++)
    {
        n_val += (val & 0x0full) * mpow(10, i);
        val = val >> 4;
    }

    return n_val;
}


/*
 *@brief 初始化id规则，指定应用、类型
 */
void id_rules_init(void)
{
    ;
}

/**
 * @brief 8字节的ID以大端形式存放在数组里
 */
bool_t id_general(uint8_t array[], uint8_t num, uint32_t *id)
{
    if (num != sizeof(identifiers_t))
        return FALSE;
    if (id == NULL)
        return FALSE;
    //*< 把ID拷贝到结构体里，便于访问
    identifiers_t uid;
    osel_memcpy(&uid, array, num);
    //*< 判断生产信息
    // if(uid.pd_info.name != NAME_ITCS)
    //     return FALSE;
    // if(uid.pd_info.category != CATE_ITCS_C)
    //     return FALSE;
    //*< 判断设备类型
    //...

    //*< 判断版本号
//   if(uid.ver.ver != ITCS_VER)
//       return FALSE;
    //*< 判断生产批次格式
    if ( !bcd_check(uid.pd_batch.year) ||
            !bcd_check(uid.pd_batch.month) ||
            !bcd_check(uid.pd_batch.day))
        return FALSE;
    //*< 判断批次号格式
    if ( !bcd_check((uint8_t)(uid.ser_num.num >> 8)) ||
            !bcd_check((uint8_t)uid.ser_num.num))
        return FALSE;

    uint64_t v_id = (uint64_t)uid.pd_batch.year     << 32 |
                    (uint64_t)uid.pd_batch.month    << 24 |
                    (uint64_t)uid.pd_batch.day      << 16 |
                    (uint64_t)(uid.ser_num.num & 0xFF) << 8  |
                    (uint64_t)uid.ser_num.num >> 8;

    // *id = S2B_UINT32(hex2int(v_id));//*< restore id to big end
    *id = hex2int(v_id); //*< sotre id to small end
    return TRUE;
}


//static uint8_t id_array[] = {0x10, 0x12, 0x1e, 0x16,0x08,0x25, 0x05,0x46};
//static uint32_t m_uid;
//
//void id_test(void)
//{
//    id_general(id_array, 8, &m_uid);
//    DBG_ASSERT(m_uid == 0xB2F4DB5F __DBG_LINE);
//}

