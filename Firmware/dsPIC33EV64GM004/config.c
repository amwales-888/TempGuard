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
#include <string.h>

#include "mcc_generated_files/memory/flash.h"
#include "config.h"


static const struct config_s defaultCfg = {

    CFGMAGIC,
    0,    
    {
        { 0, 1, 1, 20, 15.0, 0.4 },
        { 0, 1, 1, 20, 15.0, 0.4 },
        { 0, 1, 1, 20, 15.0, 0.4 },
        
    },    
    {
        { 0, 1, DHT22, 0 },
        { 0, 1, DHT22, 0 },
        { 0, 1, DHT22, 0 },                
        { 0, 1, DHT22, 0 },                
    },
    {
        { 0, 1, 0, 20, 100 },
        { 0, 1, 0, 20, 100 },
        { 0, 1, 0, 20, 100 },        
    },
    { 0 }
};

static __prog__  uint8_t flashConfigPage[FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS] __attribute__((space(prog),aligned(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS)));

void 
configWrite(const struct config_s *cfg) {

    uint32_t flashAddress;
    uint32_t flashOffset = 0;
    uint16_t *ptr = (uint16_t *)cfg;
    uint16_t numDoubleWords = sizeof(struct config_s) / 4;

    /* Flash is program space, addresses are different from RAM addresses 
     * the variable flashConfigPage is allocated from program flash
     */
    flashAddress = FLASH_GetErasePageAddress((uint32_t)&flashConfigPage[0]);

    /* Allow flash to be written 
     */
    FLASH_Unlock(FLASH_UNLOCK_KEY);
    
    /* Flash bits must be erased to 1 as writing can only be done
     * by changing bits to 0 
     */    
    FLASH_ErasePage(flashAddress);
    
    while (numDoubleWords--) {

        FLASH_WriteDoubleWord16(flashAddress+flashOffset, ptr[0], ptr[1]);
        
        ptr += 2;
        flashOffset += 4;        
    }
    
    /* Prevent accidental writes 
     */
    FLASH_Lock();    
}

void
configRead(struct config_s *cfg) {

    uint32_t flashAddress;
    uint32_t flashOffset = 0;
    uint16_t *ptr = (uint16_t *)cfg;
    uint16_t numDoubleWords = sizeof(struct config_s) / 4;

    /* Flash is program space, addresses are different from RAM addresses 
     * the variable flashConfigPage is allocated from program flash
     */
    flashAddress = FLASH_GetErasePageAddress((uint32_t)&flashConfigPage[0]);
                
    while (numDoubleWords--) {
        
        *ptr++ = FLASH_ReadWord16(flashAddress+flashOffset);
        *ptr++ = FLASH_ReadWord16(flashAddress+flashOffset+2);

        flashOffset += 4;
    }
    
    if (cfg->magic != CFGMAGIC) {
        
        /* Config invalid or has never been written, set to default 
         */        
        memcpy(cfg, &defaultCfg, sizeof(struct config_s));
    }  
}

void
configReset(void) {
    
    configWrite(&defaultCfg);
}
