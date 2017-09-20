#include "common/lib/lib.h"
#include "driver.h"

#if TIMER_DBG_EN > 0
DBG_THIS_MODULE("rtimer_arch");
#endif

RTC_HandleTypeDef  rtim;

/**
  * @brief  Enters the RTC Initialization mode.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef* hrtc)
{
    uint32_t tickstart = 0;

    tickstart = HAL_GetTick();
    /* Wait till RTC is in INIT state and if Time out is reached exit */
    while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
    {
        if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
        {
            return HAL_TIMEOUT;
        }
    }

    /* Disable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);


    return HAL_OK;
}

/**
  * @brief  Exit the RTC Initialization mode.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval HAL status
  */
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef* hrtc)
{
    uint32_t tickstart = 0;

    /* Disable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

    tickstart = HAL_GetTick();
    /* Wait till RTC is in INIT state and if Time out is reached exit */
    while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
    {
        if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
        {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

/**
  * @brief  Read the time counter available in RTC_CNT registers.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval Time counter
  */
uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef* hrtc)
{
    uint16_t high1 = 0, high2 = 0, low = 0;
    uint32_t timecounter = 0;

    high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
    low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
    high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

    if (high1 != high2)
    {
        /* In this case the counter roll over during reading of CNTL and CNTH registers,
           read again CNTL register then return the counter value */
        timecounter = (((uint32_t) high2 << 16 ) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
    }
    else
    {
        /* No counter roll over during reading of CNTL and CNTH registers, counter
           value is equal to first value of CNTL and CNTH */
        timecounter = (((uint32_t) high1 << 16 ) | low);
    }

    return timecounter;
}

HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef* hrtc, uint32_t TimeCounter)
{
    HAL_StatusTypeDef status = HAL_OK;

    /* Set Initialization mode */
    if (RTC_EnterInitMode(hrtc) != HAL_OK)
    {
        status = HAL_ERROR;
    }
    else
    {
        /* Set RTC COUNTER MSB word */
        WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16));
        /* Set RTC COUNTER LSB word */
        WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

        /* Wait for synchro */
        if (RTC_ExitInitMode(hrtc) != HAL_OK)
        {
            status = HAL_ERROR;
        }
    }

    return status;
}

/**
  * @brief  Read the time counter available in RTC_ALR registers.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @retval Time counter
  */
uint32_t RTC_ReadAlarmCounter(RTC_HandleTypeDef* hrtc)
{
    uint16_t high1 = 0, low = 0;

    high1 = READ_REG(hrtc->Instance->ALRH & RTC_CNTH_RTC_CNT);
    low   = READ_REG(hrtc->Instance->ALRL & RTC_CNTL_RTC_CNT);

    return (((uint32_t) high1 << 16 ) | low);
}

/**
  * @brief  Write the time counter in RTC_ALR registers.
  * @param  hrtc   pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  AlarmCounter: Counter to write in RTC_ALR registers
  * @retval HAL status
  */
HAL_StatusTypeDef RTC_WriteAlarmCounter(RTC_HandleTypeDef* hrtc, uint32_t AlarmCounter)
{
    HAL_StatusTypeDef status = HAL_OK;

    /* Set Initialization mode */
    if (RTC_EnterInitMode(hrtc) != HAL_OK)
    {
        status = HAL_ERROR;
    }
    else
    {
        /* Set RTC COUNTER MSB word */
        WRITE_REG(hrtc->Instance->ALRH, (AlarmCounter >> 16));
        /* Set RTC COUNTER LSB word */
        WRITE_REG(hrtc->Instance->ALRL, (AlarmCounter & RTC_ALRL_RTC_ALR));

        /* Wait for synchro */
        if (RTC_ExitInitMode(hrtc) != HAL_OK)
        {
            status = HAL_ERROR;
        }
    }

    return status;
}


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef* htim)
{
    extern void htimer_int_handler(void);
    htimer_int_handler();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    DBG_TIMER(0x14);
    HTIMER_CLEAR_OF_FLG();
    extern void hal_timer_overflow_interrupt_handler(uint16_t line);
    hal_timer_overflow_interrupt_handler(__LINE__);
}

void rtimer_start(void)
{
    RTC_HandleTypeDef RtcHandle;

    __HAL_RCC_PWR_CLK_ENABLE();                         //*< Enable the PWR clock
    HAL_PWR_EnableBkUpAccess();

    /* Enable BKP CLK for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.LSIState            = RCC_LSI_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }

    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* Configure the RTC data register and RTC prescaler */
    rtim.Instance = RTC;
    rtim.Init.AsynchPrediv = 0;    // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(0+1)
    HAL_RTC_Init(&rtim);

    /* Enable the RTC Clock */
    __HAL_RCC_RTC_ENABLE();

    /* Wait for RTC APB registers synchronisation */
    HAL_RTC_WaitForSynchro(&rtim);
    /* EXTI configuration *******************************************************/
    __HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();
    __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();

    /* Enable the RTC Wakeup Interrupt */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    RTC_WriteTimeCounter(&rtim, 0);
}










