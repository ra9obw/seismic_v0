#ifndef __ALTERA_AVALON_SPI_H_
#define __ALTERA_AVALON_SPI_H_

#include "alt_types.h"
#include "DAVE.h"

#define SPI_0_BASE			(&SPI1)
#define SPI_1_BASE			(&SPI2)
#define SPI_2_BASE			(&SPI3)
#define SPI_3_BASE			(&SPI4)

#define PLL_SLAVE_SELECT	(0)
#define DAC_SLAVE_SELECT 	(1)
#define FLASH_SLAVE_SELECT 	(2)
#define ADC1_SLAVE_SELECT	(3)
#define ADC2_SLAVE_SELECT	(4)
#define ADC3_SLAVE_SELECT	(5)

#define ALT_AVALON_SPI_COMMAND_MERGE (0x01)


int alt_avalon_spi_command(const void* pbase, alt_u32 slave,
                           alt_u32 write_length, const alt_u8 * write_data,
                           alt_u32 read_length, alt_u8 * read_data,
                           alt_u32 flags);

#endif
