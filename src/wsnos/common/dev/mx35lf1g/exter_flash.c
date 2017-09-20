/***************************************************************************
* File        : exter_flash.c
* Summary     :
* Version     : v0.1
* Author      : wangjifang
* Date        : 2016/5/5
* Change Logs :
* Date             Versi on         Author           Notes
* 2016/5/5         v0.1            wangjifang        first version
***************************************************************************/
#include "gz_sdk.h"
#include "exter_flash.h"

DBG_THIS_MODULE("exter_flash");

osel_device_t mx35lf1g_device;
static flash_page_t f_page;

static void flash_read_id(flash_id_t *flash_id)
{
    if (flash_id == NULL)
    {
        DBG_LOG(DBG_LEVEL_ORIGIN, "flash read id fail\r\n");
        return;
    }
    uint16_t temp1 = 0;
    uint16_t temp2 = 0;
    
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_READ_ID);
    mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    flash_id->mfr_id = mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    temp1 = mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    temp2 = mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    flash_id->device_id = temp1 << 8 | temp2;
    MX35LF1G_SPI_END();
}


uint8_t flash_read_status_reg(uint8_t reg_addr)
{
    if ((reg_addr != MX35LF1G_REG_STATE_1)
        && (reg_addr != MX35LF1G_REG_STATE_2)
            && (reg_addr != MX35LF1G_REG_STATE_3))
    {
        DBG_LOG(DBG_LEVEL_ORIGIN, "flash read states reg fail\r\n");
        return 0;
    }
    uint8_t byte = 0;
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_GET_FEATURE);
    mx35lf1g_spi_write_read(reg_addr);
    byte = mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    MX35LF1G_SPI_END();
    
    return byte;
}

static void flash_write_status_reg(uint8_t reg_addr, uint8_t byte)
{
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_SET_FEATURE);
    mx35lf1g_spi_write_read(reg_addr);
    mx35lf1g_spi_write_read(byte);
    MX35LF1G_SPI_END();
}

static void flash_write_enable(void)
{
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_WRITE_ENABLE);
    MX35LF1G_SPI_END();
}

void flash_write_disable(void)
{
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_WRITE_DISABLE);
    MX35LF1G_SPI_END();
}

void flash_wait_busy(void)
{
    WHILE ((flash_read_status_reg(MX35LF1G_REG_STATE_3) & 0x01) == 1, 15000, FALSE);
}

static void flash_data_transf(flash_page_t *page)
{
    uint32_t addr = page->block * FS_PAGE_PER_BLOCK + page->page;
    uint8_t add_h = (uint8_t)((addr & 0xff00) >> 8);
    uint8_t add_l = (uint8_t)(addr & 0x00ff);
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_PROGRAM_EXECUTE);
    mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    mx35lf1g_spi_write_read(add_h);
    mx35lf1g_spi_write_read(add_l);
    MX35LF1G_SPI_END();
}

static void flash_data_read_from_cache(uint16_t offset, uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0 || len > 2048)
    {
        return;
    }
    
    uint8_t offset_h = (uint8_t)((offset & 0xff00) >> 8);
    uint8_t offset_l = (uint8_t)(offset & 0x00ff);
    
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_READ_FROM_CACHE);
    mx35lf1g_spi_write_read(offset_h);
    mx35lf1g_spi_write_read(offset_l);
    mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    for (uint16_t i = 0; i < len; i++)
    {
        buf[i] = mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    }
    MX35LF1G_SPI_END();
}

static osel_uint32_t mx35lf1g_driver_write(osel_device_t *dev, osel_uint32_t pos, const void *buffer, osel_uint32_t size)
{
    if (buffer == NULL || size == 0 || size > 2048)
    {
        return 0;
    }
    hal_wdt_clear(16000);
    wdt_extern_clear();
    
    uint8_t offset_h = (uint8_t)((f_page.offset & 0xff00) >> 8);
    uint8_t offset_l = (uint8_t)(f_page.offset & 0x00ff);
    
    uint16_t i = 0;
    flash_write_enable();
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_PROGRAM_LOAD);
    mx35lf1g_spi_write_read(offset_h);  //*< page addr
    mx35lf1g_spi_write_read(offset_l);
    for (i = 0; i < size; i++)
    {
        mx35lf1g_spi_write_read(((uint8_t *)buffer)[i]);
    }
    MX35LF1G_SPI_END();
    flash_data_transf(&f_page);
    
    flash_wait_busy();
    flash_read_status_reg(MX35LF1G_REG_STATE_3);
    
    flash_write_disable();
    return size;
}

static osel_uint32_t mx35lf1g_driver_read(osel_device_t *dev, osel_uint32_t pos, void *buffer, osel_uint32_t size)
{
    if (buffer == NULL || size == 0 || size > 2048)
    {
        return 0;
    }
    hal_wdt_clear(16000);
    wdt_extern_clear();
    
    uint32_t addr = f_page.block * FS_PAGE_PER_BLOCK + f_page.page;
    uint8_t add_h = (uint8_t)((addr & 0xff00) >> 8);
    uint8_t add_l = (uint8_t)(addr & 0x00ff);
    
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_PAGE_READ);
    mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    mx35lf1g_spi_write_read(add_h);
    mx35lf1g_spi_write_read(add_l);
    MX35LF1G_SPI_END();
    
    //等待数据从flash传输到缓存BUF
    flash_wait_busy();
    flash_data_read_from_cache(f_page.offset, buffer, size);
    return size;
}

static int8_t flash_block_erase(uint32_t block)
{
    flash_write_enable();
    
    hal_wdt_clear(16000);
    wdt_extern_clear();
    
    uint32_t addr = block * FS_PAGE_PER_BLOCK + 0;
    uint8_t add_h = (uint8_t)((addr & 0xff00) >> 8);
    uint8_t add_l = (uint8_t)(addr & 0x00ff);
    
    MX35LF1G_SPI_BEGIN();
    mx35lf1g_spi_write_read(MX35LF1G_CMD_BLOCK_ERASE);
    mx35lf1g_spi_write_read(MX35LF1G_CMD_DUMMY);
    mx35lf1g_spi_write_read(add_h);
    mx35lf1g_spi_write_read(add_l);
    MX35LF1G_SPI_END();
    
    flash_wait_busy();
    
    flash_write_disable();
    
    return flash_read_status_reg(MX35LF1G_REG_STATE_3);
}


/********************************************************************************
**Routine:      mx35lf1g_driver_control
**Description:  flash设备控制
**Notes:
********************************************************************************/
static osel_uint32_t mx35lf1g_driver_control(osel_device_t *dev, osel_uint8_t cmd, void *arg)
{
    switch (cmd)
    {
    case FLASH_CTL_ID:
        {
            flash_id_t flash_id;
            osel_memset((uint8_t *)&flash_id, 0, sizeof(flash_id_t));
            flash_read_id(&flash_id);
            osel_memcpy(arg, (uint8_t *)&flash_id, sizeof(flash_id_t));
            break;
        }
        
    case FLASH_CTL_ERASE:
        {
            int8_t res = flash_block_erase(*(uint32_t *)arg);
            osel_memcpy(arg, (uint8_t *)&res, sizeof(int8_t));
        }
        break;
        
    case FLASH_W_PAGE_ADDR:
        {
            DBG_ASSERT(NULL != arg __DBG_LINE);
            osel_memcpy((uint8_t *)&f_page, arg, sizeof(flash_page_t));
            uint32_t addr = f_page.block * FS_PAGE_PER_BLOCK + f_page.page;
            uint8_t add_h = (uint8_t)((addr & 0xff00) >> 8);
            uint8_t add_l = (uint8_t)(addr & 0x00ff);
            mx35lf1g_spi_write_read(add_h);
            mx35lf1g_spi_write_read(add_l);
            break;
        }
        
    case FLASH_W_COL_ADDR:
        {
            nand_chip_t n_chip;
            DBG_ASSERT(NULL != arg __DBG_LINE);
            osel_memcpy((uint8_t *)&n_chip, arg, sizeof(nand_chip_t));
            uint32_t addr = n_chip.col_addr;
            uint8_t add_h = (uint8_t)((addr & 0xff00) >> 8);
            uint8_t add_l = (uint8_t)(addr & 0x00ff);
            mx35lf1g_spi_write_read(add_h);
            mx35lf1g_spi_write_read(add_l);
            break;
        }
        
        
    default:
        break;
    }
}

static osel_int8_t mx35lf1g_driver_open(osel_device_t *dev, osel_int16_t oflag)
{
    //*< start mx35lf1g
    return 0;
}


static osel_int8_t mx35lf1g_driver_close(osel_device_t *dev)
{
    //*< make mx35lf1g in sleep mode
    return 0;
}

/********************************************************************************
**Routine:      exter_flash_init
**Description:  初始化
**Notes:
********************************************************************************/
static osel_int8_t mx35lf1g_driver_init(osel_device_t *dev)
{
    mx35lf1g_spi_init();
    MX35LF1G_HOLD_HIGT();
    
    uint8_t temp1 = flash_read_status_reg(MX35LF1G_REG_STATE_1);
    uint8_t temp2 = flash_read_status_reg(MX35LF1G_REG_STATE_2);
    
    temp1 &= ~MX35LF1G_REG_PROTECT_SECTION_UNLOCK;                                     //解锁保护区
    temp2 |=  MX35LF1G_REG_ECC + MX35LF1G_REG_BUF;                                     //选用BUF1，ECC1模式
    
    flash_write_status_reg(MX35LF1G_REG_STATE_1, temp1);
    flash_write_status_reg(MX35LF1G_REG_STATE_2, temp2);
    return 0;
}

void mx35lf1g_device_init(void)
{
    mx35lf1g_device.init = mx35lf1g_driver_init;
    mx35lf1g_device.read = mx35lf1g_driver_read;
    mx35lf1g_device.write = mx35lf1g_driver_write;
    mx35lf1g_device.control = mx35lf1g_driver_control;
    mx35lf1g_device.open = mx35lf1g_driver_open;
    mx35lf1g_device.close = mx35lf1g_driver_close;
    
    osel_device_register(&mx35lf1g_device, EXFLASH_NAME, OSEL_DEVICE_FLAG_RDONLY);
}
