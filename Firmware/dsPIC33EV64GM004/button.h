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

/* Using the debounce algorithm from Kenneth A. Kuhn
 * https://www.kennethkuhn.com/electronics/debounce.c
 */

#ifndef BUTTON_H
#define	BUTTON_H

//#include <xc.h>
#include <stdint.h>

#include "GPIO.h"

#define DEBOUNCE_TIME               0.05    /* Seconds */
#define DEBOUNCE_SAMPLE_FREQUENCY	100     /* Hertz */

#define DEBOUNCE_MAXIMUM			(DEBOUNCE_TIME * DEBOUNCE_SAMPLE_FREQUENCY)
#define DEBOUNCE_INTEGRATOR_DELAYMS ((1.0/DEBOUNCE_SAMPLE_FREQUENCY)*1000)
#define DEBOUNCE_DELAYMS            (DEBOUNCE_TIME * 1000)

enum buttonState_e {
    
    BUTTONIDLE=0,
    BUTTONPRESSED,
    BUTTONHELD,
    BUTTONRELEASED,    
};

#define MAXBUTTONS MAXGPIOS

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

uint16_t buttonGetState(uint16_t id);
uint16_t buttonGetCount(uint16_t id);

#define BUTTON_PULLUP   0x1
#define BUTTON_PULLDOWN 0x2

void buttonEnable(uint16_t id, 
        void (*func)(uint8_t id, enum buttonState_e state, void *data),
        void *data, uint16_t flags);

void buttonDisable(uint16_t id);
uint16_t buttonGetEnabled(uint16_t id);
void buttonUpdateIntegrator(void);
void buttonUpdate(void);


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* BUTTON_H */

