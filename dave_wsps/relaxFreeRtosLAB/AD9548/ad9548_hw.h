/*
 * ad9548_hw.h
 *
 *  Created on: 10.02.2018
 *      Author: Pavlenko
 */

#ifndef AD9548_HW_H_
#define AD9548_HW_H_

#include "alt_types.h"

int init_ad9548_hw(alt_u16 lock_pll);
void print_pll_status(void);


#endif /* AD9548_HW_H_ */
