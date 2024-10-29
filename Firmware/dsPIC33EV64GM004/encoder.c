/* 
 *  Copyright (c) 2023 Angelo Masci
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

/*  Statemachine ported from python version found with MIT license
 *  see: https://github.com/miketeachman/micropython-rotary 
 */

#include <stdint.h>

#include "GPIO.h"
#include "encoder.h"

struct encoder_s {

    uint16_t spare:13;
	uint16_t enabled:1;
    uint16_t halfStep:1;
    uint16_t changed:1;
    
    uint8_t gpioIdA;
    uint8_t gpioIdB;
    
	uint16_t min;
	uint16_t max;
	uint16_t value;
    uint16_t step;	
	uint16_t state;
    
    void (* func)(uint16_t id, uint16_t value, void *data);
    void *data;
};

static struct encoder_s encoders[MAXENCODERS];

// Encoder direction
#define DIR_CW    0x10 /* Clockwise step */
#define DIR_CCW   0x20 /* Counter-clockwise step */

// Rotary Encoder States
#define R_START   0x0
#define R_CW_1    0x1
#define R_CW_2    0x2
#define R_CW_3    0x3
#define R_CCW_1   0x4
#define R_CCW_2   0x5
#define R_CCW_3   0x6
#define R_ILLEGAL 0x7

static uint8_t stateTable[8][4] = {

    // |------------- NEXT STATE -------------|            |CURRENT STATE|
    // CLK/DT    CLK/DT     CLK/DT    CLK/DT
    //   00        01         10        11
    {R_START, R_CCW_1, R_CW_1,  R_START},             // R_START
    {R_CW_2,  R_START, R_CW_1,  R_START},             // R_CW_1
    {R_CW_2,  R_CW_3,  R_CW_1,  R_START},             // R_CW_2
    {R_CW_2,  R_CW_3,  R_START, R_START | DIR_CW},    // R_CW_3
    {R_CCW_2, R_CCW_1, R_START, R_START},             // R_CCW_1
    {R_CCW_2, R_CCW_1, R_CCW_3, R_START},             // R_CCW_2
    {R_CCW_2, R_START, R_CCW_3, R_START | DIR_CCW},   // R_CCW_3
    {R_START, R_START, R_START, R_START},             // R_ILLEGAL

};

static uint8_t stateTableHalfStep[8][4] = {
    {R_CW_3,            R_CW_2,  R_CW_1,  R_START},
    {R_CW_3 | DIR_CCW,  R_START, R_CW_1,  R_START},
    {R_CW_3 | DIR_CW,   R_CW_2,  R_START, R_START},
    {R_CW_3,            R_CCW_2, R_CCW_1, R_START},
    {R_CW_3,            R_CW_2,  R_CCW_1, R_START | DIR_CW},
    {R_CW_3,            R_CCW_2, R_CW_3,  R_START | DIR_CCW},
    {R_START,           R_START, R_START, R_START},
    {R_START,           R_START, R_START, R_START},

};

#define STATE_MASK 0x07
#define DIR_MASK   0x30


void 
encoderUpdate(void) {

	struct encoder_s *encoder = encoders;
	uint16_t id;

	for (id = 0; id < MAXENCODERS; id++) {

		if (encoder->enabled) {
		
            uint8_t  gpioIdA = encoder->gpioIdA;
            uint8_t  gpioIdB = encoder->gpioIdB;            
            uint16_t pins    = (GPIOGetValue(gpioIdA) << 1) | GPIOGetValue(gpioIdB);

            // Determine next state
            if (encoder->halfStep) {
                encoder->state = stateTableHalfStep[encoder->state & STATE_MASK][pins];
            } else {
                encoder->state = stateTable[encoder->state & STATE_MASK][pins];
            }
                       
            switch (encoder->state & DIR_MASK) {
                case DIR_CW: {
                   
                    encoder->value += encoder->step;
                    if (encoder->value > encoder->max) {
                        encoder->value = encoder->max;
                    }
                    
                    encoder->changed = 1;
                    break;
                }
                case DIR_CCW: {
                    
                    encoder->value -= encoder->step;
                    if (encoder->value < encoder->min) {
                        encoder->value = encoder->min;
                    }
                    
                    encoder->changed = 1;
                    break;
                }
            }
		}
            
        encoder++;		
    }
}

void
encoderUpdateChanged(void) {

    struct encoder_s *encoder = encoders;
	uint16_t id;

	for (id = 0; id < MAXENCODERS; id++) {
		if (encoder->enabled) {
            if (encoder->changed) {
                encoder->func(id, encoder->value, encoder->data);
                encoder->changed = 0;
            }
        }

        encoder++;		
    }    
}


void 
encoderDisable(uint16_t id) {

	struct encoder_s *encoder = &encoders[id];    
    uint8_t gpioIdA = encoder->gpioIdA;
    uint8_t gpioIdB = encoder->gpioIdB;
    
    GPIODisablePullDown(gpioIdA);
    GPIODisablePullUp(gpioIdA);

    GPIODisablePullDown(gpioIdB);
    GPIODisablePullUp(gpioIdB);
        
	encoder->enabled = 0;
}

void
encoderInit(uint16_t id, 
            uint8_t gpioIdA,
            uint8_t gpioIdB,
            void (* func)(uint16_t id, uint16_t value, void *data),
            void *data, 
            uint16_t flags) {
            
	struct encoder_s *encoder = &encoders[id];
        
    encoder->gpioIdA = gpioIdA;
    encoder->gpioIdB = gpioIdB;
    
    GPIOSetType(gpioIdA, GPIODigital);
    GPIOSetDirection(gpioIdA, GPIOInput);
    
    if ((flags & ENCODER_PULLUP) == ENCODER_PULLUP) {
        GPIOEnablePullUp(gpioIdA);
    }

    if ((flags & ENCODER_PULLDOWN) == ENCODER_PULLDOWN) {    
        GPIOEnablePullDown(gpioIdA);
    }
    
    GPIOSetType(gpioIdB, GPIODigital);
    GPIOSetDirection(gpioIdB, GPIOInput);

    if ((flags & ENCODER_PULLUP) == ENCODER_PULLUP) {
        GPIOEnablePullUp(gpioIdB);
    }

    if ((flags & ENCODER_PULLDOWN) == ENCODER_PULLDOWN) { 
        GPIOEnablePullDown(gpioIdB);
    }

    encoder->func     = func;
    encoder->data     = data;
    encoder->state    = 0;
	encoder->enabled  = 0;
    encoder->halfStep = (flags & ENCODER_HALFSTEP);    
}        



void 
encoderEnable(uint16_t id) {

	encoders[id].enabled = 1;
}

uint16_t
encoderGetEnabled(uint16_t id) {
    
    return encoders[id].enabled;
}

uint16_t 
encoderGetValue(uint16_t id) {
    
    return encoders[id].value;
}

void 
encoderSetValue(uint16_t id, uint16_t value) {
    
    encoders[id].value = value;
}

void 
encoderSet(uint16_t id,
           uint16_t min, 
           uint16_t max, 
           uint16_t value, 
		   uint16_t step) {
				
	struct encoder_s *encoder = &encoders[id];

 	encoder->min      = min;
	encoder->max      = max;
	encoder->value    = value;
	encoder->step     = step;
}


