/***************************************************************************
* File        : re46c191.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/11/1
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/11/1         v0.1            wangjifang        first version
***************************************************************************/
#include "gz_sdk.h"
#include "re46c191.h"

osel_device_t re46c191_device;
static re46c191_cfg_thr_type_e re46c191_cmd_type;

static re46c191_cfg1_t re46c191_cfg1;
static re46c191_cfg2_t re46c191_cfg2;

static void re46c191_port_init(void);
static void re46c191_int_cfg(void);
static osel_int8_t re46c191_driver_init(osel_device_t *dev);

static void re46c191_pow_open(void)
{
    P7SEL &= BIT5;
    P7DIR |= BIT5;
    P7OUT |= BIT5;
}

static void re46c191_pow_close(void)
{
    P7SEL &= ~BIT5;
    P7DIR |= BIT5;
    P7OUT &= ~BIT5;
}

static void re46c191_cfg_pow_open(void)
{
    P7SEL &= ~BIT7;
    P7DIR |= BIT7;
    P7OUT |= BIT7;
    
    P4SEL &= ~BIT5;
    P4DIR |= BIT5;
    P4OUT |= BIT5;
}

static void re46c191_cfg_pow_close(void)
{
    P7SEL &= ~BIT7;
    P7DIR |= BIT7;
    P7OUT &= ~BIT7;
    
    P4SEL &= ~BIT5;
    P4DIR |= BIT5;
    P4OUT &= ~BIT5;
}

static bool_t re46c191_write_data(re46c191_cfg1_t data_1, re46c191_cfg2_t data_2, osel_device_t *dev)
{
    uint8_t cfg_data_buf[6] = {0};
    uint8_t bit_len = 8;
    re46c191_pow_close();
    delay_ms(100);
    re46c191_pow_open();
    delay_us(150);
    re46c191_config_start();
    delay_ms(1);
    re46c191_enter_mode(T7);
    delay_us(100);
    osel_memcpy(&cfg_data_buf[0], (uint8_t *)&data_1, sizeof(re46c191_cfg1_t));
    osel_memcpy(&cfg_data_buf[4], (uint8_t *)&data_2, sizeof(re46c191_cfg2_t));
    re46c191_sel_vbat();
    for(uint8_t i=0; i<6; i++)
    {
        if(i == 5)
        {
            bit_len = 4;
        }
        re46c191_send_char(cfg_data_buf[i], bit_len);
    }
    re46c191_data_save();
    re46c191_config_stop();
    re46c191_pow_close();
    delay_ms(100);
    re46c191_driver_init(dev);
    return TRUE;
}

static void re46c191_para_cfg(void)
{
    re46c191_cfg1.ltd_thr = 0;
    re46c191_cfg1.ctl_thr = 2;
    re46c191_cfg1.hul_thr = 20;
    re46c191_cfg1.hyl_thr = 15;
    re46c191_cfg1.nl_thr = 20;
    re46c191_cfg1.pagf = 1;
    
    re46c191_cfg2.it_val = 3;
    re46c191_cfg2.irc_val = 2;
    re46c191_cfg2.lb_val = 3;
    re46c191_cfg2.ltd_en = 0;
    re46c191_cfg2.hush_val = 0;
    re46c191_cfg2.lbh_en = 1;
    re46c191_cfg2.eol_en = 0;
    re46c191_cfg2.ts_val = 1;
}

bool_t re46c191_senser_test(void)
{
    re46c191_cfg_pow_open();
    re46c191_test_pin_operar();
    re46c191_cfg_pow_close();
    return TRUE;
}

osel_uint32_t re46c191_driver_write(osel_device_t *dev, osel_uint32_t pos, const void *buffer, osel_uint32_t size)
{
    uint8_t thr = *(uint8_t *)buffer;
    re46c191_cmd_type = (re46c191_cfg_thr_type_e)pos;
    if(thr > 64 || re46c191_cmd_type >= TYPE_THR_MAX)
    {
        return FALSE;
    }
    re46c191_pow_open();
    re46c191_cfg_pow_open();
    uint8_t thr_tmp = thr & 0x3f;
    
    switch(re46c191_cmd_type)
    {
    case TYPE_THR_LTD:
        re46c191_cfg1.ltd_thr = thr_tmp;
        break;
        
    case TYPE_THR_CTL:
        re46c191_cfg1.ctl_thr = thr_tmp;
        break;
        
    case TYPE_THR_HUL:
        re46c191_cfg1.hul_thr = thr_tmp;
        break;
        
    case TYPE_THR_HYL:
        re46c191_cfg1.hyl_thr = thr_tmp;
        break;
        
    case TYPE_THR_NL:
        re46c191_cfg1.nl_thr = thr_tmp;
        break;
        
    default:
        return FALSE;
        break;
    }
    if(!re46c191_write_data(re46c191_cfg1, re46c191_cfg2, dev))
    {
        re46c191_cfg_pow_close();
        re46c191_pow_close();
        return FALSE;
    }
    re46c191_cfg_pow_close();
    re46c191_pow_close();
    return TRUE;
}

static osel_int8_t re46c191_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start re46c191
    return 0;
}


static osel_int8_t re46c191_driver_close(osel_device_t *dev)
{
    //*< make re46c191 in sleep mode
    return 0;
}

static osel_int8_t re46c191_driver_init(osel_device_t *dev)
{
    re46c191_init();
    re46c191_pow_open();
    re46c191_port_init();
    re46c191_int_cfg();
    re46c191_para_cfg();
    return TRUE;
}

static osel_uint32_t re46c191_driver_control(osel_device_t *dev,osel_uint8_t cmd,void *args)
{
    re46c191_cmd_type = (re46c191_cfg_thr_type_e)cmd;
    return 0;
}

void re46c191_device_init(void)
{
    re46c191_device.init = re46c191_driver_init;
    re46c191_device.write = re46c191_driver_write;
    re46c191_device.control = re46c191_driver_control;
    re46c191_device.open = re46c191_driver_open;
    re46c191_device.close = re46c191_driver_close;
    
    osel_device_register(&re46c191_device, SMOKE_SENSOR_NAME, OSEL_DEVICE_FLAG_RDONLY);
}

static void re46c191_port_init(void)
{
    ;
}

static void re46c191_int_cfg(void)
{
    ;
}
