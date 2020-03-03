/*
 * deviceConfig.h
 *
 *  Created on: Mar 23, 2018
 *      Author: Pavlenko
 */

#ifndef DEVICECONFIG_H_
#define DEVICECONFIG_H_

#define SEISMO_BOARD

#define PHY_KSZ8081RNB	20
#define PHY_KSZ8031RNB	10


#if defined(SEISMO_BOARD)
	#define PHY_DEVICE_NAME PHY_KSZ8081RNB
#else
	#define PHY_DEVICE_NAME PHY_KSZ8031RNB
#endif

#endif /* DEVICECONFIG_H_ */
