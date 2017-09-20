/**
 * @brief       : this
 * @file        : fat_defs.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-11-23
 * change logs  :
 * Date       Version     Author        Note
 * 2016-11-23  v0.0.1  gang.cheng    first version
 */
#ifndef __FAT_DEFS_H__
#define __FAT_DEFS_H__


#include "sys_arch/osel_arch.h"

typedef enum 
{
    FAT_CMD_READ_END,
    FAT_CMD_FILE_SIZE,
    FAT_CMD_FILE_DEL,
    FAT_CMD_FORMAT,
    FAT_CMD_SPACE_FREE,
    FAT_CMD_SPACE_TOTAL,
} fat_cmd_t;

struct fat_driver
{
    bool_t      (*init)(void);
    bool_t      (*deinit)(void);

    bool_t      (*create)(char_t *path);
    int16_t     (*open)(char_t *path, uint16_t mode);
    void        (*close)(int16_t fd);

    uint16_t    (*write)(int16_t fd, uint8_t *buf, uint16_t len);
    uint16_t    (*read)(int16_t fd, uint8_t *buf, uint16_t);
    uint32_t    (*seek)(int16_t fd, uint32_t offset);
    uint32_t    (*control)(fat_cmd_t cmd, uint8_t *param);
};

#endif
