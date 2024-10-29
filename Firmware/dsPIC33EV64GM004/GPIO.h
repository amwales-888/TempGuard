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

#ifndef GPIO_H
#define	GPIO_H

//#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#define MAXGPIOS 6

/* GPIO Id Map
 * 
 * GPIO0 -> A0
 * GPIO1 -> A1
 * GPIO2 -> A9
 * GPIO3 -> C0
 * GPIO4 -> C1
 * GPIO5 -> C2
 * 
 */

enum GPIODirection_e {
    
    GPIOOutput=0,  
    GPIOInput,
};

enum GPIOType {
    
    GPIODigital=0,  
    GPIOAnalog,
};

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t GPIOGetValue(uint16_t id);
void GPIOSetValue(uint16_t id, uint8_t value);
void GPIOSetDirection(uint16_t id, enum GPIODirection_e direction);
void GPIOSetType(uint16_t id, enum GPIOType type);
void GPIOToggleValue(uint16_t id);
void GPIOEnablePullDown(uint16_t id);
void GPIODisablePullDown(uint16_t id);
void GPIOEnablePullUp(uint16_t id);
void GPIODisablePullUp(uint16_t id);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* GPIO_H */

