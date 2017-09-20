/***************************************************************************
* File        : fat_demo.c
* Summary     : 
* Version     : v0.1
* Author      : chenggang
* Date        : 2016/11/25
* Change Logs :
* Date       Versi on     Author     Notes
* 2016/11/25         v0.1            chenggang        first version
***************************************************************************/
#include "fat.h"

#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include "common/hal/hal.h"

#define LOG_ALARM_PATH          "/alarm.txt"

static void fat_init(void)
{
    osel_int_status_t status = 0;
    OSEL_ENTER_CRITICAL(status);
    lock_serial(SERIAL_1);
    lock_serial(SERIAL_2);
    lock_serial(SERIAL_3);

    if(!FAT_DEVICE.init())
    {
        FAT_DEVICE.control(FAT_CMD_FORMAT, NULL);
        FAT_DEVICE.init();
    }
    if(!FAT_DEVICE.create(LOG_ALARM_PATH))
    {
        FAT_DEVICE.control(FAT_CMD_FORMAT, NULL);
        FAT_DEVICE.init();
        FAT_DEVICE.create(LOG_ALARM_PATH);
    }
    unlock_serial(SERIAL_1);
    unlock_serial(SERIAL_2);
    unlock_serial(SERIAL_3);
    OSEL_EXIT_CRITICAL(status);
}

static void fat_write(uint8_t *buf, uint16_t len)
{
    uint32_t sfree = FAT_DEVICE.control(FAT_CMD_SPACE_FREE, NULL);
    if(sfree <= 1024)
        return;
    
    int16_t fd = -1;
    fd = FAT_DEVICE.open(LOG_ALARM_PATH, FAT_RDWR| FAT_CREATE | FAT_APPEND);
    if(fd==-1)
        return;
    
    FAT_DEVICE.write(fd, buf, len);
    FAT_DEVICE.close(fd);
}

static uint16_t fat_read(uint8_t *buf, uint16_t len)
{
    int16_t fd = -1;
    fd = FAT_DEVICE.open(LOG_ALARM_PATH, FAT_RDONLY);
    if(fd == -1)
        return 0;
    
    uint32_t size = FAT_DEVICE.control(FAT_CMD_FILE_SIZE, LOG_ALARM_PATH);
    
    FAT_DEVICE.read(fd, buf, len);
    FAT_DEVICE.close(fd);
    return size;
}



void fat_demo(void)
{
    fat_init();
    
    uint8_t read_buf[100];
    fat_read(read_buf, 100);
   
    FAT_DEVICE.control(FAT_CMD_FILE_DEL, LOG_ALARM_PATH); 
    
    uint8_t buf[] = {"Hello World!\r\n"};
    fat_write(buf, sizeof(buf)-1);
    fat_write(buf, sizeof(buf)-1);
    fat_read(read_buf, 100);
    fat_write(buf, sizeof(buf)-1);
    fat_read(read_buf, 100);
}
