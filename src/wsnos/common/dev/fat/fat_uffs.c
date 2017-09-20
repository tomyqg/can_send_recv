/**
 * @brief       : 该文件定义了uffs文件系统与底层nand之间的交互接口，同时提供了
 *  uffs与fat抽象文件驱动之间的接口
 * @file        : uffs_fat.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2016-11-23
 * change logs  :
 * Date       Version     Author        Note
 * 2016-11-23  v0.0.1  gang.cheng    first version
 */

#include "uffs/uffs_types.h"
#include "uffs/uffs_version.h"
#include "uffs/uffs_types.h"
#include "uffs/uffs_device.h"
#include "uffs/uffs_public.h"
#include "uffs/uffs_mtb.h"
#include "uffs/uffs_fd.h"
#include "uffs/uffs_utils.h"


#include "common/dev/dev.h"     //*< mx35lf1g

#include "fat_defs.h"
#include "fat.h"

DBG_THIS_MODULE("fat_uffs");

static uffs_Device g_uffs_device = {0};

static uffs_MountTable g_mount_table[] =
{
    { &g_uffs_device,  0, FS_PAR_1_BLOCKS - 1, "/" },
    //{ &demo_device_2,  PAR_1_BLOCKS, PAR_1_BLOCKS + PAR_2_BLOCKS - 1, "/" },
    { NULL, 0, 0, NULL }
};

static nand_chip_t g_nand_chip = {0, 0};
static struct uffs_StorageAttrSt g_femu_storage = {0};
/* static alloc the memory for each partition */
static uint32_t uffs_pools[UFFS_STATIC_BUFF_SIZE(FS_PAGE_PER_BLOCK, \
                           FS_PAGE_SIZE, FS_PAR_1_BLOCKS) / sizeof(uint32_t)];


static int nand_flash_init(uffs_Device *dev)
{
    nand_chip_t *chip = (nand_chip_t *)dev->attr->_private;
    if (!chip->inited)
    {
        chip->col_addr = 0x0000u;
        chip->inited = U_TRUE;
    }
    return 0;
}

static int nand_flash_release(uffs_Device *dev)
{
    nand_chip_t *chip = (nand_chip_t *)dev->attr->_private;
    return 0;
}

static int nand_page_read(uffs_Device *dev, u32 block, u32 page, u8 *data,
                          int data_len, u8 *ecc,
                          u8 *spare, int spare_len)
{
    uint8_t val = 0;
    int32_t ret = UFFS_FLASH_NO_ERR;
    nand_chip_t *chip = (nand_chip_t *)dev->attr->_private;
    flash_page_t f_page = {0, 0};

    if (data && data_len > 0)
    {
        chip->col_addr = 0x0000u;
        f_page.block = block;
        f_page.page  = page;
        f_page.offset = chip->col_addr;
        EXFLASH_DRIVER.read(&f_page, data, data_len);
    }

    if (spare && spare_len > 0)
    {
        chip->col_addr = dev->attr->page_data_size;
        f_page.block = block;
        f_page.page  = page;
        f_page.offset = chip->col_addr;
        EXFLASH_DRIVER.read(&f_page, spare, spare_len);
    }

    if (data == NULL && spare == NULL)
    {
        chip->col_addr = dev->attr->page_data_size + dev->attr->block_status_offs;
        f_page.block = block;
        f_page.page  = page;
        f_page.offset = chip->col_addr;
        EXFLASH_DRIVER.read(&f_page, &val, 1);
        ret = (val == 0xFF ? UFFS_FLASH_NO_ERR : UFFS_FLASH_BAD_BLK);
    }

    return ret;
}

static int nand_page_write(uffs_Device *dev, u32 block, u32 page,
                           const u8 *data, int data_len,
                           u8 *spare, int spare_len)
{
    uint8_t val = 0;
    int32_t ret = UFFS_FLASH_NO_ERR;
    nand_chip_t *chip = (nand_chip_t *)dev->attr->_private;
    flash_page_t f_page = {0, 0};

    if (data && data_len > 0)
    {
        chip->col_addr = 0x0000u;
        f_page.block = block;
        f_page.page  = page;
        f_page.offset = chip->col_addr;
        EXFLASH_DRIVER.write(&f_page, (uint8_t *)data, data_len);
    }

    if (spare && spare_len > 0)
    {
        chip->col_addr = dev->attr->page_data_size;
        f_page.block = block;
        f_page.page  = page;
        f_page.offset = chip->col_addr;
        EXFLASH_DRIVER.write(&f_page, (uint8_t *)spare, spare_len);
    }

    if (data == NULL && spare == NULL)
    {
        chip->col_addr = dev->attr->page_data_size + dev->attr->block_status_offs;
        f_page.block = block;
        f_page.page  = page;
        f_page.offset = chip->col_addr;
        EXFLASH_DRIVER.read(&f_page, &val, 1);
        ret = (val == 0xFF ? UFFS_FLASH_NO_ERR : UFFS_FLASH_BAD_BLK);
    }

    return ret;
}

static int nand_block_erase(uffs_Device *dev, u32 block)
{
    int32_t res = UFFS_FLASH_NO_ERR;

    EXFLASH_DRIVER.control(FLASH_CTL_ERASE, (uint8_t *)&block, (uint8_t *)&res);
    if (dev != NULL)
    {
        dev->st.block_erase_count++;
    }

    return UFFS_FLASH_NO_ERR;
}

static uffs_FlashOps nand_ops =
{
    nand_flash_init,    // InitFlash()
    nand_flash_release, // ReleaseFlash()
    nand_page_read,     // ReadPage()
    NULL,               // ReadPageWithLayout
    nand_page_write,    // WritePage()
    NULL,               // WirtePageWithLayout
    NULL,               // IsBadBlock(), let UFFS take care of it.
    NULL,               // MarkBadBlock(), let UFFS take care of it.
    nand_block_erase,   // EraseBlock()
};


static void setup_storage(struct uffs_StorageAttrSt *attr)
{
    osel_memset(attr, 0, sizeof(struct uffs_StorageAttrSt));
    // setup NAND flash attributes.
    attr->total_blocks      = FS_TOTAL_BLOCK;       /* total blocks */
    attr->page_data_size    = FS_PAGE_DATA_SIZE;    /* page datag_my_nand_ops size */
    attr->pages_per_block   = FS_PAGE_PER_BLOCK;    /* pages per block */
    attr->spare_size        = FS_PAGE_SPARE_SIZE;   /* page spare size */
    attr->block_status_offs = 0;                    /* block status offset is 5th byte in spare */
    attr->ecc_opt           = UFFS_ECC_NONE;        /* ecc option */
    attr->layout_opt        = UFFS_LAYOUT_UFFS;     /* let UFFS do the spare layout */
}

static URET device_init(uffs_Device *dev)
{
    dev->attr           = &g_femu_storage;          // NAND flash attributes
    dev->attr->_private = (void *) &g_nand_chip;    // hook nand_chip data structure to attr->_private
    dev->ops            = &nand_ops;                // NAND driver
    EXFLASH_DRIVER.init();
    return U_SUCC;
}

static URET device_release(uffs_Device *dev)
{
    return U_SUCC;
}

static int32_t uffs_fs_init(void)
{
    uffs_MountTable *mtbl = &(g_mount_table[0]);
    setup_storage(&g_femu_storage);                          /* setup nand storage attributes */
    uffs_MemSetupStaticAllocator(&mtbl->dev->mem, uffs_pools, sizeof(uffs_pools));  /* setup memory allocator */
    while (mtbl->dev)
    {
        // setup device init/release entry
        mtbl->dev->Init = device_init;
        mtbl->dev->Release = device_release;
        uffs_RegisterMountTable(mtbl);
        mtbl++;
    }

    for (mtbl = &(g_mount_table[0]); mtbl->mount != NULL; mtbl++)
    {
        // mount partitions
        if (uffs_Mount(mtbl->mount) == U_FAIL)
        {
            return U_FAIL;
        }
    }
    return uffs_InitFileSystemObjects() == U_SUCC ? 0 : -1;
}

static bool_t fat_uffs_init(void)
{
    char_t *version = uffs_Version2Str(UFFS_VERSION);
    DBG_LOG(DBG_LEVEL_INFO, "%s\r\n", version);
    if (U_FAIL == uffs_SetupDebugOutput())
    {
        DBG_LOG(DBG_LEVEL_ERROR, "[uffs_SetupDebugOutput] error\r\n");
        return FALSE;
    }
    if (U_FAIL == uffs_fs_init())
    {
        DBG_LOG(DBG_LEVEL_ERROR, "[init_filesystem] error\r\n");
        return FALSE;
    }
    return TRUE;
}


static bool_t fat_uffs_destory(void)
{
    uffs_MountTable *mtb;
    u32 ret = FALSE;
    // unmount parttions
    for (mtb = &(g_mount_table[0]); ret == 0 && mtb->mount != NULL; mtb++)
    {
        ret = uffs_UnMount(mtb->mount);
    }
    // release objects
    if (ret == 0)
        ret = (uffs_ReleaseFileSystemObjects() == U_SUCC ? TRUE : FALSE);
    return ret;
}


static bool_t fat_uffs_create(char_t *path)
{
    int16_t fd = -1;
    char_t buf[255];
    char_t *sub;
    uffs_DIR *dirp;
    struct uffs_dirent *ent;
    uffs_MountTable *mtbl = &(g_mount_table[0]);
    uffs_flush_all(mtbl->mount);
    dirp = uffs_opendir(mtbl->mount);
    if (dirp == NULL)
    {
        return FALSE;
    }
    ent = uffs_readdir(dirp);
    while (ent)
    {
        strcpy(buf, mtbl->mount);
        sub = buf;
        if (mtbl->mount[strlen(mtbl->mount) - 1] != '/')
            sub = strcat(buf, "/");
        sub = strcat(sub, ent->d_name);
        if (osel_memcmp(sub, path, strlen(path)))
        {
            uffs_closedir(dirp);
            return TRUE;
        }
        ent = uffs_readdir(dirp);
    }
    uffs_closedir(dirp);
    fd = uffs_open(path, UO_CREATE);
    if (fd > 0)
    {
        uffs_close(fd);
        uffs_flush_all(mtbl->mount);
        return TRUE;
    }
    uffs_close(fd);
    return FALSE;
}

static int16_t fat_uffs_open(char_t *path, uint16_t mode)
{
    int16_t fd = -1;
    fd = uffs_open(path, mode);
    return fd;
}

static void fat_uffs_close(int16_t fd)
{
    uffs_flush(fd);
    uffs_close(fd);
}

static uint16_t fat_uffs_read(int16_t fd, uint8_t *buf, uint16_t len)
{
    if (fd < 0)return 0;
    char_t *tmp = (char_t *) buf;
    uint16_t ret = 0;
    ret = uffs_read(fd, tmp, len);
    return ret;
}

static uint16_t fat_uffs_write(int16_t fd, uint8_t *buf, uint16_t len)
{
    if (fd < 0)return 0;
    uint16_t ret = 0;
    ret = uffs_write(fd, buf, len);
    return ret;
}

static uint32_t fat_uffs_seek(int16_t fd, uint32_t offset)
{
    uint16_t ret = 0;
    if (fd < 0)return ret;
    ret = uffs_seek(fd, offset, USEEK_SET);
    return ret;
}

static void fat_uffs_format(void)
{
    URET ret;
    UBOOL force = U_FALSE;
    uffs_Device *dev;
    uffs_MountTable *mtbl = &(g_mount_table[0]);
    dev = uffs_GetDeviceFromMountPoint(mtbl->mount);
    if (dev == NULL)
    {
        for (int i = 0; i < FS_TOTAL_BLOCK; i++)
        {
            nand_block_erase(NULL, i);
        }
        uffs_PutDevice(dev);
        return;
    }
    ret = uffs_FormatDevice(dev, force);
    if (ret != U_SUCC)
    {
        for (int i = 0; i < FS_TOTAL_BLOCK; i++)
        {
            nand_block_erase(NULL, i);
        }
    }
    uffs_PutDevice(dev);
}


static uint32_t fat_uffs_control(fat_cmd_t cmd, uint8_t *param)
{
    switch (cmd)
    {
    case FAT_CMD_FORMAT:
    {
        fat_uffs_format();
        break;
    }

    case FAT_CMD_READ_END:
        return uffs_eof(*(uint16_t *)param);

    case FAT_CMD_FILE_SIZE:
    {
        struct uffs_stat stat_buf;
        uffs_stat((char *)param, &stat_buf);
        return stat_buf.st_size;
    }
    
    case FAT_CMD_FILE_DEL:
        return ((uffs_remove((const char *)param) == 0) ? 0 : 1);

    case FAT_CMD_SPACE_FREE:
    {
        uffs_Device *dev;
        uffs_MountTable *mtbl = &(g_mount_table[0]);
        dev = uffs_GetDeviceFromMountPoint(mtbl->mount);
        return uffs_GetDeviceFree(dev);
    }

    case FAT_CMD_SPACE_TOTAL:
    {
        uffs_Device *dev;
        uffs_MountTable *mtbl = &(g_mount_table[0]);
        dev = uffs_GetDeviceFromMountPoint(mtbl->mount);
        return uffs_GetDeviceTotal(dev);
    }

    default:
        break;
    }
    return 0;
}


const struct fat_driver fat_uffs =
{
    fat_uffs_init,
    fat_uffs_destory,

    fat_uffs_create,
    fat_uffs_open,
    fat_uffs_close,

    fat_uffs_write,
    fat_uffs_read,
    fat_uffs_seek,
    fat_uffs_control,
};




