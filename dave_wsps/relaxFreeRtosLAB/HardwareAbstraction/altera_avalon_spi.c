
#include "altera_avalon_spi.h"


DIGITAL_IO_t * pslaves[6] = {&PLL_CS, &DAC_CS, &FLASH_CS, &ADC1_CS, &ADC2_CS, &ADC3_CS};

int alt_avalon_spi_transfer(const void* pbase, alt_u32 slave,
                           alt_u32 write_length, const alt_u8 * write_data,
                           alt_u32 read_length, alt_u8 * read_data,
                           alt_u32 flags)
{
	DIGITAL_IO_SetOutputLow(pslaves[slave]);//DIGITAL_IO_SetOutputLow(&PLL_CS);

	SPI_MASTER_Transfer((const SPI_MASTER_t *)pbase,
			write_data,
			read_data,
			read_length
			);

	if((flags & ALT_AVALON_SPI_COMMAND_MERGE) == 0)
		DIGITAL_IO_SetOutputHigh(pslaves[slave]);
}

int alt_avalon_spi_command(const void* pbase, alt_u32 slave,
                           alt_u32 write_length, const alt_u8 * write_data,
                           alt_u32 read_length, alt_u8 * read_data,
                           alt_u32 flags)
{
	//if((flags & ALT_AVALON_SPI_COMMAND_MERGE) != 0)
	DIGITAL_IO_SetOutputLow(pslaves[slave]);//DIGITAL_IO_SetOutputLow(&PLL_CS);

	if(write_length != 0)
	{
		SPI_MASTER_Transmit((const SPI_MASTER_t *)pbase, write_data, write_length);
		while( ((SPI_MASTER_t *)pbase)->runtime->tx_busy )
		{

		}
		/* Ensure the last byte is shifted out from the buffer at lower baud rates, when frame end mode is disabled. */
		while (SPI_MASTER_GetFlagStatus((SPI_MASTER_t *)pbase, (uint32_t)XMC_SPI_CH_STATUS_FLAG_MSLS) != 0U)
		{

		}
	}
	if(read_length != 0)
	{
		SPI_MASTER_Receive((const SPI_MASTER_t *)pbase, read_data, read_length);
		while( ((SPI_MASTER_t *)pbase)->runtime->rx_busy )
		{

		}

		/* Ensure the last byte is shifted out from the buffer at lower baud rates, when frame end mode is disabled. */
		while (SPI_MASTER_GetFlagStatus((SPI_MASTER_t *)pbase, (uint32_t)XMC_SPI_CH_STATUS_FLAG_MSLS) != 0U)
		{

		}
	}

	if((flags & ALT_AVALON_SPI_COMMAND_MERGE) == 0)
		DIGITAL_IO_SetOutputHigh(pslaves[slave]);

}
