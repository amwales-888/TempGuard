/**
  IC3 Generated Driver API Source File

  @Company
    Microchip Technology Inc.

  @File Name
    ic3.c

  @Summary
    This is the generated source file for the IC3 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for IC3.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.4
        Device            :  dsPIC33EV64GM004
    The generated drivers are tested against the following:
        Compiler          :  XC16 v2.10
        MPLAB 	          :  MPLAB X v6.05
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include "ic3.h"

/**
  IC Mode.

  @Summary
    Defines the IC Mode.

  @Description
    This data type defines the IC Mode of operation.

*/

static uint16_t         gIC3Mode;

/**
  Section: Driver Interface
*/

void IC3_Initialize (void)
{
    // ICSIDL disabled; ICM Simple Capture mode: Falling; ICTSEL TMR3; ICI Every; 
    IC3CON1 = 0x02;
    // SYNCSEL TMR3; TRIGSTAT disabled; IC32 disabled; ICTRIG Sync; 
    IC3CON2 = 0x0D;
    
    gIC3Mode = IC3CON1bits.ICM;
    
    IFS2bits.IC3IF = false;
    IEC2bits.IC3IE = true;
}


void __attribute__ ((weak)) IC3_CallBack(void)
{
    // Add your custom callback code here
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _ISR _IC3Interrupt( void )
{	
    if(IFS2bits.IC3IF)
    {
		// IC3 callback function 
		IC3_CallBack();
        IFS2bits.IC3IF = 0;
    }
}
void IC3_Start( void )
{
    IC3CON1bits.ICM = gIC3Mode;
}

void IC3_Stop( void )
{
    IC3CON1bits.ICM = 0;
}

uint16_t IC3_CaptureDataRead( void )
{
    return(IC3BUF);
}

void IC3_ManualTriggerSet( void )
{
    IC3CON2bits.TRIGSTAT= true; 
}

bool IC3_TriggerStatusGet( void )
{
    return( IC3CON2bits.TRIGSTAT );
}


void IC3_TriggerStatusClear( void )
{
    /* Clears the trigger status */
    IC3CON2bits.TRIGSTAT = 0;
}
bool IC3_HasCaptureBufferOverflowed( void )
{
    return( IC3CON1bits.ICOV );
}


bool IC3_IsCaptureBufferEmpty( void )
{
    return( ! IC3CON1bits.ICBNE );
}

/**
 End of File
*/
