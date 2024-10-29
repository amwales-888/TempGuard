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

#ifndef DHT_H
#define	DHT_H

#include <stdint.h>

enum dhtTempSensor_e {

    DHT22=0,
    DHT11,
};

enum dhtTempStatus_e {
    
    DHTTEMPERROR=0,
    DHTTEMPSUCCESS
};

#ifdef	__cplusplus
extern "C" {
#endif

    
void dhtInit(uint16_t id, 
             enum dhtTempSensor_e tempSensor,
             void (*ICInit)(void *data),
             uint16_t (*ICCaptureDataRead)(void *data),
             void (*ICStop)(void *data),
             void (*setDigitalOuputLow)(void *data),
             void (*setDigitalInput)(void *data), 
             void *data);
    
void dhtCaptureProcess(uint16_t id);
void dhtTemperatureProcess(uint16_t id);
float dhtConvertCtoF(float c);
float dhtConvertFtoC(float f); 
float dhtGetTemperature(uint16_t id, uint16_t asF);
float dhtGetHumidity(uint16_t id);
enum dhtTempStatus_e dhtGetTemperatureStatus(uint16_t id);
enum dhtTempSensor_e dhtGetSensorType(uint16_t id);
void dhtSetSensorType(uint16_t id, enum dhtTempSensor_e type);

#ifdef	__cplusplus
}
#endif

#endif	/* DHT_H */

