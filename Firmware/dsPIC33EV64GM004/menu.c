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

#include "delay.h"

#include <xc.h>
#include <libpic30.h>
#include <stdint.h>
#include <string.h>

#include "menu.h"
#include "videoBuffer.h"
#include "dht.h"
#include "encoder.h"
#include "format.h"
#include "config.h"

#define MENUROWS 4
#define MENUCOLS 20

extern uint16_t pwmGetDutyCycle(uint16_t pwmId);
extern uint16_t getMappedPWMId(uint16_t tempId);
extern uint16_t tachGetRPM(uint16_t id);




static void defaultScreen(void);
static void screen0(void);
static void dialogYesNo(void);

static uint16_t menuDialogSave(void *data);
static uint16_t menuSave(void *data);
static uint16_t menuCancel(void *data);

static uint16_t getFanEnabled(void);    
static void setFanEnabled(uint16_t value);
static uint16_t getFanType(void);    
static void setFanType(uint16_t value);
static uint16_t getFanMin(void);    
static void setFanMin(uint16_t value);
static uint16_t getFanMax(void);    
static void setFanMax(uint16_t value);
static uint16_t getProgramEnabled(void);
static void setProgramEnabled(uint16_t value);
static uint16_t getProgramTarget(void);
static void setProgramTarget(uint16_t value);
static double getProgramP(void);
static void setProgramP(double value);
static double getProgramI(void);
static void setProgramI(double value);
static uint16_t getSensorEnabled(void);
static void setSensorEnabled(uint16_t value);
static uint16_t getSensorType(void);
static void setSensorType(uint16_t value);
static uint16_t getSensorOffset(void);
static void setSensorOffset(uint16_t value);
static uint16_t getProgramAlarm(void);
static void setProgramAlarm(uint16_t value);

struct config_s localCfg;
static uint16_t cfgChanged;

static uint16_t menuCurrentProgram;
static uint16_t menuCurrentFan;
static uint16_t menuCurrentSensor;

static uint16_t programTargetValue;
static uint16_t fanMinValue;
static uint16_t fanMaxValue;
static uint16_t sensorOffsetValue;

static const struct itemInteger_s itemProgramTarget  = { 13, 3, &programTargetValue, 0, 100, 1, getProgramTarget, setProgramTarget };
static const struct itemInteger_s itemFanMin         = { 13, 3, &fanMinValue,        0, 100, 1, getFanMin,        setFanMin };
static const struct itemInteger_s itemFanMax         = { 13, 3, &fanMaxValue,        0, 100, 1, getFanMax,        setFanMax };
static const struct itemInteger_s itemSensorOffset   = { 13, 3, &sensorOffsetValue,  0, 100, 1, getSensorOffset,  setSensorOffset };

static uint16_t programPP1Value;
static uint16_t programPP2Value;
static double programPValue;
static uint16_t programIP1Value;
static uint16_t programIP2Value;
static double programIValue;

static const struct itemFloatingPoint_s itemProgramP = { 11, 5, &programPP1Value, &programPP2Value, &programPValue, 0, 99, 1, getProgramP, setProgramP };
static const struct itemFloatingPoint_s itemProgramI = { 11, 5, &programIP1Value, &programIP2Value, &programIValue, 0, 99, 1, getProgramI, setProgramI };

enum stypes0_e {

    STYPE_OFF = 0,
    STYPE_ON            
};

static const char * stypes0[] = { 
    
    "  Off",
    "   On"
};

static uint16_t programEnabledValue;
static uint16_t sensorEnabledValue;
static uint16_t fanEnabledValue;
static uint16_t programAlarmValue;

static const struct itemList_s itemProgramEnabled = { 11, 5, &programEnabledValue, 2, stypes0, getProgramEnabled, setProgramEnabled };
static const struct itemList_s itemSensorEnabled  = { 11, 5, &sensorEnabledValue,  2, stypes0, getSensorEnabled,  setSensorEnabled };
static const struct itemList_s itemFanEnabled     = { 11, 5, &fanEnabledValue,     2, stypes0, getFanEnabled,     setFanEnabled };
static const struct itemList_s itemProgramAlarm   = { 11, 5, &programAlarmValue,   2, stypes0, getProgramAlarm,    setProgramAlarm };

enum stypes1_e {

    STYPE_DHT22,   
    STYPE_DHT11
};

static const char *stypes1[] = { 
    
    "DHT22",
    "DHT11"
};

static uint16_t sensorTypeValue;

static const struct itemList_s itemSensorType = { 11, 5, &sensorTypeValue, 2, stypes1, getSensorType, setSensorType };

enum stypes2_e {

    STYPE_4PIN = 0,
    STYPE_3PIN,
    STYPE_2PIN     
};

static const char *stypes2[] = { 
    
    " 4PIN",
    " 3PIN",
    " 2PIN"
};

static uint16_t fanTypeValue;

static const struct itemList_s itemFanType = { 11, 5, &fanTypeValue, 3, stypes2, getFanType, setFanType };

static const struct item_s items0[] = {

    { "                    ", ITEMCONTAINER, (const void *)1 }
};

static const struct itemContainerArg_s itemReroute0 = { 0, menuDialogSave, (void *)0 };

static const struct item_s items1[] = {
    
     //01234567890123456789
    { " Info               ", ITEMCONTAINERARG, &itemReroute0   },
    { " Settings           ", ITEMCONTAINER,    (const void *)2 },
    { " Programs           ", ITEMCONTAINER,    (const void *)3 },    
    { " System             ", ITEMCONTAINER,    (const void *)4 }   
};

static const struct item_s items2[] = {
    
    { " Main               ", ITEMCONTAINER, (const void *)1 },
    { " Fans               ", ITEMCONTAINER, (const void *)5 }, 
    { " Sensors            ", ITEMCONTAINER, (const void *)6 } 
};

static uint16_t 
menuSelectProgram(void *data) {
    
    /* Button pressed callback, set global so other functions know which 
     * item we are operating on.
     */
    
    menuCurrentProgram = (uint16_t)data;
    return 0;
}

static const struct itemContainerArg_s itemProgram0 = { 7, menuSelectProgram, (void *)0 };
static const struct itemContainerArg_s itemProgram1 = { 7, menuSelectProgram, (void *)1 };
static const struct itemContainerArg_s itemProgram2 = { 7, menuSelectProgram, (void *)2 };

static const struct item_s items3[] = {
    
    { " Main               ", ITEMCONTAINER,    (const void *)1 },
    { " Program 0          ", ITEMCONTAINERARG, &itemProgram0   }, 
    { " Program 1          ", ITEMCONTAINERARG, &itemProgram1   }, 
    { " Program 2          ", ITEMCONTAINERARG, &itemProgram2   } 
};

static const struct itemContainerArg_s itemSave  = { 4, menuSave,  (void *)0 };
static const struct itemContainerArg_s itemReset = { 4, menuCancel, (void *)0 };

static const struct item_s items4[] = {
    
    { " Main               ", ITEMCONTAINER,    (const void *)1  },
    { " Save               ", ITEMCONTAINERARG, &itemSave        }, 
    { " Reset              ", ITEMCONTAINERARG, &itemReset       }, 
    { " About              ", ITEMCONTAINER,    (const void *)1  } 
};

static uint16_t
menuSelectFan(void *data) {
    
    /* Button pressed callback, set global so other functions know which 
     * item we are operating on.
     */

    menuCurrentFan = (uint16_t)data;
    return 0;
}

static const struct itemContainerArg_s itemFan0 = { 8, menuSelectFan, (void *)0 };
static const struct itemContainerArg_s itemFan1 = { 8, menuSelectFan, (void *)1 };
static const struct itemContainerArg_s itemFan2 = { 8, menuSelectFan, (void *)2 };

static const struct item_s items5[] = {
    
    { " Settings           ", ITEMCONTAINER,    (const void *)2 },
    { " Fan 0              ", ITEMCONTAINERARG, &itemFan0       }, 
    { " Fan 1              ", ITEMCONTAINERARG, &itemFan1       }, 
    { " Fan 2              ", ITEMCONTAINERARG, &itemFan2       } 
};

static uint16_t
menuSelectSensor(void *data) {
    
    /* Button pressed callback, set global so other functions know which 
     * item we are operating on.
     */

    menuCurrentSensor = (uint16_t)data;
    return 0;
}

static const struct itemContainerArg_s itemSensor0 = { 9, menuSelectSensor, (void *)0 };
static const struct itemContainerArg_s itemSensor1 = { 9, menuSelectSensor, (void *)1 };
static const struct itemContainerArg_s itemSensor2 = { 9, menuSelectSensor, (void *)2 };
static const struct itemContainerArg_s itemSensor3 = { 9, menuSelectSensor, (void *)3 };

static const struct item_s items6[] = {
    
    { " Settings           ", ITEMCONTAINER,    (const void *)2 },
    { " Sensor 0           ", ITEMCONTAINERARG, &itemSensor0    }, 
    { " Sensor 1           ", ITEMCONTAINERARG, &itemSensor1    }, 
    { " Sensor 2           ", ITEMCONTAINERARG, &itemSensor2    }, 
    { " Sensor 3           ", ITEMCONTAINERARG, &itemSensor3    }
};

static const struct item_s items7[] = {
    
    { " Programs           ", ITEMCONTAINER, (const void *)3     },
    { " Enabled            ", ITEMLIST,      &itemProgramEnabled }, 
    { " Target             ", ITEMINTEGER,   &itemProgramTarget  }, 
    { " P                  ", ITEMDOUBLE,    &itemProgramP       }, 
    { " I                  ", ITEMDOUBLE,    &itemProgramI       }, 
    { " Alarm              ", ITEMLIST,      &itemProgramAlarm   } 
};

static const struct item_s items8[] = {
    
    { " Fans               ", ITEMCONTAINER, (const void *)5 },
    { " Enabled            ", ITEMLIST,      &itemFanEnabled }, 
    { " Type               ", ITEMLIST,      &itemFanType    }, 
    { " Min%               ", ITEMINTEGER,   &itemFanMin     }, 
    { " Max%               ", ITEMINTEGER,   &itemFanMax     }, 
};

static const struct item_s items9[] = {
                    
    { " Sensors            ", ITEMCONTAINER, (const void *)6    },
    { " Enabled            ", ITEMLIST,      &itemSensorEnabled }, 
    { " Type               ", ITEMLIST,      &itemSensorType    }, 
    { " Offset             ", ITEMINTEGER,   &itemSensorOffset  }, 
};

static void 
closeDialog0(uint16_t value, void *data) {
    
    /* value 0 - Yes */
    /*       1 - No  */
    
    /* TODO action the yes/no */

    if (value == 0) {
        menuSave((void *)1);   
    } else {
        menuCancel((void *)1);        
    }    
}

static uint16_t dialog0Value;

static const struct itemDialog_s dialog0 = {
    
    "Save and apply      ",
    "changes?            ",
    "                    ",
    10,
    0,
    closeDialog0,
    (void *)0,
    &dialog0Value,
    dialogYesNo,
};

static const struct item_s items10[] = {

    { " Info               ", ITEMDIALOG, &dialog0 },
};

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

static const struct menu_s menus[] = {
    
    { items0,  ARRAYSIZE(items0),  screen0       },
    { items1,  ARRAYSIZE(items1),  defaultScreen },        
    { items2,  ARRAYSIZE(items2),  defaultScreen },        
    { items3,  ARRAYSIZE(items3),  defaultScreen },        
    { items4,  ARRAYSIZE(items4),  defaultScreen },        
    { items5,  ARRAYSIZE(items5),  defaultScreen },        
    { items6,  ARRAYSIZE(items6),  defaultScreen },        
    { items7,  ARRAYSIZE(items7),  defaultScreen },        
    { items8,  ARRAYSIZE(items8),  defaultScreen },        
    { items9,  ARRAYSIZE(items9),  defaultScreen },            
    { items10, ARRAYSIZE(items10), dialogYesNo   }        
};

static uint16_t menuCurrent;
static uint16_t menuCurrentItem;

static uint16_t menuOffset;
static uint16_t menuItemSelected = (uint16_t)-1;;
static uint16_t menuUpArrow;
static uint16_t menuDownArrow;
static uint8_t  menuRedraw;

static void (*userEncoderSet)(uint16_t min, uint16_t max, uint16_t value, uint16_t step, void *data);
static void *userEncoderData;

static struct configSensor_s *cfgSensor;
static struct configProgram_s *cfgProgram;
static struct configFan_s *cfgFan;

#ifdef BLINK7BIT
static void (*userVideoBlinkDelay)(void *data);
static void *userVideoBlinkDelayData;
#endif

static uint16_t 
menuDialogSave(void *data) {
    
    if (cfgChanged) {
        
        menuCurrent     = 10;
        menuCurrentItem = 0;    
        return 1;
    }

    return 0;
}

static uint16_t 
menuSave(void *data) {

//    uint16_t reRoute = (uint16_t)data;
    
    configWrite(&localCfg);

    cfgChanged = 0;
    
    __delay_ms(100);                /* We wait a little while
                                     * for the user to let go of the pushbutton
                                     * else we will catch the button press
                                     * as we start up again. A more sensible
                                     * approach would be to wait for the button
                                     * release state before continuing.
                                     */
    asm ( "RESET ");
        
//    if (reRoute) {
//        
//        menuCurrent     = 0;
//        menuCurrentItem = 0;    
//        return 1;
//    }
    
    return 0;
}

static uint16_t
menuCancel(void *data) {

    uint16_t reRoute = (uint16_t)data;

    configRead(&localCfg);
    
    cfgChanged = 0;

    if (reRoute) {

        menuCurrent     = 0;
        menuCurrentItem = 0;    
        return 1;
    }
    
    return 0;
}

#ifdef BLINK7BIT
void
menuInit(void (*encoderSet)(uint16_t min, uint16_t max, uint16_t value, uint16_t step, void *encoderdData), 
         void *encoderdData,
         void (*videoBlinkDelay)(void *videoBlinkDelayData),
         void *videoBlinkDelayData) {
#else
void
menuInit(void (*encoderSet)(uint16_t min, uint16_t max, uint16_t value, uint16_t step, void *encoderdData), 
         void *encoderdData) {
#endif
        
    configRead(&localCfg);

    cfgSensor  = &localCfg.sensor[0];
    cfgProgram = &localCfg.program[0];
    cfgFan     = &localCfg.fan[0];
    
    userEncoderSet  = encoderSet;
    userEncoderData = encoderdData;
    
#ifdef BLINK7BIT
    userVideoBlinkDelay     = videoBlinkDelay;
    userVideoBlinkDelayData = videoBlinkDelayData;
#endif
    
    userEncoderSet(0, menus[menuCurrent].numItems-1, 0, 1, userEncoderData);
}

enum doubleState_s {

    EDITINT = 0,
    EDITFRAC
} ;

static enum doubleState_s doubleState;

static void
dialogYesNo(void) {
   
    char *buf;    
    const struct item_s *item = &menus[menuCurrent].items[0];
    const struct itemDialog_s *dialog = item->iptr;
        
    buf = (char *)videoBufferGetPtr(0, 0);
    memcpy(buf, dialog->line0, 20);
    
    buf = (char *)videoBufferGetPtr(1, 0);
    memcpy(buf, dialog->line1, 20);
    
    buf = (char *)videoBufferGetPtr(2, 0);
    memcpy(buf, dialog->line2, 20);

    buf = (char *)videoBufferGetPtr(3, 0);
    memcpy(buf, "     Yes    No      ", 20);

    if (*dialog->value == 0) {        
        buf[4] = LCDCHARRIGHTARROW;        
    } else {        
        buf[11] = LCDCHARRIGHTARROW;
    }           
}

static void
defaultScreen(void) {
    
    /* This function should only be called when a button or encoder 
     * event happens.
     */
        
    uint16_t i;
    uint16_t count = menus[menuCurrent].numItems;
    const struct item_s *items = &menus[menuCurrent].items[0];
        
    for (i=0; i<MENUROWS; i++) {

        char *buf = (char *)videoBufferGetPtr(i, 0);
        
        if (i<count) {

            const struct item_s *item = &items[i+menuOffset];
                        
            memcpy(buf, item->name, 20);
            if (i+menuOffset == menuCurrentItem ) {
                buf[0] = LCDCHARRIGHTARROW;
            }

            if (item->type == ITEMINTEGER) {
                
                const struct itemInteger_s *integer = item->iptr;
                formatNumberRight(&buf[integer->col], *integer->value);                

                if (i+menuOffset == menuItemSelected) {
#ifdef BLINK7BIT
                    videoSetRangeBlink((uint8_t *)&buf[integer->col], integer->width);
#endif
                    buf[integer->col + integer->width + 1] = LCDCHARUPDOWNARROW;
                }
                
            } else if (item->type == ITEMDOUBLE) {
                
                const struct itemFloatingPoint_s *floatingPoint = item->iptr;
                
                formatNumber2Right(&buf[floatingPoint->col], *floatingPoint->p1);                
                buf[floatingPoint->col+2] = '.';
                formatNumber2(&buf[floatingPoint->col+3], *floatingPoint->p2);                
                
                if (i+menuOffset == menuItemSelected) {
#ifdef BLINK7BIT
                    if (doubleState == EDITINT) {
                        videoSetRangeBlink((uint8_t *)&buf[floatingPoint->col], 2);
                    } else {
                        videoSetRangeBlink((uint8_t *)&buf[floatingPoint->col+3], 2);                        
                    }
#endif
                    buf[floatingPoint->col + floatingPoint->width + 1] = LCDCHARUPDOWNARROW;
                }                
                
            } else if (item->type == ITEMLIST) {
                
                const struct itemList_s *list = item->iptr;
                memcpy(&buf[list->col], list->names[*list->value], list->width);

                if (i+menuOffset == menuItemSelected) {
#ifdef BLINK7BIT
                    videoSetRangeBlink((uint8_t *)&buf[list->col], list->width);
#endif
                    buf[list->col + list->width + 1] = LCDCHARUPDOWNARROW;
                }
            }

            if ((i == 0) && (menuUpArrow)) {
                buf[MENUCOLS-1] = LCDCHARUPARROW;
            }

            if ((i == MENUROWS-1) && (menuDownArrow)) {
                buf[MENUCOLS-1] = LCDCHARDOWNARROW;
            }

        } else {

            memcpy(buf, "                    ", 20);
        }
    }
}

static void
screen0(void) {
    
    uint16_t i;
    
    for (i=0; i<MENUROWS; i++) {
        
        if (i<4) {

            char *buf = (char *)videoBufferGetPtr(i, 0);
            uint16_t tempId = i;
            uint16_t pwmId = getMappedPWMId(tempId);
            uint16_t tachId = i;
            
            if (dhtGetTemperatureStatus(tempId) == DHTTEMPSUCCESS) {

                if (tempId == 3) {
                    
                    //           01234567890123456789
                    memcpy(buf, "T 00.0c             ", 20);
                    formatTemp(&buf[2], dhtGetTemperature(tempId, 0));

                } else {
                    
                    //           01234567890123456789
                    memcpy(buf, "T 00.0c F 000% 0000 ", 20);
                    formatTemp(&buf[2], dhtGetTemperature(tempId, 0));
                    formatNumber(&buf[10], pwmGetDutyCycle(pwmId));
                    formatRPM(&buf[15], tachGetRPM(tachId));
                }

            } else {

                if (tempId == 3) {
                    
                    //           01234567890123456789
                    memcpy(buf, "T --.-c             ", 20);

                } else {
                    //           01234567890123456789
                    memcpy(buf, "T --.-c F 000% 0000 ", 20);
                    formatNumber(&buf[10], pwmGetDutyCycle(pwmId));
                    formatRPM(&buf[15], tachGetRPM(tachId));
                }
            }
        }        
    }
}

void 
menuDisplay(void) {

    if ((menuCurrent == 0) || menuRedraw) {
        
        /* Only redraw if update required or we are the initial 
         * home screen 
         */
        menus[menuCurrent].displayFunc();   
        menuRedraw = 0;
    }    
}

static void
updateValues(void) {
    
    uint16_t count = menus[menuCurrent].numItems;

    for (uint16_t i=0; i<count; i++) {

        const struct item_s *item = &menus[menuCurrent].items[i];

        if (item->type == ITEMINTEGER) {

            const struct itemInteger_s *integer = item->iptr;
            *integer->value = integer->getValue();
            
        } else if (item->type == ITEMLIST) {

            const struct itemList_s *list = item->iptr;
            *list->value = list->getValue();

        } else if (item->type == ITEMDOUBLE) {

            const struct itemFloatingPoint_s *floatingPoint = item->iptr;
            *floatingPoint->value = floatingPoint->getValue();
            
            *floatingPoint->p1 = (uint16_t)*floatingPoint->value;
            *floatingPoint->p2 = (*floatingPoint->value - *floatingPoint->p1) * 100;
        }
    }
}

void 
menuButtonPressed(void) {

    const struct item_s *item = &menus[menuCurrent].items[menuCurrentItem];

    menuRedraw = 1;
    
    if ((item->type == ITEMCONTAINER) || (item->type == ITEMCONTAINERARG)) {
            
        if (item->type == ITEMCONTAINERARG) {

            const struct itemContainerArg_s *itemContainerArg = item->iptr;                        

            if (itemContainerArg->buttonPressed(itemContainerArg->data)) {
                
                /* This button press resulted in a redirect 
                 */                
                menuButtonPressed();
                return;
            }

            menuCurrent  = (const uint16_t)itemContainerArg->menu;

        } else {

            menuCurrent  = (const uint16_t)item->iptr;
        }

        menuCurrentItem  = 0;
        menuItemSelected = (uint16_t)-1;
        menuOffset       = 0;        
        menuUpArrow      = 0;
        menuDownArrow    = menus[menuCurrent].numItems > MENUROWS;        

        /* On entering a menu there may be non-container items
         * that display their set values, in that case these will need to
         * be updated
         */        
        updateValues();

        userEncoderSet(0, menus[menuCurrent].numItems-1, 0, 1, userEncoderData);                
        return;
    }
            
    if (menuItemSelected != (uint16_t)-1) {

        /* Step out from editting value back into submenu, this will save
         * the value that was changed.
         */

        menuItemSelected = menuCurrentItem;

        if (item->type == ITEMDIALOG) {

            const struct itemDialog_s *itemDialog = item->iptr;

            itemDialog->close(*itemDialog->value, itemDialog->data);
                        
            menuCurrent      = (const uint16_t)itemDialog->menuDone;            
            menuCurrentItem  = 0;
            menuItemSelected = (uint16_t)-1;
            menuOffset       = 0;        
            menuUpArrow      = 0;
            menuDownArrow    = menus[menuCurrent].numItems > MENUROWS;        

            /* On entering a menu there may be non-container items
             * that display their set values, in that case these will need to
             * be updated
             */        
            updateValues();

            userEncoderSet(0, menus[menuCurrent].numItems-1, 0, 1, userEncoderData);            
            
        } else if (item->type == ITEMINTEGER) {

            const struct itemInteger_s *integer = item->iptr;
            integer->setValue(*integer->value);

            menuItemSelected = -1;
            userEncoderSet(0, menus[menuCurrent].numItems-1, menuCurrentItem, 1, userEncoderData);                

        } else if (item->type == ITEMLIST) {

            const struct itemList_s *list = item->iptr;
            list->setValue(*list->value);

            menuItemSelected = -1;
            userEncoderSet(0, menus[menuCurrent].numItems-1, menuCurrentItem, 1, userEncoderData);                

        } else if (item->type == ITEMDOUBLE) {

            const struct itemFloatingPoint_s *floatingPoint = item->iptr;

            if (doubleState == EDITINT) {

                doubleState = EDITFRAC;                    
                userEncoderSet(0, 99, *floatingPoint->p2, 1, userEncoderData);                    

            } else {

                *floatingPoint->value = (double)((*floatingPoint->p1 * 100) + *floatingPoint->p2) / 100;                    
                floatingPoint->setValue(*floatingPoint->value);

                menuItemSelected = -1;
                userEncoderSet(0, menus[menuCurrent].numItems-1, menuCurrentItem, 1, userEncoderData);
            }
        }

        return;
    } 

    /* We have selected an item that can be edited add the
     * up-down cursor to indicate this and make a copy current value.
     * Reset the encoder limits based on value, min/max and step
     */

    menuItemSelected = menuCurrentItem;

    if (item->type == ITEMINTEGER) {

        const struct itemInteger_s *integer = item->iptr;
        *integer->value = integer->getValue();

        userEncoderSet(integer->min, integer->max, *integer->value, integer->step, userEncoderData);                

    } else if (item->type == ITEMLIST) {

        const struct itemList_s *list = item->iptr;
        *list->value = list->getValue();

        userEncoderSet(0, (list->count)-1, *list->value, 1, userEncoderData);                

    } else if (item->type == ITEMDOUBLE) {

        doubleState = EDITINT;

        const struct itemFloatingPoint_s *floatingPoint = item->iptr;
        userEncoderSet(0, 99, (int)(*floatingPoint->p1), 1, userEncoderData);                

    } else if (item->type == ITEMDIALOG) {

        const struct itemDialog_s *dialog = item->iptr;

        menuCurrent = (const uint16_t)dialog->menu;
                
        *dialog->value = 0;
        userEncoderSet(0, 1, 0, 1, userEncoderData);                
    }   
}

void 
menuEncoderChanged(uint16_t value) {

    menuRedraw = 1;
    
#ifdef BLINK7BIT
    /* User is interacting with system, disable blink for a 
     * period of time and set blinking characters to visible
     */
    userVideoBlinkDelay(userVideoBlinkDelayData);
#endif
    
    if (menuItemSelected == (uint16_t)-1) {
        
        /* We are currently scrolling through menu items, add any arrows
         * to the scrollbar that indicate more items in that direction 
         */
        
        uint16_t count = menus[menuCurrent].numItems;

        menuUpArrow   = 0;
        menuDownArrow = 0;

        if (count > MENUROWS) {

            if (value > menuCurrentItem) {	
                if (value > menuOffset + (MENUROWS-1)) {
                    menuOffset = value - (MENUROWS-1);			
                } 
            } else if (value < menuCurrentItem) {
                if (value < menuOffset) {			
                    menuOffset = value;
                }
            }

            if (menuOffset > 0) {			 
                menuUpArrow = 1;
            }

            if (menuOffset + (MENUROWS-1) < count-1) {			 
                menuDownArrow = 1;
            }
        }

        menuCurrentItem = value;

    } else {

        /* We are currently changing a value for a selected item, add the
         * up-down cursor to indicate this and update the stored value of the
         * item being changed.
         */
        
        const struct item_s *item = &menus[menuCurrent].items[menuCurrentItem];

        if (item->type == ITEMINTEGER) {

            const struct itemInteger_s *integer = item->iptr;            
            *integer->value = value;

        } else if (item->type == ITEMLIST) {

            const struct itemList_s *list = item->iptr;            
            *list->value = value;

        } else if (item->type == ITEMDOUBLE) {

            const struct itemFloatingPoint_s *floatingPoint = item->iptr;
                        
            if (doubleState == EDITINT) {
                *floatingPoint->p1 = value;
            } else {
                *floatingPoint->p2 = value;                
            }
            
            *floatingPoint->value = (double)((*floatingPoint->p1 * 100) + *floatingPoint->p2) / 100;                                

        } else if (item->type == ITEMDIALOG) {

            const struct itemDialog_s *itemDialog = item->iptr;

            *itemDialog->value = value;                
        }
    }
}

static void 
setCfgChanged(void) {
    
    cfgChanged = 1;
}

static uint16_t getFanEnabled(void) {    
    return cfgFan[menuCurrentFan].enabled;
}

static void setFanEnabled(uint16_t value) {
    
    if (cfgFan[menuCurrentFan].enabled != value) {
        cfgFan[menuCurrentFan].enabled = value;
        setCfgChanged();
    }
}

static uint16_t getFanType(void) {    
    return cfgFan[menuCurrentFan].type;
}

static void setFanType(uint16_t value) {
    
    if (cfgFan[menuCurrentFan].type != value) {
        cfgFan[menuCurrentFan].type = value;        
        setCfgChanged();
    }
}

static uint16_t getFanMin(void) {    
    return cfgFan[menuCurrentFan].minDuty;
}

static void setFanMin(uint16_t value) {

    if (cfgFan[menuCurrentFan].minDuty != value) {    
        cfgFan[menuCurrentFan].minDuty = value;
        setCfgChanged();
    }
}

static uint16_t getFanMax(void) {    
    return cfgFan[menuCurrentFan].maxDuty;
}

static void setFanMax(uint16_t value) {
    
    if (cfgFan[menuCurrentFan].maxDuty != value) {        
        cfgFan[menuCurrentFan].maxDuty = value;
        setCfgChanged();
    }
}

static uint16_t getProgramAlarm(void) {    
    return cfgProgram[menuCurrentProgram].alarm;
}

static void setProgramAlarm(uint16_t value) {
    
    if (cfgProgram[menuCurrentProgram].alarm != value) {    
        cfgProgram[menuCurrentProgram].alarm = value;
        setCfgChanged();
    }
}

static uint16_t getProgramEnabled(void) {
    return cfgProgram[menuCurrentProgram].enabled;    
}

static void setProgramEnabled(uint16_t value) {
    
    if (cfgProgram[menuCurrentProgram].enabled != value) {
        cfgProgram[menuCurrentProgram].enabled = value;        
        setCfgChanged();
    }
}

static uint16_t getProgramTarget(void) {
    return cfgProgram[menuCurrentProgram].targetTemp;    
}

static void setProgramTarget(uint16_t value) {
    
    if (cfgProgram[menuCurrentProgram].targetTemp != value) {
        cfgProgram[menuCurrentProgram].targetTemp = value;    
        setCfgChanged();
    }
}

static double getProgramP(void) {
    return cfgProgram[menuCurrentProgram].P;    
}

static void setProgramP(double value) {
    
    if (cfgProgram[menuCurrentProgram].P != value) {    
        cfgProgram[menuCurrentProgram].P = value;
        setCfgChanged();
    }
}

static double getProgramI(void) {
    return cfgProgram[menuCurrentProgram].I;    
}

static void setProgramI(double value) {
    
    if (cfgProgram[menuCurrentProgram].I != value) {    
        cfgProgram[menuCurrentProgram].I = value;
        setCfgChanged();
    }
}

static uint16_t getSensorEnabled(void) {
    return cfgSensor[menuCurrentSensor].enabled;    
}

static void setSensorEnabled(uint16_t value) {
    
    if (cfgSensor[menuCurrentSensor].enabled != value) {    
        cfgSensor[menuCurrentSensor].enabled = value;
        setCfgChanged();
    }
}

static uint16_t getSensorType(void) {
    return cfgSensor[menuCurrentSensor].type;        
}

static void setSensorType(uint16_t value) {
    
    if (cfgSensor[menuCurrentSensor].type != value) {
        cfgSensor[menuCurrentSensor].type = value;        
        setCfgChanged();
    }
}

static uint16_t getSensorOffset(void) {
    return cfgSensor[menuCurrentSensor].offset;        
}

static void setSensorOffset(uint16_t value) {
    
    if (cfgSensor[menuCurrentSensor].offset != value) {
        cfgSensor[menuCurrentSensor].offset = value;            
        setCfgChanged();
    }
}
