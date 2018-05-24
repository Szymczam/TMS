/*
 * ADC.c
 *
 *  Created on: 16.02.2018
 *      Author: Marek
 */

#include "ADC.h"



ADC_RESULT_T adc;

const float ADC_const = (3300.0/4096.0);


void ADCinit(){
    Uint16 acqps;

    EALLOW;
    //write configurations ADCA
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;      //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;   //set pulse positions to late
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;      //power up the ADCs

    //write configurations ADCB
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6;      //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;   //set pulse positions to late
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;      //power up the ADCs

    DELAY_US(1000); //delay for 1ms to allow ADC time to power up
    EDIS;

    // Determine minimum acquisition window (in SYSCLKS) based on resolution
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION){
        acqps = 14; //75ns
    }
    else{ //resolution is 16-bit
        acqps = 63; //320ns
    }

    EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin A0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC1 will convert pin A1
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC14CTL.bit.CHSEL = 14;  //SOC1 will convert pin 14
    AdcaRegs.ADCSOC14CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared ADCB

    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin B0
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 1;  //SOC1 will convert pin B1
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC1 will convert pin B2
    AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC3CTL.bit.CHSEL = 4;  //SOC1 will convert pin B4
    AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    EDIS;

}

int16_t sampleADC(void){
    int16_t temp;
    AdcaRegs.ADCSOCFRC1.all = 0x03;
    while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0){}
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;        //Clear ADCINT1
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT3 = 1;
    temp = AdcaResultRegs.ADCRESULT0;
    return(temp);
}

void ADCGetResult(){
    //start conversions immediately via software, ADCB
    AdcaRegs.ADCSOCFRC1.all = 0x0003 | 0x1<<14; //SOC0,SOC1,SOC14
    AdcbRegs.ADCSOCFRC1.all = 0x000F; //SOC0,SOC1,SOC2,SOC3
    //wait for ADCA to complete, then acknowledge flag
    while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    //wait for ADCB to complete, then acknowledge flag
    while(AdcbRegs.ADCINTFLG.bit.ADCINT1 == 0);
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    //store results
    adc.A0 = AdcaResultRegs.ADCRESULT0 * ADC_const;
    adc.A1 = AdcaResultRegs.ADCRESULT1 * ADC_const;
    adc.B0 = AdcbResultRegs.ADCRESULT0 * ADC_const;
    adc.B1 = AdcbResultRegs.ADCRESULT1 * ADC_const;
    adc.B2 = AdcbResultRegs.ADCRESULT2 * ADC_const;
    adc.B4 = AdcbResultRegs.ADCRESULT3 * ADC_const;
    adc.A14 = AdcaResultRegs.ADCRESULT14 * ADC_const;

}



void ConfigurePPB1Limits(Uint16 soc, Uint16 limitHigh, Uint16 limitLow)
{
    EALLOW;

    AdcaRegs.ADCPPB1CONFIG.bit.CONFIG = soc;  //PPB1 is associated with soc

    //
    //set high and low limits
    //
    AdcaRegs.ADCPPB1TRIPHI.bit.LIMITHI = limitHigh;
    AdcaRegs.ADCPPB1TRIPLO.bit.LIMITLO = limitLow;

    //
    //enable high and low limit events to generate interrupt
    //
    AdcaRegs.ADCEVTINTSEL.bit.PPB1TRIPHI = 1;
    AdcaRegs.ADCEVTINTSEL.bit.PPB1TRIPLO = 1;

    EDIS;
}
