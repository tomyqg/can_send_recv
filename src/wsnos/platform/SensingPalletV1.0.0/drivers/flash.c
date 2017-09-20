#include "common/lib/lib.h"
#include "driver.h"

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
/*Variable used to handle the Options Bytes*/
static FLASH_OBProgramInitTypeDef OptionsBytesStruct;

#define FLASH_PAGE_TO_BE_PROTECTED (OB_WRP1_PAGES32TO47 | OB_WRP1_PAGES48TO63)

void flash_write(uint8_t *flash_ptr,
                 const uint8_t *buffer,
                 uint16_t len)
{
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();
    /* Get pages write protection status ****************************************/
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

    /* Check if desired pages are already write protected ***********************/
    if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == FLASH_PAGE_TO_BE_PROTECTED)
    {
        /* Restore write protected pages */
        OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
        OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
        OptionsBytesStruct.WRPSector0To31 = FLASH_PAGE_TO_BE_PROTECTED;
        if (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }

        /* Generate System Reset to load the new option byte values ***************/
        HAL_FLASH_OB_Launch();
    }

    /* Lock the Options Bytes *************************************************/
    HAL_FLASH_OB_Lock();

    if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == 0x00)
    {
        ;
    }

    uint8_t *address = (uint8_t *)flash_ptr;
    uint8_t *tmp_buffer = (uint8_t *)buffer;
    for (uint16_t i = 0; i < len; i += 4)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)address, *(uint64_t *)tmp_buffer);
        tmp_buffer += 4;
        address += 4;
    }

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
}

void flash_read(const uint8_t *flash_ptr,
                uint8_t *buffer,
                uint16_t len)
{
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();
    /* Get pages write protection status ****************************************/
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

    /* Check if desired pages are already write protected ***********************/
    if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == FLASH_PAGE_TO_BE_PROTECTED)
    {
        /* Restore write protected pages */
        OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
        OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
        OptionsBytesStruct.WRPSector0To31 = FLASH_PAGE_TO_BE_PROTECTED;
        if (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }

        /* Generate System Reset to load the new option byte values ***************/
        HAL_FLASH_OB_Launch();
    }

    /* Lock the Options Bytes *************************************************/
    HAL_FLASH_OB_Lock();

    if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == 0x00)
    {
        ;
    }

    uint8_t *address = (uint8_t *)flash_ptr;
    uint8_t *tmp_buffer = (uint8_t *)buffer;
    FLASH_ProcessTypeDef pFlash;
    for (uint16_t i = 0; i < len; i ++)
    {
        /*Program word (32-bit) at a specified address.*/
        *(uint8_t *)tmp_buffer = *(__IO uint8_t *)address;
        tmp_buffer++;
        address++;
    }

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
}

/**
 * 擦除flash段或区的数据(segment or bank)
 *
 * @param *flash_ptr: 将被擦除的flash区的首地址
 * @param erase_type: 选择擦除区/段
 *
 * @return: 空
 */
void flash_erase(uint8_t *const flash_ptr,
                 uint8_t erase_type)
{
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();
    /* Get pages write protection status ****************************************/
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

    /* Check if desired pages are already write protected ***********************/
    if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == FLASH_PAGE_TO_BE_PROTECTED)
    {
        /* Restore write protected pages */
        OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
        OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
        OptionsBytesStruct.WRPSector0To31 = FLASH_PAGE_TO_BE_PROTECTED;
        if (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
        {
            DBG_ASSERT(FALSE __DBG_LINE);
        }

        /* Generate System Reset to load the new option byte values ***************/
        HAL_FLASH_OB_Launch();
    }

    /* Lock the Options Bytes *************************************************/
    HAL_FLASH_OB_Lock();


    if ((OptionsBytesStruct.WRPSector0To31 & FLASH_PAGE_TO_BE_PROTECTED) == 0x00)
    {
        /* Fill EraseInit structure************************************************/
        EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.PageAddress = ((uint32_t)flash_ptr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
        EraseInitStruct.NbPages     = 1;
        
        uint32_t PageError = 0;
        if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
        {
            /*
              Error occurred while page erase.
              User can add here some code to deal with this error.
              PageError will contain the faulty page and then to know the code error on this page,
              user can call function 'HAL_FLASH_GetError()'
            */
            DBG_ASSERT(FALSE __DBG_LINE);
        }
    }

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
}