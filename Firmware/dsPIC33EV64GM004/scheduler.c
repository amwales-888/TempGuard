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

#include "scheduler.h"
#include "queue.h"

static volatile uint32_t schedulerTick = 0;

static SLIST_HEAD(processHead, process_s) processList = SLIST_HEAD_INITIALIZER(&processList);
static struct process_s *processCurrent;
static struct process_s *processNext;

void 
schedulerRun(void) {
    
    uint32_t nowTick = schedulerTick;

    if (processCurrent) {
        
        processNext = SLIST_NEXT(processCurrent, list);
        if (processNext == SLIST_END(&processList)) {
            processNext = SLIST_FIRST(&processList);
        }        
                
        if (!processCurrent->disabled) {  
            
            if ((uint32_t)(nowTick - processCurrent->lastTick) >= processCurrent->periodTick) {
                (*processCurrent->func)(processCurrent->data);

                /* Do not update process if it removed itself */
                if (processCurrent) {
                    processCurrent->lastTick = nowTick;
                }
            }                
        }
        
        processCurrent = processNext;
    }
    
    return;
}

void 
schedulerProcessResetPeriod(struct process_s *process) { 
    
    processCurrent->lastTick = schedulerTick;
}

void 
schedulerProcessAdd(struct process_s *process) { 

    if (!processCurrent) {
        processCurrent = process;
    }

    SLIST_INSERT_HEAD(&processList, process, list);
}

void 
schedulerProcessRemove(struct process_s *process) { 

    if (process == processNext) {
        
        processNext = SLIST_NEXT(processNext, list);
        if (processNext == SLIST_END(&processList)) {
            processNext = SLIST_FIRST(&processList);
        }        
    }
        
    SLIST_REMOVE(&processList, process, process_s, list);
}

uint32_t 
schedulerGetTicks() {
    
    return schedulerTick;    
}

void 
schedulerIncrementTicks() {
    
    schedulerTick++;
}
