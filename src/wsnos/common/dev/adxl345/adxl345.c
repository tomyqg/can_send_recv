/***************************************************************************
* File        : adxl345.c
* Summary     : 
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/10/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/10/25         v0.1            wangjifang        first version
***************************************************************************/
#include "gz_sdk.h"
#include "adxl345.h"

#define ADXL345_DATA_OUT_REG_NUM    6 // X/Y/Z三轴2个寄存器
#define DEBUG_I2C                   1

osel_device_t adxl345_device;

/**
 * @brief 该函数在系统启动后传感器的寄存器初始配置操作
 * @param  
 * @return  
 */
static void accelerated_settings(void)
{
#if DEBUG_I2C
    uint8_t tmp = 0;
    for(uint8_t i= 0; i < 0x31; i++)
    {
        adxl345_read_register(i, &tmp);
    }
#endif
    uint8_t device_id = 0;
    adxl345_read_register(ADXL345_DEVID,&device_id);
    DBG_ASSERT(ADXL345_ID == device_id __DBG_LINE);
    // ADXL345_REG_POWER_CTL[3]=0设置成待机模式,即清除测试位
    adxl345_write_register(ADXL345_POWER_CTL,0x00); 
    //BW_RATE[4]=1；即设置LOW_POWER位低功耗
    //BW_RATE[3][2][1][0]=0x07，即设置输出速率12.5HZ，Idd=34uA
    //普通，100hz
//    adxl345_write_register(ADXL345_BW_RATE,0x07);                                                     
    adxl345_write_register(ADXL345_BW_RATE,0x0a);  
   //THRESH_TAP: 比例因子为62.5mg/lsb  建议大于3g 
    //2g=0X20,,,4g=0x40,,,8g=0x80,,,16g=0xff   3.5g=0x38
    adxl345_write_register(ADXL345_THRESH_TAP,0x38); 
                                                     
    adxl345_write_register(ADXL345_OFSX,0x00);       // Z轴偏移量
    adxl345_write_register(ADXL345_OFSY,0x00);       // Y轴偏移量
    adxl345_write_register(ADXL345_OFSZ,0x00);       // Z轴偏移量
    //DUR:比例因子为625us/LSB，建议大于10ms
    //6.25ms=0x0A //12.5ms=0x14。
    adxl345_write_register(ADXL345_DUR,0x14); 
    //Latent:比例因子为1.25ms/LSB，建议大于20ms
    //2.5ms=0x02，，20ms=0x10，，，25ms=0x14
    adxl345_write_register(ADXL345_LATENT,0x14);    
    //window:比例因子为1.25ms/LSB，建议大于80ms 
    //10ms=0x08，，80ms=0x40
    adxl345_write_register(ADXL345_WINDOW,0x41);    
    //THRESH_ACT:比例因子为62.5mg/LSB，
    //2g=0X20,,,4g=0x40,,,8g=0x80,,,16g=0xff,,,//1.5g=0x18
    adxl345_write_register(ADXL345_THRESH_ACT,0x18);
    //THRESH_INACT:比例因子为62.5mg/LSB   
    //1g=0x10  //2g=0X20,,,4g=0x40,,,8g=0x80,,,16g=0xff
    adxl345_write_register(ADXL345_THRESH_INACT,0x10);
                                                    
    //TIME_INACT:比例因子为1sec/LSB      //1s=0x01                                           
    adxl345_write_register(ADXL345_TIME_INACT,0x05);
    //设置为直流耦合：当前加速度值直接与门限值比较，以确定是否运动或静止  
    //x,y,z参与检测活动或静止
    adxl345_write_register(ADXL345_ACT_INACT_CTL,0x77);
    //用于自由落地检测，比例因子为62.5mg/LSB   
    //建议设置成300mg~600mg（0x05~0x09）
    adxl345_write_register(ADXL345_THRESH_FF,0x06);       
    //所有轴的值必须小于此设置值，才会触发中断;比例因子5ms/LSB   
    //建议设成100ms到350ms(0x14~~0x46),200ms=0x28
    adxl345_write_register(ADXL345_TIME_FF,0x28);         
    //TAP_AXES:单击/双击轴控制寄存器； 
    //1）不抑制双击  2）使能x.y,z进行敲击检查
    adxl345_write_register(ADXL345_TAP_AXES,0x07);  
    // 中断使能   
    //1）DATA_READY[7]   2)SINGLE_TAP[6]  3)DOUBLE_TAP[5]  4)Activity[4]
    //5)inactivity[3]    6)FREE_FALL[2]   7)watermark[1]   8)overrun[0]
    adxl345_write_register(ADXL345_INT_ENABLE,0xfc); 
                                                    
    //INT_MAC中断映射：任意位设为0发送到INT1位，，设为1发送到INT2位
    //1）DATA_READY[7]   2)SINGLE_TAP[6]  3)DOUBLE_TAP[5]  4)Activity[4]
    //5)inactivity[3]    6)FREE_FALL[2]   7)watermark[1]   8)overrun[0] 
    adxl345_write_register(ADXL345_INT_MAP,0x40);   
    
    //1）SELF_TEST[7];2)SPI[6]; 3)INT_INVERT[5]：设置为0中断高电平有效，
    // 数据输出格式  高电平触发
    adxl345_write_register(ADXL345_DATA_FORMAT,0x0B);
    //adxl345_write_register(ADXL345_DATA_FORMAT,0x2B);// 数据输出格式  低电平触发         
                                                    //反之设为1低电平有效    rang[1][0]
    //设置 FIFO模式
    adxl345_write_register(ADXL345_FIFO_CTL,0xBF);
    // 进入测量模式
    //1)链接位[5]    2)AUTO_SLEEP[4]   3)测量位[3]  4)休眠位[2]  5)唤醒位[1][0]
    adxl345_write_register(ADXL345_POWER_CTL,0x28);
    uint8_t int_source; 
    adxl345_read_register(ADXL345_INT_SOURCE, &int_source);                                             
}

osel_int8_t adxl345_driver_init(osel_device_t *dev)
{
    adxl345_i2c_init();//I2C模块的初始化:包括使用前I2C的释放操作和寄存器的配置
    accelerated_settings();
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
osel_uint32_t adxl345_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size)
{
    if (buffer == NULL || size != ADXL345_DATA_OUT_REG_NUM)
    {
        return 0;
    }
    
    uint8_t accbuf[ADXL345_DATA_OUT_REG_NUM] = {0};
    uint8_t tmpbuf[ADXL345_DATA_OUT_REG_NUM] = {0};
    uint8_t ret = I2C_OK;               // 读写返回值
    
    ret = adxl345_read_buff(ADXL345_DATAX0 + pos , accbuf , size);
    DBG_ASSERT(I2C_OK == ret __DBG_LINE);

    uint16_t temp_data = accbuf[1] << 8 | accbuf[0];
    osel_memcpy(&tmpbuf[0], &temp_data, 2);
    temp_data = accbuf[3] << 8 | accbuf[2];
    osel_memcpy(&tmpbuf[2], &temp_data, 2);
    temp_data = accbuf[5] << 8 | accbuf[4];
    osel_memcpy(&tmpbuf[4], &temp_data, 2);
    
    osel_memcpy(buffer, tmpbuf, size);
    return ADXL345_DATA_OUT_REG_NUM;
}

static osel_int8_t adxl345_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start adxl345
    return 0;
}

static osel_int8_t adxl345_driver_close(osel_device_t *dev)
{
    //*< make adxl345 in sleep mode
    return 0;
}

void adxl345_device_init(void)
{
    adxl345_device.init = adxl345_driver_init;
    adxl345_device.read = adxl345_driver_read;
    adxl345_device.open = adxl345_driver_open;
    adxl345_device.close = adxl345_driver_close;
    
    osel_device_register(&adxl345_device, ACC_SENSOR_NAME, OSEL_DEVICE_FLAG_RDONLY);
};
