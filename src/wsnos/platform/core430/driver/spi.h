/**
 * @brief       : this
 * @file        : spi.h
 * @version     : v0.0.1
 * @author      : gang.cheng
 * @date        : 2017-04-24
 * change logs  :
 * Date       Version     Author        Note
 * 2017-04-24  v0.0.1  gang.cheng    first version
 */
#ifndef __SPI_H__
#define __SPI_H__

#include "common/lib/data_type_def.h"

typedef enum {
    SPI1,
    SPI2
} spi_enum;

typedef struct spi_s
{
    uint8_t  Spi;
    pin_id_t Mosi;
    pin_id_t Miso;
    pin_id_t Sclk;
    pin_id_t Nss;
} Spi_t;

void spi_com_set(Spi_t *spi, uint8_t com);
/*!
 * \brief Initializes the SPI object and MCU peripheral
 *
 * \remark When NSS pin is software controlled set the pin name to NC otherwise
 *         set the pin name to be used.
 *
 * \param [IN] spi  SPI object
 * \param [IN] mosi SPI MOSI pin name to be used
 * \param [IN] miso SPI MISO pin name to be used
 * \param [IN] sclk SPI SCLK pin name to be used
 * \param [IN] nss  SPI NSS pin name to be used
 */
void spi_init(Spi_t *spi,
              pin_name_e mosi,
              pin_name_e miso,
              pin_name_e sclk,
              pin_name_e nss);

void spi_disable(Spi_t *spi);
void spi_enable(Spi_t *spi);

void spi_deinit(Spi_t *spi);

/*!
 * \brief Configures the SPI peripheral
 *
 * \remark Slave mode isn't currently handled
 *
 * \param [IN] obj   SPI object
 * \param [IN] bits  Number of bits to be used. [8 or 16]
 * \param [IN] cpol  Clock polarity
 * \param [IN] cpha  Clock phase
 * \param [IN] slave When set the peripheral acts in slave mode
 */
void spi_format(Spi_t *spi, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave);

/*!
 * \brief Sets the SPI speed
 *
 * \param [IN] obj SPI object
 * \param [IN] hz  SPI clock frequency in hz
 */
void spi_frequency( Spi_t *spi, uint32_t hz );

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] spi     SPI object
 * \param [IN] outData Byte to be sent
 * \retval inData      Received byte.
 */
uint16_t spi_inout( Spi_t *spi, uint16_t out_data );



#endif