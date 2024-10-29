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

#include <xc.h>
#include <stdint.h>

#include "mcc_generated_files/pin_manager.h"
#include "GPIO.h"

void
GPIOEnablePullDown(uint16_t id) {

    switch (id) {
        case 0:
            CNPDA |= (1 << 0); // Enable Pull-down A0
            return;
        case 1:
            CNPDA |= (1 << 1); // Enable Pull-down A1
            return;
        case 2:
            CNPDA |= (1 << 9); // Enable Pull-down A9
            return;
        case 3:
            CNPDC |= (1 << 0); // Enable Pull-down C0
            return;
        case 4:
            CNPDC |= (1 << 1); // Enable Pull-down C1
            return;
        case 5:
            CNPDC |= (1 << 2); // Enable Pull-down C2
            return;
    }
}

void
GPIOEnablePullUp(uint16_t id) {

    switch (id) {
        case 0:
            CNPUA |= (1 << 0); // Enable Pull-up A0
            return;
        case 1:
            CNPUA |= (1 << 1); // Enable Pull-up A1
            return;
        case 2:
            CNPUA |= (1 << 9); // Enable Pull-up A9
            return;
        case 3:
            CNPUC |= (1 << 0); // Enable Pull-up C0
            return;
        case 4:
            CNPUC |= (1 << 1); // Enable Pull-up C1
            return;
        case 5:
            CNPUC |= (1 << 2); // Enable Pull-up C2
            return;
    }
}

void
GPIODisablePullDown(uint16_t id) {
    
    switch (id) {
        case 0:
            CNPDA &= ~(1 << 0); // Disable Pull-down A0
            return;
        case 1:
            CNPDA &= ~(1 << 1); // Disable Pull-down A1
            return;
        case 2:
            CNPDA &= ~(1 << 9); // Disable Pull-down A9
            return;
        case 3:
            CNPDC &= ~(1 << 0); // Disable Pull-down C0
            return;
        case 4:
            CNPDC &= ~(1 << 1); // Disable Pull-down C1
            return;
        case 5:
            CNPDC &= ~(1 << 2); // Disable Pull-down C2
            return;
    }
}

void
GPIODisablePullUp(uint16_t id) {
    
    switch (id) {
        case 0:
            CNPUA &= ~(1 << 0); // Disable Pull-up A0
            return;
        case 1:
            CNPUA &= ~(1 << 1); // Disable Pull-up A1
            return;
        case 2:
            CNPUA &= ~(1 << 9); // Disable Pull-up A9
            return;
        case 3:
            CNPUC &= ~(1 << 0); // Disable Pull-up C0
            return;
        case 4:
            CNPUC &= ~(1 << 1); // Disable Pull-up C1
            return;
        case 5:
            CNPUC &= ~(1 << 2); // Disable Pull-up C2
            return;
    }
}

uint8_t
GPIOGetValue(uint16_t id) {

    switch (id) {
        case 0:
            return GPIO0_GetValue();
        case 1:
            return GPIO1_GetValue();
        case 2:
            return GPIO2_GetValue();
        case 3:
            return GPIO3_GetValue();
        case 4:
            return GPIO4_GetValue();
        case 5:
            return GPIO5_GetValue();
    }

    return 0;
}

void
GPIOSetValue(uint16_t id, uint8_t value) {

    if (value) {
        switch (id) {
            case 0:
                GPIO0_SetHigh();
                return;
            case 1:
                GPIO1_SetHigh();
                return;
            case 2:
                GPIO2_SetHigh();
                return;
            case 3:
                GPIO3_SetHigh();
                return;
            case 4:
                GPIO4_SetHigh();
                return;
            case 5:
                GPIO5_SetHigh();
                return;
        } 
        
    } else {
        
        switch (id) {
            case 0:
                GPIO0_SetLow();
                return;
            case 1:
                GPIO1_SetLow();
                return;
            case 2:
                GPIO2_SetLow();
                return;
            case 3:
                GPIO3_SetLow();
                return;
            case 4:
                GPIO4_SetLow();
                return;
            case 5:
                GPIO5_SetLow();
                return;
        }        
    }
}

void
GPIOToggleValue(uint16_t id) {

    switch (id) {
        case 0:
            GPIO0_Toggle();
            return;
        case 1:
            GPIO1_Toggle();
            return;
        case 2:
            GPIO2_Toggle();
            return;
        case 3:
            GPIO3_Toggle();
            return;
        case 4:
            GPIO4_Toggle();
            return;
        case 5:
            GPIO5_Toggle();
            return;
    } 
        
}

void
GPIOSetDirection(uint16_t id, enum GPIODirection_e direction) {

    if (direction == GPIOOutput) {        
        
        switch (id) {
            case 0:
                GPIO0_SetDigitalOutput();
                return; 
            case 1:
                GPIO1_SetDigitalOutput();
                return; 
            case 2:
                GPIO2_SetDigitalOutput();
                return; 
            case 3:
                GPIO3_SetDigitalOutput();
                return; 
            case 4:
                GPIO4_SetDigitalOutput();
                return; 
            case 5:
                GPIO5_SetDigitalOutput();
                return; 
        }
        
    } else {

        switch (id) {
            case 0:
                GPIO0_SetDigitalInput();
                return; 
            case 1:
                GPIO1_SetDigitalInput();
                return; 
            case 2:
                GPIO2_SetDigitalInput();
                return; 
            case 3:
                GPIO3_SetDigitalInput();
                return; 
            case 4:
                GPIO4_SetDigitalInput();
                return; 
            case 5:
                GPIO5_SetDigitalInput();
                return; 
        }        
    }
}

void
GPIOSetType(uint16_t id, enum GPIOType type) {
        
    switch (id) {
        case 0:
            ANSELAbits.ANSA0 = type;
            return;
        case 1:
            ANSELAbits.ANSA1 = type;
            return;
        case 2:
            ANSELAbits.ANSA9 = type;
            return;
        case 3:
            ANSELCbits.ANSC0 = type;
            return;
        case 4:
            ANSELCbits.ANSC1 = type;
            return;
        case 5:
            ANSELCbits.ANSC2 = type;
            return;
    }            
}
