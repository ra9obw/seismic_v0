/*
 * sheet_happens.h
 *
 *  Created on: Mar 28, 2018
 *      Author: Pavlenko
 */
#ifndef __SHEET_HAPPENS_H_
#define __SHEET_HAPPENS_H_

#include "alt_types.h"

void usleep(unsigned int mks_delay);
void swap_bytes(alt_u8 *pd, alt_u8 num);
void fstat(float *pdt, int num, float *po_mean, float *po_std, float *po_p2p);

#endif //__SHEET_HAPPENS_H_
