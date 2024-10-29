/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
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

#include <xc.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/tmr4.h"
#include "mcc_generated_files/ic1.h"
#include "mcc_generated_files/ic2.h"
#include "mcc_generated_files/ic3.h"
#include "mcc_generated_files/ic4.h"
#include "mcc_generated_files/pwm.h"

#include "scheduler.h"
#include "pid.h"
#include "button.h"
#include "encoder.h"
#include "lcd.h"
#include "dht.h"
#include "videoBuffer.h"
#include "tach.h"
#include "menu.h"
#include "config.h"
#include "format.h"

static struct config_s cfg;
static uint8_t videoBuffer[LCDROWS * LCDCOLS];

#define TMR4TICKTOUS      1.8
#define TMR4MSTOTICK(ms) ((ms)/(0.001*(TMR4TICKTOUS)))
#define TMR4USTOTICK(us) ((us)/TMR4TICKTOUS)

#define TEMPID0 0
#define TEMPID1 1
#define TEMPID2 2
#define TEMPID3 3

#define ENCODERID 0 

#define ENCODERGPIOA  4 /* GPIO3 */
#define ENCODERGPIOB  5 /* GPIO4 */
#define BUTTONGPIOID  3 /* GPIO5 */

#define PWMPERIOD1 5600 /* PHASE1 25KHz */
#define PWMPERIOD2 5600 /* PHASE2 25KHz */
#define PWMPERIOD3 5600 /* PHASE3 25KHz */

static void setFanOn(uint16_t pwmId);
static void setFanOff(uint16_t pwmId);
static void pwmSetDutyCycle(uint16_t pwmId, uint32_t dutyCyclePercentage);
static void userInterface(void *data);
static void buttonDebounce(void *data);
static void buttonPoll(void *data);
static void buttonCallback(uint8_t id, enum buttonState_e state, void *data);
static void encoderPoll(void *data);
static void lcdUpdateVideo(void *data);
static void readTemperatureDelay(void *data);
static void readTemperature(void *data);
static void ICInit(void *data);
static void ICStop(void *data);
static uint16_t ICCaptureDataRead(void *data);
static void ICSetDigitalOuputLow(void *data);
static void ICSetDigitalInput(void *data);
static void monitorTemp(uint16_t tempId);
static void monitorAllTemps(void *data);
static void updateLCDInit(void *data);
static void tachRead(void *data);
static void updateLCDUpload(void *data);
#ifdef BLINK7BIT
static void videoBlink(void *data);
#endif
static void logData(void *data);
static void monitorAlarm(void *data);


static struct config_s cfg;
static uint8_t videoBuffer[LCDROWS * LCDCOLS];

const uint8_t charTable[] = {

  0b10000, /* Right Arrow */
  0b11000,
  0b11100,
  0b11110,
  0b11100,
  0b11000,
  0b10000,    
  0b00000,

  0b00001, /* Left Arrow */
  0b00011,
  0b00111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,    
  0b00000,

  0b00100, /* Up Arrow */
  0b01110,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  
  0b00000, /* Down Arrow */
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
   
  0b00100, /* UpDown Arrow */
  0b01110,
  0b11111,
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b00000,

};

static struct uploadChar_s uploadCharData = {

    0,
    0,
    5,
    &charTable[0],    
};

static uint8_t updateLCDCTX;
static uint8_t temperatureDelayCTX;

/* Processes */

static PROCESSALLOCATE(userInterfaceProcess,        250,                          userInterface,        (void *)0 );
static PROCESSALLOCATE(buttonDebounceProcess,       DEBOUNCE_INTEGRATOR_DELAYMS,  buttonDebounce,       (void *)0 );
static PROCESSALLOCATE(buttonPollProcess,           DEBOUNCE_DELAYMS,             buttonPoll,           (void *)0 );
static PROCESSALLOCATE(encoderPollProcess,          10,                           encoderPoll,          (void *)0 );
static PROCESSALLOCATE(updateLCDInitProcess,        0,                            updateLCDInit,        (void *)&updateLCDCTX );
static PROCESSALLOCATE(updateLCDUploadProcess,      0,                            updateLCDUpload,      (void *)&uploadCharData );
static PROCESSALLOCATE(updateVideoProcess,          0,                            lcdUpdateVideo,       (void *)0 );
static PROCESSALLOCATE(readTemperatureDelayProcess, 250,                          readTemperatureDelay, (void *)&temperatureDelayCTX );
static PROCESSALLOCATE(readTemperatureProcess0,     10,                           readTemperature,      (void *)TEMPID0 );
static PROCESSALLOCATE(readTemperatureProcess1,     10,                           readTemperature,      (void *)TEMPID1 );
static PROCESSALLOCATE(readTemperatureProcess2,     10,                           readTemperature,      (void *)TEMPID2 );
static PROCESSALLOCATE(readTemperatureProcess3,     10,                           readTemperature,      (void *)TEMPID3 );
static PROCESSALLOCATE(monitorAllTempsProcess,      250,                          monitorAllTemps,      (void *)0 );
static PROCESSALLOCATE(tachReadProcess,             250,                          tachRead,             (void *)0 );
static PROCESSALLOCATE(logDataProcess,              1000,                         logData,              (void *)0 );
static PROCESSALLOCATE(monitorAlarmProcess,         1000,                         monitorAlarm,         (void *)0 );

#ifdef BLINK7BIT
static PROCESSALLOCATE(videoBlinkProcess,           500,                          videoBlink,           (void *)0 );
#endif

#ifdef BLINK7BIT
static uint16_t videoBlinkCount;

static void
videoBlinkDelay(void *data) {

    struct process_s *process = (struct process_s *)data;
        
    videoBlinkCount = 2;
    schedulerProcessResetPeriod(process);
    videoSetBlink(0);
}


static void 
videoBlink(void *data) {
    
    if (videoBlinkCount == 0) {
        videoToggleBlink();
    } else {
        videoBlinkCount--;
    }
}
#endif

static void
readTemperatureDelay(void *data) {
    
    uint8_t *ctx = (uint8_t *)data;

    /* start each temperature reading with a delayed offset and
     * finally remove ourself from schedule 
     */

    switch (*ctx) {
        case 0: {
            if (cfg.sensor[0].enabled) {
                schedulerProcessAdd(&readTemperatureProcess0);            
            }
            break;
        }
        case 1: {
            if (cfg.sensor[1].enabled) {
                schedulerProcessAdd(&readTemperatureProcess1);            
            }
            break;
        }
        case 2: {
            if (cfg.sensor[2].enabled) {
                schedulerProcessAdd(&readTemperatureProcess2);            
            }
            break;
        }
        case 3: {
            if (cfg.sensor[3].enabled) {
                schedulerProcessAdd(&readTemperatureProcess3);            
            }
            break;
        }
        case 4: {
            schedulerProcessRemove(&readTemperatureDelayProcess);
            break;        
        }
    }
    
    (*ctx)++;   
}

static void 
readTemperature(void *data) {
    
    uint16_t id = (uint16_t)data;
    
    dhtTemperatureProcess(id);
}

static void 
encoderPoll(void *data) {        
    
    encoderUpdate();    
}

static void 
buttonDebounce(void *data) {
    
    buttonUpdateIntegrator();
}

static void 
buttonPoll(void *data) {

    buttonUpdate();
}

static void 
userInterface(void *data) {

    encoderUpdateChanged(); // Check for any encoder updates, this will
                            // trigger any encoder callbacks, no point doing this more
                            // often that it needs to be done
    
    menuDisplay();
}


void 
TMR1_CallBack(void) {

    /* Callback from TMR1 Interrupt */
    
    schedulerIncrementTicks();
}

enum LCDUploadState_e {
    
    LCDUploading=0,
    LCDUploadComplete,    
};

static enum LCDUploadState_e LCDUploadState;

static void
updateLCDUpload(void *data) {
    
    struct uploadChar_s *p = (struct uploadChar_s *)data;
    
    if (p->count == 0) {
        
        LCDUploadState = LCDUploadComplete;
        
        /* Start video refreshing */
        schedulerProcessAdd(&updateVideoProcess);

        // All characters uploaded remove ourself from process list
        schedulerProcessRemove(&updateLCDUploadProcess);
        return;
    }
            
    if (lcdBusy() == 0) {

        switch (uploadChar(p->position, *(p->ptr), &(p->ctx))) {
            case CHARSTARTEDUPLOAD:
                break;

            case CHARUPLOADING:
                p->ptr++;
                break;

            case CHARUPLOADCOMPLETE:
                p->position++;
                p->count--;
                p->ctx = 0;
                break;
        }
    }
}

static void
updateLCDInit(void *data) {
    
    static uint16_t last = 0;
           uint16_t now  = TMR4_Counter16BitGet();

    /* Using high resolution timer 
     * Cannot use the busy flag during init so wait a 
     * minimum of 5ms between processing
     */ 
    if ((uint16_t)(now - last) > TMR4MSTOTICK(5)) {
                
        if (lcdInitProcess((uint8_t *)data) == LCDREADY) {

            // Upload user defined characters
            schedulerProcessAdd(&updateLCDUploadProcess);
            
            // Init completed remove ourselves from list of active processes        
            schedulerProcessRemove(&updateLCDInitProcess);
            return;
        }

        last = now;
    }
}

static void 
lcdUpdateVideo(void *data) {

    videoBufferRefresh();
}

static void 
encoderCallback(uint16_t id, uint16_t value, void *data) {

    if (id == ENCODERID) {
        
        menuEncoderChanged(value);    
    }
}

static void 
buttonCallback(uint8_t id, enum buttonState_e state, void *data) {

    if (id == BUTTONGPIOID) {
        if (state == BUTTONPRESSED) {       

            menuButtonPressed();
        }
    }
}

static void 
LCDRWSet(uint8_t value, void *data) {

    if (value) {
        LCD_RW_SetHigh();
    } else {
        LCD_RW_SetLow();
    }
}

static void 
LCDRSSet(uint8_t value, void *data) {
    
    if (value) {
        LCD_RS_SetHigh();
    } else {
        LCD_RS_SetLow();        
    }
}

static void 
LCDWriteDB(uint8_t byte, void *data) {
    
    // Set DB portbits to output
    
    TRISB &= 0xFE1F; 
    TRISC &= 0xFC3F;
    
    LATB &= (uint16_t)0xFE1F;
    LATB |= (((uint16_t)byte & 0x0F) << 5);
    
    LATC &= (uint16_t)0xFC3F;
    LATC |= (((uint16_t)byte & 0xF0) << 2);      
}

static uint8_t 
LCDReadDB(void *data) {
    
    uint8_t byte = 0;
    
    // Set DB portbits to input
    
    TRISB |= ~0xFE1F; 
    TRISC |= ~0xFC3F; 
   
    byte |= (PORTB & (~0xFE1F)) >> 5; 
    byte |= (PORTC & (~0xFC3F)) >> 2;

    return byte;
}

static void 
LCDESet(uint8_t value, void *data) {
    
    if (value) {
        LCD_E_SetHigh();
    } else {
        LCD_E_SetLow();        
    }
}

static void 
ICInit(void *data) {
    
    uint16_t id = (uint16_t)data;
            
    TMR3 = 0;       // Reset timer used by Input Capture
                    // The timer is shared but the start and duration of
                    // the calls do not overlap so it is safe to reset the
                    // the timer.

    switch (id) {
        case 0: {        
            IC1_Initialize();
            IC1_Start();
            break;
        }
        case 1: {
            IC2_Initialize();
            IC2_Start();
            break;
        }
        case 2: {
            IC3_Initialize();
            IC3_Start();
            break;
        }
        case 3: {
            IC4_Initialize();
            IC4_Start();
            break;
        }
    }
}

static void 
ICStop(void *data) {
   
    uint16_t id = (uint16_t)data;

    switch (id) {
        case 0: {
            IC1_Stop();
            break;
        }
        case 1: {
            IC2_Stop();
            break;
        }
        case 2: {
            IC3_Stop();
            break;
        }
        case 3: {
            IC4_Stop();
            break;
        }
    }
}

static uint16_t
ICCaptureDataRead(void *data) {
    
    uint16_t tempId = (uint16_t)data;

    switch (tempId) {
        case 0: {
            return IC1_CaptureDataRead();
        }
        case 1: {
            return IC2_CaptureDataRead();
        }
        case 2: {
            return IC3_CaptureDataRead();
        }
        case 3: {
            return IC4_CaptureDataRead();
        }
    }
    
    return 0;
}

static void 
ICSetDigitalOuputLow(void *data) {

    uint16_t tempId = (uint16_t)data;

    switch (tempId) {
        case 0: {
            /* IC1 */           
            TRISAbits.TRISA4 = 0; /* Set Digital Output */
            PORTAbits.RA4    = 0; /* Pull Low           */
            break;
        }
        case 1: {
            /* IC2 */
            TRISAbits.TRISA3 = 0; /* Set Digital Output */
            PORTAbits.RA3    = 0; /* Pull Low           */
            break;
        }
        case 2: {            
            /* IC3 */
            TRISBbits.TRISB4 = 0; /* Set Digital Output */
            PORTBbits.RB4    = 0; /* Pull Low           */
            break;
        }
        case 3: {
            /* IC4 */
            TRISAbits.TRISA8 = 0; /* Set Digital Output */
            PORTAbits.RA8    = 0; /* Pull Low           */
            break;
        }
    }    
}

static void 
ICSetDigitalInput(void *data) {

    uint16_t tempId = (uint16_t)data;

    switch (tempId) {
        case 0: {
            /* IC1 */           
            TRISAbits.TRISA4 = 1; /* Set Digital Input, line will be pulled HIGH */
            break;
        }
        case 1: {
            /* IC2 */
            TRISAbits.TRISA3 = 1; /* Set Digital Input, line will be pulled HIGH */
            break;
        }
        case 2: {
            /* IC3 */
            TRISBbits.TRISB4 = 1; /* Set Digital Input, line will be pulled HIGH */
            break;
        }
        case 3: {
            /* IC4 */
            TRISAbits.TRISA8 = 1; /* Set Digital Input, line will be pulled HIGH */
            break;
        }
    }    
}

static void
setFanOff(uint16_t pwmId) {
    
//    if (!cfg.fan[pwmId].enabled) return;    
    
    // In the case of a 4 PIN fan we want to stop the fan completely
    // This is down by pulling the line down and causing the FET to 
    // remove power.
    
    // In the case of a 3 PIN fan the PWM signal is applied to the 
    // FET controlling the power and regulating the fan speed.
    
    // Currently we assume 4 pin FAN and override the power to 
    // always OFF

// TODO
//    if (fan[id].Use3Pin) {
//
//       switch (id) {
//           case 0:
//               PWM_OverrideHighDisable(3);
//               break;
//           case 1:
//               PWM_OverrideHighDisable(1);
//               break;
//           case 2:
//               PWM_OverrideHighDisable(2);
//               break;
//       }
//
//   } else {    
    
        switch (pwmId) {
            case 0:
                
                PWM_OverrideHighEnable(3);
                PWM_OverrideDataHighSet(3, 0);
                break;
            case 1:
                PWM_OverrideHighEnable(1);
                PWM_OverrideDataHighSet(1, 0);
                break;
            case 2:

                PWM_OverrideHighEnable(2);
                PWM_OverrideDataHighSet(2, 0);
                break;
        }
//    }
}

static void
setFanOn(uint16_t pwmId) {
    
    if (!cfg.fan[pwmId].enabled) return;    

    // In the case of a 4 PIN fan we want to stop the fan completely
    // This is down by pulling the line up and causing the FET to 
    // apply power.
    
    // In the case of a 3 PIN fan the PWM signal is applied to the 
    // FET controlling the power and regulating the fan speed.
    
    // Currently we assume 4 pin FAN and override the power to 
    // always ON

    
// TODO
//    if (fan[id].Use3Pin) {
//
//       switch (id) {
//           case 0:
//               PWM_OverrideHighDisable(3);
//               break;
//           case 1:
//               PWM_OverrideHighDisable(1);
//               break;
//           case 2:
//               PWM_OverrideHighDisable(2);
//               break;
//       }
//
//   } else {    
        
        switch (pwmId) {
            case 0:
                
                PWM_OverrideHighEnable(3);
                PWM_OverrideDataHighSet(3, 1);
                break;
            case 1:

                PWM_OverrideHighEnable(1);
                PWM_OverrideDataHighSet(1, 1);
                break;
            case 2:

                PWM_OverrideHighEnable(2);
                PWM_OverrideDataHighSet(2, 1);
                break;
        }
//    }
}

static double 
pidGetInputValue(void *data) {

    uint16_t tempId = (int)data;
    
    return (double)dhtGetTemperature(tempId, 0);
}

uint16_t 
getMappedPWMId(uint16_t tempId) {

    uint16_t pwmId = (uint16_t)-1;
    
    /* Temp0 -> PWM1
     * Temp1 -> PWM2 
     * Temp2 -> PWM3 
     * Temp3 
     */    
    switch (tempId) {
        case 0: 
            pwmId = 0;
            break;
        case 1:
            pwmId = 1;
        case 2:
            pwmId = 2;
    }
    
    return pwmId;
}

static void
pidSetOutputValue(double outValue, void *data) {

    uint16_t tempId = (int)data;
    uint16_t pwmId  = getMappedPWMId(tempId);
    
    if (pwmId != (uint16_t)-1) {

        if (!cfg.fan[pwmId].enabled) return;    
        
        pwmSetDutyCycle(pwmId, (uint16_t)outValue);

        if (outValue < cfg.fan[pwmId].minDuty) {
            setFanOff(pwmId);
        } else {
            setFanOn(pwmId);        
        }
    }
}

static void 
monitorTemp(uint16_t tempId) {

    if (dhtGetTemperatureStatus(tempId) == DHTTEMPSUCCESS) pidRun(tempId);        
}

static void 
monitorAllTemps(void *data) {

    if (cfg.sensor[0].enabled) monitorTemp(TEMPID0);
    if (cfg.sensor[1].enabled) monitorTemp(TEMPID1);
    if (cfg.sensor[2].enabled) monitorTemp(TEMPID2);
    if (cfg.sensor[3].enabled) monitorTemp(TEMPID3);
}

static void 
logSensorData(uint16_t tempId) {

    /* Assumption that the TX ring buffer UART1_CONFIG_TX_BYTEQ_LENGTH is large
     * enough to hold log data for ALL sensors and that the data is drained by the
     * next time ALL sensors log any more data.
     * 
     * In this case we log 8 bytes * 4 sensors once a second.
     */
#define LOGDATABUFLEN 8
    
    uint8_t  buf[LOGDATABUFLEN];
    
    if (dhtGetTemperatureStatus(tempId) == DHTTEMPSUCCESS) {

                   //012345 6 7 
        memcpy(buf, "#:00.0\r\n", LOGDATABUFLEN);
        buf[0] = '0' + tempId;
        formatTemp((char *)&buf[2], dhtGetTemperature(tempId, 0));

    } else {        
                   //012345 6 7 
        memcpy(buf, "#:--.-\r\n", LOGDATABUFLEN);
        buf[0] = '0' + tempId;
    }
    
    UART1_WriteBuffer(buf, LOGDATABUFLEN);
}

static void 
logData(void *data) {

    if (cfg.sensor[0].enabled) logSensorData(TEMPID0);
    if (cfg.sensor[1].enabled) logSensorData(TEMPID1);
    if (cfg.sensor[2].enabled) logSensorData(TEMPID2);
    if (cfg.sensor[3].enabled) logSensorData(TEMPID3);
}

uint16_t
pwmGetDutyCycle(uint16_t pwmId) {
    
    switch(pwmId) { 
        case 0:
            return (uint16_t)((uint32_t)PDC3 * 100 / PWMPERIOD3);                
        case 1:
            return (uint16_t)((uint32_t)PDC1 * 100 / PWMPERIOD1);                
        case 2:
            return (uint16_t)((uint32_t)PDC2 * 100 / PWMPERIOD2);                
    }

    return 0;
}

static void 
pwmSetDutyCycle(uint16_t pwmId, uint32_t dutyCyclePercentage) {
        
    switch(pwmId) { 
        case 0:
            PWM_DutyCycleSet(3, (uint16_t)((uint32_t)PWMPERIOD3 * dutyCyclePercentage / 100));
            break;
        case 1:
            PWM_DutyCycleSet(1, (uint16_t)((uint32_t)PWMPERIOD1 * dutyCyclePercentage / 100));
            break;
        case 2:
            PWM_DutyCycleSet(2, (uint16_t)((uint32_t)PWMPERIOD2 * dutyCyclePercentage / 100));
            break;
    }
}

void 
IC1_CallBack(void) {

    /* Callback from IC1 Interrupt */
    dhtCaptureProcess(TEMPID0);
}

void 
IC2_CallBack(void) {

    /* Callback from IC2 Interrupt */
    dhtCaptureProcess(TEMPID1);
}

void 
IC3_CallBack(void) {

    /* Callback from IC3 Interrupt */
    dhtCaptureProcess(TEMPID2);
}

void 
IC4_CallBack(void) {

    /* Callback from IC4 Interrupt */
    dhtCaptureProcess(TEMPID3);
}

void 
CN_CallBack(void)
{
    /* Something happened to our pins */
    
    tachUpdate(TACH0_GetValue(),
               TACH1_GetValue(),
               TACH2_GetValue());            
}

static void
tachRead(void *data) {

    tachUpdateRPM();
}

static void 
videoSetCursor(uint16_t row, uint16_t col, void *data) {
    
    lcdSetCursor(row, col);
}

static void 
videoWriteChar(char c, void *data) {
    
    lcdWriteChar(c);
}

static uint8_t 
videoBusy(void *data) {
    
    // Callback used by video refresh to determine if the LCD is
    // available to receive data by checking the status of the busy flag
    // and the completion of any initialization that is required.
        
    if (LCDUploadState != LCDUploadComplete) {
        return 1;
    }
        
    return lcdBusy();
}

static void 
menuEncoderSet(uint16_t min, uint16_t max, uint16_t value, uint16_t step, void *data) {
    
    uint16_t id = (uint16_t)data;
    
    encoderSet(id, min, max, value, step);
}

static uint16_t programStatus[3];

static void
alarmSignal(uint16_t pwmId) {

    UART1_Write('0'+pwmId);    
    UART1_WriteBuffer((uint8_t *)":ALARM\r\n", 8);
    GPIOSetValue(pwmId, 1);
}

static void
alarmReset(uint16_t pwmId) {
    
    UART1_Write('0'+pwmId);    
    UART1_WriteBuffer((uint8_t *)":CLEAR\r\n", 8);
    GPIOSetValue(pwmId, 0);
}

#define PROGRAMOK              0x00
#define PROGRAMFANFAILED       0x01
#define PROGRAMFANDISABLED     0x02
#define PROGRAMSENSORFAILED    0x04
#define PROGRAMSENSORDISABLED  0x08

static uint16_t 
testProgramStatus(uint16_t id) {

    uint16_t status = PROGRAMOK;
    
    if ((cfg.program[id].enabled) && (cfg.program[id].alarm)) {
        
        uint16_t pwmId  = id;
        uint16_t tempId = id;

        if (cfg.fan[pwmId].enabled) {
            if ((pwmGetDutyCycle(pwmId) > 0) && (tachGetRPM(pwmId) == 0)) {                
                status |= PROGRAMFANFAILED;
            }                        
        } else {
            status |= PROGRAMFANDISABLED;
        }
        
        if (cfg.sensor[id].enabled) {            
            if (dhtGetTemperatureStatus(tempId) != DHTTEMPSUCCESS) {
                status |= PROGRAMSENSORFAILED;
            }
        } else {
            status |= PROGRAMSENSORDISABLED;
        }        
    }    
        
    return status;
}


static void
monitorAlarm(void *data) {
    
    /* Delay the start of this task by at least 5 seconds 
     */
    
    static uint16_t delayCount = 5;
    if (delayCount > 0) {
        delayCount--;
        return;
    }
    
    for (uint16_t i=0; i<3; i++) {

        uint16_t status = testProgramStatus(i);
        if (status != programStatus[i]) {

            if (status == PROGRAMOK) {
                alarmReset(i);
            } else {
                alarmSignal(i);
            }
            
            programStatus[i] = status;
        }
    }    
}



int 
main(void)
{            
    // Hardware Initialisation for dsPIC
    // MCU using 20MHz crystal, configured with 140MHz FOSC ( 70 MIPS )
    //
    // 4 Input Capture devices for interfacing with temp sensors supporting DHT11 and DHT22
    // 3 2-Channel PWM devices running at 25KHz for controlling 4pin PWN fans, 
    //   also support for 3pin and 2pin fans using PWM on power pin if required
    // 1 UART for communication, loggin with a host, configured at 9600 8N1 
    // 12 GPIOs - 2 dedicated to rotary encoder ( GPIO4, GPIO5 )
    //            1 dedicated to a button ( GPIO3 )
    //            3 Inputs set to interrupt on change reading TACH from fans to calculate RPM
    //            3 Output to FETs controlling FAN 12V input in the case of 3pin and 2pin fans
    //              and for power off control of 4pin fans
    //            3 GPIOS GPIO0, GPIO1, GPIO2 used to signal alarms for programs with failed fans, sensors
    // 3 Timers - TMR1 used as system timer used to monitor 10ms tick for scheduling
    //            TMR3 used by PWM hardware
    //            TMR4 used as high frequency timer with 7.3us tick
       
    CONFIGASSERT();


    // initialize the device
    SYSTEM_Initialize();

    // Get stored config data from flash or generate default
    configRead(&cfg);
        
    
    // Set GPIO0, GPIO1 and GPIO2 to digital outputs to be used
    // as alarms to signal when fans are not rotating
    
    GPIOSetType(0, GPIODigital);
    GPIOSetDirection(0, GPIOOutput);
    GPIOSetValue(0, 0);

    GPIOSetType(1, GPIODigital);
    GPIOSetDirection(1, GPIOOutput);
    GPIOSetValue(1, 0);

    GPIOSetType(2, GPIODigital);
    GPIOSetDirection(2, GPIOOutput);
    GPIOSetValue(2, 0);
        
    // Start with ALL fans off    
    setFanOff(0);
    setFanOff(1);
    setFanOff(2);

    // Video bringup    
    videoBufferInit(LCDROWS, LCDCOLS, videoBuffer, videoSetCursor, videoWriteChar, videoBusy, (void *)0);
    videoBufferClear();
        
    // Start bringing up the LCD    
    lcdInit(LCDROWS, LCDCOLS, LCDRWSet, LCDRSSet, LCDWriteDB, LCDReadDB, LCDESet, (void *)0);
        
    // Setup HAL for temp sensor capture hardware
    if (cfg.sensor[0].enabled) {
        dhtInit(0, cfg.sensor[0].type, ICInit, ICCaptureDataRead, ICStop, ICSetDigitalOuputLow, ICSetDigitalInput, (void *)TEMPID0);         
    }
    
    if (cfg.sensor[1].enabled) {
        dhtInit(1, cfg.sensor[1].type, ICInit, ICCaptureDataRead, ICStop, ICSetDigitalOuputLow, ICSetDigitalInput, (void *)TEMPID1); 
    }
    
    if (cfg.sensor[2].enabled) {
        dhtInit(2, cfg.sensor[2].type, ICInit, ICCaptureDataRead, ICStop, ICSetDigitalOuputLow, ICSetDigitalInput, (void *)TEMPID2); 
    }
    
    if (cfg.sensor[3].enabled) {
        dhtInit(3, cfg.sensor[3].type, ICInit, ICCaptureDataRead, ICStop, ICSetDigitalOuputLow, ICSetDigitalInput, (void *)TEMPID3); 
    }    
        
    // Startup all tasks with the exception of those that are delayed, they
    // are scheduled separately
    schedulerProcessAdd(&readTemperatureDelayProcess);
    schedulerProcessAdd(&monitorAllTempsProcess);
    schedulerProcessAdd(&userInterfaceProcess);
    schedulerProcessAdd(&buttonDebounceProcess);
    schedulerProcessAdd(&buttonPollProcess);
    schedulerProcessAdd(&encoderPollProcess);
    schedulerProcessAdd(&updateLCDInitProcess);                
    schedulerProcessAdd(&tachReadProcess);
    schedulerProcessAdd(&logDataProcess);
    schedulerProcessAdd(&monitorAlarmProcess);
#ifdef BLINK7BIT
    schedulerProcessAdd(&videoBlinkProcess);
#endif    
            
    // Encoder available to GPIO4 and GPIO5, set initial value, limits and step
    
#define ENCODERMIN   0    
#define ENCODERMAX   0
#define ENCODERVALUE 0
#define ENCODERSTEP  1
    
    encoderInit(ENCODERID, ENCODERGPIOB, ENCODERGPIOA, encoderCallback, (void *)ENCODERID, ENCODER_HALFSTEP);
    encoderSet(ENCODERID, ENCODERMIN, ENCODERMAX, ENCODERVALUE, ENCODERSTEP);
    encoderEnable(ENCODERID);

    // Button on GPIO3 ( encoder pushbutton )
    buttonEnable(BUTTONGPIOID, buttonCallback, (void *)0, 0);

    
    // Init a PID controller per TempSensor/FanPWM pair
    // NOTE - TempSensor3 has no associated FanPWM
        
    if (cfg.program[0].enabled) {        
        pidInit(0, cfg.program[0].targetTemp, cfg.program[0].P, cfg.program[0].I, pidGetInputValue, pidSetOutputValue, cfg.fan[0].minDuty, cfg.fan[0].maxDuty, ICInit, (void *)TEMPID0);
    }

    if (cfg.program[1].enabled) {
        pidInit(1, cfg.program[1].targetTemp, cfg.program[1].P, cfg.program[1].I, pidGetInputValue, pidSetOutputValue, cfg.fan[1].minDuty, cfg.fan[1].maxDuty, ICInit, (void *)TEMPID1);        
    }

    if (cfg.program[2].enabled) {
        pidInit(2, cfg.program[2].targetTemp, cfg.program[2].P, cfg.program[2].I, pidGetInputValue, pidSetOutputValue, cfg.fan[2].minDuty, cfg.fan[2].maxDuty, ICInit, (void *)TEMPID2);        
    }
    
    menuInit(menuEncoderSet, (void *)0, videoBlinkDelay, (void *)&videoBlinkProcess);

    while (1) {
        
        schedulerRun();
    }
    
    return 1; 
}
/**
 End of File
*/

