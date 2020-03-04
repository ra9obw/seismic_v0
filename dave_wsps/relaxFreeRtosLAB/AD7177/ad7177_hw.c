/*
 * ad7177_hw.c
 *
 *  Created on: 16.02.2018
 *      Author: Pavlenko
 */

#include "ad7177_hw.h"
#include "system.h"
#include "altera_avalon_spi.h"
#include "global_func.h"

int read_ADC_status(adc_inst_handler_type *padc)
{
	int ret = 0;
	//TODO!!!
	alt_u8 rd_tmp[4];
	alt_u8 wr_tmp[1];
	alt_u16 adc_id;

	wr_tmp[0] = 0x40 | 0x07;//Read ADC ID
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, 0);
	swap_bytes(rd_tmp, 2);
	padc->adc_regs.id = *(alt_u16*)rd_tmp;
//	alt_printf("ADC ID:\t0x%x\t", *(alt_u16*)rd_tmp);

	wr_tmp[0] = 0x40 | 0x00;//Read ADC Status
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 1, rd_tmp, 0);
	padc->adc_regs.status = rd_tmp[0];
//	alt_printf("Status:\t0x%x\t", rd_tmp[0]);
//
	wr_tmp[0] = 0x40 | 0x01;//ADC Mode Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, 0);
	swap_bytes(rd_tmp, 2);
	padc->adc_regs.adcmode = *(alt_u16*)rd_tmp;
//	alt_printf("Mode reg:\t0x%x\t", *(alt_u16*)rd_tmp);
//
	wr_tmp[0] = 0x40 | 0x02;//Interface Mode Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, 0);
	swap_bytes(rd_tmp, 2);
	padc->adc_regs.ifmode = *(alt_u16*)rd_tmp;
//	alt_printf("Interface reg:\t0x%x\t", *(alt_u16*)rd_tmp);
//
	wr_tmp[0] = 0x40 | 0x06;//GPIO Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, 0);
	swap_bytes(rd_tmp, 2);
	padc->adc_regs.gpiocon = *(alt_u16*)rd_tmp;
//	alt_printf("GPIO reg:\t0x%x\t", *(alt_u16*)rd_tmp);
//
	wr_tmp[0] = 0x40 | 0x10;//Channel 0 Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, 0);
	swap_bytes(rd_tmp, 2);
	padc->adc_regs.ch0 = *(alt_u16*)rd_tmp;
//	alt_printf("Ch0:\t0x%x\t", *(alt_u16*)rd_tmp);
//
	wr_tmp[0] = 0x40 | 0x20;//Setup 0 Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, 0);
	swap_bytes(rd_tmp, 2);
	padc->adc_regs.setup0 = *(alt_u16*)rd_tmp;
//	alt_printf("Setup0:\t0x%x\t", *(alt_u16*)rd_tmp);
//
	wr_tmp[0] = 0x40 | 0x28;//Filter 0 Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, 0);
	swap_bytes(rd_tmp, 2);
	padc->adc_regs.filt0 = *(alt_u16*)rd_tmp;
//	alt_printf("Filter0:\t0x%x\t", *(alt_u16*)rd_tmp);
//
	wr_tmp[0] = 0x40 | 0x30;//Offset 0 Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 3, rd_tmp, 0);
	swap_bytes(rd_tmp, 3);
	rd_tmp[3]=0;
	padc->adc_regs.offs0 = *(alt_u32*)rd_tmp;
//	alt_printf("Offset0:\t0x%x\t", *(alt_u16*)rd_tmp);
//
	wr_tmp[0] = 0x40 | 0x38;//Gain 0 Reg
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 3, rd_tmp, 0);
	swap_bytes(rd_tmp, 3);
	rd_tmp[3]=0;
	padc->adc_regs.gain0 = *(alt_u32*)rd_tmp;
//	alt_printf("Gain0:\t0x%x\n", *(alt_u16*)rd_tmp);
	return ret;
}

void spi_reset_ADC(adc_inst_handler_type *padc)
{
	//TODO!!!
	alt_u8 wr_tmp[10];
	alt_u8 rd_tmp[3];

	memset(wr_tmp, 0xFF, 10);
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 10, wr_tmp, 0, rd_tmp, 0);

}

void configure_ADC(adc_inst_handler_type *padc)
{
	alt_u8 rd_tmp[3];
	alt_u8 wr_tmp[3];
	alt_u16 config_word = FILTCON_ENHFILT_20SPS | FILTCON_ODR_1KSPS;//FILTCON_ODR_100SPS FILTCON_ODR_10KSPS FILTCON_ODR_1KSPS

	wr_tmp[0] = AD7177_SPI_WR | AD7177_FILTCON0;//0x514
	wr_tmp[1] = ((config_word >> 8) & 0xFF);
	wr_tmp[2] = (config_word & 0xFF);
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 3, wr_tmp, 0, rd_tmp, 0);

	wr_tmp[0] = AD7177_SPI_WR | AD7177_CH0;//
	config_word = CH_EN | CH_AINPOS_AIN0 | CH_AINNEG_AIN1/*| CH_AINPOS_AIN2 | CH_AINNEG_AIN3*/;
	wr_tmp[1] = ((config_word >> 8) & 0xFF);
	wr_tmp[2] = (config_word & 0xFF);
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 3, wr_tmp, 0, rd_tmp, 0);

	wr_tmp[0] = AD7177_SPI_WR | AD7177_SETUPCON0;//
	config_word = SETUPCON_OFFSET_BINARY_FORMAT | SETUPCON_AINBUF_P_EN | SETUPCON_AINBUF_N_EN | SETUPCON_REFBUF_P_EN | SETUPCON_REFBUF_N_EN | SETUPCON_REF_EXT; //0x1320
	wr_tmp[1] = ((config_word >> 8) & 0xFF);
	wr_tmp[2] = (config_word & 0xFF);
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 3, wr_tmp, 0, rd_tmp, 0);
//
	wr_tmp[0] = AD7177_SPI_WR | AD7177_ADCMODE;//
	config_word = ADCMODE_REF_EN | ADCMODE_CLOCK_EXT; //0x8000
	wr_tmp[1] = ((config_word >> 8) & 0xFF);
	wr_tmp[2] = (config_word & 0xFF);
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 3, wr_tmp, 0, rd_tmp, 0);

	wr_tmp[0] = AD7177_SPI_WR | AD7177_IFMODE;//
	config_word = IFMODE_WL32 | IFMODE_CONT_READ;
	wr_tmp[1] = ((config_word >> 8) & 0xFF);
	wr_tmp[2] = (config_word & 0xFF);
	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 3, wr_tmp, 0, rd_tmp, ALT_AVALON_SPI_COMMAND_MERGE);

//	wr_tmp[0] = 0x40 | 0x28;//Filter 0 Reg
//	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 2, rd_tmp, ALT_AVALON_SPI_COMMAND_MERGE);//alt_avalon_spi_command(base_addr, 0, 1, wr_tmp, 2, rd_tmp, ALT_AVALON_SPI_COMMAND_MERGE);
//
//	swap_bytes(rd_tmp, 2);
//	alt_printf("Config Filter0:\t0x%x\n", *(alt_u16*)rd_tmp);
	//TODO!!!
}

int make_ADC_sync(void)
{
	DIGITAL_IO_SetOutputLow(&ADC_SYNC);
	usleep(10);
	DIGITAL_IO_SetOutputHigh(&ADC_SYNC);
	usleep(10);
	return 0;
}

int make_ADC_reset(void)
{
	//TODO!!!
	return 0;
}
