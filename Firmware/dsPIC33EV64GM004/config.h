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

#ifndef CONFIG_H
#define	CONFIG_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

#include "dht.h"

struct config_s {
    
    uint32_t magic;
    uint8_t  version;
    
    struct configProgram_s {
        
        uint16_t spare:14;
        uint8_t  enabled:1;
        uint8_t  alarm:1;
        uint16_t targetTemp;
        double   P;
        double   I;

    } program[3];
    
    struct configSensor_s {
        
        uint16_t spare:15;
        uint8_t  enabled:1;
        uint16_t type;
        uint16_t offset;

    } sensor[4];
    
    struct configFan_s {
        
        uint16_t spare:15;
        uint8_t  enabled:1;
        uint16_t type;        
        uint16_t minDuty;
        uint16_t maxDuty;

    } fan[3];    
    
    uint8_t spare[166];
};

/* config size MUST be a multiple of 4 bytes to comply with 
   assumptions made by configWrite and configRead where flash is read/written 
   in double words. If config size is changed then this needs to be updated */

#define CONFIGSIZE 256
#define CONFIGASSERT() do { if (sizeof(struct config_s) != CONFIGSIZE) { while (1) {} }; } while (0)

#define CFGMAGIC 0xDEADBEEF


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

void configWrite(const struct config_s *cfg);
void configRead(struct config_s *cfg);

void configReset(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* CONFIG_H */

