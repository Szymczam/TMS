/*
 * ePWM.c
 *
 *  Created on: 10.02.2018
 *      Author: Marek
 */

#include "ePWM.h"

//clk_spwm = 200_000_000 Hz

EPWM_INFO epwm1 = {
                   .ePwmRegs        = &EPwm7Regs,
                   .Period          = 5000,
                   .Deadband        = 200,
                   .CMPA_Direction  = 1
                    };
EPWM_INFO epwm2 = {
                   .ePwmRegs        = &EPwm8Regs,
                   .Period          = 5000,
                   .Deadband        = 200,
                   .CMPA_Direction  = 1
                    };
EPWM_INFO epwm3 = {
                   .ePwmRegs        = &EPwm9Regs,
                   .Period          = 5000,
                   .Deadband        = 200,
                   .CMPA_Direction  = 1
                    };



void EPwmInitGpio(void){
    // Enable clock
    CpuSysRegs.PCLKCR2.bit.EPWM7=1;
    CpuSysRegs.PCLKCR2.bit.EPWM8=1;
    CpuSysRegs.PCLKCR2.bit.EPWM9=1;

    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 1;    // Disable pull-up on GPIO12 (EPWM7A)
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 1;    // Disable pull-up on GPIO13 (EPWM7B)
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;   // Configure GPIO12 as EPWM7A
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1;   // Configure GPIO13 as EPWM7B

    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 1;    // Disable pull-up on GPIO14 (EPWM8A)
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 1;    // Disable pull-up on GPIO15 (EPWM8B)
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;   // Configure GPIO14 as EPWM8A
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;   // Configure GPIO15 as EPWM8B

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;    // Disable pull-up on GPIO16 (EPWM9A)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;    // Disable pull-up on GPIO17 (EPWM9B)
    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 1;   // Configure GPIO16 as EPWM9A
    GpioCtrlRegs.GPAGMUX2.bit.GPIO17 = 1;   // Configure GPIO17 as EPWM9B
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;   // Configure GPIO16 as EPWM9A
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;   // Configure GPIO17 as EPWM9B

    EDIS;
}

void EPwmInitTz_Gpio(void){
    // For External Trigger, GPIOX as the trigger for TripZone
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;    // Enable pull-up on GPIOX (TZ1)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = 2;  // Asynch input GPIOX (TZ1)
    EALLOW;
    InputXbarRegs.INPUT1SELECT = 2;
    EDIS;
}


void EPwmInit(EPWM_INFO *ePwm){
   // Enable TZ1 as one cycle-by-cycle trip sources
   EALLOW;
   ePwm->ePwmRegs->TZSEL.bit.OSHT1 = 1;
   ePwm->ePwmRegs->TZCTL.bit.TZA = TZ_FORCE_LO;
   ePwm->ePwmRegs->TZCTL.bit.TZB = TZ_FORCE_LO;
   ePwm->ePwmRegs->TZEINT.bit.OST = 1;    // Enable TZ interrupt
   EDIS;
   // Setup TBCLK
   ePwm->ePwmRegs->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
   ePwm->ePwmRegs->TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
   ePwm->ePwmRegs->TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
   ePwm->ePwmRegs->TBCTL.bit.CLKDIV = TB_DIV1;
   // Setup Period
   ePwm->ePwmRegs->TBPRD = ePwm->Period;             // Set timer period
   // Setup initial values
   ePwm->ePwmRegs->TBPHS.bit.TBPHS = 0x0000;            // Phase is 0
   ePwm->ePwmRegs->TBCTR = 0x0000;                      // Clear counter
   // Setup shadow register load on ZERO
   ePwm->ePwmRegs->CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
   ePwm->ePwmRegs->CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   ePwm->ePwmRegs->CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   ePwm->ePwmRegs->CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
   // Set Compare values
   ePwm->ePwmRegs->CMPA.bit.CMPA = ePwm->Deadband;   // Set compare A value
   // Set actions
   ePwm->ePwmRegs->AQCTLA.bit.CAU = AQ_SET;             // Clear PWMxA on event A, up count
   ePwm->ePwmRegs->AQCTLA.bit.CAD = AQ_CLEAR;           // Set PWMxA on Zero
   ePwm->ePwmRegs->AQCTLB.bit.CAU = AQ_CLEAR;           // Clear PWMxB on event B, up count
   ePwm->ePwmRegs->AQCTLB.bit.CAD = AQ_SET;             // Set PWMxB on Zero
   // Start by increasing CMPA & CMPB
   ePwm->CMPA_Direction = EPWM_CMP_UP;
   // Active Low PWMs - Setup Deadbandf
   ePwm->ePwmRegs->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
   ePwm->ePwmRegs->DBCTL.bit.POLSEL = DB_ACTV_HIC;
   ePwm->ePwmRegs->DBCTL.bit.IN_MODE = DBA_ALL;
   ePwm->ePwmRegs->DBRED.bit.DBRED = ePwm->Deadband;
   ePwm->ePwmRegs->DBFED.bit.DBFED = ePwm->Deadband;
   // Interrupt where we will change the Compare Values
   ePwm->ePwmRegs->ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
   ePwm->ePwmRegs->ETSEL.bit.INTEN = 1;               // Enable INT
   ePwm->ePwmRegs->ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event
}



int i;
// update_compare - Update the compare values for the specified EPWM
void update_compare(EPWM_INFO *ePwm){

    i++;
    if (i==5){
 i =0;
   if(ePwm->CMPA_Direction == EPWM_CMP_UP){
       if(ePwm->ePwmRegs->CMPA.bit.CMPA < ePwm->Period){
           ePwm->ePwmRegs->CMPA.bit.CMPA++;
       }
       else{
           ePwm->CMPA_Direction = EPWM_CMP_DOWN;
           ePwm->ePwmRegs->CMPA.bit.CMPA--;
       }
   }
   else{
       if(ePwm->ePwmRegs->CMPA.bit.CMPA == ePwm->Deadband){
           ePwm->CMPA_Direction = EPWM_CMP_UP;
           ePwm->ePwmRegs->CMPA.bit.CMPA++;
       }
       else{
           ePwm->ePwmRegs->CMPA.bit.CMPA--;
       }
   }
    }
   return;
}



