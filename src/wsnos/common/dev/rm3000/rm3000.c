/**
 * @brief       : this
 * @file        : rm3000.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-04-13
 * change logs  :
 * Date       Version     Author        Note
 * 2016-04-13  v0.0.1  gang.cheng    first version
 */
#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "platform/platform.h"

#include "rm3000.h"

DBG_THIS_MODULE("rm3000");

static osel_device_t rm3000_device;

static pin_id_t rm3000_int_gpio;

#define READ_MASK                           0x7F
#define WRITE_MASK                          0x80

/**
 * @note if rm3000 drdy not come, device will reset by watchdog
 */
#define RM3000_WAIT_DRDY_BY_INT()                   \
    do                                              \
    {                                               \
        uint32_t i = 0;                             \
        while(!rm3000_drdy)                         \
        {                                           \
            if (i++ > 70000)                        \
            {                                       \
                DBG_ASSERT(FALSE __DBG_LINE);       \
            }                                       \
            lpm_arch();                             \
        }                                           \
        rm3000_drdy = FALSE;                        \
    } while(__LINE__ == -1)


static volatile bool_t rm3000_drdy = FALSE;

void rm3000_int_handler(void)
{
    rm3000_drdy = TRUE;
}


static int8_t rm3000_driver_init(osel_device_t *dev)
{
    rm3000_spi_init();  //*< hardware spi init
    
//    rm3000_int_init();  //*< hardware interrupt io init
    GPIO_NAME_TO_PIN(&rm3000_int_gpio, PA_2);
    gpio_init(rm3000_int_gpio, PIN_INPUT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0);
    gpio_interrupt_set(rm3000_int_gpio, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, &rm3000_int_handler );
    
    rm3000_clear_init();

    delay_ms(6);

    rm3000_drdy = FALSE;
    
    return 0;
}


static uint32_t rm3000_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buf, uint32_t len)
{
    if ((buf == NULL) || (len == 0))
        return 0;

    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    RM3000_SPI_BEGIN();
    rm3000_clear_set();
    //LEGACY_PS_512 -- 5.4ms检测时间，检测功耗大概1.57ma
    //LEGACY_PS_256 -- 3.0ms检测时间，检测功耗大概1.27ma
    uint8_t cmd = (LEGACY_PS_256 << LEGACY_PS_OFFSET) + (LEGACY_AS_Z_AXIS << LEGACY_AS_OFFSET);
    uint8_t val = rm3000_spi_write_read(READ_MASK & cmd); //*< Z AXIS
    OSEL_EXIT_CRITICAL(s);
    
    if(val == STANDARD_MODE)
    {
        DBG_LOG(DBG_LEVEL_TRACE, "rm3000 in standard mode\n");
        return 0;
    }
    else if(val == LEGACY_MODE)
    {
        ;
    }
    else
        DBG_ASSERT(FALSE __DBG_LINE);
    
    RM3000_WAIT_DRDY_BY_INT();
    
    
    OSEL_ENTER_CRITICAL(s);
    uint8_t array[2];
    array[1] = rm3000_spi_write_read(0xFF);   //*< MSB
    array[0] = rm3000_spi_write_read(0xFF);   //*< LSB
    OSEL_EXIT_CRITICAL(s);
    //*< LEGACY_PS_256
    int16_t data = *(int16_t *)array;
    fp32_t tmp_data = (data/LEGACY_CC_TO_GAIN_256)*10/2.27;    //*< 之前是以2.2mgauss为计算单位的，1mg=0.1uT
    data = (int16_t)tmp_data;
    osel_memcpy(buf, &data, 2);

    RM3000_SPI_END();

    return 2;
}

static osel_uint32_t
rm3000_driver_control(osel_device_t *dev,
                      osel_uint8_t cmd,
                      void *args)
{
    
    return 0;
}

static osel_int8_t rm3000_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start mpl3115a2
    return 0;
}


static osel_int8_t rm3000_driver_close(osel_device_t *dev)
{
    //*< make mpl3115a2 in sleep mode
    return 0;
}

void rm3000_device_init(void)
{
    rm3000_device.init    = rm3000_driver_init;
    rm3000_device.read    = rm3000_driver_read;
    rm3000_device.control = rm3000_driver_control;
    rm3000_device.open    = rm3000_driver_open;
    rm3000_device.close   = rm3000_driver_close;
    
    osel_device_register(&rm3000_device, MAG_SENSOR_NAME, OSEL_DEVICE_FLAG_RDWR);
}



