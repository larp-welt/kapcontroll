/* 
 * File:   configuration.h
 * Author: tiggr
 *
 * Created on 4. April 2013, 18:44
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#define EEPROM_SIGNATURE         0x3E4B4150
#define EEPROM_CONF_VERSION      3


struct config_t {
    int32_t signature;
    int16_t version;
    int8_t mode;
    float tiltP, tiltI, tiltD;
    float panP, panI, panD;
    float rcTiltY0, rcTiltM;
    int16_t accXZero, accYZero, accZZero;
};

extern config_t config;

void eepromReadConfiguration();
void eepromWriteConfiguration();

#endif	/* CONFIGURATION_H */

