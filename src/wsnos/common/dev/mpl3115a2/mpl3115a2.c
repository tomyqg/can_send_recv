/***************************************************************************
* File        : pressure_defs.h
* Summary     :
* Version     : v0.1
* Author      : zhouyimeng
* Date        : 2017/02/28
* Change Logs :
* Date              Versi on            Author              Notes
* 2017/02/28         v0.1            zhouyimeng        first version
* 2017/03/17        v0.0.2             gang.cheng       用设备管理驱动方式重写驱动
***************************************************************************/
#include "gz_sdk.h"
#include "mpl3115a2.h"

osel_device_t mpl3115a2_device;

static mpl_cmd_type_e mpl3115a2_cmd_type = MPL_PRESSURE;

void mpl3115a2_set_mode_standby(void)
{
    uint8_t ctrlReg = 0;
    
    mpl3115a2_read_reg(CTRL_REG1, &ctrlReg);
    ctrlReg &= ~SBYB;                                   // 清SBYB位，置为就绪模式
    mpl3115a2_write_reg(CTRL_REG1, ctrlReg);
}

void mpl3115a2_set_mode_active(void)
{
    uint8_t ctrlReg = 0;
    
    mpl3115a2_read_reg(CTRL_REG1, &ctrlReg);
    ctrlReg |= SBYB;                                     // 设置SBYB位，置为活动模式
    mpl3115a2_write_reg(CTRL_REG1, ctrlReg);
}

void mpl3115a2_set_mode_barometer(void)
{
    uint8_t ctrlReg = 0;
    
    mpl3115a2_set_mode_standby();
    
    mpl3115a2_read_reg(CTRL_REG1, &ctrlReg);
    ctrlReg &= ~ALT;                                     //将ALT清0
    mpl3115a2_write_reg(CTRL_REG1, ctrlReg);
    
    mpl3115a2_set_mode_active();
}

void mpl3115a2_set_mode_altimeter(void)
{
    uint8_t ctrlReg = 0;
    
    mpl3115a2_set_mode_standby();
    
    mpl3115a2_read_reg(CTRL_REG1, &ctrlReg);
    ctrlReg |= ALT;                                      // 将ALT置1
    mpl3115a2_write_reg(CTRL_REG1, ctrlReg);
    
    mpl3115a2_set_mode_active();
}

void mpl3115a2_toggle_oneshot(void)
{
    uint8_t ctrlReg = 0;
    
    mpl3115a2_set_mode_standby();
    
    mpl3115a2_read_reg(CTRL_REG1, &ctrlReg);
    ctrlReg &= ~OST;                                     // 将OST清0
    mpl3115a2_write_reg(CTRL_REG1, ctrlReg);
    
    mpl3115a2_read_reg(CTRL_REG1, &ctrlReg);
    ctrlReg |= OST;                                      // 将OST置1
    mpl3115a2_write_reg(CTRL_REG1, ctrlReg);
    
    mpl3115a2_set_mode_active();
}


osel_uint32_t mpl3115a2_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size)
{
    uint8_t msb = 0, csb = 0, lsb = 0;
    uint8_t status = 0;
    if (mpl3115a2_cmd_type == MPL_PRESSURE)
    {
        uint8_t tempBuf[3];
        
        mpl3115a2_set_mode_barometer();
	while (! (status & PDR)) 
        {
            mpl3115a2_read_reg(STATUS_REG, &status);
            HAL_Delay(10);
        }
        mpl3115a2_read_regs(OUT_P_MSB_REG, tempBuf, 3);
        
        msb = tempBuf[0];
        csb = tempBuf[1];
        lsb = tempBuf[2];
        
        float pressure = (float)((msb << 16 | csb << 8 | lsb) >> 6);
        lsb &= 0x30;                                // 5/4位为分数分量
        lsb >>= 4;                                  // 对齐
        float decimal = ((float)lsb) / 4.0;
        pressure = pressure + decimal;
        osel_memcpy(buffer, (uint8_t *)&pressure, 4);
        return 0;
    }
    else if (mpl3115a2_cmd_type == MPL_ALTITUDE)
    {
        uint8_t tempBuf[3];
        
        mpl3115a2_set_mode_altimeter();
        while (! (status & PDR)) 
        {
            mpl3115a2_read_reg(STATUS_REG, &status);
            HAL_Delay(10);
        }
        mpl3115a2_read_regs(OUT_P_MSB_REG, tempBuf, 3);
        
        msb = tempBuf[0];
        csb = tempBuf[1];
        lsb = tempBuf[2];
        
        float altitude = 0;
        float decimal = ((float)(lsb >> 4)) / 16.0;
        altitude = (float)((int16_t)((msb << 8) | csb)) + decimal;
        osel_memcpy(buffer, (uint8_t *)&altitude, 4);
        return 0;
    }
    else
    {
        uint8_t tempBuf[2];
        bool negSign = false;
        uint8_t val;
        float temperature = 0;
        
        while (! (status & TDR)) 
        {
            mpl3115a2_read_reg(STATUS_REG, &status);
            HAL_Delay(10);
        }
        mpl3115a2_read_regs(OUT_T_MSB_REG, tempBuf, 2);
        
        msb = tempBuf[0];
        lsb = tempBuf[1];
        
        if (msb > 0x7F)
        {
            val = ~((msb << 8) + lsb) + 1;
            msb = val >> 8;
            lsb = val & 0x00F0;
            negSign = true;
        }
        
        if (negSign == true)
        {
            temperature = 0 - (msb + (float)((lsb >> 4) / 16.0));
        }
        else
        {
            temperature = msb + (float)((lsb >> 4) / 16.0);
        }
        osel_memcpy(buffer, (uint8_t *)&temperature, 4);
        return 0;
    }
}

bool_t mpl3115a2_time_step_set(uint8_t time_step)//自动采样时间:(2^time_step)s
{
    uint8_t ctrlReg = 0;
    
    if (time_step > 0xF)
        return FALSE;
    
    mpl3115a2_read_reg(CTRL_REG2, &ctrlReg);
    ctrlReg &= ~ST;
    ctrlReg |= time_step;
    
    mpl3115a2_write_reg(CTRL_REG2, ctrlReg);
    
    return TRUE;
}

void mpl3115a2_reset(void)
{
    mpl3115a2_write_reg(CTRL_REG1, RST);
}

/****************************************************************************/
static osel_int8_t mpl3115a2_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start mpl3115a2
    return 0;
}


static osel_int8_t mpl3115a2_driver_close(osel_device_t *dev)
{
    //*< make mpl3115a2 in sleep mode
    return 0;
}



static osel_int8_t mpl3115a2_driver_init(osel_device_t *dev)
{
    uint8_t reg;
    
    dev_iic_init();
    pressure_port1_init();
    pressure_int1_cfg();
    
    mpl3115a2_write_reg(PT_DATA_CFG_REG, DREM | PDEFE | TDEFE);// 使能读压力、温度数据
    mpl3115a2_write_reg(CTRL_REG1, OS_128 | SBYB);// 设置采样率
    // 检查 MPL3115 ID
    mpl3115a2_read_reg(WHO_AM_I_REG, &reg);
    if (reg != MPL3115A2_ID)
        return -1;
    
    return 0;
}

static osel_uint32_t
mpl3115a2_driver_control(osel_device_t *dev,
                         osel_uint8_t cmd,
                         void *args)
{
    mpl3115a2_cmd_type = (mpl_cmd_type_e)cmd;
    return 0;
}

void mpl3115a2_device_init(void)
{
    mpl3115a2_device.init = mpl3115a2_driver_init;
    mpl3115a2_device.read = mpl3115a2_driver_read;
    mpl3115a2_device.control = mpl3115a2_driver_control;
    mpl3115a2_device.open = mpl3115a2_driver_open;
    mpl3115a2_device.close = mpl3115a2_driver_close;
    
    osel_device_register(&mpl3115a2_device, PRE_SENSOR_NAME, OSEL_DEVICE_FLAG_RDONLY);
}

















