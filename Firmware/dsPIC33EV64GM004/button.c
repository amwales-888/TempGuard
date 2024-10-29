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

#include <stdint.h>

#include "GPIO.h"
#include "button.h"

struct button_s {

  uint8_t  integrator;
  uint16_t count;

  uint8_t  enabled:1;
  uint8_t  dstate:1;
  uint8_t  report:1;
  
  enum buttonState_e state:2;  
    
  void (*func)(uint8_t id, enum buttonState_e state, void *data); 
  void *data;  
};

static struct button_s buttons[MAXBUTTONS];

void 
buttonUpdateIntegrator(void) {

    // This function MUST be called every 1/DEBOUNCE_SAMPLE_FREQUENCY seconds
        
    struct button_s *button = buttons;
    uint16_t id;
    
    for (id = 0; id < MAXBUTTONS; id++) {
    
        if (button->enabled) {
            if (GPIOGetValue(id) == 0) {                
                if (button->integrator > 0) {                
                    button->integrator--;
                }                               
            } else if (button->integrator < DEBOUNCE_MAXIMUM) {
                button->integrator++;
            }
        }
        
        button++;
    }    
}

void 
buttonUpdate(void) {

    // This function MUST be called every DEBOUNCE_TIME seconds

    struct button_s *button = buttons;
    uint16_t id;
    
    for (id = 0; id < MAXBUTTONS; id++) {
    
        if (button->enabled) {
                        
            if  (button->integrator == DEBOUNCE_MAXIMUM) {
                if (button->dstate == 0) {
                    
                    button->state = BUTTONPRESSED;  
                    button->report = 1;
                    
                } else {
                    
                    button->state = BUTTONHELD;  
                    button->report = 1;
                }
                
                button->count++;
                button->dstate = 1;
                
            } else if (button->integrator == 0) {
                
                if (button->dstate) {
                    
                    /* Released */
                    button->state = BUTTONRELEASED;  
                    button->report = 1;

                } else {
                    
                    button->state = BUTTONIDLE;                      
                }
                
                button->count  = 0;
                button->dstate = 0;
            } 
        }

        button++;
    }

    /* Resolve any callback */
        
    button = buttons;
    for (id = 0; id < MAXBUTTONS; id++) {
    
        if (button->enabled) {
            if (button->report) {
                if (button->func) {
                    (*button->func)(id, button->state, button->data);
                }                
                button->report = 0;
            }
        }

        button++;
    }
}

uint16_t
buttonGetState(uint16_t id) {
    
    return buttons[id].dstate;
}

uint16_t
buttonGetCount(uint16_t id) {
    
    return buttons[id].count;
}

void
buttonEnable(uint16_t id, 
        void (*func)(uint8_t id, enum buttonState_e state, void *data),
        void *data, uint16_t flags) {

    GPIOSetType(id, GPIODigital);
    GPIOSetDirection(id, GPIOInput);

    if ((flags & BUTTON_PULLUP) == BUTTON_PULLUP) {
        GPIOEnablePullUp(id);
    }

    if ((flags & BUTTON_PULLDOWN) == BUTTON_PULLDOWN) {    
        GPIOEnablePullDown(id);
    }
        
    buttons[id].dstate     = 0;
    buttons[id].integrator = 0;
    buttons[id].count      = 0;        
    buttons[id].func       = func;
    buttons[id].data       = data;
    buttons[id].enabled    = 1;
}

void
buttonDisable(uint16_t id) {

    GPIODisablePullDown(id);
    GPIODisablePullUp(id);

    buttons[id].enabled = 0;
}

uint16_t 
buttonGetEnabled(uint16_t id) {
    
    return buttons[id].enabled;
}

