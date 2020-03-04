/*
 * adc_daq_task.c
 *
 *  Created on: 28.03.2018
 *      Author: Pavlenko
 */
#include "adc_daq_task.h"
#include "system.h"
//#include "altera_avalon_pio_regs.h"

#include "math.h"

#include "ad9548_hw.h"
#include "ad7177_hw.h"
#include "dac1282_hw.h"

#include "altera_avalon_spi.h"
#include "global_func.h"
#include "seismo_gpio.h"

static TaskHandle_t xTaskToNotify = NULL;

adc_inst_handler_type ADC0_handler = {.pspi = SPI_0_BASE, .slave_select_num = ADC1_SLAVE_SELECT, .adc_sample_rate = 0};
adc_inst_handler_type ADC1_handler = {.pspi = SPI_1_BASE, .slave_select_num = ADC2_SLAVE_SELECT, .adc_sample_rate = 0};
adc_inst_handler_type ADC2_handler = {.pspi = SPI_2_BASE, .slave_select_num = ADC3_SLAVE_SELECT, .adc_sample_rate = 0};



#define ADC_DUMP_NUM	(1000)
#define ADC_BUFF_COUNT	(1)


float adc0_samples[ADC_DUMP_NUM];//__attribute__ ((section (".sdram_mem")));
float adc1_samples[ADC_DUMP_NUM];//__attribute__ ((section (".sdram_mem")));
float adc2_samples[ADC_DUMP_NUM];//__attribute__ ((section (".sdram_mem")));
//
//uint32_t read_ADC_sample(adc_inst_handler_type *padc)
//{
//	alt_u8 rd_tmp[5] = { 0, 0, 0, 0, 0};
//	alt_u8 wr_tmp[5] = { 0, 0xFF, 0xFF, 0xFF, 0xFF};
////	alt_u8 wr_tmp[5] = { 0, 0, 0, 0, 0};
//	//READ VIA SPI ADC DATA
//	wr_tmp[0] = 0x40 | 0x04;//ADC Data Reg
////	alt_avalon_spi_transfer(padc->pspi, padc->slave_select_num, 0, wr_tmp, 5, rd_tmp, ALT_AVALON_SPI_COMMAND_MERGE);
//	alt_avalon_spi_command(padc->pspi, padc->slave_select_num, 1, wr_tmp, 4, rd_tmp, ALT_AVALON_SPI_COMMAND_MERGE);
//	return *((uint32_t*)rd_tmp);
//}

//static inline void read_ADC_all(adc_inst_handler_type ** padc, uint32_t * pdata)
//{
//	SPI_MASTER_t *handle;
//	//alt_u8 rd_tmp[4];
//	//start transfer in all spi channels
//	for(int ii=0; ii<ADC_COUNT; ++ii)
//	{
//		handle = padc[ii]->pspi;
//		XMC_USIC_CH_TXFIFO_Flush(handle->channel);
//		XMC_USIC_CH_RXFIFO_Flush(handle->channel);
//		/* Clear RBF0 */
//		(void)XMC_SPI_CH_GetReceivedData(handle->channel);
//		/* Clear RBF1 */
//		(void)XMC_SPI_CH_GetReceivedData(handle->channel);
//
//		//put 4 bytes into TX FIFO
//		XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);
//		XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);
//		XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);
//		XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);
//
//	}
//	//wait latest spi channel busy status
//	while((uint32_t)XMC_USIC_CH_GetTransmitBufferStatus(handle->channel) == (uint32_t)XMC_USIC_CH_TBUF_STATUS_BUSY)
//	{
//	}
//	//get data from all spi rx fifo`s
//	for(int ii=0; ii<ADC_COUNT; ++ii)
//	{
//		configASSERT( pdata != NULL )
//		handle = padc[ii]->pspi;
//		((uint8_t*)(&pdata[ii]))[3] = (uint8_t)handle->channel->OUTR;
//		((uint8_t*)(&pdata[ii]))[2] = (uint8_t)handle->channel->OUTR;
//		((uint8_t*)(&pdata[ii]))[1] = (uint8_t)handle->channel->OUTR;
//		((uint8_t*)(&pdata[ii]))[0] = (uint8_t)handle->channel->OUTR;
//		//(*(pdata[ii])) = *((uint32_t*)rd_tmp);
//	}
//}



//Do not do  such things

adc_inst_handler_type * ADC_handler_store[ADC_COUNT] = {&ADC0_handler, &ADC1_handler, &ADC2_handler};

adc_data_store_t adc_data_dump_store[ADC_DUMP_NUM*ADC_BUFF_COUNT];
static uint32_t adc_code_store_wr_ind = 0;

uint32_t rdy_flag = 0;

static inline uint32_t * get_adc_code_store_ptr(BaseType_t * pNotify)
{
	uint32_t * tmp_ptr = (uint32_t *)&(adc_data_dump_store[adc_code_store_wr_ind].adc_code);

	if(++adc_code_store_wr_ind > ADC_DUMP_NUM*ADC_BUFF_COUNT)
	{
		adc_code_store_wr_ind = 0;
		//save timestamp and
		//notify task that buff 1 ready
		rdy_flag++;
		disable_adc_drdy_irq();
	}
	else if(adc_code_store_wr_ind >= ADC_DUMP_NUM)
	{
		//save timestamp and
		//notify task that buff 0 ready
	}

	return tmp_ptr;
}
//TODO: Make it better!!

//void adc_drdy_irq_routing(void*handle) __attribute__ ((section (".ram_code")));
static uint32_t * curr_adc_code_store;
void ERU0_3_IRQHandler(void);// __attribute__ ((section (".ram_code")));
void ERU0_3_IRQHandler(void)//void adc_drdy_irq_routing(void/**handle*/)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	DIGITAL_IO_ToggleOutput(&LED_0);

	{
		SPI_MASTER_t *handle;
		//alt_u8 rd_tmp[4];
		//start transfer in all spi channels
		for(int ii=0; ii<ADC_COUNT; ++ii)
		{
			handle = ADC_handler_store[ii]->pspi;
			XMC_USIC_CH_TXFIFO_Flush(handle->channel);
			XMC_USIC_CH_RXFIFO_Flush(handle->channel);
			/* Clear RBF0 */
			(void)XMC_SPI_CH_GetReceivedData(handle->channel);
			/* Clear RBF1 */
			(void)XMC_SPI_CH_GetReceivedData(handle->channel);

			//put 4 bytes into TX FIFO
			XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);
			XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);
			XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);
			XMC_USIC_CH_TXFIFO_PutDataHPCMode(handle->channel, 0x0000U, (uint32_t)XMC_SPI_CH_MODE_STANDARD);

		}
		//Obtain pointer to data buffer while data are transfered via SPI
		curr_adc_code_store = get_adc_code_store_ptr(&xHigherPriorityTaskWoken);
		//get data from all spi rx fifo`s
		for(int ii=0; ii<ADC_COUNT; ++ii)
		{
			handle = ADC_handler_store[ii]->pspi;
			//check SPI TX busy status
			while((uint32_t)XMC_USIC_CH_GetTransmitBufferStatus(handle->channel) == (uint32_t)XMC_USIC_CH_TBUF_STATUS_BUSY) { 	}
			//data from ADC comes in Big Endian format, convert it to LE during FIFO reading
			((uint8_t*)(&curr_adc_code_store[ii]))[3] = (uint8_t)handle->channel->OUTR;
			((uint8_t*)(&curr_adc_code_store[ii]))[2] = (uint8_t)handle->channel->OUTR;
			((uint8_t*)(&curr_adc_code_store[ii]))[1] = (uint8_t)handle->channel->OUTR;
			((uint8_t*)(&curr_adc_code_store[ii]))[0] = (uint8_t)handle->channel->OUTR;
		}
	}

	//ACK IRQ
	acknowledge_adc_drdy_irq();

	if( xTaskToNotify != NULL )
	{
		/* Notify the task that the transmission is complete. */
		vTaskNotifyGiveFromISR( xTaskToNotify, &xHigherPriorityTaskWoken );
		/* There are no transmissions in progress, so no tasks to notify. */
		xTaskToNotify = NULL;
	}

	DIGITAL_IO_ToggleOutput(&LED_0);

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}




#define mainTASK_ADC_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainTASK_GPS_PRIORITY		( tskIDLE_PRIORITY + 1 )
//-----------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------
static void prvGarbageTask( void *pvParameters );
static void prvGPSTask( void *pvParameters );

static void prvGPSTask( void *pvParameters )
{
	for( ;; )
	{
		//TODO!!!: parse GPS data
		vTaskDelay(100);
//		char c = alt_getchar();
//		alt_printf("%c", c);
	}
}

/*-----------------------------------------------------------*/
static void prvGarbageTask( void *pvParameters )
{

	int send_ind=0;
	uint32_t ulNotifiedValue;
	disable_adc_drdy_irq();

	init_ad9548_hw(0);

	//vTaskDelay(1000);


	spi_reset_ADC(&ADC0_handler);
	spi_reset_ADC(&ADC1_handler);
	spi_reset_ADC(&ADC2_handler);

	read_ADC_status(&ADC0_handler);
	read_ADC_status(&ADC1_handler);
	read_ADC_status(&ADC2_handler);

	configure_ADC(&ADC0_handler);
	configure_ADC(&ADC1_handler);
	configure_ADC(&ADC2_handler);
//
//	read_ADC_status(&ADC0_handler);
//	read_ADC_status(&ADC1_handler);
//	read_ADC_status(&ADC2_handler);


	make_ADC_sync();

//	configure_DAC();

	configASSERT( xTaskToNotify == NULL );

	/* Store the handle of the calling task. */
	xTaskToNotify = xTaskGetCurrentTaskHandle();

//	make_ADC_sync();
//	curr_adc_code_store = (uint32_t *)&(adc_data_dump_store[0].adc_code);
	enable_adc_drdy_irq();


	while(1)
	{
		xTaskNotifyWait( 0x00,      /* Don't clear any notification bits on entry. */
		                         -1ul, /* Reset the notification value to 0 on exit. */
		                         &ulNotifiedValue, /* Notified value pass out in
		                                              ulNotifiedValue. */
		                         portMAX_DELAY );  /* Block indefinitely. */
//		if(rdy_flag != 0)
		{
			float adc_mean, adc_std, adc_p2p;
			int kk = 0;
			for(int adc0_dt_rd_addr = 0; adc0_dt_rd_addr<ADC_DUMP_NUM; adc0_dt_rd_addr++)
			{
				unsigned int adc_sample0, adc_sample1, adc_sample2;
				adc_sample0 = adc_data_dump_store[adc0_dt_rd_addr].adc_code[0];
				adc0_samples[adc0_dt_rd_addr] = (float)((int)adc_sample0-(int)0x80000000);//*ADC_QUANT

				adc_sample1 = adc_data_dump_store[adc0_dt_rd_addr].adc_code[1];
				adc1_samples[adc0_dt_rd_addr] = (float)((int)adc_sample1-(int)0x80000000);//*ADC_QUANT

				adc_sample2 = adc_data_dump_store[adc0_dt_rd_addr].adc_code[2];
				adc2_samples[adc0_dt_rd_addr] = (float)((int)adc_sample2-(int)0x80000000);//*ADC_QUANT


				lUDPLoggingPrintf("\t%u\t%u\t%u\t%u\n", send_ind++, adc_sample0, adc_sample1, adc_sample2);
				if(++kk == 32)
				{
					kk = 0;
					vTaskDelay(5);
				}

			}

			vTaskDelay(50);

			fstat(adc0_samples, ADC_DUMP_NUM, &adc_mean, &adc_std, &adc_p2p);
//			printf("adc0: %i\t\t%i\t\t%i\n", (int)adc_mean, (int)adc_std, (int)adc_p2p);
//
			fstat(adc1_samples, ADC_DUMP_NUM, &adc_mean, &adc_std, &adc_p2p);
//			printf("adc1: %i\t\t%i\t\t%i\n", (int)adc_mean, (int)adc_std, (int)adc_p2p);

			fstat(adc2_samples, ADC_DUMP_NUM, &adc_mean, &adc_std, &adc_p2p);
//			printf("adc2: %i\t\t%i\t\t%i\n", (int)adc_mean, (int)adc_std, (int)adc_p2p);

			rdy_flag = 0;
			xTaskToNotify = xTaskGetCurrentTaskHandle();
			enable_adc_drdy_irq();
		}
	}
}


//====================================================================
//============		Public functions		==========================
//====================================================================
int init_data_acq_task(UBaseType_t uxPriority)
{
	initPIOcommon();

	make_ADC_reset();

	xTaskCreate( prvGarbageTask, 	"TaskX", 2*configMINIMAL_STACK_SIZE, NULL, mainTASK_ADC_PRIORITY, NULL );
}


int init_data_acquisition(UBaseType_t uxPriority)
{
//	disable_adc_drdy_irq();

	//TODO: install interrupt handler
//	alt_irq_register( PIN_IRQ, NULL, adc_drdy_irq_routing );//alt_ic_isr_register(PIN_IRQ_INTERRUPT_CONTROLLER_ID, PIN_IRQ, adc_drdy_irq_routing, NULL, NULL);

	initPIOcommon();

	make_ADC_reset();

//	xTaskCreate( prvGarbageTask, 	"TaskX", 2*configMINIMAL_STACK_SIZE, NULL, mainTASK_ADC_PRIORITY, NULL );
//	xTaskCreate( prvGPSTask, 		"GPS", configMINIMAL_STACK_SIZE, NULL, mainTASK_GPS_PRIORITY, NULL );
	prvGarbageTask(0);

}

