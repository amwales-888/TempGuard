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

#ifndef SCHEDULER_H
#define	SCHEDULER_H

//#include <xc.h>
#include <stdint.h>

#include "process.h"

/* ---------------------------------------------------------------------- */

#define TICKPERIODMS     10 

/* ---------------------------------------------------------------------- */

#define MSTOTICKS(ms)    ((ms) ? ((((uint32_t)(ms)-1)/(TICKPERIODMS))+1) : 0)
#define TICKSTOMS(ticks) ((ticks)*(TICKPERIODMS))

/* ---------------------------------------------------------------------- */

#ifdef	__cplusplus
extern "C" {
#endif

void schedulerRun(void);
void schedulerProcessAdd(struct process_s *process);
void schedulerProcessRemove(struct process_s *process);
uint32_t schedulerGetTicks(void);
void schedulerIncrementTicks(void);
void schedulerProcessResetPeriod(struct process_s *process);

#ifdef	__cplusplus
}
#endif

#endif	/* SCHEDULER_H */