/*
 * ePWM.h
 *
 *  Created on: 10.02.2018
 *      Author: Marek
 */

#ifndef EPWM_H_
#define EPWM_H_

#include "F28x_Project.h"     // DSP28x Headerfile

#define EPWM_CMP_UP           1
#define EPWM_CMP_DOWN         0

typedef struct{
    volatile struct EPWM_REGS *ePwmRegs;
    Uint16 Period;
    Uint16 Deadband;
    Uint16 CMPA_Direction;
}EPWM_INFO;

extern EPWM_INFO epwm1;
extern EPWM_INFO epwm2;
extern EPWM_INFO epwm3;

void EPwmInitGpio(void);
void EPwmInitTz_Gpio(void);
void EPwmInit(EPWM_INFO*);

void update_compare(EPWM_INFO*);


#endif /* EPWM_H_ */
