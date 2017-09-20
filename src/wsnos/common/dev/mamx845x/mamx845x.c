/***************************************************************************
* File        : mamx845x.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/
#include "gz_sdk.h"
#include "mamx845x.h"

#define MAMX_DATA_OUT_REG_NUM    6 // X/Y/Z三轴2个寄存器
#define DEBUG_MAMX8452_I2C        0
#if DEBUG_MAMX8452_I2C
#include <stdio.h>
#endif

osel_device_t mamx845x_device;


/*********************************************************\
* Put MMA845xQ into Active Mode
\*********************************************************/
void MMA845x_Active ()
{
    uint8_t tmp = 0;
    mma845x_read_register(CTRL_REG1, &tmp);
    
    mma845x_write_register(CTRL_REG1, tmp | ACTIVE_MASK);
}

/*********************************************************\
* Put MMA845xQ into Standby Mode
\*********************************************************/
void MMA845x_Standby (void)
{
  uint8_t n;
  /*
  **  Read current value of System Control 1 Register.
  **  Put sensor into Standby Mode.
  **  Return with previous value of System Control 1 Register.
  */
  mma845x_read_register(CTRL_REG1, &n);
  mma845x_write_register(CTRL_REG1, n & ~ACTIVE_MASK);
}

/**
 * @brief 该函数在系统启动后传感器的寄存器初始配置操作
 * @param  
 * @return  
 */
static void mamx845x_settings(void)
{
    uint8_t device_id = 0;
    mma845x_read_register(WHO_AM_I_REG,&device_id);
    DBG_ASSERT(0x1A == device_id __DBG_LINE);
    
    MMA845x_Standby();
  /*
  **  Configure sensor for:
  **    - System Output Data Rate of 800Hz (1.25ms)
  **    - Full Scale of +/-4g
  */
  mma845x_write_register(CTRL_REG1, DATA_RATE_1250US  + LNOISE_MASK);
  mma845x_write_register(XYZ_DATA_CFG_REG, FULL_SCALE_4G);  
  mma845x_write_register(F_SETUP_REG, F_MODE0_MASK);  
  mma845x_write_register(OFF_X_REG, 0x00); 
  mma845x_write_register(OFF_Y_REG, 0x00); 
  mma845x_write_register(OFF_Z_REG, 0x00); 
  
  MMA845x_Active(); 
}

osel_int8_t mamx845x_driver_init(osel_device_t *dev)
{
    mma845x_i2c_init();//I2C模块的初始化:包括使用前I2C的释放操作和寄存器的配置
    mamx845x_settings();
    return 0;
}

/**
 * @brief 获得三轴加速度传感器各轴检测结果
 * @param  dev    操作设备的结构体指针
 * @param  pos    读偏移量,pos = 0,读取X;pos = 2,读取Y；pos = 4,读取Z
 * @param  buffer 存放X/Y/Z轴数据的缓存地址
 * @param  size   预期读取多少字节
 * @return        成功读取多少字节
 */
osel_uint32_t mamx845x_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size)
{
    if (buffer == NULL || size != MAMX_DATA_OUT_REG_NUM)
    {
        return 0;
    }
    uint8_t src_reg;
    uint8_t accbuf[6] = {0};
    uint8_t tmpbuf[6] = {0};
    uint8_t ret = I2C_OK;               // 读写返回值
    
    mma845x_read_register(0,&src_reg);
    ret = mma845x_read_buff(0x01 + pos , accbuf , size);
    DBG_ASSERT(I2C_OK == ret __DBG_LINE);

    uint8_t flag = (accbuf[0] & 0x80) > 0 ? 0xe0 : 0x00;
    uint8_t hi = (((accbuf[0] & 0x7c) >> 2) + flag);
    uint8_t low = ((accbuf[0] & 0x03) << 6) + ((accbuf[1] & 0xfc) >> 2);
    uint16_t temp_data = ((uint16_t)hi << 8) + (uint16_t)low;
    osel_memcpy(&tmpbuf[0], &temp_data, 2);
    
    flag = (accbuf[2] & 0x80) > 0 ? 0xe0 : 0x00;
    hi = (((accbuf[2] & 0x7c) >> 2) + flag);
    low = ((accbuf[2] & 0x03) << 6) + ((accbuf[3] & 0xfc) >> 2);
    temp_data = ((uint16_t)hi << 8) + (uint16_t)low;
    osel_memcpy(&tmpbuf[2], &temp_data, 2);
    
    flag = (accbuf[4] & 0x80) > 0 ? 0xe0 : 0x00;
    hi = (((accbuf[4] & 0x7c) >> 2) + flag);
    low = ((accbuf[4] & 0x03) << 6) + ((accbuf[5] & 0xfc) >> 2);
    temp_data = ((uint16_t)hi << 8) + (uint16_t)low;
    osel_memcpy(&tmpbuf[4], &temp_data, 2);
    osel_memcpy(buffer, tmpbuf, size);
    
    mma845x_read_register(0,&src_reg);
    
    return MAMX_DATA_OUT_REG_NUM;
}

static osel_int8_t mamx845x_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start mma845x
    return 0;
}

static osel_int8_t mamx845x_driver_close(osel_device_t *dev)
{
    //*< make mma845x in sleep mode
    return 0;
}

void mamx845x_device_init(void)
{
    mamx845x_device.init = mamx845x_driver_init;
    mamx845x_device.read = mamx845x_driver_read;
    mamx845x_device.open = mamx845x_driver_open;
    mamx845x_device.close = mamx845x_driver_close;
    
    osel_device_register(&mamx845x_device, ACC_SENSOR_NAME, OSEL_DEVICE_FLAG_RDONLY);
}
