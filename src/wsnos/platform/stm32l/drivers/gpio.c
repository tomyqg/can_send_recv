#include "common/lib/lib.h"
#include "driver.h"

gpio_irq_handler_t gpio_irq_array[16];

static inline void gpio_find_port(pin_id_t *pin_id)
{
    DBG_ASSERT(NULL != pin_id __DBG_LINE);

    switch (pin_id->pin)
    {
    case PORT1:
        pin_id->port = GPIOA;
        break;
    case PORT2:
        pin_id->port = GPIOB;
        break;
    case PORT3:
        pin_id->port = GPIOC;
        break;
    case PORT4:
        pin_id->port = GPIOD;
        break;
    case PORT5:
        pin_id->port = GPIOE;
        break;
    case PORT6:
        pin_id->port = GPIOH;
        break;
    default:
        DBG_ASSERT(FALSE __DBG_LINE);
        break;
    }
}

void gpio_set(pin_id_t pin_id)
{
    gpio_find_port(&pin_id);
    HAL_GPIO_WritePin(pin_id.port, pin_id.bit, (GPIO_PinState)1);
}

void gpio_clr(pin_id_t pin_id)
{
    gpio_find_port(&pin_id);
    HAL_GPIO_WritePin(pin_id.port, pin_id.bit, (GPIO_PinState)0);
}

void gpio_toggle(pin_id_t pin_id)
{
    gpio_find_port(&pin_id);
    HAL_GPIO_TogglePin(pin_id.port, pin_id.bit);
}

void gpio_make_input(pin_id_t pin_id)
{
    ;
}

bool_t gpio_is_input(pin_id_t pin_id)
{
    return TRUE;
}


void gpio_make_output(pin_id_t pin_id)
{
    ;
}

bool_t gpio_is_output(pin_id_t pin_id)
{
    return TRUE;
}

void gpio_sel(pin_id_t pin_id)
{
    ;
}

bool_t gpio_get(pin_id_t pin_id)
{
    gpio_find_port(&pin_id);
    return HAL_GPIO_ReadPin(pin_id.port, pin_id.bit);
}

void gpio_interrupt_enable(pin_id_t pin_id)
{
    IRQn_Type irq_nb = EXTI0_IRQn;
    switch (pin_id.bit)
    {
    case GPIO_PIN_0:
        irq_nb = EXTI0_IRQn;
        break;
    case GPIO_PIN_1:
        irq_nb = EXTI1_IRQn;
        break;
    case GPIO_PIN_2:
        irq_nb = EXTI2_IRQn;
        break;
    case GPIO_PIN_3:
        irq_nb = EXTI3_IRQn;
        break;
    case GPIO_PIN_4:
        irq_nb = EXTI4_IRQn;
        break;
    case GPIO_PIN_5:
    case GPIO_PIN_6:
    case GPIO_PIN_7:
    case GPIO_PIN_8:
    case GPIO_PIN_9:
        irq_nb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
    case GPIO_PIN_13:
    case GPIO_PIN_14:
    case GPIO_PIN_15:
        irq_nb = EXTI15_10_IRQn;
        break;
    default:
        DBG_ASSERT(FALSE __DBG_LINE);
        break;
    }

    HAL_NVIC_EnableIRQ(irq_nb);
}

void gpio_interrupt_disable(pin_id_t pin_id)
{
    IRQn_Type irq_nb = EXTI0_IRQn;
    switch (pin_id.bit)
    {
    case GPIO_PIN_0:
        irq_nb = EXTI0_IRQn;
        break;
    case GPIO_PIN_1:
        irq_nb = EXTI1_IRQn;
        break;
    case GPIO_PIN_2:
        irq_nb = EXTI2_IRQn;
        break;
    case GPIO_PIN_3:
        irq_nb = EXTI3_IRQn;
        break;
    case GPIO_PIN_4:
        irq_nb = EXTI4_IRQn;
        break;
    case GPIO_PIN_5:
    case GPIO_PIN_6:
    case GPIO_PIN_7:
    case GPIO_PIN_8:
    case GPIO_PIN_9:
        irq_nb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
    case GPIO_PIN_13:
    case GPIO_PIN_14:
    case GPIO_PIN_15:
        irq_nb = EXTI15_10_IRQn;
        break;
    default:
        DBG_ASSERT(FALSE __DBG_LINE);
        break;
    }

    HAL_NVIC_EnableIRQ(irq_nb);
}

void gpio_interrupt_edge(pin_id_t pin_id, uint8_t edge)
{
    gpio_find_port(&pin_id);

    GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.Pin   = pin_id.bit;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    if (edge == IRQ_RISING_EDGE)
    {
        GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    }
    else if (edge == IRQ_FALLING_EDGE)
    {
        GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    }
    else
    {
        GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
    }

    HAL_GPIO_Init(pin_id.port, &GPIO_InitStructure);
}

void gpio_interrupt_clear(pin_id_t pin_id)
{

}

void gpio_interrupt_set(pin_id_t pin_id, irq_mode_e irq_mode, irq_prioritie_e irq_prio, gpio_irq_handler_t irq_handler)
{
//    DBG_ASSERT(NULL != irq_handler __DBG_LINE);

    uint32_t priority = 0;
    IRQn_Type irq_nb = EXTI0_IRQn;
    GPIO_InitTypeDef GPIO_InitStructure;

    if (irq_handler == NULL)
    {
        return;
    }

    gpio_find_port(&pin_id);

    GPIO_InitStructure.Pin   = pin_id.bit;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    if (irq_mode == IRQ_RISING_EDGE)
    {
        GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    }
    else if (irq_mode == IRQ_FALLING_EDGE)
    {
        GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    }
    else
    {
        GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
    }

    HAL_GPIO_Init(pin_id.port, &GPIO_InitStructure);

    switch (irq_prio)
    {
    case IRQ_VERY_LOW_PRIORITY:
    case IRQ_LOW_PRIORITY:
        priority = 3;
        break;
    case IRQ_MEDIUM_PRIORITY:
        priority = 2;
        break;
    case IRQ_HIGH_PRIORITY:
        priority = 1;
        break;
    default:
        break;
    }

     switch (pin_id.bit)
    {
    case GPIO_PIN_0:
        irq_nb = EXTI0_IRQn;
        break;
    case GPIO_PIN_1:
        irq_nb = EXTI1_IRQn;
        break;
    case GPIO_PIN_2:
        irq_nb = EXTI2_IRQn;
        break;
    case GPIO_PIN_3:
        irq_nb = EXTI3_IRQn;
        break;
    case GPIO_PIN_4:
        irq_nb = EXTI4_IRQn;
        break;
    case GPIO_PIN_5:
    case GPIO_PIN_6:
    case GPIO_PIN_7:
    case GPIO_PIN_8:
    case GPIO_PIN_9:
        irq_nb = EXTI9_5_IRQn;
        break;
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
    case GPIO_PIN_13:
    case GPIO_PIN_14:
    case GPIO_PIN_15:
        irq_nb = EXTI15_10_IRQn;
        break;
    default:
        DBG_ASSERT(FALSE __DBG_LINE);
        break;
    }
    
    uint8_t index = 0;
    uint16_t bit = pin_id.bit;
    if( bit > 0 )
    {
        while( bit != 0x01 )
        {
            bit = bit >> 1;
            index++;
        }
    }

    gpio_irq_array[index] = irq_handler;

    HAL_NVIC_SetPriority(irq_nb, priority, 0);
    HAL_NVIC_EnableIRQ(irq_nb);
}

void gpio_clr_interrupt(pin_id_t *pin_id)
{
    DBG_ASSERT(NULL != pin_id __DBG_LINE);

    gpio_find_port(pin_id);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = pin_id->bit;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;

    HAL_GPIO_Init(pin_id->port, &GPIO_InitStructure);

    //*< 是否想要清空中断注册、中断使能？？？
}

bool_t gpio_init(pin_id_t pin_id, pin_mode_e mode, pin_config_e config, pin_type_e type, uint32_t value)
{
    gpio_find_port(&pin_id);

    switch (pin_id.pin)
    {
    case PORT1:
        __HAL_RCC_GPIOA_CLK_ENABLE();
        break;
    case PORT2:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
    case PORT3:
        __HAL_RCC_GPIOC_CLK_ENABLE();
        break;
    case PORT4:
        __HAL_RCC_GPIOD_CLK_ENABLE();
        break;
    case PORT5:
        __HAL_RCC_GPIOE_CLK_ENABLE();
        break;
    case PORT6:
        __HAL_RCC_GPIOH_CLK_ENABLE();
        break;
    default:
        DBG_ASSERT(FALSE __DBG_LINE);
        break;
    }

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin   = pin_id.bit;
    GPIO_InitStructure.Pull  = type;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    if ( mode == PIN_INPUT )
    {
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    }
    else if ( mode == PIN_ANALOGIC )
    {
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    }
    else if ( mode == PIN_ALTERNATE_FCT )
    {
        if ( config == PIN_OPEN_DRAIN )
        {
            GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
        }
        else
        {
            GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        }
        GPIO_InitStructure.Alternate = value;
    }
    else // mode ouptut
    {
        if ( config == PIN_OPEN_DRAIN )
        {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
        }
        else
        {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        }
    }

    HAL_GPIO_Init(pin_id.port, &GPIO_InitStructure);

    if (mode == PIN_OUTPUT)
    {
        HAL_GPIO_WritePin(pin_id.port, pin_id.bit, (GPIO_PinState)value);
    }

    return TRUE;
}

void gpio_deinit(pin_id_t pin_id)
{
    gpio_find_port(&pin_id);
    HAL_GPIO_DeInit(pin_id.port, pin_id.bit);
}


void EXTI0_IRQHandler( void )
{
    OSEL_ISR_ENTRY();
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );
    OSEL_ISR_EXIT();
}

void EXTI1_IRQHandler( void )
{
    OSEL_ISR_ENTRY();
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );
    OSEL_ISR_EXIT();
}

void EXTI2_IRQHandler( void )
{
    OSEL_ISR_ENTRY();
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_2 );
    OSEL_ISR_EXIT();
}

void EXTI3_IRQHandler( void )
{
    OSEL_ISR_ENTRY();
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_3 );
    OSEL_ISR_EXIT();
}

void EXTI4_IRQHandler( void )
{
    OSEL_ISR_ENTRY();
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_4 );
    OSEL_ISR_EXIT();
}

void EXTI9_5_IRQHandler( void )
{
    OSEL_ISR_ENTRY();
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_5 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_8 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_9 );
    OSEL_ISR_EXIT();
}

void EXTI15_10_IRQHandler( void )
{
    OSEL_ISR_ENTRY();
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_10 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_11 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_12 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_14 );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_15 );
    OSEL_ISR_EXIT();
}

void HAL_GPIO_EXTI_Callback( uint16_t gpioPin )
{
    uint8_t callbackIndex = 0;

    if( gpioPin > 0 )
    {
        while( gpioPin != 0x01 )
        {
            gpioPin = gpioPin >> 1;
            callbackIndex++;
        }
    }

    if( gpio_irq_array[callbackIndex] != NULL )
    {
        gpio_irq_array[callbackIndex]( );
    }
}
