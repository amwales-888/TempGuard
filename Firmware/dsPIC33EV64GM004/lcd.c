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

#include "delay.h" // Declare FCY needed by delay32 ensure theis is changed in
                   // clock is changed

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>

#include "lcd.h"

enum lcdInitState_e {
    
    LCD_INIT_8BIT1 = 0,
    LCD_INIT_8BIT2,
    LCD_INIT_8BIT3,
    LCD_INIT_CLEAR,
    LCD_INIT_FUNCTION,
    LCD_INIT_DISPLAY,
    LCD_INIT_ENTRY,
    LCD_INIT_COMPLETE,
    
};

static uint16_t lcdRows;
static uint16_t lcdCols;

static void (*userRWSet)(uint8_t value, void *data);
static void (*userRSSet)(uint8_t value, void *data);
static void (*userWriteDB)(uint8_t byte, void *data);
static uint8_t (*userReadDB)(void *data);
static void (*userESet)(uint8_t value, void *data);
static void *userData;


static void lcdWriteByte(uint8_t byte, enum lcdMode_e mode);


void 
lcdWriteChar(char c) {
    
    lcdWriteByte((uint8_t)c, LCDDATA);
}

#define LCDRS 0x2
#define LCDRW 0x1

static void 
lcdWriteByte(uint8_t byte, enum lcdMode_e mode) {

    userRSSet((mode & LCDRS), userData);
    userRWSet((mode & LCDRW), userData);    
              
    userWriteDB(byte, userData);
    
    Nop(); // 2*14ns Delay
    Nop();
    
    // Toggle Enable
    userESet(1, userData);

    Nop(); // 2*14ns Delay
    Nop();

    userESet(0, userData);

    Nop(); // 2*14ns Delay
    Nop();
    Nop(); // 2*14ns Delay
    Nop();
    Nop(); // 2*14ns Delay
    Nop();
    Nop(); // 2*14ns Delay
    Nop();
    Nop(); // 2*14ns Delay
    Nop();
}

uint8_t
lcdBusy(void) {
    
    uint8_t byte;
       
    userRSSet(0, userData);
    userRWSet(1, userData);
    
    Nop(); // 2*14ns Delay
    Nop();
    
    // Toggle Enable note setup delay
    userESet(1, userData);
    
    __delay32(15); // 15*14ns Delay
    
    byte = userReadDB(userData);

    userESet(0, userData);
    
    Nop(); // 6*14ns Delay
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();

    return ((byte >> 7) & 0x01);
}


enum lcdState_e 
lcdInitProcess(uint8_t *ctx) {

    enum lcdState_e state = LCDINITIALIZING;
    
    // Call me with a 1ms period
        
    switch (*ctx) {
        case LCD_INIT_8BIT1:
            lcdWriteByte(0b00110000, LCDCMD);    // 8 bit
            *ctx = LCD_INIT_8BIT2;
            break;
        case LCD_INIT_8BIT2:
            lcdWriteByte(0b00110000, LCDCMD);    // 8 bit
            *ctx = LCD_INIT_8BIT3;
            break;
        case LCD_INIT_8BIT3:
            lcdWriteByte(0b00110000, LCDCMD);    // 8 bit
            *ctx = LCD_INIT_CLEAR;
            break;
        case LCD_INIT_CLEAR:
            lcdWriteByte(0b00000001, LCDCMD);    // Clear display
            *ctx = LCD_INIT_FUNCTION;
            break;
        case LCD_INIT_FUNCTION:                  
            lcdWriteByte(0b00111110, LCDCMD);    // Function set 4 line display   
            *ctx = LCD_INIT_DISPLAY;
            break;
        case LCD_INIT_DISPLAY:
            lcdWriteByte(0b00001100, LCDCMD);    // Display on/off
            *ctx = LCD_INIT_ENTRY;
            break;
        case LCD_INIT_ENTRY:
            lcdWriteByte(0b00000110, LCDCMD);    // Entry mode
            *ctx = LCD_INIT_COMPLETE;
            break;
        case LCD_INIT_COMPLETE:
            state = LCDREADY;
            break;            
    }   
    
    return state;
}

void 
lcdInit(uint16_t rows, uint16_t cols, 
        void (*RWSet)(uint8_t value, void *data),
        void (*RSSet)(uint8_t value, void *data),
        void (*WriteDB)(uint8_t byte, void *data),
        uint8_t (*ReadDB)(void *data),
        void (*ESet)(uint8_t value, void *data),
        void *data) {

    userRWSet   = RWSet;
    userRSSet   = RSSet;
    userWriteDB = WriteDB;
    userReadDB  = ReadDB;
    userESet    = ESet;
    userData    = data;
    
    lcdRows     = rows;
    lcdCols     = cols;
    
    // Set initial state to initialise display
    // the rest of the work is done by lcdInitProcess
}

#define LCD_CLEAR        0x01
#define LCD_SETDDRAMADDR 0x80

void 
lcdClear() {

    lcdWriteByte(LCD_CLEAR, LCDCMD);
}

void 
lcdSetCursor(uint16_t row, uint16_t col) {    

    static uint16_t rowTable[] = { 
        0x00, 0x40, 0x14, 0x54 
    };

    lcdWriteByte(LCD_SETDDRAMADDR | (col + rowTable[row]), LCDCMD);
}


#define LCD_SETCGRAMADDR 0x40

enum uploadCharState_e
uploadChar(uint8_t position, uint8_t data, uint8_t *ctx) {

    enum uploadCharState_e state = CHARUPLOADING;    
    
    switch (*ctx) {
        
        case 0:
            lcdWriteByte(LCD_SETCGRAMADDR | ((position & 0x07) << 3), LCDCMD);
            *ctx = 1;
            state = CHARSTARTEDUPLOAD;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8: {
            lcdWriteByte(data, LCDDATA);
            (*ctx)++;
            break;
        }
        case 9:
            state = CHARUPLOADCOMPLETE;
            break;
    }

    return state;
}


//void 
//lcdBlockingInit(uint16_t rows, uint16_t cols, 
//                void (*RWSet)(uint8_t value, void *data),
//                void (*RSSet)(uint8_t value, void *data),
//                void (*WriteDB)(uint8_t byte, void *data),
//                uint8_t (*ReadDB)(void *data),
//                void (*ESet)(uint8_t value, void *data),
//                void *data) {
//    
//    uint8_t ctx = 0;
//        
//    lcdInit(rows, cols, RWSet, RSSet, WriteDB, ReadDB, ESet, data);
//        
//    while (lcdInitProcess(&ctx) != LCDREADY) {    
//        __delay_ms(5);
//    }
//    
//    __delay_ms(5);
//}
//
//
//
//void 
//lcdBlockingUploadChar(struct uploadChar_s *data) {
//   
//    struct uploadChar_s *p = (struct uploadChar_s *)data;
//            
//    while (p->count != 0) {
//        
//        switch (uploadChar(p->position, *(p->ptr), &(p->ctx))) {
//            case CHARSTARTEDUPLOAD:
//                break;
//
//            case CHARUPLOADING:
//                p->ptr++;
//                break;
//
//            case CHARUPLOADCOMPLETE:
//                p->position++;
//                p->count--;
//                p->ctx = 0;
//                break;
//        }
//
//        __delay_ms(5);
//    }
//}
//
//void 
//lcdBlockingSetCursor(uint16_t row, uint16_t col) {    
//
//    lcdSetCursor(row, col);
//
//    __delay_ms(5);
//}
//
//void 
//lcdBlockingWriteChar(char c) {
//    
//    lcdWriteChar(c);
//    
//    __delay_ms(5);
//}
