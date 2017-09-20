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
    HAL_IWDG_Refresh(&hiwdg);
}

void wdt_stop(uint64_t time)
{
    ;
}