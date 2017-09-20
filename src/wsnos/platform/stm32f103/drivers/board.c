#include "common/lib/lib.h"
#include "driver.h"
#include "sys_arch/osel_arch.h"
#include <stdlib.h>

/*!
 * Unique Devices IDs register set ( STM32F103 )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )

void board_reset(void)
{
    HAL_NVIC_SystemReset();
}


void board_init(void)
{
    clk_init(SYSCLK_8MHZ);
//    wdt_start(16000);
}

void board_deinit(void)
{
    ;
}

void led_init(void)
{
    GPIO_InitTypeDef  gpio_init_struct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio_init_struct.Pin = GPIO_PIN_14 | GPIO_PIN_13;
    gpio_init_struct.Mode   = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull   = GPIO_NOPULL;
    gpio_init_struct.Speed  = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
}


void energy_init(void)
{
    ;
}

uint8_t energy_get(void)
{
    return 100;
}


uint16_t energy_value(void)
{
    return 4200ul;
}

void srand_arch(void)
{
    extern void l_srand(uint32_t seed);
    l_srand(HTIMER_GET_COMPREG());   //*< 用自定义的srand替代标准srand
}

void lpm_arch(void)
{
    //@noto 如果用仿真器调试，关闭低功耗，否则仿真器无法正常调试
//    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); //配置使用RTC低功耗alarm
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); //配套使用TIM低功耗定时器，可以停止
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

void BoardGetUniqueId( uint8_t *id )
{
    // id[7] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) ) >> 24;
    // id[6] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) ) >> 16;
    // id[5] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) ) >> 8;
    // id[4] = ( ( *( uint32_t* )ID1 ) + ( *( uint32_t* )ID3 ) );
    // id[3] = ( ( *( uint32_t* )ID2 ) ) >> 24;
    // id[2] = ( ( *( uint32_t* )ID2 ) ) >> 16;
    // id[1] = ( ( *( uint32_t* )ID2 ) ) >> 8;
    // id[0] = ( ( *( uint32_t* )ID2 ) );

    id[7] = ( ID_01 ) ;
    id[6] = ( ID_01 ) >> 8;
    id[5] = ( ID_01 ) >> 16;
    id[4] = (ID_01 ) >> 24;
    nwk_id = ID_01;
    id[3] = ( ID_02 ) ;
    id[2] = ( ID_02 ) >> 8;
    id[1] = ( ID_02 ) >> 16;
    id[0] = (ID_02 ) >> 24;
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

    osel_memcpy(&ID_01, id_buf, sizeof(uint32_t));
    nwk_id = ID_01;
    osel_memcpy(&ID_02, &id_buf[4], sizeof(uint32_t));
}

void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
