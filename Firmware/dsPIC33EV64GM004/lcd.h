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

#ifndef LCD_H
#define	LCD_H

#include <stdint.h>

#define LCDROWS 4
#define LCDCOLS 20

#if (LCDROWS != 4)
#error "Only LCD with 4 Rows currently supported"
#endif

#if (LCDCOLS != 20)
#error "Only LCD with 20 Cols currently supported"
#endif

enum lcdMode_e {
    
    LCDCMD=0,  // RS=0 RW=0
    LCDDATA=2, // RS=1 RW=0
};

enum lcdState_e {
    
    LCDINITIALIZING=0,
    LCDREADY,
};


enum uploadCharState_e {
    
    CHARSTARTEDUPLOAD=0,
    CHARUPLOADING,
    CHARUPLOADCOMPLETE,
};

struct uploadChar_s {
    
    uint8_t ctx;
    uint8_t position;
    uint8_t count;    
    const uint8_t *ptr;
};


#ifdef	__cplusplus
extern "C" {
#endif

enum lcdState_e lcdInitProcess(uint8_t *ctx);
uint8_t lcdBusy(void);

void lcdInit(uint16_t rows, uint16_t cols, 
             void (*RWSet)(uint8_t value, void *data),
             void (*RSSet)(uint8_t value, void *data),
             void (*WriteDB)(uint8_t byte, void *data),
             uint8_t (*ReadDB)(void *data),
             void (*ESet)(uint8_t value, void *data),
             void *data);

void lcdClear(void);
void lcdSetCursor(uint16_t row, uint16_t col);
void lcdWriteChar(char c);
enum uploadCharState_e uploadChar(uint8_t position, uint8_t data, uint8_t *ctx);



//void lcdBlockingInit(uint16_t rows, uint16_t cols, 
//                void (*RWSet)(uint8_t value, void *data),
//                void (*RSSet)(uint8_t value, void *data),
//                void (*WriteDB)(uint8_t byte, void *data),
//                uint8_t (*ReadDB)(void *data),
//                void (*ESet)(uint8_t value, void *data),
//                void *data);
//
//void lcdBlockingUploadChar(struct uploadChar_s *data);
//void lcdBlockingSetCursor(uint16_t row, uint16_t col);
//void lcdBlockingWriteChar(char c);



#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

