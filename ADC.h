/*
 * ADC.h
 *
 *  Created on: 16.02.2018
 *      Author: Marek
 */

#ifndef ADC_H_
#define ADC_H_

#include "F28x_Project.h"     // DSP28x Headerfile



#define CONV_WAIT           1L //Micro-seconds to wait for ADC conversion. Longer than necessary.

typedef struct{
    Uint16 A14;
    Uint16 A0;
    Uint16 A1;
    Uint16 B0;
    Uint16 B1;
    Uint16 B2;
    Uint16 B4;
}ADC_RESULT_T;

extern ADC_RESULT_T adc;



void ADCinit();
void ADCGetResult();
int16_t sampleADC(void);
void ConfigurePPB1Limits(Uint16 soc, Uint16 limitHigh, Uint16 limitLow);



#endif /* ADC_H_ */
