/**
 * @brief       : this
 * @file        : flash.c
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2015-10-15
 * change logs  :
 * Date       Version     Author        Note
 * 2015-10-15  v0.0.1  gang.cheng    first version
 * 2016-04-12  v0.0.2  gang.cheng    implement flash read/write/erase
 */

#include "driver.h"
#include "sys_arch/osel_arch.h"
#include "common/lib/lib.h"
#include <stdlib.h>
#include <stdio.h>

DBG_THIS_MODULE("FLASH")

/**
 * @brief the file of flash stash data
 */
#define FLASH_ROM_FILE          \
    "flash.rom"

static pthread_mutex_t flash_rom_mutex = PTHREAD_MUTEX_INITIALIZER;

void flash_write(uint8_t *flash_ptr,
                 const uint8_t *buffer,
                 uint16_t len)
{
    FILE *fp = NULL;
    pthread_mutex_lock(&flash_rom_mutex);

    fp = fopen(FLASH_ROM_FILE, "r+");
    if (NULL != fp)
    {
        fseek(fp, (long int)flash_ptr, SEEK_SET);
        fwrite(buffer, sizeof(uint8_t), len, fp);
        fclose(fp);
    }
    else{
        fp = fopen(FLASH_ROM_FILE, "w+");
        if(NULL != fp)
            PRINTF("flash rom init\n");
        else
            PRINTF("flash wirte rom open failed\n");
    }

    pthread_mutex_unlock(&flash_rom_mutex);
}

void flash_read(const uint8_t *flash_ptr,
                uint8_t *buffer,
                uint16_t len)
{
    FILE *fp = NULL;
    pthread_mutex_lock(&flash_rom_mutex);

    fp = fopen(FLASH_ROM_FILE, "r");
    if (NULL != fp)
    {
        fseek(fp, (long int)flash_ptr, SEEK_SET);
       int size =  fread(buffer, sizeof(uint8_t), len, fp);
        fclose(fp);
    }
    else{
        fp = fopen(FLASH_ROM_FILE, "w");
        if(NULL != fp)
            PRINTF("flash rom init\n");
        else
            PRINTF("flash read rom open failed\n");
    }

    pthread_mutex_unlock(&flash_rom_mutex);
}

void flash_erase(uint8_t *const flash_ptr,
                 uint8_t erase_type)
{
    long int offset = (long int)flash_ptr;
    int size = 0;
    if(offset > INFO_A_ADDR)
        size = FLASH_PAGE_SIZE;
    else
        size = FLASH_INFO_SIZE;
    long int index = offset / size;

    FILE *fp = NULL;
    pthread_mutex_lock(&flash_rom_mutex);

    fp = fopen(FLASH_ROM_FILE, "r+");
    if (NULL != fp)
    {
        fseek(fp, index * size, SEEK_SET);
        for (uint16_t i = 0; i < size; i++)
            fputc(0xFF, fp);

        fclose(fp);
    }
    else{
        fp = fopen(FLASH_ROM_FILE, "w+");
        if(NULL != fp)
            PRINTF("flash rom init\n");
        else
            PRINTF("flash erase rom open failed\n");
    }

    pthread_mutex_unlock(&flash_rom_mutex);
}






