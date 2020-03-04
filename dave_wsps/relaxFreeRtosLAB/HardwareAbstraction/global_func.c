/*
 * sheet_happens.c
 *
 *  Created on: Mar 28, 2018
 *      Author: Pavlenko
 */

#include "global_func.h"

void usleep(unsigned int mks_delay)
{
	unsigned int ii, jj;
	for(ii=0;ii<mks_delay;ii++)
	{
		for(jj=0;jj<10;jj++)
		{
			__asm volatile( "nop" );
		}
	}
}


void swap_bytes(alt_u8 *pd, alt_u8 num)
{
	alt_u8 ii;
	alt_u8 tmp;
	for(ii=0;ii<(num>>1);ii++)
	{
		tmp = pd[ii];
		pd[ii] = pd[num-ii-1];
		pd[num-ii-1] = tmp;
	}
}



void fstat(float *pdt, int num, float *po_mean, float *po_std, float *po_p2p)
{
	double mean = 0.0, std = 0.0, mmin = pdt[0], mmax = pdt[0];
	int ii;

	for(ii=0;ii<num;ii++)
	{
		mean += pdt[ii];
		//
		if(pdt[ii] < mmin)
			mmin = pdt[ii];
		//
		if(pdt[ii] > mmax)
			mmax = pdt[ii];
	}
	mean = mean/num;
	*po_mean = mean;

	for(ii=0;ii<num;ii++)
	{
		std += (pdt[ii] - mean)*(pdt[ii] - mean);
	}

	std = sqrt(std/(num-1));
	*po_std = std;
	*po_p2p = mmax-mmin;

}
