/*
 * dac1282_hw.c
 *
 *  Created on: 20.02.2018
 *      Author: Pavlenko
 */

#include "dac1282_hw.h"
#include "system.h"
#include "altera_avalon_spi.h"


void write_DAC_reg(alt_u16 addr, alt_u8 data)
{
	alt_u8 rd_tmp[9];
	alt_u8 wr_tmp[3];

	wr_tmp[0] = DAC_WR_CMD | addr; //WREG | ADDR
	wr_tmp[1] = 1; //Number of registers
	wr_tmp[2] = data;
	alt_avalon_spi_command(SPI_3_BASE, DAC_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);

}

void configure_DAC(void)
{
//==
	alt_u8 rd_tmp[9];
	alt_u8 wr_tmp[3];

	wr_tmp[0] = DAC_RD_CMD | DAC_GANMOD_REG; //RREG | ADDR
	wr_tmp[1] = 9; //Number of registers to read
	alt_avalon_spi_command(SPI_3_BASE, DAC_SLAVE_SELECT, 2, wr_tmp, 9, rd_tmp, 0);

	wr_tmp[0] = DAC_WR_CMD | DAC_GANMOD_REG; //WREG | ADDR
	wr_tmp[1] = 1; //Number of registers to read
	wr_tmp[2] = DAC_MODE_SIN | DAC_GAIN_1V25;
	alt_avalon_spi_command(SPI_3_BASE, DAC_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);

//	write_DAC_reg(DAC_GANMOD_REG, DAC_MODE_PULSE);
//	write_DAC_reg(DAC_PULSA_REG, 0x14);
//	write_DAC_reg(DAC_PULSB_REG, 0x0C);

	wr_tmp[0] = DAC_WR_CMD | DAC_SWN_REG; //WREG | ADDR
	wr_tmp[1] = 1; //Number of registers to read
	wr_tmp[2] = DAC_SW_DIFF;//DAC_SW_OPEN;//DAC_SW_DIFF;
	alt_avalon_spi_command(SPI_3_BASE, DAC_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);

	wr_tmp[0] = DAC_RD_CMD | DAC_GANMOD_REG; //RREG | ADDR
	wr_tmp[1] = 9; //Number of registers to read
	alt_avalon_spi_command(SPI_3_BASE, DAC_SLAVE_SELECT, 2, wr_tmp, 9, rd_tmp, 0);
//==
}
