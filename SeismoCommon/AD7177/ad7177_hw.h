/*
 * ad7177_hw.h
 *
 *  Created on: 16.02.2018
 *      Author: Pavlenko
 */

#ifndef AD7177_HW_H_
#define AD7177_HW_H_


#define AD7177_SPI_WR	(0)
#define AD7177_SPI_RD	(1 << 6)

#define AD7177_STATUS		(0)
	#define STATUS_RDYn					(1<<7)
	#define STATUS_ADC_ERROR			(1<<6)
	#define STATUS_CRC_ERROR			(1<<5)
	#define STATUS_REG_ERROR			(1<<4)
	#define STATUS_CHANNEL_mask			(0x3)

#define AD7177_ADCMODE		(1)
	//
	#define ADCMODE_REF_EN				(1<<15)
	#define ADCMODE_HIDE_DELAY 			(1<<14)
	#define ADCMODE_SING_CYC 			(1<<13)
	#define ADCMODE_DELAY_offs 			(8)
	#define ADCMODE_DELAY_mask 			(0x7 << ADCMODE_DELAY_offs)
	//ADC conversion mode
	#define ADCMODE_MODE_CONTINUOUS		(0)
	#define ADCMODE_MODE_SINGLE			(0x10)
	#define ADCMODE_MODE_STANDBY		(0x20)
	#define ADCMODE_MODE_POWEROFF		(0x30)
	#define ADCMODE_MODE_INT_OFF_CALIB	(0x40)
	#define ADCMODE_MODE_SYS_OFF_CALIB	(0x60)
	#define ADCMODE_MODE_SYS_GAIN_CALIB	(0x70)
	//ADC clock select
	#define ADCMODE_CLOCK_INT			(0x0)
	#define ADCMODE_CLOCK_INT_OUT		(0x1 << 2)
	#define ADCMODE_CLOCK_EXT			(0x2 << 2)
	#define ADCMODE_CLOCK_EXT_QX		(0x3 << 2)

#define AD7177_IFMODE		(2)
	#define IFMODE_CONT_READ			(1<<7)
	#define IFMODE_WL32					(1<<1)
#define AD7177_REGCHECK		(3)
#define AD7177_DATA			(4)
#define AD7177_GPIOCON		(6)
#define AD7177_ID			(7)
	#define  AD7177_ID_mask				(0xFFF0)
	#define  AD7177_ID_VAL 				(0x4FD0)

#define AD7177_CH0			(0x10)
#define AD7177_CH1			(0x11)
#define AD7177_CH2			(0x12)
#define AD7177_CH3			(0x13)
#define CH_EN				(1<<15)
	#define CH_SETUP_SEL0		(0<<12)
	#define CH_SETUP_SEL1		(1<<12)
	#define CH_SETUP_SEL2		(2<<12)
	#define CH_SETUP_SEL3		(3<<12)

	#define CH_AINPOS_AIN0		(0x00<<5)
	#define CH_AINPOS_AIN1		(0x01<<5)
	#define CH_AINPOS_AIN2		(0x02<<5)
	#define CH_AINPOS_AIN3		(0x03<<5)
	#define CH_AINPOS_AIN4		(0x04<<5)
	#define CH_AINPOS_TEMP_P	(0x11<<5)
	#define CH_AINPOS_TEMP_N	(0x12<<5)
	#define CH_AINPOS_VPP_P		(0x13<<5)
	#define CH_AINPOS_VPP_N		(0x14<<5)
	#define CH_AINPOS_REF_P		(0x15<<5)
	#define CH_AINPOS_REF_N		(0x16<<5)

	#define CH_AINNEG_AIN0		(0x00<<0)
	#define CH_AINNEG_AIN1		(0x01<<0)
	#define CH_AINNEG_AIN2		(0x02<<0)
	#define CH_AINNEG_AIN3		(0x03<<0)
	#define CH_AINNEG_AIN4		(0x04<<0)
	#define CH_AINNEG_TEMP_P	(0x11<<0)
	#define CH_AINNEG_TEMP_N	(0x12<<0)
	#define CH_AINNEG_VPP_P		(0x13<<0)
	#define CH_AINNEG_VPP_N		(0x14<<0)
	#define CH_AINNEG_REF_P		(0x15<<0)
	#define CH_AINNEG_REF_N		(0x16<<0)


#define AD7177_SETUPCON0	(0x20)
#define AD7177_SETUPCON1	(0x21)
#define AD7177_SETUPCON2	(0x22)
#define AD7177_SETUPCON3	(0x23)
	#define SETUPCON_OFFSET_BINARY_FORMAT	(1<<12)
	#define SETUPCON_REFBUF_P_EN	(1<<11)
	#define SETUPCON_REFBUF_N_EN	(1<<10)
	#define SETUPCON_AINBUF_P_EN	(1<<9)
	#define SETUPCON_AINBUF_N_EN	(1<<8)
	#define SETUPCON_BURNOUT		(1<<7)
	#define SETUPCON_REF_EXT		(0x0<<4)
	#define SETUPCON_REF_INT		(0x2<<4)
	#define SETUPCON_REF_VPP		(0x3<<4)

#define AD7177_FILTCON0			(0x28)
#define AD7177_FILTCON1			(0x29)
#define AD7177_FILTCON2			(0x2A)
#define AD7177_FILTCON3			(0x2B)
	#define FILTCON_SINC3_MAP		(1<<15)

	#define FILTCON_ENHFILTEN		(1<<11)
	#define FILTCON_ENHFILT_27SPS 	(0x2<<8)
	#define FILTCON_ENHFILT_25SPS 	(0x3<<8)
	#define FILTCON_ENHFILT_20SPS 	(0x5<<8)
	#define FILTCON_ENHFILT_16SPS 	(0x6<<8)

	#define FILTCON_SINC5_SINC1		(0x0<<5)
	#define FILTCON_SINC3			(0x3<<5)

	#define FILTCON_ODR_10KSPS		(0x07)
	#define FILTCON_ODR_5KSPS		(0x08)
	#define FILTCON_ODR_2K5SPS		(0x09)
	#define FILTCON_ODR_1KSPS		(0x0A)
	#define FILTCON_ODR_500SPS		(0x0B)
	#define FILTCON_ODR_397SPS5		(0x0C)
	#define FILTCON_ODR_200SPS		(0x0D)
	#define FILTCON_ODR_100SPS		(0x0E)
	#define FILTCON_ODR_59SPS92		(0x0F)
	#define FILTCON_ODR_49SPS96		(0x10)
	#define FILTCON_ODR_20SPS		(0x11)
	#define FILTCON_ODR_16SPS66		(0x12)
	#define FILTCON_ODR_10SPS		(0x13)
	#define FILTCON_ODR_5SPS		(0x14)

#define AD7177_OFFSET0		(0x30)
#define AD7177_OFFSET1		(0x31)
#define AD7177_OFFSET2		(0x32)
#define AD7177_OFFSET3		(0x33)

#define AD7177_GAIN0		(0x38)
#define AD7177_GAIN1		(0x39)
#define AD7177_GAIN2		(0x3A)
#define AD7177_GAIN3		(0x3B)

#include "alt_types.h"
#include "DAVE.h"


typedef struct{
	alt_u16	id;
	alt_u8	status;
	alt_u16 adcmode;
	alt_u16 ifmode;
	alt_u16 gpiocon;
	alt_u16 ch0;
	alt_u16 ch1;
	alt_u16 ch2;
	alt_u16 ch3;
	alt_u16 setup0;
	alt_u16 setup1;
	alt_u16 setup2;
	alt_u16 setup3;
	alt_u16 filt0;
	alt_u16 filt1;
	alt_u16 filt2;
	alt_u16 filt3;
	alt_u32 offs0;
	alt_u32 offs1;
	alt_u32 offs2;
	alt_u32 offs3;
	alt_u32 gain0;
	alt_u32 gain1;
	alt_u32 gain2;
	alt_u32 gain3;
} adc_row_regs_type;


typedef struct{
	SPI_MASTER_t *	pspi;
	alt_u32 		slave_select_num;
	alt_u32			adc_sample_rate;
	adc_row_regs_type	adc_regs;
} adc_inst_handler_type;

int read_ADC_status(adc_inst_handler_type *padc);
void spi_reset_ADC(adc_inst_handler_type *padc);
void configure_ADC(adc_inst_handler_type *padc);


int make_ADC_sync(void);
int make_ADC_reset(void);

#endif /* AD7177_HW_H_ */
