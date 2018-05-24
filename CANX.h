/*
 * CANX.h
 *
 *  Created on: 16.02.2018
 *      Author: Marek
 */

#ifndef CANX_H_
#define CANX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"


extern void InitCanGPIO();
extern void InitCan(Uint32 CANX, tCANMsgObject *TXCAN, tCANMsgObject *RXCAN, unsigned char *TXData, unsigned char *RXData);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* CANX_H_ */
