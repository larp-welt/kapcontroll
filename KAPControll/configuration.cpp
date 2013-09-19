/*
 *
 */

#include <Arduino.h>
#include <avr/eeprom.h>

#include "def.h"
#include "configuration.h"

config_t config;


void eepromReadConfiguration() {
    // http://playground.arduino.cc/Code/EEPROMWriteAnything
    eeprom_read_block((void*)&config, (void*)0, sizeof(config));
    if (config.signature != EEPROM_SIGNATURE || config.version != EEPROM_CONF_VERSION) {
        // no config found, lets asume some reasonable defaults
        config.signature = EEPROM_SIGNATURE;
        config.version = EEPROM_CONF_VERSION;
        config.mode = MODE_RC;
        // pan and pilt PID: do nothing!
        config.tiltP = 0.0;
        config.tiltI = 0.0;
        config.tiltD = 0.0;
        config.panP = 0.0;
        config.panI = 0.0;
        config.panD = 0.0;
        config.rcTiltY0 = -51.0;
        config.rcTiltM = 0.06;
        config.accXZero = 0;
        config.accYZero = 0;
        config.accZZero = 0;
    } 
}


void eepromWriteConfiguration() {
    // http://playground.arduino.cc/Code/EEPROMWriteAnything
    eeprom_write_block((const void*)&config, (void*)0, sizeof(config));
}
