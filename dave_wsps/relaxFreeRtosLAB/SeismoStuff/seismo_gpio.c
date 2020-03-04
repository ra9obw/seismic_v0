/*
 * seismo_gpio.c
 *
 *  Created on: 28.03.2018
 *      Author: Pavlenko
 */


#include "seismo_gpio.h"
#include "system.h"
//#include "altera_avalon_pio_regs.h"


//
//void portBitSet(alt_u32 set_mask){
////	alt_u32 porTmp;
////	porTmp = IORD_ALTERA_AVALON_PIO_DATA(POUT_BASE);
////	porTmp |= set_mask;
////	IOWR_ALTERA_AVALON_PIO_DATA(POUT_BASE, porTmp);
//
//}
//void portBitClr(alt_u32 clr_mask){
////	alt_u32 porTmp;
////	porTmp = IORD_ALTERA_AVALON_PIO_DATA(POUT_BASE);
////	porTmp &= (~clr_mask);
////	IOWR_ALTERA_AVALON_PIO_DATA(POUT_BASE, porTmp);
//}
//void portBitTgl(alt_u32 tgl_mask){
////	alt_u32 porTmp;
////	porTmp = IORD_ALTERA_AVALON_PIO_DATA(POUT_BASE);
////	porTmp ^= (tgl_mask);
////	IOWR_ALTERA_AVALON_PIO_DATA(POUT_BASE, porTmp);
//}

int initPIOcommon(){
//	IOWR_ALTERA_AVALON_PIO_DATA(POUT_BASE, 0);
//	PIN_INTERRUPT_Disable(&ADC_DRDY);
	DIGITAL_IO_SetOutputHigh(&PLL_CS);
	DIGITAL_IO_SetOutputHigh(&DAC_CS);
	DIGITAL_IO_SetOutputHigh(&FLASH_CS);
	DIGITAL_IO_SetOutputLow(&PLL_RST);
	DIGITAL_IO_SetOutputHigh(&GPS_RSTn);
	DIGITAL_IO_SetOutputHigh(&DAC_RSTn);
	DIGITAL_IO_SetOutputHigh(&ADC1_CS);
	DIGITAL_IO_SetOutputHigh(&ADC2_CS);
	DIGITAL_IO_SetOutputHigh(&ADC3_CS);
	DIGITAL_IO_SetOutputHigh(&ADC_SYNC);
	return 0;
}

//======================================================================================

int configure_adc_drdy_irq(void)
{
	return 0;
}

int acknowledge_adc_drdy_irq(void)
{
	NVIC_ClearPendingIRQ(ADC_DRDY.IRQn);
	return 0;
}


int enable_adc_drdy_irq(void)
{
	XMC_ERU_Enable(ADC_DRDY.eru);
	NVIC_ClearPendingIRQ(ADC_DRDY.IRQn);
	PIN_INTERRUPT_Enable(&ADC_DRDY);
	XMC_ERU_OGU_SetServiceRequestMode(ADC_DRDY.eru, ADC_DRDY.ogu, XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER);//XMC_ERU_OGU_SERVICE_REQUEST_DISABLED
//	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(ADC_DRDY, 1);
//	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(ADC_DRDY, 1);
	return 0;
}

int disable_adc_drdy_irq(void)
{
	XMC_ERU_Disable(ADC_DRDY.eru);
	XMC_ERU_OGU_SetServiceRequestMode(ADC_DRDY.eru, ADC_DRDY.ogu, XMC_ERU_OGU_SERVICE_REQUEST_DISABLED);
	PIN_INTERRUPT_Disable(&ADC_DRDY);
//	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIN_BASE, 0);
	return 0;
}
