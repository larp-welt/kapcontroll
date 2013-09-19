/*

 */

#include <Arduino.h>
#include <stdint.h>
#include "def.h"
#include "config.h"
#include "SerialCommand.h"
#include "RX.h"
#include "configuration.h"


SerialCommand sCmd;
int8_t submode = SUBMODE_NULL;
        
const char *onoff[] = {"OFF", "ON"};


static int freeRAM () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

static void printChannelData(int8_t channel, int16_t data) {
    Serial.print(F("CHANNEL "));
    Serial.print(channel);
    Serial.print(F(": "));
    Serial.println(data);
}

static void ok() { Serial.println(F("OK")); }

static void error(const char *errmsg) { 
    Serial.print(F("ERROR: "));
    Serial.println(errmsg);
}


void ToggleLED() {
    char *arg;
    
    arg = sCmd.next();

    if (arg == NULL) {
        int8_t state;
        state = LEDPIN_STATE
        Serial.print(F("LED "));
        Serial.println(onoff[state]);
    } else {
    if (strcmp(arg, "ON") == 0) {
        ok();
        LEDPIN_ON
    } else {
    if (strcmp(arg, "OFF") == 0) {
        ok();
        LEDPIN_OFF
    } else { error("unknown param"); }
    }}
}


void Show() {
    /* show different values */
    char *arg;
    
    arg = sCmd.next();
    
    if (strcmp(arg, "RAM") == 0) {
        Serial.print(F("RAM: "));
        Serial.println(freeRAM()); } else {
    if (strcmp(arg, "RC") == 0) {
        char *schannel = sCmd.next();
        if (schannel == NULL) {
            for (int8_t i=0; i<8; i++) {
                printChannelData(i, rcData[i]);
        }} else {
        int8_t channel = atoi(schannel);
        if (channel >= 0 && channel <8) {
            printChannelData(channel, rcData[channel]);
        } else { error("unknown param"); }}}
    }
}


void Echo() {
    char *arg;
    
    arg = sCmd.next();
    while (arg != NULL) { 
        Serial.print(arg);
        Serial.print(" ");
        arg = sCmd.next();
    }
    Serial.println();
}


void Mode() {
    const char *modes[] = {"RC", "AUTO", "PANO"};
    char *arg;
    
    arg = sCmd.next();
    
    if (arg == NULL) {
        Serial.print(F("MDE "));
        Serial.println(modes[config.mode]); } else {
    if (strcmp(arg, "RC") == 0 ) {
        config.mode = MODE_RC;
        ok(); } else {
    if (strcmp(arg, "AUTO") == 0 ) {
        config.mode = MODE_AUTO;
        ok(); } else {
    if (strcmp(arg, "PANO") == 0 ) {
        config.mode = MODE_PANO;
        ok(); } else { error("unknown param"); }
    }}}
}


void Write() {
    eepromWriteConfiguration();
    ok();
}


void setPid() {
    char *axis;
    
    axis = sCmd.next();
    
    if (axis == NULL) {
        Serial.print(F("PID TILT "));
        Serial.print(config.tiltP);
        Serial.print(F(" "));
        Serial.print(config.tiltI);
        Serial.print(F(" "));
        Serial.println(config.tiltD);
        
        Serial.print(F("PID PAN "));
        Serial.print(config.panP);
        Serial.print(F(" "));
        Serial.print(config.panI);
        Serial.print(F(" "));
        Serial.println(config.panD);
    } else {
        float values[3];
        char *arg;
       
        for (int8_t i=0; i<3; i++) {
            arg = sCmd.next();
            if (arg == NULL) {
                values[0] = -999;
                values[1] = -999;
                values[2] = -999;
            } else {
                values[i] = atof(arg);
            }
        }
     
        if (values[0] == -999 && values[1] == -999 && values[2] == -999) {
            if (strcmp(axis, "TILT") == 0) {
                Serial.print(F("PIN TILT "));
                Serial.print(config.tiltP);
                Serial.print(F(" "));
                Serial.print(config.tiltI);
                Serial.print(F(" "));
                Serial.println(config.tiltD);
            } else {
            if (strcmp(axis, "PAN") == 0) {
                Serial.print(F("PIN PAN "));
                Serial.print(config.panP);
                Serial.print(F(" "));
                Serial.print(config.panI);
                Serial.print(F(" "));
                Serial.println(config.panD);
            } else { error("unknown param"); }}
        } else {
            if (strcmp(axis, "TILT") == 0) {
                config.tiltP = values[0];
                config.tiltI = values[1];
                config.tiltD = values[2];
                ok();
            } else {
            if (strcmp(axis, "PAN") == 0) {
                config.panP = values[0];
                config.panI = values[1];
                config.panD = values[2];
                ok();
            } else { error("unknown param"); }}
        }
    }
}


void NoOp() {;}


void UnknownCommand(const char *command) { error(command); }