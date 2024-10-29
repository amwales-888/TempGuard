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

#include "format.h"

void 
formatNumber2Right(char *ptr, uint16_t n) {
    
    // Convert uint value to string ##
    
    if (n < 0) {
        
        *ptr++ = 'L';
        *ptr++ = 'O';

    } else if (n > 99) {
        
        *ptr++ = 'H';
        *ptr++ = 'I';

    } else {

        if (n < 10) {
                        
            ptr += 1; 
            
            *ptr-- = '0' + (n % 10);        
            *ptr   = ' ';
            
        } else if (n < 100) {
            
            ptr += 1; 

            *ptr-- = '0' + (n % 10);
            n /= 10;
            *ptr   = '0' + (n % 10);
        }        
    }
}

void 
formatNumber2(char *ptr, uint16_t n) {
    
    // Convert uint value to string ##
    
    if (n < 0) {
        
        *ptr++ = 'L';
        *ptr++ = 'O';

    } else if (n > 99) {
        
        *ptr++ = 'H';
        *ptr++ = 'I';

    } else {

        if (n < 10) {
                        
            ptr += 1; 
            
            *ptr-- = '0' + (n % 10);        
            *ptr   = '0';
            
        } else if (n < 100) {
            
            ptr += 1; 

            *ptr-- = '0' + (n % 10);
            n /= 10;
            *ptr   = '0' + (n % 10);
        }        
    }
}






void 
formatNumberRight(char *ptr, uint16_t n) {
    
    // Convert uint value to string ###
    
    if (n < 0) {
        
        *ptr++ = 'L';
        *ptr++ = 'O';
        *ptr   = ' ';

    } else if (n > 999) {
        
        *ptr++ = 'H';
        *ptr++ = 'I';
        *ptr   = ' ';

    } else {

        if (n < 10) {
                        
            ptr += 2; 
            
            *ptr-- = '0' + (n % 10);        
            *ptr-- = ' ';
            *ptr   = ' ';
            
        } else if (n < 100) {
            
            ptr += 2; 

            *ptr-- = '0' + (n % 10);
            n /= 10;
            *ptr-- = '0' + (n % 10);
            *ptr   = ' ';

        } else if (n < 1000) {
            
            ptr += 2; 

            *ptr-- = '0' + (n % 10);
            n /= 10;
            *ptr-- = '0' + (n % 10);
            n /= 10;
            *ptr   = '0' + (n % 10);
        }        
    }
}

void 
formatRPM(char *ptr, uint16_t n) {
    
    // Convert uint value to string ####
    
    if (n < 0) {
        
        *ptr++ = 'L';
        *ptr++ = 'O';
        *ptr++ = ' ';
        *ptr   = ' ';

    } else if (n > 9999) {
        
        *ptr++ = 'H';
        *ptr++ = 'I';
        *ptr++ = ' ';
        *ptr   = ' ';

    } else {

        ptr += 3; 
        
        *ptr-- = '0' + (n % 10);        
        n /= 10;
        *ptr-- = '0' + (n % 10);        
        n /= 10;
        *ptr-- = '0' + (n % 10);
        n /= 10;
        *ptr = '0' + (n % 10);
        
        // remove leading zeros

        if (*ptr == '0') *ptr++ = ' ';
        if (*ptr == '0') *ptr++ = ' ';
        if (*ptr == '0') *ptr++ = ' ';
    }
}

void 
formatNumber(char *ptr, uint16_t n) {
    
    // Convert uint value to string ###
    
    if (n < 0) {
        
        *ptr++ = 'L';
        *ptr++ = 'O';
        *ptr   = ' ';

    } else if (n > 100) {
        
        *ptr++ = 'H';
        *ptr++ = 'I';
        *ptr   = ' ';

    } else {

        ptr += 2; 
        
        *ptr-- = '0' + (n % 10);        
        n /= 10;
        *ptr-- = '0' + (n % 10);
        n /= 10;
        *ptr = '0' + (n % 10);
        
        // remove leading zeros

        if (*ptr == '0') *ptr++ = ' ';
        if (*ptr == '0') *ptr++ = ' ';
    }
}


void 
formatTemp(char *ptr, double v) {

    /* Convert double value to string ##.# */

    if (v < 0) {
        
        *ptr++ = 'L';
        *ptr++ = 'O';
        *ptr++ = ' ';
        *ptr   = ' ';

    } else if (v < 100) {
        
        int n = (int)v;
        int f = (v - n) * 10;

        ptr += 3; 
        
        *ptr-- = '0' + f;
        *ptr-- = '.';
        *ptr-- = '0' + (n % 10);
        
        n /= 10;
        if (n > 0) {
            *ptr = '0' + (n % 10);
        } else {
            *ptr = ' ';
        }
        
    } else {
        
        *ptr++ = 'H';
        *ptr++ = 'I';
        *ptr++ = ' ';
        *ptr   = ' ';
    }
}

