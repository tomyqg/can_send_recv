/**
 * @brief       : Clear Channel Assessment and relative functions
 *
 * @file        : phy_cca.c
 * @author      : gang.cheng
 * @version     : v0.0.1
 * @date        : 2015/5/7
 *
 * Change Logs  :
 *
 * Date        Version      Author      Notes
 * 2015/5/7    v0.0.1      gang.cheng    first version
 */
#include "common/lib/lib.h"
#include "platform/platform.h"
#include "common/dev/dev.h"
#include "phy_cca.h"
#include "phy_state.h"
static int16_t rssi_dbm = 0;
bool_t phy_cca(void)
{
    /* CCA期间不接收数据，接收时间较短，无溢出危险 */    
     // uint8_t value = 0;
//     SSN_RADIO.int_cfg(RX_OK_INT, NULL, INT_DISABLE);
     // SSN_RADIO.get_value(RF_CCA, &value);
//     SSN_RADIO.int_cfg(RX_OK_INT, NULL, INT_ENABLE);
    
     // return value;
   return TRUE;
}

bool_t phy_cca_stop(void)
{
	return TRUE;
}

int8_t phy_get_rssi_largest(void)
{
//    return rf_get_rssi();
    return 0;
}

int16_t phy_get_rssi(void)
{
	return rssi_dbm;
    //return 0;
}

int8_t phy_rssi_average(uint8_t num)
{
    return 0;
}
