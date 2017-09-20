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

void gpio_low_power_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                               GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                               GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                               GPIO_PIN_12;
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | 
                               GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                               GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                               GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    
    GPIO_InitStructure.Pin   = GPIO_PIN_All;
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    __HAL_RCC_GPIOD_CLK_ENABLE();
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    __HAL_RCC_GPIOE_CLK_ENABLE();
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    __HAL_RCC_GPIOH_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
    
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
    
}

void hal_gpio_all_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
/**********************************GPIOA*********************************/ 
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 |GPIO_PIN_2 | GPIO_PIN_3 |
                                   GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                                   GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                                   GPIO_PIN_12;
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;//输入
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_All, GPIO_PIN_SET);    

    pin_id_t nfc_power;//PA1,NFC的pwer
    nfc_power.pin = 0;
    nfc_power.bit = 0x01 << ( 1 );    
    gpio_clr(nfc_power);       
    __HAL_RCC_GPIOA_CLK_DISABLE();
/*********************************GPIOB**********************************/ 
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 |GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3 |
                                   GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
                                   GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                                   GPIO_PIN_12| GPIO_PIN_13| GPIO_PIN_14; 
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;//输入
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;    
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_All, GPIO_PIN_SET);

    pin_id_t adx345_power;//PB7  adx345 power en 
    adx345_power.pin = 1;
    adx345_power.bit = 0x01 << ( 7 );    
    gpio_clr(adx345_power);
    
    pin_id_t buzzer_io;//PB12  buzzer io 
    buzzer_io.pin = 1;
    buzzer_io.bit = 0x01 << ( 12 );    
    gpio_clr(buzzer_io);    
        
    pin_id_t dht12_power;//PB15, DHT12_PWER 
    dht12_power.pin = 1;
    dht12_power.bit = 0x01 << ( 15 );    
    gpio_clr(dht12_power);
    __HAL_RCC_GPIOB_CLK_DISABLE();
/*********************************GPIOC**********************************/ 
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3 |
                                   GPIO_PIN_4 | GPIO_PIN_5 |
                                   GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                                   GPIO_PIN_12| GPIO_PIN_13; //| GPIO_PIN_14 | GPIO_PIN_15
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;//输入
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;   
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);    
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_SET);

    pin_id_t gsm_key_power;//PC0  关闭gprs powerkey电源
    gsm_key_power.pin = 2;
    gsm_key_power.bit = 0x01 << ( 0 );    
    gpio_clr(gsm_key_power); 
    
    pin_id_t hx711_power;//PC6  关闭hx711 powre en电源  高电平关闭    
    hx711_power.pin = 2;
    hx711_power.bit = 0x01 << ( 6 );
    gpio_init( hx711_power, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 ); //下电 
    
    pin_id_t hx711_rate_power;//PC7  有下拉，所以输出低
    hx711_rate_power.pin = 2;
    hx711_rate_power.bit = 0x01 << ( 7 );    
    gpio_clr(hx711_rate_power);
    
    pin_id_t osc32_in;//PC14  外部时钟
    osc32_in.pin = 2;
    osc32_in.bit = 0x01 << ( 14 );    
    //gpio_clr(osc32_in);
    gpio_init( osc32_in, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
    
    pin_id_t osc32_out;//PC15  外部时钟
    osc32_out.pin = 2;
    osc32_out.bit = 0x01 << ( 15 );    
    //gpio_clr(osc32_out);
    gpio_init( osc32_out, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
    __HAL_RCC_GPIOC_CLK_DISABLE();
/*********************************GPIOD**********************************/ 
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 |GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3 |
                                   GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                                   GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | 
                                   GPIO_PIN_14 | GPIO_PIN_15; 
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;//输入
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;      
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);    
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_All, GPIO_PIN_SET);
    
    pin_id_t rf_power;//PD11  关闭RF电源
    rf_power.pin = 3;
    rf_power.bit = 0x01 << ( 11 );    
    gpio_clr(rf_power);     
    
    pin_id_t gsm_en_power;//PD12  关闭gprs en电源
    gsm_en_power.pin = 3;
    gsm_en_power.bit = 0x01 << ( 12 );    
    gpio_clr(gsm_en_power);
    
    pin_id_t weight_en_power;//PD13  关闭weight en电源
    weight_en_power.pin = 3;
    weight_en_power.bit = 0x01 << ( 13 );    
    gpio_clr(weight_en_power);
    __HAL_RCC_GPIOD_CLK_DISABLE();
/*********************************GPIOE**********************************/
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 |GPIO_PIN_1 |GPIO_PIN_2 | 
                                   GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                                   GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11 |
                                   GPIO_PIN_12| GPIO_PIN_13| GPIO_PIN_14| GPIO_PIN_15; 
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;//输入
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;      
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);    
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_All, GPIO_PIN_SET);

    pin_id_t led01;//PE3  LED1
    led01.pin = 4;
    led01.bit = 0x01 << ( 3 );
    gpio_init( led01, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
    
    pin_id_t led02;//PE4  LED2
    led02.pin = 4;
    led02.bit = 0x01 << ( 4 );
    gpio_init( led02, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );    
    
    pin_id_t adc25_power;//PE10  关闭adc25 电源
    adc25_power.pin = 4;
    adc25_power.bit = 0x01 << ( 10 );
    gpio_init( adc25_power, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 ); //下电     
    __HAL_RCC_GPIOE_CLK_DISABLE();
/*********************************GPIOH**********************************/    
    __HAL_RCC_GPIOH_CLK_ENABLE();
    GPIO_InitStructure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
    __HAL_RCC_GPIOH_CLK_DISABLE();
}

void clk_msi_set(void)
{
    HAL_Init();
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};     
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
 
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_0;/*!< MSI = 65.536 KHz  */
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        DBG_ASSERT(FALSE __DBG_LINE);
    }    

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
    clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)	{
        DBG_ASSERT(FALSE __DBG_LINE);
    }    

    SystemCoreClockUpdate();
}

void board_init(void)
{
//    gpio_lowpower_init(void);
    clk_init(SYSCLK_8MHZ);
    systick_init(OSEL_TICK_PER_MS);
//    wdt_start(16000);
    hal_gpio_all_init();
    //gpio_low_power_init();
    buzzer_init();
    //hx711_deinit();
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
    //HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI); 
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);//PWR_SLEEPENTRY_WFE,PWR_SLEEPENTRY_WFI

    //HAL_PWREx_EnableLowPowerRunMode();

//    HAL_RCC_DeInit();
//    //__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);
//    //__HAL_FLASH_PREFETCH_BUFFER_DISABLE();
//    //__HAL_FLASH_ACC64_DISABLE();
//    
//    
//    /* Select the Voltage Range (1.5V) */
//    __HAL_RCC_PWR_CLK_ENABLE();
//    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
//    while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};
//      
//    
//    
//    clk_msi_set();
//    
//    /* Enable The ultra Low Power Mode */
//    HAL_PWREx_EnableUltraLowPower();
//    HAL_PWREx_EnableFastWakeUp();
//    /* Enable the power down mode during Sleep mode */
//    __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();
//    /* Request to enter SLEEP mode with regulator ON */
//    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFE);
//    //HAL_PWR_EnableSleepOnExit();
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

