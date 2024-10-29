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

#include "process.h"

#ifdef PROCESSDYNAMICALLOCATION

static struct process_s processHeap[MAXPROCESSES];

struct process_s *
processAlloc(void) {

    static struct process_s process = { 1, 0, 0, 0, (void (*)(void *))0, (void *)0, { (void *)0 } };
    
    for (int i=0; i<MAXPROCESSES; i++) {
        if (!processHeap[i].allocated) {
            
            processHeap[i] = process;
            return &processHeap[i];
        }
    }
    
    return (struct process_s *)0;
}

void 
procesFree(struct process_s *process) {
           
    process->allocated = 0;
}

void
processInit(struct process_s *process, 
        uint32_t periodMs, void (*func)(void *data), void *data) {
    
    process->allocated  = 1;
    process->periodTick = MSTOTICKS(periodTick);
    process->func       = func;
    process->data       = data;
}

#endif

void 
processEnable(struct process_s *process) {
    process->disabled = 0;
}

void 
processDisable(struct process_s *process) {
    process->disabled = 1;
}
