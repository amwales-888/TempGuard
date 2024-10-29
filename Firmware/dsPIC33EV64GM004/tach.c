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

#include <stdint.h>

#include "GPIO.h"
#include "tach.h"

volatile static uint16_t tachCount0;
volatile static uint16_t tachCount1;
volatile static uint16_t tachCount2;

static uint16_t lastTach0;
static uint16_t lastTach1;
static uint16_t lastTach2;
static uint16_t tachRPM0;
static uint16_t tachRPM1;
static uint16_t tachRPM2;

uint16_t 
tachGetRPM(uint16_t id) {
    
    switch (id) {
        case 0:
            return tachRPM0;
        case 1:
            return tachRPM1;
        case 2:
            return tachRPM2;
    }

    return 0;
}

void
tachUpdate(uint16_t value0, 
           uint16_t value1,
           uint16_t value2) {
    
    /* Keep count of falling edges */
    
    if (value0 != lastTach0) {
        if (value0 == 0) {
            tachCount0++;
        }
    }

    if (value1 != lastTach1) {
        if (value1 == 0) {
            tachCount1++;
        }
    }

    if (value2 != lastTach2) {
        if (value2 == 0) {
            tachCount2++;
        }
    }
    
    lastTach0 = value0;
    lastTach1 = value1;
    lastTach2 = value2;    
}

void
tachUpdateRPM(void) {
    
    /* This function MUST be called every 250ms */

    /* Frequency = count * (1000 / 250) 
     *           = count * 4
     * 
     * Sensor on fan triggers 2 events per
     * revolution
     * 
     * EPR       = 2
     * 
     * RPM       = Freq * 60 / 2
     * 
     * Simplified
     * 
     * RPM       = count * 4 * 60 / 2 
     *           = count * 120
     */
    
    tachRPM0 = tachCount0 * 120;
    tachCount0 = 0;
    tachRPM1 = tachCount1 * 120;
    tachCount1 = 0;
    tachRPM2 = tachCount2 * 120;
    tachCount2 = 0;

    // If the value of tacRPM is zero and the FAN is 
    // set to on then the user needs to be alerted of a fault.    
    //TODO Alert    
}
