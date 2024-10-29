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


#ifndef ENCODER_H
#define	ENCODER_H

//#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#include "GPIO.h"

#define MAXENCODERS (MAXGPIOS/2)

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
    
#define ENCODER_HALFSTEP 0x1
#define ENCODER_PULLDOWN 0x2
#define ENCODER_PULLUP   0x4    

void encoderEnable(uint16_t id);
void encoderDisable(uint16_t id);
uint16_t encoderGetEnabled(uint16_t id);

void encoderInit(uint16_t id, 
                 uint8_t gpioIdA,
                 uint8_t gpioIdB,
                 void (* func)(uint16_t id, uint16_t value, void *data),
                 void *data, 
                 uint16_t flags);

void encoderSet(uint16_t id, uint16_t min, uint16_t max, uint16_t value, uint16_t step);
void encoderUpdateChanged(void);
uint16_t encoderGetValue(uint16_t id);
void encoderSetValue(uint16_t id, uint16_t value);
void encoderUpdate(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* ENCODER_H */

