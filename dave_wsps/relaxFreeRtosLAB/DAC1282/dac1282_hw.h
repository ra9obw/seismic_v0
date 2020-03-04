/*
 * dac1282_hw.h
 *
 *  Created on: 20.02.2018
 *      Author: Pavlenko
 */

#ifndef DAC1282_HW_H_
#define DAC1282_HW_H_

#include "alt_types.h"

//DAC Settings
#define DAC_RD_CMD		(0x20)
#define DAC_WR_CMD		(0x40)

//#define DAC_SLAVE_SELECT	(0)

#define	DAC_GANMOD_REG	(0x00)
	//Waveform
	#define DAC_MODE_SIN		(0x00 << 0)
	#define DAC_MODE_DC			(0x01 << 0)
	#define DAC_MODE_DDM		(0x02 << 0)
	#define DAC_MODE_PULSE		(0x03 << 0)
	//Gain
	#define DAC_GAIN_2V5		(0x00 << 2)
	#define DAC_GAIN_1V25		(0x01 << 2)
	#define DAC_GAIN_0V625		(0x02 << 2)
	#define DAC_GAIN_0V312		(0x03 << 2)
	#define DAC_GAIN_0V156		(0x04 << 2)
	#define DAC_GAIN_0V078		(0x05 << 2)
	#define DAC_GAIN_0V039		(0x06 << 2)
	//ID
	#define DAC_ID_MASK			(0xE0)
#define DAC_SINEG_REG	(0x01)
#define DAC_SWN_REG		(0x02)
	#define DAC_FREQ_BIT		(0x80)
	#define DAC_SW_OPEN			(0x00 << 4)
	#define DAC_SW_DIFF			(0x01 << 4)
	#define DAC_SW_DIFF_REV		(0x02 << 4)
	#define DAC_SW_CMRR_P		(0x03 << 4)
	#define DAC_SW_CMRR_N		(0x04 << 4)
	#define DAC_SW_SING_P		(0x05 << 4)
	#define DAC_SW_SING_N		(0x06 << 4)
	#define DAC_SW_SHORT		(0x07 << 4)
	#define DAC_M_DIV_MASK		(0x0F)
#define DAC_PULSA_REG	(0x07)
#define DAC_PULSB_REG	(0x08)

void write_DAC_reg(alt_u16 addr, alt_u8 data);

void configure_DAC(void);

#endif /* DAC1282_HW_H_ */
