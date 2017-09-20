/***************************************************************************
* File        : re46c191.h
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/11/1
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/11/1         v0.1            wangjifang        first version
***************************************************************************/

#ifndef __RE46C191_H__
#define __RE46C191_H__

typedef enum
{
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    T8,
    T9,
    T10,
    T11,
    T12,
    MODE_MAX,
} re46c191_mode_e;

typedef struct
{
uint32_t    ltd_thr     :   6,      //*<长期漂移采样位
            ctl_thr     :   6,      //*<探测腔测试限制位
            hul_thr     :   6,      //*<静默限制值位
            hyl_thr     :   6,      //*<迟滞限制值位
            nl_thr      :   6,      //*<正常限制值位
            pagf        :   2;      //*<光电放大器增益因数位
} re46c191_cfg1_t;

typedef struct
{
uint16_t    it_val      :   2,      //*<积分时间位
            irc_val     :   2,      //*<ired电流位
            lb_val      :   3,      //*<电池低电量跳变点位
            ltd_en      :   1,      //*<长期漂移使能位
            hush_val    :   1,      //*<静默选项位
            lbh_en      :   1,      //*<电池低电量静默使能位
            eol_en      :   1,      //*<寿命结束使能位
            ts_val      :   1,      //*<蜂鸣模式选择位
            reserve     :   4;
} re46c191_cfg2_t;

typedef enum
{
    TYPE_THR_LTD,
    TYPE_THR_CTL,
    TYPE_THR_HUL,
    TYPE_THR_HYL,
    TYPE_THR_NL,
    TYPE_THR_MAX,
} re46c191_cfg_thr_type_e;

#define SMOKE_SENSOR_NAME         "RE46C191"

void re46c191_device_init(void);

#endif