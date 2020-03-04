/*
 * ad9548_hw.c
 *
 *  Created on: 10.02.2018
 *      Author: Pavlenko
 */


#include "ad9548_hw.h"
#include "altera_avalon_spi.h"
#include "system.h"
#include "AD9548.h"

#include "global_func.h"

//PLL Settings
//#define PLL_SLAVE_SELECT	(1)

#define PLL_RD_CMD	(0x8000)
#define PLL_WR_CMD	(0x0000)

#define PLL_1BYTE	(0x00 << 13)
#define PLL_2BYTE	(0x01 << 13)
#define PLL_3BYTE	(0x02 << 13)
#define PLL_STREAM	(0x03 << 13)

#define PLL_ADDR_MASK	(0x1FFF)

#define PLL_SPI_CTRL_REG				(0x0000)
#define PLL_SPI_CTRL_IO_UPDATE			(0x0005)
	#define PLL_SPI_UNIDIR					(0x80)
	#define PLL_SPI_LSB_FIRST				(0x40)
	#define PLL_SPI_SOFT_RST				(0x20)
	#define PLL_SPI_LONG_INSTR				(0x10)
#define PLL_SILICON_REV_REG				(0x0003)
#define PLL_DEV_ID_REG					(0x0004)
#define PLL_SYSCLK_0_REG				(0x0100)
#define PLL_SYSCLK_SYS_STAB_2_REG		(0x0108)
//0x0102
	#define SYSCLK_LF_SRC	(0x01)
	#define SYSCLK_PLL_ENA	(0x04)


#define PLL_STATUS_EEPROM				(0x0D00)
#define PLL_STATUS_SYSCLK				(0x0D01)

void pllConfigureSystemClock(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[11];
	alt_u8 wr_tmp[11];
	alt_u8  sys_clk_multi = 58; //928MHz
	alt_u32 sys_clk_in_fs = 1000000000/(58*16);//1077586; //

	cmd_word = PLL_WR_CMD | PLL_STREAM | PLL_SYSCLK_SYS_STAB_2_REG;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[10] = 0x18;
	wr_tmp[9] = sys_clk_multi;
	wr_tmp[8] = SYSCLK_LF_SRC | SYSCLK_PLL_ENA;
	wr_tmp[7] = (sys_clk_in_fs & 0xFF);
	wr_tmp[6] = ((sys_clk_in_fs >> 8) & 0xFF);
	wr_tmp[5] = ((sys_clk_in_fs >> 16) & 0x1F);
	wr_tmp[4] = 0x01;
	wr_tmp[3] = 0x00;
	wr_tmp[2] = 0x00;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 11, wr_tmp, 0, rd_tmp, 0);
}
void pllUpdate(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[3];

	cmd_word = PLL_WR_CMD | PLL_STREAM | AD9548_REG_IO_UPDATE;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[2] = 1;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);
}
void pllCalibrateSysClk(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[3];

	cmd_word = PLL_WR_CMD | PLL_1BYTE | AD9548_REG_CAL_SYNC;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[2] = 1;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);
}
void pllSyncDistribution(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[3];

	cmd_word = PLL_WR_CMD | PLL_1BYTE | AD9548_REG_CAL_SYNC;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[2] = 2;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);

	pllUpdate();

	cmd_word = PLL_WR_CMD | PLL_1BYTE | AD9548_REG_CAL_SYNC;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[2] = 0;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);

	pllUpdate();
}
alt_u8 pllGetSysClkStatus(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[2];

	cmd_word = PLL_RD_CMD | PLL_1BYTE | AD9548_REG_SYSTEM_CLOCK;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 2, wr_tmp, 1, rd_tmp, 0);

	return rd_tmp[0];

}

void pllConfigSpi_4Wire(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[3];

	cmd_word = PLL_WR_CMD | PLL_1BYTE | PLL_SPI_CTRL_REG;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[2] = PLL_SPI_UNIDIR | PLL_SPI_LONG_INSTR;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);
}

void pllUpdateFTW(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[3];

	cmd_word = PLL_WR_CMD | PLL_STREAM | AD9548_REG_UPDATE_TW;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[2] = 1;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);
}

void pllUploudFTW(alt_u32 high, alt_u32 low)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[8];

	cmd_word = PLL_WR_CMD | PLL_STREAM | AD9548_REG_TUNING_WORD_5;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	//
	wr_tmp[7] = ((low  >>  0) & 0xFF);
	wr_tmp[6] = ((low  >>  8) & 0xFF);
	wr_tmp[5] = ((low  >> 16) & 0xFF);
	wr_tmp[4] = ((low  >> 24) & 0xFF);
	wr_tmp[3] = ((high >>  0) & 0xFF);
	wr_tmp[2] = ((high >>  8) & 0xFF);

	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 8, wr_tmp, 0, rd_tmp, 0);
}
alt_u8 pllDDSstatus0(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[2];

	cmd_word = PLL_RD_CMD | PLL_1BYTE | AD9548_REG_DLL_STATUS_0;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 2, wr_tmp, 1, rd_tmp, 0);

	return rd_tmp[0];
}
alt_u8 pllDDSstatus1(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[2];

	cmd_word = PLL_RD_CMD | PLL_1BYTE | AD9548_REG_DLL_STATUS_1;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 2, wr_tmp, 1, rd_tmp, 0);

	return rd_tmp[0];
}
alt_u8 pllRefAstatus(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[2];

	cmd_word = PLL_RD_CMD | PLL_1BYTE | AD9548_REG_REF_A;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 2, wr_tmp, 1, rd_tmp, 0);

	return rd_tmp[0];
}

void pllSetReg(alt_u16 addr, alt_u8 data)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[3];

	cmd_word = PLL_WR_CMD | PLL_STREAM | addr;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	wr_tmp[2] = data;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 3, wr_tmp, 0, rd_tmp, 0);
}

alt_u8 pllGetReg(alt_u16 addr)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[1];
	alt_u8 wr_tmp[2];

	cmd_word = PLL_RD_CMD | PLL_1BYTE | addr;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 2, wr_tmp, 1, rd_tmp, 0);

	return rd_tmp[1];
}

void pllConfigFreeRunningDDS(void)
{
	alt_u8 pll_dds_status;

	pll_dds_status = pllDDSstatus0();
//	printf("DDS Status:\t0x%x\n", pll_dds_status);

	pllUploudFTW(0x234F, 0x72C234F7); //128MHz
//	pllUploudFTW(0x1E58, 0x469EE584); //110MHz

	pllUpdateFTW();

	pllSetReg(AD9548_REG_LOOP_MODE, 0x20);//Free RUN Mode

	pllUpdate();

	pll_dds_status = pllDDSstatus0();
//	printf("DDS Status:\t0x%x\n", pll_dds_status);

	pllSetReg(AD9548_REG_DISTRIBUTION_CHANNEL_MODES_2 , 0x0A);
	pllSetReg(AD9548_REG_DISTRIBUTION_CHANNEL_MODES_3 , 0x0A);
	pllSetReg(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_8, 31); //Set OUT2_p to 4MHz
	pllSetReg(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_12, 7); //Set OUT3_P to 16MHz
//	pllSetReg(AD9548_REG_DISTRIBUTION_CHANNEL_DIVIDERS_12, 127); //Set OUT3_P to 1MHz

	pllSetReg(AD9548_REG_DISTRIBUTION_SETTINGS, 0x00);
	pllSetReg(AD9548_REG_DISTRIBUTION_ENABLE, 0x0C);

	pllSyncDistribution();

	pllUpdate();
}

int pll_read_ID(alt_u8 *pid)
{
	//TODO

	alt_u16 cmd_word;
	alt_u8 rd_tmp[11];
	alt_u8 wr_tmp[11];

	cmd_word = PLL_RD_CMD | PLL_STREAM | AD9548_REG_IO_UPDATE;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;
	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 2, wr_tmp, 6, rd_tmp, 0);

	pid[0] = rd_tmp[2];
	pid[1] = rd_tmp[3];
	//	printf("PLL ID:\t0x%x\t0x%x\n", rd_tmp[2], rd_tmp[3]);
	return 0;
}

int pllConfigureProfiler0(void)
{
	alt_u16 cmd_word;
	alt_u8 rd_tmp[50];
	alt_u8 wr_tmp[52];

	//Configure Profile 0
	cmd_word = PLL_WR_CMD | PLL_STREAM | AD9548_REG_PROFILE_0_FREQ_LOCK_DETECTORS_8;
	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
	wr_tmp[1] = cmd_word & 0xFF;

	wr_tmp[51] = 0x09;
	//Nominal Period in [fs]
	wr_tmp[50] = 0x00;
	wr_tmp[49] = 0x80;
	wr_tmp[48] = 0xC6;
	wr_tmp[47] = 0xA4;
	wr_tmp[46] = 0x7E;
	wr_tmp[45] = 0x8D;
	wr_tmp[44] = 0x03;
	//Inner Tolerance
	wr_tmp[43] = 0x10;//wr_tmp[43] = 0xE8;
	wr_tmp[42] = 0x27;//wr_tmp[42] = 0x03;
	wr_tmp[41] = 0x00;//wr_tmp[41] = 0x00;
	//Outer tolerance
	wr_tmp[40] = 0xE8;
	wr_tmp[39] = 0x03;
	wr_tmp[38] = 0x00;
	//Validation Timer
	wr_tmp[37] = 0xE8;
	wr_tmp[36] = 0x03;
	//Redetect Timer
	wr_tmp[35] = 0x88;
	wr_tmp[34] = 0x13;
	//Alpha-0//Alpha-0
	wr_tmp[33] = 0x47;//wr_tmp[33] = 0x58;
	wr_tmp[32] = 0xE0;//wr_tmp[32] = 0x89;
	//Alpha-1&2//Alpha-1&2
	wr_tmp[31] = 0x06;//wr_tmp[31] = 0x05;
	//Beta-0//Beta-0
	wr_tmp[30] = 0x68;//wr_tmp[30] = 0x16;
	wr_tmp[29] = 0x05;//wr_tmp[29] = 0x03;
	wr_tmp[28] = 0x5E;//wr_tmp[28] = 0x5E;
	//Gamma-0//Gamma-0
	wr_tmp[27] = 0xDF;//wr_tmp[27] = 0xDF;
	wr_tmp[26] = 0x07;//wr_tmp[26] = 0x07;
	//Gamma-1//Gamma-1
	wr_tmp[25] = 0x2F;//wr_tmp[25] = 0x2F;
	//Delta-0//Delta-0
	wr_tmp[24] = 0x99;//wr_tmp[24] = 0x99;
	wr_tmp[23] = 0xF4;//wr_tmp[23] = 0x74;
	//Alpha-3//Alpha-3
	wr_tmp[22] = 0x0B;//wr_tmp[22] = 0x0B;//0x03;
	//Frequency Multiplication
	//R
	wr_tmp[21] = 0x00;//0x03;
	wr_tmp[20] = 0x00;//0x03;
	wr_tmp[19] = 0x00;//0x03;
	wr_tmp[18] = 0x00;//0x03;
	//S
	wr_tmp[17] = 0xFF;//0x03;
	wr_tmp[16] = 0x1F;//0x03;
	wr_tmp[15] = 0xA1;//0x03;
	wr_tmp[14] = 0x07;//0x03;
	//V and U
	wr_tmp[13] = 0x00;//0x03;
	wr_tmp[12] = 0x00;//0x03;
	wr_tmp[11] = 0x00;//0x03;
	//Lock Detect
	//Phase Lock Treshold
	wr_tmp[10] = 0xE8;//0x03;
	wr_tmp[ 9] = 0xFD;//0x03;
	//Phase Lock Fill Rate
	wr_tmp[ 8] = 0x20;//0x03;
	//Phase Lock Drain Rate
	wr_tmp[ 7] = 0x44;//0x03;
	//Freq Lock Treshold
	wr_tmp[ 6] = 0x00;//wr_tmp[ 6] = 0x80;
	wr_tmp[ 5] = 0x24;//wr_tmp[ 5] = 0x96;
	wr_tmp[ 4] = 0xF4;//wr_tmp[ 4] = 0x98;
	//Freq Lock Fill Rate
	wr_tmp[ 3] = 0x20;//0x03;
	//Freq Lock Drain Rate
	wr_tmp[ 2] = 0x44;//0x03;

	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 52, wr_tmp, 0, rd_tmp, 0);
	return 0;
}

int init_ad9548_hw(alt_u16 lock_pll)
{
	int ret = 0;

	alt_u8 pll_id[2];
	alt_u8 pll_status;

	pllConfigSpi_4Wire();

	usleep(1000);
	pll_read_ID(pll_id);
	usleep(1000);

	if( (pll_id[0] != 0x48) || (pll_id[1] != 0xC6))
	{
		return -1;
	}

//
//	cmd_word = PLL_RD_CMD | PLL_STREAM | AD9548_REG_SYSTEM_CLK_STABILITY_2;
//	wr_tmp[0] = (cmd_word >> 8) & 0xFF;
//	wr_tmp[1] = cmd_word & 0xFF;
//	alt_avalon_spi_command(SPI_3_BASE, PLL_SLAVE_SELECT, 2, wr_tmp, 9, rd_tmp, 0);

	pllConfigureSystemClock();
	pllUpdate();
	pllCalibrateSysClk();
	pllUpdate();
	do
	{
		pll_status = pllGetSysClkStatus();
//		printf("PLL Status:\t0x%x\n", pll_status);
		usleep(250);
	}
	while((pll_status & 0x13) != 0x11);

	pllConfigFreeRunningDDS();

	if(lock_pll)
	{
		//Configure 1PPS Lock
		alt_u8 pll_status, pll_status1;

		//Set Ref_A to CMOS and assign Profile0 to it
		pllSetReg(AD9548_REG_REFERENCE_LOGIC_FAMILY_0, 0x03);//Config RefA as CMOS
		pllSetReg(AD9548_REG_MANUAL_REFERENCE_PROFILE_SEL_0, 0x08);//Assign Profile 0 to RefA
		pllUpdate();

		pllConfigureProfiler0();

		//DPLL Mode
		pllSetReg(AD9548_REG_LOOP_MODE, 0x18);//Selection manual and RefA select
		pllSetReg(AD9548_REG_RESET_FUNC, 0x10);
		pllSetReg(AD9548_REG_REFERENCE_PROFILE_DETECT, 0x1);
		pllSetReg(AD9548_REG_RESET_FUNC, 0x74);

		pllUpdate();

		do
		{
			usleep(250*1000);
			pll_status = pllDDSstatus0();
			pll_status1 = pllDDSstatus1();
//			printf("DPLL Status0:\t0x%x\tStatus1:\t0x%x\t", pll_status, pll_status1);
			pll_status1 = pllRefAstatus();
//			printf("RefA Status:\t0x%x\n", pll_status1);
			pllUpdate();

		}
		while((pll_status & 0x32) != 0x32);

	}
	else
	{

		//pllConfigureClockOut();
	}

	usleep(250*1000);

	return ret;
}

void print_pll_status(void)
{
//	alt_u8 pll_status, pll_status1;
//	pll_status = pllDDSstatus0();
//	pll_status1 = pllDDSstatus1();
////	printf("DPLL Status0:\t0x%x\tStatus1:\t0x%x\t", pll_status, pll_status1);
//	pll_status1 = pllRefAstatus();
////	printf("RefA Status:\t0x%x\n", pll_status1);
//	pllUpdate();

}
