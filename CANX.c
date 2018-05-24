/*
 * CANX.c
 *
 *  Created on: 16.02.2018
 *      Author: Marek
 */
#include "CANX.h"


void InitCanGPIO(){
    GPIO_SetupPinMux(72, GPIO_MUX_CPU1, 5);  //GPIO72 - CANRXA
    GPIO_SetupPinMux(73, GPIO_MUX_CPU1, 5);  //GPIO73 - CANTXA
    GPIO_SetupPinOptions(72, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinOptions(73, GPIO_OUTPUT, GPIO_PUSHPULL);
}

void InitCan(Uint32 CANX, tCANMsgObject *TXCAN, tCANMsgObject *RXCAN, unsigned char *TXData, unsigned char *RXData){
    // Enable test mode and select external loopback
    HWREG(CANX + CAN_O_CTL) |= CAN_CTL_TEST;
    HWREG(CANX + CAN_O_TEST) = CAN_TEST_EXL;
    CANEnable(CANX);// Enable the CAN for operation.
    // Initialize the CAN controller
    CANInit(CANX);
    // Setup CAN to be clocked off the M3/Master subsystem clock
    CANClkSourceSelect(CANX, 0);
    CANBitRateSet(CANX, 200000000, 500000);
    // Initialize the message object that will be used for sending CAN
    // messages.  The message will be 4 bytes that will contain an incrementing
    // value.  Initially it will be set to 0.
 /*   TXCAN->ui32MsgID = 1;                      // CAN message ID - use 1
    TXCAN->ui32MsgIDMask = 0;                  // no mask needed for TX
    TXCAN->ui32Flags = MSG_OBJ_TX_INT_ENABLE;  // enable interrupt on TX
    TXCAN->ui32MsgLen = sizeof(TXData);   // size of message is 4
    TXCAN->pucMsgData = TXData;           // ptr to message content
    // Initialize the message object that will be used for receiving CAN
    // messages.
    RXCAN->ui32MsgID = 1;                      // CAN message ID - use 1
    RXCAN->ui32MsgIDMask = 0;                  // no mask needed for TX
    RXCAN->ui32Flags = MSG_OBJ_NO_FLAGS;
    RXCAN->ui32MsgLen = sizeof(RXData);   // size of message is 4
    RXCAN->pucMsgData = RXData;           // ptr to message content
*/
}






