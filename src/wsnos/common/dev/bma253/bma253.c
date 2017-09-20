/**
 * @brief       : this is implement of bma253 driver(digital, triaxial acceleration sensor)
 * @file        : bma253.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-06-02
 * change logs  :
 * Date       Version     Author        Note
 * 2015-06-02  v0.0.1  gang.cheng    first version
 */
#include "common/lib/lib.h"
#include "sys_arch/osel_arch.h"
#include "platform/platform.h"

#include "bma253.h"

osel_device_t bma253_device;
bma253_context_t bma253_cont;

typedef struct _bma253_frame_
{
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
} bma253_frame_t;

static int8_t bma253_set_power_mode(bma253_context_t *ct, BMA253_POWER_MODE_T power)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    uint8_t reg = 0;
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_PMU_LPW, 1, &reg) != 0)
        return -1;
    reg = reg & _BMA253_PMU_LPW_RESERVED_MASK;
    reg &= ~(_BMA253_PMU_LPW_POWER_MODE_MASK
             << _BMA253_PMU_LPW_POWER_MODE_SHIFT);
    reg |= (power << _BMA253_PMU_LPW_POWER_MODE_SHIFT);

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_PMU_LPW, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_set_range(bma253_context_t *ct, BMA253_RANGE_T range)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_PMU_RANGE, 1, (uint8_t *)&range) != 0)
        return -1;

    switch (range)
    {
    case BMA253_RANGE_2G:
        ct->acc_scale = RANGE_2G_MG_LSB;
        break;
    case BMA253_RANGE_4G:
        ct->acc_scale = RANGE_4G_MG_LSB;
        break;
    case BMA253_RANGE_8G:
        ct->acc_scale = RANGE_8G_MG_LSB;
        break;
    case BMA253_RANGE_16G:
        ct->acc_scale = RANGE_16G_MG_LSB;
        break;
    default:
        break;
    }

    return 0;
}

static int8_t bma253_set_bandwidth(bma253_context_t *ct, BMA253_BW_T bw)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_PMU_BW, 1, (uint8_t *)&bw) != 0)
        return -1;

    return 0;
}

static int8_t bma253_enable_register_shadowing(bma253_context_t *ct, bool_t shadow)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    uint8_t reg = 0;
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_ACCD_HBW, 1, &reg) != 0)
        return -1;

    reg = reg & ~_BMA253_ACC_HBW_RESERVED_BITS;

    if (shadow)
        reg &= ~BMA253_ACC_HBW_SHADOW_DIS;
    else
        reg |= BMA253_ACC_HBW_SHADOW_DIS;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_ACCD_HBW, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_enable_output_filtering(bma253_context_t *ct, bool_t filter)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    uint8_t reg = 0;
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_ACCD_HBW, 1, &reg) != 0)
        return -1;

    reg = reg & ~_BMA253_ACC_HBW_RESERVED_BITS;

    if (filter)
        reg &= ~BMA253_ACC_HBW_DATA_HIGH_BW;
    else
        reg |= BMA253_ACC_HBW_DATA_HIGH_BW;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_ACCD_HBW, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_fifo_config(const bma253_context_t *ct,
                                 BMA253_FIFO_MODE_T mode,
                                 BMA253_FIFO_DATA_SEL_T axes)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    uint8_t reg = ( (mode << _BMA253_FIFO_CONFIG_1_FIFO_MODE_SHIFT) |
                    (axes << _BMA253_FIFO_CONFIG_1_FIFO_DATA_SHIFT) );

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_FIFO_CONFIG_1, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_fifo_set_watermark(const bma253_context_t *ct, uint8_t wm)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    uint8_t reg = wm & _BMA253_FIFO_CONFIG_0_WATER_MARK_MASK;
    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_FIFO_CONFIG_0, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_enable_fifo(bma253_context_t *ct, bool_t use_fifo)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    ct->use_fifo = use_fifo;
    return 0;
}


static int8_t bma253_fifo_get_frame_count(const bma253_context_t *ct, uint8_t *size)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_FIFO_STATUS, 1, size) != 0)
        return -1;
    *size &= _BMA253_FIFO_STATUS_FRAME_COUNTER_MASK;
    return 0;
}

static int8_t bma253_set_low_power_mode2(const bma253_context_t *ct)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    uint8_t reg = 0;
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_PMU_LOW_POWER, 1, &reg) != 0)
        return -1;

    reg &= ~_BMA253_LOW_POWER_RESERVED_BITS;

    // we simply set the low power mode to 2.  Low power mode 1 slows
    // down register write accesses, and we can't handle that.  In the
    // words of the late Admiral Akbar: "We cannot handle firepower of
    // that magnitude!" :(

    reg |= BMA253_LOW_POWER_LOWPOWER_MODE;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_PMU_LOW_POWER, 1, &reg) != 0)
        return -1;

    return 0;
}


static int8_t bma253_get_intr_enable(bma253_context_t *ct,
                                     uint8_t int_src,
                                     uint8_t *bits)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    DBG_ASSERT(bits != NULL __DBG_LINE);

    switch (int_src)
    {
    case 0:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_EN_0, 1, bits) != 0)
            return -1;
        *bits &= ~_BMA253_INT_EN_0_RESERVED_BITS;
        break;
    }

    case 1:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_EN_1, 1, bits) != 0)
            return -1;
        *bits &= ~_BMA253_INT_EN_1_RESERVED_BITS;
        break;
    }

    case 2:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_EN_2, 1, bits) != 0)
            return -1;
        *bits &= ~_BMA253_INT_EN_2_RESERVED_BITS;
        break;
    }

    }
    return 0;
}

static int8_t bma253_set_intr_enable(bma253_context_t *ct,
                                     uint8_t int_src,
                                     uint8_t bits)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    uint8_t reg = bits;
    switch (int_src)
    {
    case 0:
    {
        reg &= ~_BMA253_INT_EN_0_RESERVED_BITS;
        if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_EN_0, 1, &reg) != 0)
            return -1;
        break;
    }

    case 1:
    {
        reg &= ~_BMA253_INT_EN_1_RESERVED_BITS;
        if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_EN_1, 1, &reg) != 0)
            return -1;
        break;
    }

    case 2:
    {
        reg &= ~_BMA253_INT_EN_2_RESERVED_BITS;
        if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_EN_2, 1, &reg) != 0)
            return -1;
        break;
    }

    }
    return 0;
}

static int8_t bma253_get_intr_map(bma253_context_t *ct,
                                  uint8_t int_src,
                                  uint8_t *bits)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    switch (int_src)
    {
    case 0:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_MAP_0, 1, bits) != 0)
            return -1;
        break;
    }

    case 1:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_MAP_1, 1, bits) != 0)
            return -1;
        break;
    }

    case 2:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_MAP_2, 1, bits) != 0)
            return -1;
        break;
    }

    }
    return 0;
}


static int8_t bma253_set_intr_map(bma253_context_t *ct,
                                  uint8_t int_src,
                                  uint8_t bits)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    uint8_t reg = bits;
    switch (int_src)
    {
    case 0:
    {
        if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_MAP_0, 1, &reg) == 0)
            return -1;
        break;
    }

    case 1:
    {
        if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_MAP_1, 1, &reg) != 0)
            return -1;
        break;
    }

    case 2:
    {
        if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_MAP_2, 1, &reg) != 0)
            return -1;
        break;
    }

    }
    return 0;
}

static int8_t bma253_get_intr_src(bma253_context_t *ct, uint8_t *val)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_SRC, 1, val) != 0)
        return -1;

    *val &= _BMA253_INT_SRC_RESERVED_BITS;
    return 0;
}

static int8_t bma253_set_intr_src(bma253_context_t *ct, uint8_t bits)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    uint8_t reg = bits & _BMA253_INT_SRC_RESERVED_BITS;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_SRC, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_get_intr_output_ctrl(bma253_context_t *ct, uint8_t *val)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_OUT_CTRL, 1, val) != 0)
        return -1;

    *val &= _BMA253_INT_OUT_CTRL_INT1_RESERVED_BITS;
    return 0;
}

static int8_t bma253_set_intr_output_ctrl(bma253_context_t *ct, uint8_t bits)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    uint8_t reg = bits & _BMA253_INT_OUT_CTRL_INT1_RESERVED_BITS;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_OUT_CTRL, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_clr_intr_latches(const bma253_context_t *ct)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    uint8_t val = 0;
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_RST_LATCH, 1, &val) != 0)
        return -1;

    val &= _BMA253_INT_RST_LATCH_RESERVED_BITS;

    val |= BMA253_INT_RST_LATCH_RESET_INT;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_OUT_CTRL, 1, &val) != 0)
        return -1;

    return 0;
}

static int8_t bma253_get_intr_latch_behavior(bma253_context_t *ct, uint8_t *val)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_RST_LATCH, 1, val) != 0)
        return -1;

    *val &= _BMA253_INT_RST_LATCH_RESERVED_BITS;

    *val &= (_BMA253_INT_RST_LATCH_MASK << _BMA253_INT_RST_LATCH_SHIFT);

    return 0;
}

static int8_t bma253_set_intr_latch_behavior(bma253_context_t *ct, BMA253_RST_LATCH_T latch)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    uint8_t reg = 0;
    if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_RST_LATCH, 1, &reg) != 0)
        return -1;

    reg &= _BMA253_INT_RST_LATCH_RESERVED_BITS;

    reg &= ~(_BMA253_INT_RST_LATCH_MASK << _BMA253_INT_RST_LATCH_SHIFT);
    reg |= (latch << _BMA253_INT_RST_LATCH_SHIFT);

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_INT_OUT_CTRL, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_get_intr_status(bma253_context_t *ct,
                                     uint8_t int_src,
                                     uint8_t *bits)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    switch (int_src)
    {
    case 0:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_STATUS_0, 1, bits) != 0)
            return -1;

        break;
    }

    case 1:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_STATUS_1, 1, bits) != 0)
            return -1;
        *bits &= _BMA253_INT_STATUS_1_RESERVED_BITS;
        break;
    }

    case 2:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_STATUS_2, 1, bits) != 0)
            return -1;
        break;
    }

    case 3:
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_INT_STATUS_3, 1, bits) != 0)
            return -1;
        break;
    }

    }
    return 0;
}

static int8_t bma253_set_self_test(const bma253_context_t *ct,
                                   bool_t sign, bool_t amp,
                                   BMA253_SELFTTEST_AXIS_T axis)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    uint8_t reg = (axis << _BMA253_PMU_SELFTTEST_AXIS_SHIFT);

    if (amp)
        reg |= BMA253_PMU_SELFTTEST_AMP;

    if (sign)
        reg |= BMA253_PMU_SELFTTEST_SIGN;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_PMU_SELF_TEST, 1, &reg) != 0)
        return -1;

    return 0;
}

static int8_t bma253_reset(const bma253_context_t *ct)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);
    uint8_t reg = BMA253_RESET_BYTE;

    if (bma253_write_regs(ct->i2c_addr, BMA253_REG_BGW_SOFTRESET, 1, &reg) != 0)
        return -1;

    delay_ms(1000);

    return 0;
}

static int8_t bma253_driver_devinit(bma253_context_t *ct,
                                    BMA253_POWER_MODE_T power,
                                    BMA253_RANGE_T range,
                                    BMA253_BW_T bw)
{
    DBG_ASSERT(ct != NULL __DBG_LINE);

    bma253_set_power_mode(ct, power);

    delay_ms(50);   // 50ms, in case we are waking up
    // set our range and bandwidth, make sure register shadowing is
    // enabled, enable output filtering, and set our FIFO config

    if (bma253_set_range(ct, range)
            || bma253_set_bandwidth(ct, bw)
            || bma253_enable_register_shadowing(ct, TRUE)
            || bma253_enable_output_filtering(ct, TRUE)
            || bma253_fifo_config(ct, BMA253_FIFO_MODE_FIFO,
                                  BMA253_FIFO_DATA_SEL_XYZ))
    {
        DBG_ASSERT(FALSE __DBG_LINE);
        return -1;
    }

    bma253_enable_fifo(ct, TRUE);

    // make sure low power mode LPM2 is enabled in case we go to low
    // power or suspend mode. LPM1 mode (the default) requires register
    // writes to be drastically slowed down when enabled, which we
    // cannot handle.
    bma253_set_low_power_mode2(ct);

    delay_ms(50);

    bma253_set_intr_output_ctrl(ct, BMA253_INT_OUT_CTRL_INT1_LVL | BMA253_INT_OUT_CTRL_INT2_LVL);
//    bma253_set_intr_enable(ct, 1,
//                           BMA253_INT_EN_1_DATA_EN | BMA253_INT_EN_1_INT_FWM_EN | BMA253_INT_EN_1_INT_FFULL_EN);

//    bma253_set_intr_map(ct, 1,
//                        BMA253_INT_MAP_1_INT2_DATA | BMA253_INT_MAP_1_INT2_FWM | BMA253_INT_MAP_1_INT2_FFULL);

    bma253_fifo_set_watermark(ct, WATERMARK_COUNT);  //*< 25 frames to occur interrupt, leave 7 frames for buffer.

    return 0;
}

static void bma253_int1_handler(void)
{
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);

    //*< do something in interrupt
    //

    OSEL_EXIT_CRITICAL(s);
}

#if 0
static void bma253_int2_handler(void)
{
    osel_int_status_t s;
    OSEL_ENTER_CRITICAL(s);
    //*< 不能在中断里面读写寄存器，会导致I2C死锁
    //*< do something in interrupt

    if (bma253_device.rx_indicate != NULL)
    {
        bma253_device.rx_indicate(&bma253_device, WATERMARK_COUNT);
    }

    OSEL_EXIT_CRITICAL(s);
}
#endif


static int8_t bma253_driver_init(osel_device_t *dev)
{
    uint8_t reg_val = 0;

    bma253_context_t *ct = (bma253_context_t *)dev->user_data;
    DBG_ASSERT(ct != NULL __DBG_LINE);

    // set PIN#11 'Protocol select' pin: 1 selects i2c
    bma253_iic_init();

    for(uint8_t i=0;i<3;i++)
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_BGW_CHIPID, 1, &reg_val) != 0)
        {
            delay_us(100);
            continue;
        }
        else
            break;
    }

    if (reg_val != BMA253_CHIP_ID)
    {
        DBG_ASSERT(FALSE __DBG_LINE);
        return -2;
    }
    if (bma253_driver_devinit(ct, BMA253_POWER_MODE_NORMAL, BMA253_RANGE_2G, BMA253_BW_500))
    {
        DBG_ASSERT(FALSE __DBG_LINE);
        return -3;
    }
    
//    bma253_int_init(&bma253_int1_handler, &bma253_int2_handler);
    bma253_fifo_config(ct, BMA253_FIFO_MODE_FIFO,
                                  BMA253_FIFO_DATA_SEL_XYZ);    //*< clean fifo
    
    return 0;
}


static osel_uint32_t bma253_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size)
{
    bma253_context_t *ct = (bma253_context_t *)dev->user_data;
    DBG_ASSERT(ct != NULL __DBG_LINE);


    uint8_t frame_len = sizeof(bma253_frame_t);
//    DBG_ASSERT(((size % frame_len) == 0) __DBG_LINE);

    uint8_t buf[6];
    uint16_t acc_x, acc_y, acc_z;
    bma253_frame_t frame;

//    if ( size < (WATERMARK_COUNT * frame_len))
//        return 0;

    for (uint8_t i = 0; i < size; i++)
    {
        if (bma253_read_regs(ct->i2c_addr, BMA253_REG_FIFO_DATA, 6, buf) != 0)
            return -1;

        acc_x = (((uint16_t)buf[1] << 8) | (buf[0] & (_BMA253_ACCD12_LSB_MASK << _BMA253_ACCD12_LSB_SHIFT))) >> _BMA253_ACCD12_LSB_SHIFT;
        acc_y = (((uint16_t)buf[3] << 8) | (buf[2] & (_BMA253_ACCD12_LSB_MASK << _BMA253_ACCD12_LSB_SHIFT))) >> _BMA253_ACCD12_LSB_SHIFT;
        acc_z = (((uint16_t)buf[5] << 8) | (buf[4] & (_BMA253_ACCD12_LSB_MASK << _BMA253_ACCD12_LSB_SHIFT))) >> _BMA253_ACCD12_LSB_SHIFT;
        
        frame.acc_x = (int16_t)(acc_x>2048?(int16_t)acc_x-4096:acc_x);
        frame.acc_y = (int16_t)(acc_y>2048?(int16_t)acc_y-4096:acc_y);
        frame.acc_z = (int16_t)(acc_z>2048?(int16_t)acc_z-4096:acc_z);
        
//        frame.acc_x = (int16_t)acc_x * ct->acc_scale;
//        frame.acc_y = (int16_t)acc_y * ct->acc_scale;
//        frame.acc_z = (int16_t)acc_z * ct->acc_scale;
        osel_memcpy((uint8_t *)buffer+frame_len * i, (uint8_t *)&frame, frame_len);
    }

    return size * frame_len;
}

static osel_uint32_t bma253_driver_control(osel_device_t *dev, osel_uint8_t cmd, void *args)
{
    bma253_context_t *ct = (bma253_context_t *)dev->user_data;
    DBG_ASSERT(ct != NULL __DBG_LINE);

    switch (cmd)
    {
    case BMA253_GET_FRAME_COUNT:
    {
        DBG_ASSERT(args != NULL __DBG_LINE);
        bma253_fifo_get_frame_count(ct, (uint8_t *)args);
    }
    break;
    
    case BMA253_CLR_FRAME_COUNT:
    {
        bma253_fifo_config(&bma253_cont, BMA253_FIFO_MODE_FIFO,
                                  BMA253_FIFO_DATA_SEL_XYZ);
    }
    break;
    }

    return 0;
}


void bma253_device_init(void)
{
    bma253_device.init    = bma253_driver_init;
    bma253_device.read    = bma253_driver_read;
    bma253_device.control = bma253_driver_control;

    bma253_cont.i2c_addr = BMA253_DEFAULT_ADDR;
    bma253_device.user_data = &bma253_cont;

    osel_device_register(&bma253_device, ACC_SENSOR_NAME, OSEL_DEVICE_FLAG_RDONLY);
}