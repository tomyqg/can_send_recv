#include "common/lib/lib.h"
#include "driver.h"

IWDG_HandleTypeDef hiwdg;
void wdt_start(uint64_t time)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Reload = (uint64_t)(time/6.4);
    HAL_IWDG_Init(&hiwdg);
    HAL_IWDG_Start(&hiwdg);
}

void wdt_clear(uint64_t time)
{
    IWDG_ENABLE_WRITE_ACCESS(&hiwdg);
   __HAL_IWDG_RELOAD_COUNTER(&hiwdg);

   IWDG_DISABLE_WRITE_ACCESS(&hiwdg);
   
   //*< 操作IWDT的值会导致flash产生写错误标志，需要清空，否则后面Flash操作会判断错误

   __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
}

void wdt_stop(uint64_t time)
{
    ;
}