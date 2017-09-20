#include "common/lib/lib.h"
#include "driver.h"
#include "sys_arch/osel_arch.h"
#include <stdlib.h>

/*!
 * Unique Devices IDs register set ( STM32L1xxx )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )


void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    wsnos_ticks();
}

static void systick_init(uint16_t ms)
{
    RTC_HandleTypeDef RtcHandle;

    __HAL_RCC_PWR_CLK_ENABLE();                         //*< Enable the PWR clock

    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BACKUPRESET_FORCE();
    __HAL_RCC_BACKUPRESET_RELEASE();

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState            = RCC_LSE_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
    }

    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* Configure the RTC data register and RTC prescaler */
    RtcHandle.Instance = RTC;
    RtcHandle.Init.AsynchPrediv = 0x7F;
    RtcHandle.Init.SynchPrediv  = 0xFF;
    RtcHandle.Init.HourFormat   = RTC_HOURFORMAT_24;
    HAL_RTC_Init(&RtcHandle);

    /* Enable the RTC Clock */
    __HAL_RCC_RTC_ENABLE();

    /* Wait for RTC APB registers synchronisation */
    //RTC_WaitForSynchro(); //HAL_RTC_WaitForSynchro(&RtcHandle);
    /* EXTI configuration *******************************************************/
    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();

    /* Enable the RTC Wakeup Interrupt */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

    /* Configure the RTC WakeUp Clock source: RTC/div16 (488.28us/tick) */
    /* Enable the RTC Wakeup Interrupt */
    /* Enable Wakeup Counter */
    uint16_t ticks = (ms * 1000) / 488;
    HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, ticks, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
}

void board_reset(void)
{
    HAL_NVIC_SystemReset();
}


void board_init(void)
{
//    gpio_lowpower_init(void);
    clk_init(SYSCLK_8MHZ);
    systick_init(OSEL_TICK_PER_MS);
//    wdt_start(16000);
}

void board_deinit(void)
{
    ;
}

void led_init(void)
{
    pin_id_t led1,led2;
    led1.pin = 4;
    led1.bit = 0x01 << ( 3 );
    gpio_init( led1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 ); 

    led2.pin = 4;
    led2.bit = 0x01 << ( 4 );
    gpio_init( led2, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );      
}

void buzzer_init(void)
{
    pin_id_t buzzer;
    buzzer.pin = 1;
    buzzer.bit = 0x01 << ( 12 );
    gpio_init( buzzer, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
}

COMP_HandleTypeDef Comp1Handle;

void energy_init(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStructure.Pin =  GPIO_PIN_10;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    __HAL_RCC_OPAMP_CLK_ENABLE();
    Comp1Handle.Instance = COMP1;
    Comp1Handle.Init.NonInvertingInput = COMP_NONINVERTINGINPUT_PE10;
    Comp1Handle.Init.NonInvertingInputPull = COMP_NONINVERTINGINPUT_400KPD;
    HAL_COMP_Init(&Comp1Handle);
    HAL_COMP_Start(&Comp1Handle);
}

uint8_t energy_get(void)
{
    return HAL_COMP_GetOutputLevel(&Comp1Handle);
}

void srand_arch(void)
{
    extern void l_srand(uint32_t seed);
    l_srand(__HAL_TIM_GET_COUNTER(&rtim));   //*< 用自定义的srand替代标准srand
}

void lpm_arch(void)
{
    //@noto 如果用仿真器调试，关闭低功耗，否则仿真器无法正常调试
//    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI); 
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}


void delay_us(uint32_t us)
{
    for (uint32_t i = 0; i < us; i++)
    {
        for (uint32_t j = 0; j < 8; j++)
        {
            ;
        }
    }
}


uint32_t BoardGetRandomSeed( void )
{
    return ( ( *( uint32_t* )ID1 ) ^ ( *( uint32_t* )ID2 ) ^ ( *( uint32_t* )ID3 ) );
}

static uint32_t ID_01 = 0x10090001;
static uint32_t ID_02 = 0x10153216;
static uint32_t nwk_id = 0;
#define USED_ORIGIN_ID_RULE     (1u)

void BoardGetUniqueId( uint8_t *id )
{
#if USED_ORIGIN_ID_RULE
    id[7] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) ) >> 24;
    id[6] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) ) >> 16;
    id[5] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) ) >> 8;
    id[4] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) );
    id[3] = ( ( *( uint32_t* )ID2 ) ) >> 24;
    id[2] = ( ( *( uint32_t* )ID2 ) ) >> 16;
    id[1] = ( ( *( uint32_t* )ID2 ) ) >> 8;
    id[0] = ( ( *( uint32_t* )ID2 ) );
#else
    id[7] = ( ID_01 ) ;
    id[6] = ( ID_01 ) >> 8;
    id[5] = ( ID_01 ) >> 16;
    id[4] = (ID_01 ) >> 24;
    nwk_id = ID_01;
    id[3] = ( ID_02 ) ;
    id[2] = ( ID_02 ) >> 8;
    id[1] = ( ID_02 ) >> 16;
    id[0] = (ID_02 ) >> 24;
#endif
}


uint32_t lorawan_device_addr(void)
{
    return nwk_id;
}

void lorawan_uid_set(unsigned char *id)
{
    uint8_t id_buf[8];
    for (uint8_t i = 0; i < 8; i++)
    {
        id_buf[i] = *(id + 7 - i);
    }

#if USED_ORIGIN_ID_RULE
#else
    osel_memcpy(&ID_01, id_buf, sizeof(uint32_t));
    nwk_id = ID_01;
    osel_memcpy(&ID_02, &id_buf[4], sizeof(uint32_t));
#endif
}

