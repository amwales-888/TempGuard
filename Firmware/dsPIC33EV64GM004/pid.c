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

#include "pid.h"

static struct pid_s pid[MAXPIDS];

void pidInit(uint16_t id,
        double desiredValue,
        double P,        
        double I, 
        double (*getInputValue)(void *data), 
        void (*setOutputValue)(double outValue, void *data), 
        double minOuputValue,
        double maxOuputValue,        
        void (*ICInit)(void *data),
        void *data) { 

    struct pid_s *p = &pid[id];

    p->enabled = 1;
    p->desiredValue = desiredValue;
    p->P = P;
    p->I = I;
    p->sum = 0;

    p->getInputValue = getInputValue;
    p->setOutputValue = setOutputValue;
    p->minOuputValue = minOuputValue;
    p->maxOuputValue = maxOuputValue;
    p->data = data;
}

void pidRun(uint16_t id) {
    
    struct pid_s *p = &pid[id];
    
    if (p->enabled) {
        
        double inputValue = p->getInputValue(p->data);
        double diff = inputValue - p->desiredValue;
        double sum = p->sum + diff;
        double pDiff = diff * p->P;
        double iDiff = sum * p->I;
        double outputValue = pDiff + iDiff;
        double maxOuputValue = p->maxOuputValue;
        double minOuputValue = p->minOuputValue;
        
        if (outputValue > maxOuputValue) {
            outputValue = maxOuputValue;
        }

        if (outputValue < minOuputValue) {
            outputValue = minOuputValue;
        }

        p->sum = sum;
        
        if (sum > maxOuputValue) {
            p->sum = maxOuputValue;
        }

        if (sum < -maxOuputValue) {
            p->sum = -maxOuputValue;
        }

        p->setOutputValue(outputValue, p->data);
    }
}

void pidEnable(uint16_t id) {
    pid[id].enabled = 1;
}

void pidDisable(uint16_t id) {
    pid[id].enabled = 0;
}

void pidReset(uint16_t id) {    
    pid[id].sum = 0;
}
