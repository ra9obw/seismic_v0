/*
 * seismo_gpio.h
 *
 *  Created on: 28.03.2018
 *      Author: Pavlenko
 */

#ifndef SEISMO_GPIO_H_
#define SEISMO_GPIO_H_

//#define LED_0 			(1 << 0)
//#define LED_1 			(1 << 1)

//#define ADC_SYNC 		(1 << 8)
//#define ADC_RST			(1 << 9)
//
//#define ADC0_MUX_A1 	(1 << 10)
//#define ADC0_MUX_A0		(1 << 11)
//
//#define ADC1_MUX_A1		(1 << 12)
//#define ADC1_MUX_A0		(1 << 13)
//
//#define ADC2_MUX_A1		(1 << 14)
//#define ADC2_MUX_A0		(1 << 15)
//
//#define CDAC_MUX_A1		(1 << 16)
//#define CDAC_MUX_A0		(1 << 17)

#include "alt_types.h"

//void initPIOcommon();
//void portBitSet(alt_u32 set_mask);
//void portBitClr(alt_u32 clr_mask);
//void portBitTgl(alt_u32 tgl_mask);

int enable_adc_drdy_irq(void);
int disable_adc_drdy_irq(void);
int acknowledge_adc_drdy_irq(void);
int configure_adc_drdy_irq(void);

#endif /* SEISMO_GPIO_H_ */
