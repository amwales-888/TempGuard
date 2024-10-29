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

#ifndef PROCESS_H
#define	PROCESS_H

//#include <xc.h>
#include <stdint.h>

#include "queue.h"

// #define PROCESSDYNAMICALLOCATION 1

struct process_s {
    
#ifdef PROCESSDYNAMICALLOCATION
    uint32_t allocated:1;
    uint32_t enabled:1
    uint32_t spare:30;
#else
    uint32_t disabled:1;
    uint32_t spare:31;
#endif
    
    uint32_t lastTick;
    uint32_t periodTick;
    void (*func)(void *data);    
    void *data;

	SLIST_ENTRY(process_s) list;
};

#ifdef PROCESSDYNAMICALLOCATION
#define PROCESSALLOCATE(name, periodMs, func, data) \
    struct process_s name = {                         \
        1, 0, 0, 0,                                         \
        0,                                            \
        MSTOTICKS(periodMs), func, data, { (void *)0 } }       
#else
#define PROCESSALLOCATE(name, periodMs, func, data) \
    struct process_s name = {                         \
        0, 0, 0,                                            \
        MSTOTICKS(periodMs), func, data, { (void *)0 } }       
#endif


#define MAXPROCESSES 16

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef PROCESSDYNAMICALLOCATION
struct process_s *processAlloc(void);
void procesFree(struct process_s *process);
void processInit(struct process_s *process, 
        uint32_t periodMs, void (*func)(void *data), void *data);
#endif

void processEnable(struct process_s *process);
void processDisable(struct process_s *process);

#ifdef	__cplusplus
}
#endif

#endif	/* PROCESS_H */

