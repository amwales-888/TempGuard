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

#ifndef VIDEOBUFFER_H
#define	VIDEOBUFFER_H

#include <stdint.h>

#define MAXVIDEOBUFFERX 20
#define MAXVIDEOBUFFERY 4

#ifdef	__cplusplus
extern "C" {
#endif

void videoBufferInit(uint16_t rows, uint16_t cols, uint8_t *buffer,
                     void (*setCursor)(uint16_t row, uint16_t col, void *data),
                     void (*writeChar)(char c, void *data),
                     uint8_t (*busy)(void *data),
                     void *data);

void videoBufferClear(void);
void videoBufferRefresh(void);
void videoBufferPut(uint16_t row, uint16_t col, uint8_t *buf, uint16_t len);
uint8_t *videoBufferGetPtr(uint16_t row, uint16_t col);

/* The MSB bit of the character data is used to indicate a character should
 * be blinking at some specified interval, this reduces the available characters
 * to 7bit
 */
#define BLINK7BIT 1 

#ifdef BLINK7BIT
void videoToggleBlink(void);
void videoSetBlink(uint8_t value);
void videoSetRangeBlink(uint8_t *ptr, uint16_t count);
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* VIDEOBUFFER_H */

