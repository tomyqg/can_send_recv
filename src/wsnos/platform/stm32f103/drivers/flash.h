/**
 * @brief       : this
 * @file        : flash.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-12-03
 * change logs  :
 * Date       Version     Author        Note
 * 2015-12-03  v0.0.1  gang.cheng    first version
 */
#ifndef __FLASH_H__
#define __FLASH_H__

#define STM32_FLASH_SIZE        512  // 所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN        1    // stm32芯片内容FLASH 写入使能(0，禁用;1，使能)

#define DEVICE_INFO_ADDR                (0x08070000)
#define DEVICE_INFO_ADDR_0              (0x08070400)
#define DEVICE_INFO_ADDR_1              (0x08070800)
#define DEVICE_INFO_ADDR_2              (0x08070c00)
#define DEVICE_INFO_ADDR_3              (0x08071000)
#define DEVICE_INFO_ADDR_4              (0x08071400)
#define FLASH_SEG_ERASE                 (0u)
/**
 * 向flash中写数据，应先禁止全局中断
 *
 * @param *buffer: 存放待写数据的源地址指针(缓冲区)
 * @param *flash_ptr: 将被写入的目的地址指针(flash 区)
 * @param len: 将被写入的数据的长度,最长为128字节
 *
 * @return: 空
 */
void flash_write(uint8_t *flash_ptr, const uint8_t *buffer, uint16_t len);

/**
 * 从flash区读取数据
 *
 * @param *flash_ptr: 将被读取的源地址指针(flash 区)
 * @param *buffer: 存放读出数据的目的地址指针(缓冲区)
 * @param len: 将被读取的数据的长度,最长为128字节
 *
 * @return: 空
 */
void flash_read(const uint8_t *flash_ptr, uint8_t *buffer, uint16_t len);

/**
 * 擦除flash段或区的数据(segment or bank)
 *
 * @param *flash_ptr: 将被擦除的flash区的首地址
 * @param erase_type: 选择擦除区/段
 *
 * @return: 空
 */
void flash_erase(uint8_t *const flash_ptr,
                 uint8_t erase_type);

#endif
