/**
 * @brief       : this
 * @file        : bma253_defs.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-06-02
 * change logs  :
 * Date       Version     Author        Note
 * 2017-06-02  v0.0.1  gang.cheng    first version
 */
#ifndef __BMA253_DEFS_H__
#define __BMA253_DEFS_H__

#define BMA253_DEFAULT_ADDR                 0x18

#define BMA253_CHIP_ID                      0xFA
#define BMA253_RESET_BYTE                   0xB6

// NOTE: Reserved registers must not be written into.  Reading
// from them may return indeterminate values.  Registers
// containing reserved bitfields must be written as 0.  Reading
// reserved bitfields may return indeterminate values.

/**
 * BMA253 registers
 */
typedef enum
{
    BMA253_REG_BGW_CHIPID                   = 0x00,    // R
    // 0x01 reserved
    BMA253_REG_ACCD_X_LSB                   = 0x02,    // R    acc_x_lsb<3:0>:new_data_x
    BMA253_REG_ACCD_X_MSB                   = 0x03,    // R    acc_x_msb<11:4>
    BMA253_REG_ACCD_Y_LSB                   = 0x04,    // R    acc_y_lsb<3:0>:new_data_y
    BMA253_REG_ACCD_Y_MSB                   = 0x05,    // R    acc_y_msb<11:4>
    BMA253_REG_ACCD_Z_LSB                   = 0x06,    // R    acc_z_lsb<3:0>:new_data_z
    BMA253_REG_ACCD_Z_MSB                   = 0x07,    // R    acc_z_msb<11:4>
    BMA253_REG_ACCD_TEMP                    = 0x08,    // R    temp<7:0>
    BMA253_REG_INT_STATUS_0                 = 0x09,    // R    falt:orient:s_tap:d_tap:slo_no_mot:slope:high:low
    BMA253_REG_INT_STATUS_1                 = 0x0A,    // R    data:fifo_wm:fifo_full
    BMA253_REG_INT_STATUS_2                 = 0x0B,    // R    tap_sign:tap_first_<z:y:x>:slope_sign:slope_first_<z:y:x>
    BMA253_REG_INT_STATUS_3                 = 0x0C,    // R    flat:orient<2:0>:high_sign:high_first_<z:y:x>
    // 0x0d reserved
    BMA253_REG_FIFO_STATUS                  = 0x0E,    // R    fifo_overrun:fifo_frame_counter<6:0>
    BMA253_REG_PMU_RANGE                    = 0x0F,    // R/W  range<3:0>
    BMA253_REG_PMU_BW                       = 0x10,    // R/W  bw<3:0>
    BMA253_REG_PMU_LPW                      = 0x11,    // R/W  suspend:lowpower_en:deep_suspend:sleep_dur<3:0>
    BMA253_REG_PMU_LOW_POWER                = 0x12,    // R/W  lowpower_mode:sleeptimer_mode
    BMA253_REG_ACCD_HBW                     = 0x13,    // R/W  data_high_bw:shadow_dis
    BMA253_REG_BGW_SOFTRESET                = 0x14,    // W    softreset
    // 0x15 reserved
    BMA253_REG_INT_EN_0                     = 0x16,    // R/W  falt:orient:s_tap:d_tap:slope_en_<z:y:x>
    BMA253_REG_INT_EN_1                     = 0x17,    // R/W  int_fwm:int_ffull:data:low:high_en_<z:y:x>
    BMA253_REG_INT_EN_2                     = 0x18,    // R/W  slo_no_mot:slo_no_mot_en_<z:y:x>
    BMA253_REG_INT_MAP_0                    = 0x19,    // R/W  int1_flat:int1_orient:int1_s_tap:int1_d_tap:int1_slo_no_mot:int1_slope:int1_high:int1_low
    BMA253_REG_INT_MAP_1                    = 0x1A,    // R/W  int2_data:int2_fwm:int2_ffull:int1_ffull:int1_fwm:int1_data
    BMA253_REG_INT_MAP_2                    = 0x1B,    // R/W  int2_flat:int2_orient:int2_s_tap:int2_d_tap:int2_slo_no_mot:int2_slope:int2_high:int2_low
    // 0x1c-0x1d reserved
    BMA253_REG_INT_SRC                      = 0x1E,    // R/W  int_src_data:int_src_tap:int_src_slo_no_mot:int_src_slope:int_src_high:int_src_low
    // 0x1f reserved
    BMA253_REG_INT_OUT_CTRL                 = 0x20,    // R/W  int2_od:int2_lvl:int1_od:int1_lvl
    BMA253_REG_INT_RST_LATCH                = 0x21,    // R/W  reset_int:latch_int<3:0>
    BMA253_REG_INT_0                        = 0x22,    // R/W  low_dur<7:0>
    BMA253_REG_INT_1                        = 0x23,    // R/W  low_th<7:0>
    BMA253_REG_INT_2                        = 0x24,    // R/W  high_hy<1:0>:low_mode:low_hy<1:0>
    BMA253_REG_INT_3                        = 0X25,    // R/W  high_dur<7:0>
    BMA253_REG_INT_4                        = 0X26,    // R/W  high_th<7:0>
    BMA253_REG_INT_5                        = 0X27,    // R/W  slot_no_mot_dur<5:0>
    BMA253_REG_INT_6                        = 0X28,    // R/W  slope_th<7:0>
    BMA253_REG_INT_7                        = 0X29,    // R/W  slo_no_mot_th<5:0>
    BMA253_REG_INT_8                        = 0X2A,    // R/W  tap_quiet:tap_shock:tap_dur<2:0>
    BMA253_REG_INT_9                        = 0X2B,    // R/W  tap_samp<1:0>:tap_th<4:0>
    BMA253_REG_INT_A                        = 0X2C,    // R/W  orient_hyst<2:0>:orient_blocking<1:0>:orient_mode<1:0>
    BMA253_REG_INT_B                        = 0X2D,    // R/W  orient_ud_en:orient_theta<5:0>
    BMA253_REG_INT_C                        = 0X2E,    // R/W  flat_theta<5:0>
    BMA253_REG_INT_D                        = 0X2F,    // R/W  flat_hold_time<1:0>:flat_hy<2:0>
    BMA253_REG_FIFO_CONFIG_0                = 0X30,    // R/W  fifo_water_mark_level_trigger_retain<5:0>
    // 0x31 reserved
    BMA253_REG_PMU_SELF_TEST                = 0X32,    // R/W  self_test_amp:self_test_sign:self_test_axis<1:0>
    BMA253_REG_TRIM_NVM_CTRL                = 0X33,    // R/W  nvm_remain<3:0>:nvm_load:nvm_rdy:nvm_prog_trig:nvm_prog_mode
    BMA253_REG_BGW_SPI3_WDT                 = 0X34,    // R/W  i2c_wdt_en:i2c_wdt_sel:spi3
    // 0x35 reserved
    BMA253_REG_OFC_CTRL                     = 0x36,    // R/W  offset_rest:cal_trigger<1:0>:cal_rdy:hp_<z:y:x>_en
    BMA253_REG_OFC_SETTING                  = 0x37,    // R/W  offset_target_<z:y:x><1:0>:cut_off
    BMA253_REG_OFC_OFFSET_X                 = 0x38,    // R/W  offset_x<7:0>
    BMA253_REG_OFC_OFFSET_Y                 = 0x39,    // R/W  offset_y<7:0>
    BMA253_REG_OFC_OFFSET_Z                 = 0x3A,    // R/W  offset_z<7:0>
    BMA253_REG_TRIM_GP0                     = 0x3B,    // R/W  GP0<7:0>
    BMA253_REG_TRIM_GP1                     = 0x3C,    // R/W  GP1<7:0>
    // 0x3d reserved
    BMA253_REG_FIFO_CONFIG_1                = 0x3E,    // R/W  fifo_mode<1:0>:fifo_data_select<1:0>
    BMA253_REG_FIFO_DATA                    = 0x3F,    // R/W  fifo_data_output_register<7:0>
} BMA253_REGS_T;

/**
 * REG_ACCD_*_LSB bits - handle X, Y, and Z LSB regs, for 10 bit
 * resolution
 */
typedef enum
{
    BMA253_ACCD10_LSB_NEW_DATA              = 0x01, // data
    // updated
    // since last
    // read

    // 0x02-0x20 reserved

    BMA253_ACCD10_LSB0                      = 0x40, // lower 2
    // bits of
    // LSB data
    BMA253_ACCD10_LSB1                      = 0x80,
    _BMA253_ACCD10_LSB_MASK                 = 3,
    _BMA253_ACCD10_LSB_SHIFT                = 6
} BMA253_ACCD10_LSB_BITS_T;

/**
 * REG_ACCD_*_LSB bits - handle X, Y, and Z LSB regs, for 12 bit
 * resolution
 */
typedef enum
{
    BMA253_ACCD12_LSB_NEW_DATA              = 0x01, // data
    // updated
    // since last
    // read

    // 0x02-0x08 reserved

    BMA253_ACCD12_LSB0                      = 0x10, // lower 4
    // bits of
    // LSB data
    BMA253_ACCD12_LSB1                      = 0x20,
    BMA253_ACCD12_LSB2                      = 0x40,
    BMA253_ACCD12_LSB3                      = 0x80,
    _BMA253_ACCD12_LSB_MASK                 = 15,
    _BMA253_ACCD12_LSB_SHIFT                = 4
} BMA253_ACCD12_LSB_BITS_T;

/**
 * REG_INT_STATUS_0 bits
 */
typedef enum
{
    BMA253_INT_STATUS_0_LOW                 = 0x01,
    BMA253_INT_STATUS_0_HIGH                = 0x02,
    BMA253_INT_STATUS_0_SLOPE               = 0x04,
    BMA253_INT_STATUS_0_SLO_NOT_MOT         = 0x08,
    BMA253_INT_STATUS_0_D_TAP               = 0x10,
    BMA253_INT_STATUS_0_S_TAP               = 0x20,
    BMA253_INT_STATUS_0_ORIENT              = 0x40,
    BMA253_INT_STATUS_0_FLAT                = 0x80
} BMA253_INT_STATUS_0_BITS_T;

/**
 * REG_INT_STATUS_1 bits
 */
typedef enum
{
    _BMA253_INT_STATUS_1_RESERVED_BITS      = 0x0f | 0x10,
    // 0x01-0x10 reserved
    BMA253_INT_STATUS_1_FIFO_FULL           = 0x20,
    BMA253_INT_STATUS_1_FIFO_WM             = 0x40,
    BMA253_INT_STATUS_1_DATA                = 0x80 // data ready int
} BMA253_INT_STATUS_1_BITS_T;

/**
 * REG_INT_STATUS_2 bits
 */
typedef enum
{
    BMA253_INT_STATUS_2_SLOPE_FIRST_X       = 0x01,
    BMA253_INT_STATUS_2_SLOPE_FIRST_Y       = 0x02,
    BMA253_INT_STATUS_2_SLOPE_FIRST_Z       = 0x04,
    BMA253_INT_STATUS_2_SLOPE_SIGN          = 0x08,
    BMA253_INT_STATUS_2_TAP_FIRST_X         = 0x10,
    BMA253_INT_STATUS_2_TAP_FIRST_Y         = 0x20,
    BMA253_INT_STATUS_2_TAP_FIRST_Z         = 0x40,
    BMA253_INT_STATUS_2_TAP_SIGN            = 0x80
} BMA253_INT_STATUS_2_BITS_T;

/**
 * REG_INT_STATUS_3 bits
 */
typedef enum
{
    BMA253_INT_STATUS_3_HIGH_FIRST_X        = 0x01,
    BMA253_INT_STATUS_3_HIGH_FIRST_Y        = 0x02,
    BMA253_INT_STATUS_3_HIGH_FIRST_Z        = 0x04,
    BMA253_INT_STATUS_3_HIGH_SIGN           = 0x08,

    BMA253_INT_STATUS_3_ORIENT0             = 0x10,
    BMA253_INT_STATUS_3_ORIENT1             = 0x20,
    BMA253_INT_STATUS_3_ORIENT2             = 0x40,
    _BMA253_INT_STATUS_3_ORIENT_MASK        = 7,
    _BMA253_INT_STATUS_3_ORIENT_SHIFT       = 4,

    BMA253_INT_STATUS_3_FLAT                = 0x80
} INT_STATUS_3_BITS_T;

/**
 * INT_STATUS_3_ORIENT values
 */
typedef enum
{
    BMA253_ORIENT_POTRAIT_UPRIGHT           = 0,
    BMA253_ORIENT_POTRAIT_UPSIDE_DOWN       = 1,
    BMA253_ORIENT_LANDSCAPE_LEFT            = 2,
    BMA253_ORIENT_LANDSCAPE_RIGHT           = 3,
} BMA253_ORIENT_T;

/**
 * REG_FIFO_STATUS bits
 */
typedef enum
{
    BMA253_FIFO_STATUS_FRAME_COUNTER0       = 0x01,
    BMA253_FIFO_STATUS_FRAME_COUNTER1       = 0x02,
    BMA253_FIFO_STATUS_FRAME_COUNTER2       = 0x04,
    BMA253_FIFO_STATUS_FRAME_COUNTER3       = 0x08,
    BMA253_FIFO_STATUS_FRAME_COUNTER4       = 0x10,
    BMA253_FIFO_STATUS_FRAME_COUNTER5       = 0x20,
    BMA253_FIFO_STATUS_FRAME_COUNTER6       = 0x40,
    _BMA253_FIFO_STATUS_FRAME_COUNTER_MASK  = 127,
    _BMA253_FIFO_STATUS_FRAME_COUNTER_SHIFT = 0,

    BMA253_FIFO_STATUS_FIFO_OVERRUN         = 0x80
} BMA253_FIFO_STATUS_BITS_T;

/**
 * REG_PMU_RANGE bits
 */
typedef enum
{
    BMA253_PMU_RANGE0                       = 0x01,
    BMA253_PMU_RANGE1                       = 0x02,
    BMA253_PMU_RANGE2                       = 0x04,
    BMA253_PMU_RANGE3                       = 0x08,
    _BMA253_PMU_RANGE_MASK                  = 15,
    _BMA253_PMU_RANGE_SHIFT                 = 0

            // 0x10-0x80 reserved
} BMA253_PMU_RANGE_BITS_T;

/**
 * PMU_RANGE (accelerometer g-range) values
 */
typedef enum
{
    BMA253_RANGE_2G                         = 3,
    BMA253_RANGE_4G                         = 5,
    BMA253_RANGE_8G                         = 8,
    BMA253_RANGE_16G                        = 12
} BMA253_RANGE_T;

#define RANGE_2G_MG_LSB                     (0.98)
#define RANGE_4G_MG_LSB                     (1.95)
#define RANGE_8G_MG_LSB                     (3.91)
#define RANGE_16G_MG_LSB                    (7.81)

/**
 * REG_PMU_BW bits
 */
typedef enum
{
    BMA253_PMU_BW0                          = 0x01,
    BMA253_PMU_BW1                          = 0x02,
    BMA253_PMU_BW2                          = 0x04,
    BMA253_PMU_BW3                          = 0x08,
    BMA253_PMU_BW4                          = 0x10,
    _BMA253_PMU_BW_MASK                     = 31,
    _BMA253_PMU_BW_SHIFT                    = 0

            // 0x20-0x80 reserved
} BMA253_PMU_BW_BITS_T;

/**
 * PMU_BW (accelerometer filter bandwidth) values
 */
typedef enum
{
    BMA253_BW_7_81                          = 8, // 7.81 Hz
    BMA253_BW_15_63                         = 9,
    BMA253_BW_31_25                         = 10,
    BMA253_BW_62_5                          = 11,
    BMA253_BW_125                           = 12,
    BMA253_BW_250                           = 13,
    BMA253_BW_500                           = 14,
    BMA253_BW_1000                          = 15
} BMA253_BW_T;

/**
 * REG_PMU_LPW bits
 */
typedef enum
{
    // 0x01 reserved
    _BMA253_PMU_LPW_RESERVED_MASK           = 0x01,

    BMA253_PMU_LPW_SLEEP_DUR0               = 0x02, // sleep dur
    // in low
    // power mode
    BMA253_PMU_LPW_SLEEP_DUR1               = 0x04,
    BMA253_PMU_LPW_SLEEP_DUR2               = 0x08,
    BMA253_PMU_LPW_SLEEP_DUR3               = 0x10,
    _BMA253_PMU_LPW_SLEEP_MASK              = 15,
    _BMA253_PMU_LPW_SLEEP_SHIFT             = 1,

    // These are separate bits, deep_suspend, lowpower_en and
    // suspend (and if all 0, normal).  Since only specific
    // combinations are allowed, we will treat this as a 3 bit
    // bitfield called POWER_MODE.
    BMA253_PMU_LPW_POWER_MODE0              = 0x20, // deep_suspend
    BMA253_PMU_LPW_POWER_MODE1              = 0x40, // lowpower_en
    BMA253_PMU_LPW_POWER_MODE2              = 0x80, // suspend
    _BMA253_PMU_LPW_POWER_MODE_MASK         = 7,
    _BMA253_PMU_LPW_POWER_MODE_SHIFT        = 5
} BMA253_PMU_LPW_BITS_T;

/**
 * SLEEP_DUR values
 */
typedef enum
{
    BMA253_SLEEP_DUR_0_5                    = 0, // 0.5ms
    BMA253_SLEEP_DUR_1                      = 6,
    BMA253_SLEEP_DUR_2                      = 7,
    BMA253_SLEEP_DUR_4                      = 8,
    BMA253_SLEEP_DUR_6                      = 9,
    BMA253_SLEEP_DUR_10                     = 10,
    BMA253_SLEEP_DUR_25                     = 11,
    BMA253_SLEEP_DUR_50                     = 12,
    BMA253_SLEEP_DUR_100                    = 13,
    BMA253_SLEEP_DUR_500                    = 14,
    BMA253_SLEEP_DUR_1000                   = 15
} BMA253_SLEEP_DUR_T;

/**
 * POWER_MODE values
 */
typedef enum
{
    BMA253_POWER_MODE_NORMAL                = 0,
    BMA253_POWER_MODE_DEEP_SUSPEND          = 1,
    BMA253_POWER_MODE_LOW_POWER             = 2,
    BMA253_POWER_MODE_SUSPEND               = 4
} BMA253_POWER_MODE_T;

/**
 * REG_PMU_LOW_POWER bits
 */
typedef enum
{
    _BMA253_LOW_POWER_RESERVED_BITS         = 0x0f | 0x10 | 0x80,

    // 0x01-0x10 reserved
    BMA253_LOW_POWER_SLEEPTIMER_MODE        = 0x20,
    BMA253_LOW_POWER_LOWPOWER_MODE          = 0x40  // LPM1 or
            // LPM2
            // mode. see
            // DS.
            // 0x80 reserved
} BMA253_LOW_POWER_BITS_T;

/**
 * REG_ACC_HBW bits
 */
typedef enum
{
    _BMA253_ACC_HBW_RESERVED_BITS           = 0x0f | 0x10 | 0x20,

    // 0x01-0x20 reserved
    BMA253_ACC_HBW_SHADOW_DIS               = 0x40,
    BMA253_ACC_HBW_DATA_HIGH_BW             = 0x80
} BMA253_ACC_HBW_BITS_T;

/**
 * REG_INT_EN_0 bits
 */
typedef enum
{
    _BMA253_INT_EN_0_RESERVED_BITS          = 0x08,

    BMA253_INT_EN_0_SLOPE_EN_X              = 0x01,
    BMA253_INT_EN_0_SLOPE_EN_Y              = 0x02,
    BMA253_INT_EN_0_SLOPE_EN_Z              = 0x04,

    // 0x08 reserved

    BMA253_INT_EN_0_D_TAP_EN                = 0x10,
    BMA253_INT_EN_0_S_TAP_EN                = 0x20,
    BMA253_INT_EN_0_ORIENT_EN               = 0x40,
    BMA253_INT_EN_0_FLAT_EN                 = 0x80
} BMA253_INT_EN_0_BITS_T;

/**
 * REG_INT_EN_1 bits
 */
typedef enum
{
    _BMA253_INT_EN_1_RESERVED_BITS          = 0x80,

    BMA253_INT_EN_1_HIGH_EN_X               = 0x01,
    BMA253_INT_EN_1_HIGH_EN_Y               = 0x02,
    BMA253_INT_EN_1_HIGH_EN_Z               = 0x04,
    BMA253_INT_EN_1_LOW_EN                  = 0x08,
    BMA253_INT_EN_1_DATA_EN                 = 0x10,
    BMA253_INT_EN_1_INT_FFULL_EN            = 0x20, // fifo full
    BMA253_INT_EN_1_INT_FWM_EN              = 0x40  // fifo watermark

            // 0x80 reserved
} BMA253_INT_EN_1_BITS_T;

/**
 * REG_INT_EN_2 bits
 */
typedef enum
{
    _BMA253_INT_EN_2_RESERVED_BITS          = 0xf0,

    BMA253_INT_EN_2_SLO_NO_MOT_EN_X         = 0x01,
    BMA253_INT_EN_2_SLO_NO_MOT_EN_Y         = 0x02,
    BMA253_INT_EN_2_SLO_NO_MOT_EN_Z         = 0x04,
    BMA253_INT_EN_2_SLO_NO_MOT_SEL          = 0x08

            // 0x10-0x80 reserved
} BMA253_INT_EN_2_BITS_T;

/**
 * REG_INT_MAP_0 bits
 */
typedef enum
{
    BMA253_INT_MAP_0_INT1_LOW               = 0x01,
    BMA253_INT_MAP_0_INT1_HIGH              = 0x02,
    BMA253_INT_MAP_0_INT1_SLOPE             = 0x04,
    BMA253_INT_MAP_0_INT1_SLO_NO_MOT        = 0x08,
    BMA253_INT_MAP_0_INT1_D_TAP             = 0x10,
    BMA253_INT_MAP_0_INT1_S_TAP             = 0x20,
    BMA253_INT_MAP_0_INT1_ORIENT            = 0x40,
    BMA253_INT_MAP_0_INT1_FLAT              = 0x80
} BMA253_INT_MAP_0_BITS_T;

/**
 * REG_INT_MAP_1 bits
 */
typedef enum
{
    _BMA253_INT_MAP_1_INT1_RESERVED_BITS    = 0x08 | 0x10,

    BMA253_INT_MAP_1_INT1_DATA              = 0x01,
    BMA253_INT_MAP_1_INT1_FWM               = 0x02,
    BMA253_INT_MAP_1_INT1_FFULL             = 0x04,

    // 0x08-0x10 reserved

    BMA253_INT_MAP_1_INT2_FFULL             = 0x20,
    BMA253_INT_MAP_1_INT2_FWM               = 0x40,
    BMA253_INT_MAP_1_INT2_DATA              = 0x80
} BMA253_INT_MAP_1_BITS_T;

/**
 * REG_INT_MAP_2 bits
 */
typedef enum
{
    BMA253_INT_MAP_2_INT2_LOW               = 0x01,
    BMA253_INT_MAP_2_INT2_HIGH              = 0x02,
    BMA253_INT_MAP_2_INT2_SLOPE             = 0x04,
    BMA253_INT_MAP_2_INT2_SLO_NO_MOT        = 0x08,
    BMA253_INT_MAP_2_INT2_D_TAP             = 0x10,
    BMA253_INT_MAP_2_INT2_S_TAP             = 0x20,
    BMA253_INT_MAP_2_INT2_ORIENT            = 0x40,
    BMA253_INT_MAP_2_INT2_FLAT              = 0x80
} BMA253_INT_MAP_2_BITS_T;

/**
 * REG_INT_SRC bits
 */
typedef enum
{
    _BMA253_INT_SRC_RESERVED_BITS           = 0x40 | 0x80,

    BMA253_INT_SRC_LOW                      = 0x01,
    BMA253_INT_SRC_HIGH                     = 0x02,
    BMA253_INT_SRC_SLO_NO_MOT               = 0x04,
    BMA253_INT_SRC_SLOPE                    = 0x08,
    BMA253_INT_SRC_TAP                      = 0x10,
    BMA253_INT_SRC_DATA                     = 0x20

            // 0x40-0x80 reserved
} BMA253_INT_SRC_BITS_T;

/**
 * REG_INT_OUT_CTRL bits
 */
typedef enum
{
    _BMA253_INT_OUT_CTRL_INT1_RESERVED_BITS = 0xf0,

    BMA253_INT_OUT_CTRL_INT1_LVL            = 0x01, // level or edge
    BMA253_INT_OUT_CTRL_INT1_OD             = 0x02, // push-pull
    // or open
    // drain
    BMA253_INT_OUT_CTRL_INT2_LVL            = 0x04,
    BMA253_INT_OUT_CTRL_INT2_OD             = 0x08

            // 0x10-0x80 reserved
} BMA253_INT_OUT_CTRL_BITS_T;

/**
 * REG_INT_RST_LATCH bits
 */
typedef enum
{
    _BMA253_INT_RST_LATCH_RESERVED_BITS     = 0x10 | 0x20 | 0x40,

    BMA253_INT_RST_LATCH0                   = 0x01,
    BMA253_INT_RST_LATCH1                   = 0x02,
    BMA253_INT_RST_LATCH2                   = 0x04,
    BMA253_INT_RST_LATCH3                   = 0x08,
    _BMA253_INT_RST_LATCH_MASK              = 15,
    _BMA253_INT_RST_LATCH_SHIFT             = 0,

    // 0x10-0x40 reserved

    BMA253_INT_RST_LATCH_RESET_INT          = 0x80
} BMA253_INT_RST_LATCH_BITS_T;

/**
 * RST_LATCH values
 */
typedef enum
{
    BMA253_RST_LATCH_NON_LATCHED            = 0,
    BMA253_RST_LATCH_TEMPORARY_250MS        = 1,
    BMA253_RST_LATCH_TEMPORARY_500MS        = 2,
    BMA253_RST_LATCH_TEMPORARY_1S           = 3,
    BMA253_RST_LATCH_TEMPORARY_2S           = 4,
    BMA253_RST_LATCH_TEMPORARY_4S           = 5,
    BMA253_RST_LATCH_TEMPORARY_8S           = 6,
    BMA253_RST_LATCH_LATCHED                = 7,

    // 8 == non latched

    BMA253_RST_LATCH_TEMPORARY_250US        = 9,
    BMA253_RST_LATCH_TEMPORARY_500US        = 10,
    BMA253_RST_LATCH_TEMPORARY_1MS          = 11,
    BMA253_RST_LATCH_TEMPORARY_12_5MS       = 12,
    BMA253_RST_LATCH_TEMPORARY_25MS         = 13,
    BMA253_RST_LATCH_TEMPORARY_50MS         = 14

            // 15 == latched
} BMA253_RST_LATCH_T;

/**
 * REG_INT_2 bits
 */
typedef enum
{
    BMA253_INT_2_LOW_HY0                    = 0x01,
    BMA253_INT_2_LOW_HY1                    = 0x02,
    _BMA253_INT_2_LOW_HY_MASK               = 3,
    _BMA253_INT_2_LOW_HY_SHIFT              = 0,

    BMA253_INT_2_LOW_MODE                   = 0x04,

    // 0x08-0x20 reserved

    BMA253_INT_2_HIGH_HY0                   = 0x40,
    BMA253_INT_2_HIGH_HY1                   = 0x80,
    _BMA253_INT_2_HIGH_HY_MASK              = 3,
    _BMA253_INT_2_HIGH_HY_SHIFT             = 6
} BMA253_INT_2_BITS_T;

/**
 * REG_INT_5 bits
 */
typedef enum
{
    BMA253_INT_5_SLOPE_DUR0                 = 0x01,
    BMA253_INT_5_SLOPE_DUR1                 = 0x02,
    _BMA253_INT_5_SLOPE_DUR_MASK            = 3,
    _BMA253_INT_5_SLOPE_DUR_SHIFT           = 0,

    BMA253_INT_5_SLO_NO_MOT_DUR0            = 0x04,
    BMA253_INT_5_SLO_NO_MOT_DUR1            = 0x08,
    BMA253_INT_5_SLO_NO_MOT_DUR2            = 0x10,
    BMA253_INT_5_SLO_NO_MOT_DUR3            = 0x20,
    BMA253_INT_5_SLO_NO_MOT_DUR4            = 0x40,
    BMA253_INT_5_SLO_NO_MOT_DUR5            = 0x80,
    _BMA253_INT_5_SLO_NO_MOT_DUR_MASK       = 63,
    _BMA253_INT_5_SLO_NO_MOT_DUR_SHIFT      = 2
} BMA253_INT_5_BITS_T;

/**
 * REG_INT_8 bits
 */
typedef enum
{
    BMA253_INT_8_TAP_DUR0                   = 0x01,
    BMA253_INT_8_TAP_DUR1                   = 0x02,
    BMA253_INT_8_TAP_DUR2                   = 0x04,
    _BMA253_INT_8_TAP_DUR_MASK              = 7,
    _BMA253_INT_8_TAP_DUR_SHIFT             = 0,

    // 0x08-0x20 reserved

    BMA253_INT_8_TAP_SHOCK                  = 0x40,
    BMA253_INT_8_TAP_QUIET                  = 0x80
} BMA253_INT_8_BITS_T;

/**
 * REG_INT_9 bits
 */
typedef enum
{
    BMA253_INT_9_TAP_TH0                    = 0x01,
    BMA253_INT_9_TAP_TH1                    = 0x02,
    BMA253_INT_9_TAP_TH2                    = 0x04,
    BMA253_INT_9_TAP_TH3                    = 0x08,
    BMA253_INT_9_TAP_TH4                    = 0x10,
    _BMA253_INT_5_TAP_TH_MASK               = 31,
    _BMA253_INT_5_TAP_TH_SHIFT              = 0,

    // 0x20 reserved

    BMA253_INT_9_TAP_SAMP0                  = 0x40,
    BMA253_INT_9_TAP_SAMP1                  = 0x80,
    BMA253_INT_9_TAP_SAMP1_MASK             = 3,
    BMA253_INT_9_TAP_SAMP1_SHIFT            = 6
} BMA253_INT_9_BITS_T;

/**
 * REG_INT_A bits
 */
typedef enum
{
    BMA253_INT_A_ORIENT_MODE0               = 0x01,
    BMA253_INT_A_ORIENT_MODE1               = 0x02,
    _BMA253_INT_A_ORIENT_MODE_MASK          = 3,
    _BMA253_INT_A_ORIENT_MODE_SHIFT         = 0,

    BMA253_INT_A_ORIENT_BLOCKING0           = 0x04,
    BMA253_INT_A_ORIENT_BLOCKING1           = 0x08,
    _BMA253_INT_A_ORIENT_BLOCKING_MASK      = 3,
    _BMA253_INT_A_ORIENT_BLOCKING_SHIFT     = 2,

    BMA253_INT_A_ORIENT_HYST0               = 0x10,
    BMA253_INT_A_ORIENT_HYST1               = 0x20,
    BMA253_INT_A_ORIENT_HYST2               = 0x40,
    _BMA253_INT_A_ORIENT_HYST_MASK          = 7,
    _BMA253_INT_A_ORIENT_HYST_SHIFT         = 4

            // 0x80 reserved
} BMA253_INT_A_BITS_T;

/**
 * INT_A_ORIENT_MODE values
 */
typedef enum
{
    BMA253_ORIENT_MODE_SYMETRICAL           = 0,
    BMA253_ORIENT_MODE_HIGH_ASYMETRICAL     = 1,
    BMA253_ORIENT_MODE_LOW_ASYMETRICAL      = 2
} BMA253_ORIENT_MODE_T;

/**
 * INT_A_ORIENT_BLOCKING values
 */
typedef enum
{
    BMA253_ORIENT_BLOCKING_NONE             = 0,
    BMA253_ORIENT_BLOCKING_THETA_ACC_1_5G   = 1,
    BMA253_ORIENT_BLOCKING_THETA_ACC_0_2G_1_5G = 2,
    BMA253_ORIENT_BLOCKING_THETA_ACC_0_4G_1_5G = 3
} BMA253_ORIENT_BLOCKING_T;

/**
 * REG_INT_B bits
 */
typedef enum
{
    BMA253_INT_B_ORIENT_THETA0              = 0x01,
    BMA253_INT_B_ORIENT_THETA1              = 0x02,
    BMA253_INT_B_ORIENT_THETA2              = 0x04,
    BMA253_INT_B_ORIENT_THETA3              = 0x08,
    BMA253_INT_B_ORIENT_THETA4              = 0x10,
    BMA253_INT_B_ORIENT_THETA5              = 0x20,
    _BMA253_INT_B_ORIENT_THETA_MASK         = 63,
    _BMA253_INT_B_ORIENT_THETA_SHIFT        = 0,

    BMA253_INT_B_ORIENT_UD_EN               = 0x40
            // 0x80 reserved
} BMA253_INT_B_BITS_T;

/**
 * REG_INT_C bits
 */
typedef enum
{
    BMA253_INT_B_FLAT_THETA0               = 0x01,
    BMA253_INT_B_FLAT_THETA1               = 0x02,
    BMA253_INT_B_FLAT_THETA2               = 0x04,
    BMA253_INT_B_FLAT_THETA3               = 0x08,
    BMA253_INT_B_FLAT_THETA4               = 0x10,
    BMA253_INT_B_FLAT_THETA5               = 0x20,
    _BMA253_INT_B_FLAT_THETA_MASK          = 63,
    _BMA253_INT_B_FLAT_THETA_SHIFT         = 0,

    // 0x40-0x80 reserved
} BMA253_INT_C_BITS_T;

/**
 * REG_INT_D bits
 */
typedef enum
{
    BMA253_INT_D_FLAT_HY0                  = 0x01,
    BMA253_INT_D_FLAT_HY1                  = 0x02,
    BMA253_INT_D_FLAT_HY2                  = 0x04,
    _BMA253_INT_B_FLAT_HY_MASK             = 7,
    _BMA253_INT_B_FLAT_HY_SHIFT            = 0,

    // 0x08 reserved

    BMA253_INT_D_FLAT_HOLD_TIME0           = 0x10,
    BMA253_INT_D_FLAT_HOLD_TIME1           = 0x20,
    _BMA253_INT_B_FLAT_HOLD_TIME_MASK      = 3,
    _BMA253_INT_B_FLAT_HOLD_TIME_SHIFT     = 4

            // 0x40-0x80 reserved
} BMA253_INT_D_BITS_T;

/**
 * REG_FIFO_CONFIG_0 bits
 */
typedef enum
{
    _BMA253_FIFO_CONFIG_0_RESERVED_BITS    = 0x80 | 0x40,

    BMA253_FIFO_CONFIG_0_WATER_MARK0       = 0x01,
    BMA253_FIFO_CONFIG_0_WATER_MARK1       = 0x02,
    BMA253_FIFO_CONFIG_0_WATER_MARK2       = 0x04,
    BMA253_FIFO_CONFIG_0_WATER_MARK3       = 0x08,
    BMA253_FIFO_CONFIG_0_WATER_MARK4       = 0x10,
    BMA253_FIFO_CONFIG_0_WATER_MARK5       = 0x20,
    _BMA253_FIFO_CONFIG_0_WATER_MARK_MASK  = 63,
    _BMA253_FIFO_CONFIG_0_WATER_MARK_SHIFT = 0
} BMA253_FIFO_CONFIG_0_BITS_T;

/**
 * REG_PMU_SELFTTEST bits
 */
typedef enum
{
    BMA253_PMU_SELFTTEST_AXIS0             = 0x01,
    BMA253_PMU_SELFTTEST_AXIS1             = 0x02,
    _BMA253_PMU_SELFTTEST_AXIS_MASK        = 3,
    _BMA253_PMU_SELFTTEST_AXIS_SHIFT       = 0,

    BMA253_PMU_SELFTTEST_SIGN              = 0x04,

    // 0x08 reserved

    BMA253_PMU_SELFTTEST_AMP               = 0x10,

    // 0x20-0x80 reserved
} BMA253_PMU_SELFTTEST_BITS_T;

/**
 * PMU_SELFTTEST_AXIS values
 */
typedef enum
{
    BMA253_SELFTTEST_AXIS_NONE             = 0,
    BMA253_SELFTTEST_AXIS_X                = 1,
    BMA253_SELFTTEST_AXIS_Y                = 2,
    BMA253_SELFTTEST_AXIS_Z                = 3,
} BMA253_SELFTTEST_AXIS_T;

/**
 * REG_TRIM_NVM_CTRL bits
 */
typedef enum
{
    BMA253_TRIM_NVM_CTRL_NVM_PROG_MODE     = 0x01,
    BMA253_TRIM_NVM_CTRL_NVM_PROG_TRIG     = 0x02,
    BMA253_TRIM_NVM_CTRL_NVM_PROG_RDY      = 0x04,
    BMA253_TRIM_NVM_CTRL_NVM_PROG_LOAD     = 0x08,

    BMA253_TRIM_NVM_CTRL_NVM_REMAIN0       = 0x10,
    BMA253_TRIM_NVM_CTRL_NVM_REMAIN1       = 0x20,
    BMA253_TRIM_NVM_CTRL_NVM_REMAIN2       = 0x40,
    BMA253_TRIM_NVM_CTRL_NVM_REMAIN3       = 0x80,
    _BMA253_TRIM_NVM_CTRL_NVM_REMAIN_MASK  = 15,
    _BMA253_TRIM_NVM_CTRL_NVM_REMAIN_SHIFT = 4
} BMA253_TRIM_NVM_CTRL_BITS_T;

/**
 * REG_SPI3_WDT bits
 */
typedef enum
{
    _BMA253_SPI3_WDT_RESERVED_BITS         = 0xf0 | 0x08,

    BMA253_SPI3_WDT_SPI3                   = 0x01, // 3-wire SPI
    // - NOT
    // SUPPORTED

    BMA253_SPI3_WDT_I2C_WDT_SEL            = 0x02,
    BMA253_SPI3_WDT_I2C_WDT_EN             = 0x04

            // 0x08-0x80 reserved
} BMA253_SPI3_WDT_BITS_T;

/**
 * REG_OFC_CTRL bits
 */
typedef enum
{
    BMA253_OFC_CTRL_HP_X_EN                = 0x01,
    BMA253_OFC_CTRL_HP_Y_EN                = 0x02,
    BMA253_OFC_CTRL_HP_Z_EN                = 0x04,

    // 0x08 reserved

    BMA253_OFC_CTRL_CAL_RDY                = 0x10,

    BMA253_OFC_CTRL_CAL_TRIGGER0           = 0x20,
    BMA253_OFC_CTRL_CAL_TRIGGER1           = 0x40,
    _BMA253_OFC_CTRL_CAL_TRIGGER_MASK      = 3,
    _BMA253_OFC_CTRL_CAL_TRIGGER_SHIFT     = 5,

    BMA253_OFC_CTRL_OFFSET_RESET           = 0x80

} BMA253_OFC_CTRL_BITS_T;

/**
 * OFC_CTRL_CAL_TRIGGER values
 */
typedef enum
{
    BMA253_CAL_TRIGGER_NONE                = 0,
    BMA253_CAL_TRIGGER_X                   = 1,
    BMA253_CAL_TRIGGER_Y                   = 2,
    BMA253_CAL_TRIGGER_Z                   = 3
} BMA253_CAL_TRIGGER_T;

/**
 * REG_OFC_SETTING bits
 */
typedef enum
{
    BMA253_OFC_SETTING_CUT_OFF             = 0x01,

    BMA253_OFC_SETTING_OFFSET_TARGET_X0    = 0x02,
    BMA253_OFC_SETTING_OFFSET_TARGET_X1    = 0x04,
    _BMA253_OFC_SETTING_OFFSET_TARGET_X_MASK = 3,
    _BMA253_OFC_SETTING_OFFSET_TARGET_X_SHIFT = 1,

    BMA253_OFC_SETTING_OFFSET_TARGET_Y0    = 0x08,
    BMA253_OFC_SETTING_OFFSET_TARGET_Y1    = 0x10,
    _BMA253_OFC_SETTING_OFFSET_TARGET_Y_MASK = 3,
    _BMA253_OFC_SETTING_OFFSET_TARGET_Y_SHIFT = 3,

    BMA253_OFC_SETTING_OFFSET_TARGET_Z0    = 0x20,
    BMA253_OFC_SETTING_OFFSET_TARGET_Z1    = 0x40,
    _BMA253_OFC_SETTING_OFFSET_TARGET_Z_MASK = 3,
    _BMA253_OFC_SETTING_OFFSET_TARGET_Z_SHIFT = 5

            // 0x80 reserved
} BMA253_OFC_SETTING_BITS_T;

/**
 * OFC_SETTING_OFFSET_TARGET (for X, Y and Z axis) values
 */
typedef enum
{
    BMA253_OFFSET_TARGET_0G                = 0,
    BMA253_OFFSET_TARGET_PLUS_1G           = 1,
    BMA253_OFFSET_TARGET_MINUS_1G          = 2,
    // 3 == 0G
} BMA253_OFFSET_TARGET_T;

/**
 * REG_FIFO_CONFIG_1 bits
 */
typedef enum
{
    BMA253_FIFO_CONFIG_1_FIFO_DATA_SEL0     = 0x01,
    BMA253_FIFO_CONFIG_1_FIFO_DATA_SEL1     = 0x02,
    _BMA253_FIFO_CONFIG_1_FIFO_DATA_SEL     = 3,
    _BMA253_FIFO_CONFIG_1_FIFO_DATA_SHIFT   = 0,

    // 0x04-0x20 reserved

    BMA253_FIFO_CONFIG_1_FIFO_MODE0         = 0x40,
    BMA253_FIFO_CONFIG_1_FIFO_MODE1         = 0x80,
    _BMA253_FIFO_CONFIG_1_FIFO_MODE_MASK    = 3,
    _BMA253_FIFO_CONFIG_1_FIFO_MODE_SHIFT   = 6
} BMA253_FIFO_CONFIG_1_BITS_T;

/**
 * FIFO_DATA_SEL values
 */
typedef enum
{
    BMA253_FIFO_DATA_SEL_XYZ               = 0,
    BMA253_FIFO_DATA_SEL_X                 = 1,
    BMA253_FIFO_DATA_SEL_Y                 = 2,
    BMA253_FIFO_DATA_SEL_Z                 = 3
} BMA253_FIFO_DATA_SEL_T;

/**
 * FIFO_MODE values
 */
typedef enum
{
    BMA253_FIFO_MODE_BYPASS                = 0,
    BMA253_FIFO_MODE_FIFO                  = 1,
    BMA253_FIFO_MODE_STREAM                = 2

            // 3 == reserved (execute self-destruct :)
} BMA253_FIFO_MODE_T;

// interrupt selection for installISR() and uninstallISR()
typedef enum
{
    BMA253_INTERRUPT_INT1,
    BMA253_INTERRUPT_INT2
} BMA253_INTERRUPT_PINS_T;

// Different variants of this chip support different resolutions.
// The 0xf9 variant supports 10b, while the 0xfa variant (bmx055)
// supports 12 bits.
typedef enum
{
    BMA253_RESOLUTION_10BITS,
    BMA253_RESOLUTION_12BITS
} BMA253_RESOLUTION_T;


#endif