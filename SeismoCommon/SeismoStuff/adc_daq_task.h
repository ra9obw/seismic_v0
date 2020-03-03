/*
 * adc_daq_task.h
 *
 *  Created on: 28.03.2018
 *      Author: Pavlenko
 */

#ifndef ADC_DAQ_TASK_H_
#define ADC_DAQ_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#define ADC_COUNT	(3)
typedef struct
{
	uint32_t adc_code[ADC_COUNT];
} adc_data_store_t;

int init_data_acquisition(UBaseType_t uxPriority);
int init_data_acq_task(UBaseType_t uxPriority);

#endif /* ADC_DAQ_TASK_H_ */
