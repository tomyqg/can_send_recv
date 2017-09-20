/**
 * @brief       : 添加设备管理功能，参考rtthread的device驱动
 * @file        : wsnos_device.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-02-08
 * change logs  :
 * Date       Version     Author        Note
 * 2017-02-08  v0.0.1  gang.cheng    first version
 */
#ifndef __WSNOS_DEVICE_H__
#define __WSNOS_DEVICE_H__

#include "common/lib/list.h"

/**
 * device flags defitions
 */
#define OSEL_DEVICE_FLAG_DEACTIVATE     0x000   /**< device is not initialized */
#define OSEL_DEVICE_FLAG_RDONLY         0x001   /**< read only */
#define OSEL_DEVICE_FLAG_WRONLY         0x002   /**< write only */
#define OSEL_DEVICE_FLAG_RDWR           0x003   /**< read and write */

#define OSEL_DEVICE_FLAG_ACTIVATED      0x010   /**< device is activated */


#define OSEL_DEVICE_OFLAG_CLOSE         0x000   /**< device is closed */
#define OSEL_DEVICE_OFLAG_RDONLY        0x001   /**< read only access */
#define OSEL_DEVICE_OFLAG_WRONLY        0x002   /**< write only access */
#define OSEL_DEVICE_OFLAG_RDWR          0x003   /**< read and write access */
#define OSEL_DEVICE_OFLAG_OPEN          0x008   /**< device is opened */


typedef struct osel_device osel_device_t;

struct osel_device
{
    // struct osel_device *next;
    list_head_t node;

    char name[OSEL_NAME_MAX];           /**< name of kernel object */
    osel_int16_t flag;                  /**< device flag */
    osel_int16_t oflag;                 /**< device open flag */
    osel_int16_t ref_count;             /**< reference count */
    osel_uint32_t magicflag;            /**< differ register or unregister */
    /** device call back */
    osel_int8_t (*rx_indicate)(osel_device_t *dev, osel_uint32_t size);
    osel_int8_t (*tx_complate)(osel_device_t *dev, void *buffer);

    /* common device interface */
    osel_int8_t (*init)   (osel_device_t *dev);
    osel_int8_t (*open)   (osel_device_t *dev, osel_int16_t oflag);
    osel_int8_t (*close)  (osel_device_t *dev);
    osel_uint32_t (*read)   (osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size);
    osel_uint32_t (*write)  (osel_device_t *dev, osel_uint32_t pos, const void *buffer, osel_uint32_t size);
    osel_uint32_t (*control)(osel_device_t *dev, osel_uint8_t cmd, void *args);

    /* device private data */
    void *user_data;
};


/**
 * @brief 设备管理模块初始化
 * @return  [description]
 */
osel_int8_t osel_device_init_all(void);

/**
 * @brief 根据设备名查找对应设备的句柄
 * @param  name 设备名
 * @return      对应设备的句柄，如果未查找到返回NULL
 */
osel_device_t *osel_device_find(const char *name);

/**
 * @brief 注册设备
 * @param  dev   注册设备的结构体指针
 * @param  name  注册设备的名字
 * @param  flags 注册设备类型：只读、只写、可读可写
 * @return       设备注册是否成功，0 成功，-1 失败
 */
osel_int8_t osel_device_register(osel_device_t *dev,
                                 const char *name,
                                 osel_uint16_t flags);

/**
 * @brief 注销设备
 * @param  dev 注销设备的结构体指针
 * @return     设备注销是否成功，0 成功，-1 失败
 */
osel_int8_t osel_device_unregister(osel_device_t *dev);

/**
 * @brief 注册接收数据回调
 * @param dev 要注册接收回调的设备指针
 * @param rx_ind 注册的接收数据回调函数
 * @return 回调注册是否成功，0 成功，-1 失败
 */
osel_int8_t
osel_device_set_rx_indicate(osel_device_t *dev,
                            osel_int8_t (*rx_ind)(osel_device_t *dev, osel_uint32_t size));

/**
 * @brief 注册发送数据回调
 * @param dev 要注册发送回调的设备指针
 * @param rx_ind 注册的发送数据回调函数
 * @return 回调注册是否成功，0 成功，-1 失败
 */
osel_int8_t
osel_device_set_tx_complete(osel_device_t *dev,
                            osel_int8_t (*tx_done)(osel_device_t *dev, void *buffer));

/**
 * @brief 设备初始化
 * @param  dev 初始化设备的结构体指针
 * @return     初始化是否成功，0 成功
 */
osel_int8_t   osel_device_init (osel_device_t *dev);

/**
 * @brief 打开设备，从低功耗中唤醒
 * @param  dev   操作设备的结构体指针
 * @param  oflag 操作标志
 * @return       是否成功，0 成功
 */
osel_int8_t   osel_device_open (osel_device_t *dev, osel_uint16_t oflag);

/**
 * @brief 关闭设备，进入低功耗
 * @param  dev 操作设备的结构体指针
 * @return     是否成功，0 成功
 */
osel_int8_t   osel_device_close(osel_device_t *dev);

/**
 * @brief 设备读操作
 * @param  dev    操作设备的结构体指针
 * @param  pos    读偏移量
 * @param  buffer 存放读取数据的缓存地址
 * @param  size   预期读取多少字节
 * @return        成功读取多少字节
 */
osel_uint32_t osel_device_read (osel_device_t *dev,
                                osel_uint32_t  pos,
                                void          *buffer,
                                osel_uint32_t  size);

/**
 * @brief 设备写操作
 * @param  dev    操作设备的结构体指针
 * @param  pos    写偏移量
 * @param  buffer 存放写入数据的缓存地址
 * @param  size   预期写入多少字节
 * @return        成功写入多少字节
 */
osel_uint32_t osel_device_write(osel_device_t *dev,
                                osel_uint32_t  pos,
                                const void    *buffer,
                                osel_uint32_t  size);

/**
 * @brief 设备控制操作
 * @param  dev 操作设备的结构指针
 * @param  cmd 命令
 * @param  arg 命令参数
 * @return     操作是否成功，0 成功
 */
osel_uint32_t  osel_device_control(osel_device_t *dev, osel_uint8_t cmd, void *arg);

#endif
