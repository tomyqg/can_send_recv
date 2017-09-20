/***************************************************************************
* File        : exter_flash.h
* Summary     :
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/5/6
* Change Logs :
* Date             Versi on         Author           Notes
* 2016/5/6         v0.1            wangjifang        first version
***************************************************************************/
#pragma once

typedef struct
{
    uint32_t block;
    uint32_t page;
    uint16_t offset;
} flash_page_t;

typedef struct
{
    uint16_t col_addr;
    bool_t inited;
} nand_chip_t;


/**
 * 外部flash备驱动的结构体定义
 */

typedef enum _flash_ctl_e_
{
    FLASH_CTL_ID,
    FLASH_CTL_ERASE,

    FLASH_W_PAGE_ADDR,
    FLASH_W_COL_ADDR,
} flash_ctl_e;

#define MX35LF1G_REG_STATE_1                        0xA1
#define MX35LF1G_REG_STATE_2                        0xB1
#define MX35LF1G_REG_STATE_3                        0xC1

#define MX35LF1G_REG_PROTECT_SECTION_UNLOCK         0x7C
#define MX35LF1G_REG_ECC                            0x10
#define MX35LF1G_REG_BUF                            0x08

#define MX35LF1G_CMD_WRITE_ENABLE                   0x06
#define MX35LF1G_CMD_WRITE_DISABLE                  0x04
#define MX35LF1G_CMD_SET_FEATURE                    0x1F
#define MX35LF1G_CMD_GET_FEATURE                    0x0F

#define MX35LF1G_CMD_PAGE_READ                      0x13
#define MX35LF1G_CMD_READ_FROM_CACHE                0x03
#define MX35LF1G_CMD_READ_FROM_CACHE2               0x3B
#define MX35LF1G_CMD_READ_FROM_CACHE4               0x6B
#define MX35LF1G_CMD_READ_ID                        0x9F

#define MX35LF1G_CMD_BLOCK_ERASE                    0xD8

#define MX35LF1G_CMD_PROGRAM_EXECUTE                0x10
#define MX35LF1G_CMD_PROGRAM_LOAD                   0x02
#define MX35LF1G_CMD_PROGRAM_LOAD_RAND_DATA         0x84
#define MX35LF1G_CMD_PROGRAM_LOAD4                  0x32
#define MX35LF1G_CMD_PROGRAM_LOAD_RAND_DATA4        0x34

#define MX35LF1G_CMD_RESET                          0xFF
#define MX35LF1G_CMD_DUMMY                          0xFF
#define MX35LF1G_CMD_CA_H                           0x00
#define MX35LF1G_CMD_CA_L                           0x00

#define FS_TOTAL_BLOCK                              (1024)
#define FS_PAGE_PER_BLOCK                           (64)
#define FS_PAGE_DATA_SIZE                           (2048)  //Main Memory Array 2048 byte
#define FS_PAGE_SPARE_SIZE                          (64)    //Spare Area 64 byte

#define FS_PAGE_SIZE            (FS_PAGE_DATA_SIZE+FS_PAGE_SPARE_SIZE)
#define FS_BLOCK_DATA_SIZE      (FS_PAGE_PER_BLOCK*FS_PAGE_DATA_SIZE)

#define FS_NR_PARTITION         (1)
#define FS_PAR_1_BLOCKS         (100)
#define FS_PAR_2_BLOCKS         (FS_TOTAL_BLOCK-FS_PAR_1_BLOCKS)

#pragma pack(1)

typedef struct _flash_id_t_
{
    uint8_t mfr_id;
    uint16_t device_id;
} flash_id_t;

#pragma pack()

#define EXFLASH_NAME         "MX35LF1G"

void mx35lf1g_device_init(void);
