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

#include "dht.h"
#include "scheduler.h"


enum dhtCaptureState_e {
    
    DHTERROR=0,
    DHTSUCCESS,
    DHTCOLLECTDATA,
    DHTCOLLECTED,
};

enum tempState_e {
    
    TEMPIDLE=0,
    TEMPSENDAQUIRE,
    TEMPREAD,
};

struct dht_s {

    uint16_t dhtInitialised;
    uint16_t dhtTcount;
    uint16_t dhtLast;
    uint16_t dhtBitCount;
    uint16_t dhtByteIdx;
    uint8_t  dhtByte[5];
    
    enum dhtCaptureState_e dhtCaptureState;

    uint8_t dhtIntegralRH;
    uint8_t dhtDecimalRH;
    uint8_t dhtIntegralT;
    uint8_t dhtDecimalT;

    enum dhtTempSensor_e tempSensor;
    enum tempState_e tempState; 
    enum dhtTempStatus_e dhtTempStatus; 

    uint16_t tempCount; 
    
    void (*captureInit)(void *data);
    uint16_t (*captureDataRead)(void *data);
    void (*captureStop)(void *data);
    void (*captureOuputLow)(void *data);
    void (*captureInput)(void *data);    
    void *data;
};

#define TEMPIDLECOUNT   (MSTOTICKS(2500))  /* measured in process ticks, approx 2500ms before we aquire */
#define TEMPAQUIRECOUNT (MSTOTICKS(30))    /* measured in process ticks, approx 30ms */
#define TEMPREADCOUNT   (MSTOTICKS(20))    /* measured in process ticks, approx 20ms */

static struct dht_s dht[4];
           
void 
dhtInit(uint16_t id, 
        enum dhtTempSensor_e tempSensor,
        void (*captureInit)(void *data),
        uint16_t (*captureDataRead)(void *data),
        void (*captureStop)(void *data),
        void (*captureOuputLow)(void *data),
        void (*captureInput)(void *data), 
        void *data) {

    struct dht_s *p = &dht[id];
    
    p->dhtInitialised = 1;
    p->dhtTcount = 0;
    p->dhtLast = 0;
    p->dhtBitCount = 0;
    p->dhtByteIdx = 0;
    p->dhtByte[0] = 0;
    p->dhtByte[1] = 0;
    p->dhtByte[2] = 0;
    p->dhtByte[3] = 0;
    p->dhtByte[4] = 0;
    p->dhtCaptureState = DHTERROR;

    p->dhtTempStatus = DHTTEMPERROR; 
    p->dhtIntegralRH = 0;
    p->dhtDecimalRH = 0;
    p->dhtIntegralT = 0;
    p->dhtDecimalT = 0;

    p->tempSensor = tempSensor;
    p->tempState = TEMPIDLE; 
    p->tempCount = TEMPIDLECOUNT; 
    
    p->captureInit     = captureInit;
    p->captureDataRead = captureDataRead;
    p->captureStop     = captureStop;
    p->captureOuputLow = captureOuputLow;
    p->captureInput    = captureInput;
    p->data = data;
}
    

static void 
dhtStartCollect(uint16_t id) {

    struct dht_s *p = &dht[id];
    if (!p->dhtInitialised) return;
    
    p->dhtTcount   = 0;
    p->dhtLast     = 0;
    p->dhtBitCount = 0;
    p->dhtByteIdx  = 0;

    p->dhtByte[0]  = 0;
    p->dhtByte[1]  = 0;
    p->dhtByte[2]  = 0;
    p->dhtByte[3]  = 0;

    p->dhtCaptureState = DHTCOLLECTDATA;

    p->captureInit(p->data);
}

void 
dhtCaptureProcess(uint16_t id) {
        
// Interrupt callback from input capture hardware   
//    
// https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf
//
//  +--+    +----+        +--+       +------+
//     |    |    |        |  |       |      |
//     +----+    +--------+  +-------+      +
//   10  79   82     51    23     51    67
//        preamble          0 Bit        1 Bit
//    10       161          74          118     
//     0         1           2            3

#define DHTZEROBITLENGTH 90 
        
    struct dht_s *p = &dht[id];    
    if (!p->dhtInitialised) return;

    uint16_t now    = p->captureDataRead(p->data);    
    
    if ((p->dhtTcount > 1) && 
        (p->dhtTcount < 42)) {
        
        p->dhtByte[p->dhtByteIdx] <<= 1;
        if ((now - p->dhtLast) > DHTZEROBITLENGTH) {
            p->dhtByte[p->dhtByteIdx] |= 1; 
        }

        p->dhtBitCount++;
        if (p->dhtBitCount > 7) {
            p->dhtBitCount = 0;

            p->dhtByteIdx++;
            if (p->dhtByteIdx > 4) {
                p->dhtCaptureState = DHTCOLLECTED;            
            }
        }
    }
                
    p->dhtLast = now;
    p->dhtTcount++;
}




static void 
dhtStopCollect(uint16_t id) {

    struct dht_s *p  = &dht[id];
    
    p->captureStop(p->data);
    if (p->dhtCaptureState == DHTCOLLECTED) {
        
        uint8_t chksum = p->dhtByte[0] + p->dhtByte[1] + p->dhtByte[2] + p->dhtByte[3];        
        if (chksum == p->dhtByte[4]) {

            p->dhtCaptureState = DHTSUCCESS;

        } else {

            p->dhtCaptureState = DHTERROR;
        }
    } else {
        
        p->dhtCaptureState = DHTERROR;
    }

}

enum dhtTempSensor_e 
dhtGetSensorType(uint16_t id) {
    return dht[id].tempSensor;
}

void 
dhtSetSensorType(uint16_t id, enum dhtTempSensor_e type) {
    dht[id].tempSensor = type;
}




void 
dhtTemperatureProcess(uint16_t id) {
    
    /* We expect to be called every 10ms as routine is used to 
     * initiate the temperature capture and collect the data
     */

    struct dht_s *p = &dht[id];
    if (!p->dhtInitialised) return;
   
    switch (p->tempState) {
       
        case TEMPIDLE: {

            p->tempCount--;
            if (p->tempCount == 0) {
                p->tempState = TEMPSENDAQUIRE;
                p->tempCount = TEMPAQUIRECOUNT;
            }

            break;
        }

        case TEMPSENDAQUIRE: {

            p->tempCount--;
            if (p->tempCount == TEMPAQUIRECOUNT-1) {
                
                /* Set low for at least 20ms         */
                /* Set pin to output and pull it low */
                
                p->captureOuputLow(p->data);
                
            } else if (p->tempCount == 0) {

                /* 20ms should have expired since we pulled pin Low */
                /* Set A3 Input, this will force it to get pulledup */
                
                p->captureInput(p->data);                                    
                dhtStartCollect(id);
                               
                p->tempState = TEMPREAD;
                p->tempCount = TEMPREADCOUNT;
            }
            
            break;
        }

        case TEMPREAD: {

            p->tempCount--;
            if (p->tempCount == 0) {

                /* We have read all data now, if not then we have timed out */         

                dhtStopCollect(id);
                                             
                if (p->dhtCaptureState == DHTSUCCESS) {

                    p->dhtIntegralRH = p->dhtByte[0];
                    p->dhtDecimalRH  = p->dhtByte[1];
                    p->dhtIntegralT  = p->dhtByte[2];
                    p->dhtDecimalT   = p->dhtByte[3];                    

                    p->dhtTempStatus = DHTTEMPSUCCESS;

                } else {
                
                    p->dhtTempStatus = DHTTEMPERROR;
                }
                
                p->tempState = TEMPIDLE;
                p->tempCount = TEMPIDLECOUNT;
            } 
            
            break;
        }
    }
}

float 
dhtConvertCtoF(float c) { 

    return c * 1.8 + 32; 
}

float 
dhtConvertFtoC(float f) { 
    
    return (f - 32) * 0.55555; 
}


enum dhtTempStatus_e 
dhtGetTemperatureStatus(uint16_t id) {
    return dht[id].dhtTempStatus;
}

float 
dhtGetTemperature(uint16_t id, uint16_t asF) {

    struct dht_s *p = &dht[id];
    float f = 0.0;
    
    switch (p->tempSensor) {
        
        case DHT11: {

            f = p->dhtIntegralT;
            if (p->dhtDecimalT & 0x80) {
              f = -1 - f;
            }
            f += (float)(p->dhtDecimalT & 0x0f) * 0.1;
    
            break;
        }
        case DHT22: {
                        
            f = (((int16_t)(p->dhtIntegralT & 0x7F)) << 8) | p->dhtDecimalT;
            f *= 0.1;

            if (p->dhtIntegralT & 0x80) {
                f *= -1;
            }

            break;
        }
    }
    
    if (asF) {
      f = dhtConvertCtoF(f);
    }

    return f;
}

float 
dhtGetHumidity(uint16_t id) {

    struct dht_s *p = &dht[id];
    float f = 0.0;
    
    switch (p->tempSensor) {

        case DHT11: {

            f = (float)p->dhtIntegralRH + p->dhtDecimalRH * 0.1;
            break;
        }
        case DHT22: {
            
            f = ((int16_t)p->dhtIntegralRH) << 8 | p->dhtDecimalRH;
            f *= 0.1;
            break;
        }
    }

    return f;
}


