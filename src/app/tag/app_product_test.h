#pragma once

#include "common/lib/lib.h"

#define CMD_AT_HEAD_SIZE                    2u
#define CMD_CR                              0x0D
#define CMD_LF                              0x0A
#define CMD_SYSSTART                        "SYSSTART\r\n"
#define CMD_OK                              "OK\r\n"
#define CMD_ALOHA                           "ALOHA\r\n"

#define CMD_ERROR00                         "ERROR00\r\n"
#define CMD_ERROR01                         "ERROR01\r\n"
#define CMD_ERROR02                         "ERROR02\r\n"
#define CMD_ERROR03                         "ERROR03\r\n"
#define CMD_ERROR04                         "ERROR04\r\n"
#define CMD_ERROR05                         "ERROR05\r\n"
#define CMD_ERROR06                         "ERROR06\r\n"

#define CMD_TEST_TYPE                       0x0D
#define CMD_EXTER_TYPE                      '+'
#define CMD_EXTER_PARA                      '='
#define CMD_EXTER_ASK                       '?'

#define CMD_EXTER_ALOHA                     "ALOHA"
#define CMD_EXTER_RST                       "RST"
#define CMD_EXTER_HELP                      "HELP"
#define CMD_EXTER_ADXL345S                  "ADXL345S"
#define CMD_EXTER_ADXL345E                  "ADXL345E"
#define CMD_EXTER_MMA8451S                  "MMA8451S"
#define CMD_EXTER_MMA8451E                  "MMA8451E"
#define CMD_EXTER_CANTXS                    "CANTXS"
#define CMD_EXTER_CANTXE                    "CANTXE"
#define CMD_EXTER_CANRXS                    "CANRXS"
#define CMD_EXTER_CANRXE                    "CANRXE"
#define CMD_EXTER_OPERATE_EXIT              "EXIT"

#define CMD_EXTER_OPERATE_VER              "VER"

#define APP_TEST_EVENT_MAX                       (10u)       //*< 最多处理10个事件
#define APP_TEST_TASK_PRIO                  (4u)        //*< 任务优先级4
#define MAG_CYCLE                           (180u)      //*< 传感器采样40毫秒

#pragma pack(1)

typedef enum
{
    APP_SERIAL_1_EVENT =  ((APP_TEST_TASK_PRIO<<8) | 0x01),
    APP_SERIAL_2_EVENT,
    APP_CAN_EVENT,
    APP_RF_TXOK_EVENT,
    APP_RF_RXOK_EVENT,
    APP_RF_UND_EVENT,
    APP_MAGSENSOR_TIMER_EVENT,
    RTC_READ_EVENT,
    RTC_WRITE_EVENT,
    APP_TEST_RESERVE_EVENT,
} app_test_task_sig_enum_t;

#pragma pack()

void app_production_test_init(void);