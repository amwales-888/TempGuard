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

#ifndef MENU_H
#define	MENU_H

//#include <xc.h>
#include <stdint.h>
#include "videoBuffer.h"


#define LCDCHARRIGHTARROW  0
#define LCDCHARLEFTARROW   1
#define LCDCHARUPARROW     2
#define LCDCHARDOWNARROW   3
#define LCDCHARUPDOWNARROW 4

struct itemList_s {
    
    const uint16_t col;
    const uint16_t width;

    uint16_t * const value;
    const uint16_t count;
    const char * const *names;
    uint16_t (* const getValue)(void);
    void (* const setValue)(uint16_t value);
};

struct itemInteger_s {
    
    const uint16_t col;
    const uint16_t width;
    
    uint16_t * const value;
    const uint16_t min;
    const uint16_t max;
    const uint16_t step;
    uint16_t (* const getValue)(void);
    void (* const setValue)(uint16_t value);
};

struct itemFloatingPoint_s {
    
    const uint16_t col;
    const uint16_t width;
    
    uint16_t * const p1;
    uint16_t * const p2;
    
    double * const value;
    
    const uint16_t min;
    const uint16_t max;
    const uint16_t step;

    double (* const getValue)(void);
    void (* const setValue)(double value);
};

struct itemContainerArg_s {
    
    const uint16_t menu;
    uint16_t (* const buttonPressed)(void *data);
    void * const data;
};

struct itemDialog_s {
    
    const char *line0;
    const char *line1;
    const char *line2;
    
    const uint16_t menu;
    const uint16_t menuDone;    
    void (* const close)(uint16_t value, void *data);
    void * const data;
    uint16_t * const value;    
    void (* const displayFunc)(void);
};

enum itemType_e {
    
    ITEMCONTAINER=0,
    ITEMCONTAINERARG,
    ITEMINTEGER,
    ITEMLIST,
    ITEMDOUBLE,
    ITEMDIALOG,
};

struct menu_s;

struct item_s {
    
    const char *name;
    const enum itemType_e type;
    const void *iptr;
};

struct menu_s {

    const struct item_s *items;
    const uint16_t numItems;
    void (* const displayFunc)(void);    
};

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */


extern struct menu_s *currMenu;

#ifdef BLINK7BIT
void menuInit(void (*encoderSet)(uint16_t min, uint16_t max, uint16_t value, uint16_t step, void *encoderdData), 
              void *encoderdData,
              void (*videoBlinkDelay)(void *videoBlinkDelayData),
              void *videoBlinkDelayData);
#else
void menuInit(void (*encoderSet)(uint16_t min, uint16_t max, uint16_t value, uint16_t step, void *encoderdData), 
              void *encoderdData);
#endif

void menuDisplay(void);
void menuButtonPressed(void);
void menuEncoderChanged(uint16_t value);
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* MENU_H */

