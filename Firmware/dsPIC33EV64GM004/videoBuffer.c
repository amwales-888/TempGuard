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

#include "videoBuffer.h"

static uint8_t *videoBuffer;
static uint8_t *videoPtr;
static uint16_t videoCursor;
static uint16_t videoCols;
static uint16_t videoRows;
static uint16_t videoLen;


static void (*userSetCursor)(uint16_t row, uint16_t col, void *data);
static void (*userWriteChar)(char c, void *data);
static uint8_t (*userBusy)(void *data);
static void *userData;

void 
videoBufferInit(uint16_t rows, uint16_t cols, uint8_t *buffer,
                void (*setCursor)(uint16_t row, uint16_t col, void *data),
                void (*writeChar)(char c, void *data),
                uint8_t (*busy)(void *data),
                void *data) {
        
    userSetCursor = setCursor;
    userWriteChar = writeChar;
    userBusy      = busy;
    userData      = data;
    
    videoBuffer   = buffer;
    videoPtr      = videoBuffer;
    videoRows     = rows;
    videoCols     = cols;
    videoLen      = videoRows * videoCols;        
    
    videoBufferClear();
}

void 
videoBufferClear(void) {

    uint8_t *ptr = (uint8_t *)videoBuffer;
    uint16_t count = videoLen;

    while (count--) {
        *ptr++ = ' ';
    }
}

enum videoOutputState_e {
    
    STATE_CURSOR0 = 0,
    STATE_CURSOR1,
    STATE_CURSOR2,
    STATE_CURSOR3,
    STATE_DATA
};

#ifdef BLINK7BIT

static uint8_t videoBlink = 0;

void
videoToggleBlink(void) {
    
    videoBlink ^= 1;
}

void
videoSetBlink(uint8_t value) {
    
    videoBlink = value;
}

void 
videoSetRangeBlink(uint8_t *ptr, uint16_t count) {

    while(count--) {
        *ptr++ |= 0x80;        
    }
}



#endif


void 
videoBufferRefresh(void) {

    static enum videoOutputState_e state = STATE_CURSOR0;

    /* We can only write when the device is not busy
     */    
    if (userBusy(userData) == 0) {
           
        switch (state) {
            
            case STATE_CURSOR0:

                videoPtr    = &videoBuffer[videoCols * 0];
                videoCursor = videoCols * 0; 

                userSetCursor(0, 0, userData);
                state = STATE_DATA;
                break;
                
            case STATE_CURSOR1:

                videoPtr    = &videoBuffer[videoCols * 1];
                videoCursor = videoCols * 1; 

                userSetCursor(1, 0, userData);
                state = STATE_DATA;
                break;
                
            case STATE_CURSOR2:

                videoPtr    = &videoBuffer[videoCols * 2];
                videoCursor = videoCols * 2; 

                userSetCursor(2, 0, userData);
                state = STATE_DATA;
                break;
                
            case STATE_CURSOR3:

                videoPtr    = &videoBuffer[videoCols * 3];
                videoCursor = videoCols * 3; 

                userSetCursor(3, 0, userData);
                state = STATE_DATA;
                break;
                
            case STATE_DATA: {
                
                uint8_t c = *videoPtr++;
                
#ifdef BLINK7BIT
                if ((videoBlink) && (c & 0x80)) {
                    c = (uint8_t)' ';
                } else {
                    c &= 0x7f;                    
                }               
#endif
                
                userWriteChar((char)(c), userData);
                videoCursor++;
                
                if ((videoCursor == videoCols * 0) ||
                    (videoCursor == videoCols * 4)) {
                    
                    state = STATE_CURSOR0;                    
                } else 
                if (videoCursor == videoCols * 1) {                    

                    state = STATE_CURSOR1;
                } else 
                if (videoCursor == videoCols * 2) {                    

                    state = STATE_CURSOR2;
                } else 
                if (videoCursor == videoCols * 3) {                    

                    state = STATE_CURSOR3;
                }

                break;
            }
        }
    }
}


void 
videoBufferPut(uint16_t row, uint16_t col, uint8_t *buf, uint16_t len) {

    uint8_t *ptr = (uint8_t *)&videoBuffer[(row * videoCols) + col];
    
    while (len--) {
        *ptr++ = *buf++;
    }
}

uint8_t *
videoBufferGetPtr(uint16_t row, uint16_t col) {
    
    return (uint8_t *)&videoBuffer[(row * videoCols) + col];
}
