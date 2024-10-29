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

#ifndef PID_H
#define	PID_H

#include <stdint.h>

#define MAXPIDS 4

struct pid_s {
    
    uint8_t enabled;
    
    double desiredValue;
    double P;
    double I;
    double sum;
        
    double (*getInputValue)(void *data);
    void (*setOutputValue)(double outValue, void *data);
    double minOuputValue;
    double maxOuputValue;
    void *data;
};

#ifdef	__cplusplus
extern "C" {
#endif

void pidInit(uint16_t id,
        double desiredValue,
        double P,        
        double I, 
        double (*getInputValue)(void *data), 
        void (*setOutputValue)(double outValue, void *data), 
        double minOuputValue,
        double maxOuputValue,        
        void (*ICInit)(void *data),
        void *data);

void pidRun(uint16_t id);
void pidEnable(uint16_t id);
void pidDisable(uint16_t id);
void pidReset(uint16_t id);   

#ifdef	__cplusplus
}
#endif

#endif	/* PID_H */

