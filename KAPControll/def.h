/* 
 * File:   def.h
 * Author: tiggr
 *
 * Created on 11. Oktober 2012, 23:07
 */

#ifndef DEF_H
#define	DEF_H

#include "MPU6050.h"

#define KAPCONTROLL "0.1 alpha"

// Proc auto detection
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #define PROMINI
#endif
#if defined(__AVR_ATmega32U4__) || defined(TEENSY20)
  #define PROMICRO
#endif
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
  #define MEGA
#endif

// operation modes
#define MODE_RC         0
#define MODE_AUTO       1
#define MODE_PANO       2
#define MODE_CALIB      3

#define SUBMODE_ACC     1
#define SUBMODE_RC      2
#define SUBMODE_NULL    0

// RC channels
#define PAN             0
#define TILT            1
#define SHOT            2
#define ZOOM            3
#define EXPOSURE        4
#define MODE            5
#define AUX1            6
#define AUX2            7
#define ROLL            99

// pins
#define PAN_SERVO       6
#define TILT_SERVO      7
#define ROLL_SERVO      8       // XXX we have it in the defs, but we don't use it
#define CAMPIN          9

#define SERIAL_SUM_PPM          PAN,TILT,SHOT,ZOOM,EXPOSURE,MODE,AUX1,AUX2
#define PPM_PIN_INTERRUPT       attachInterrupt(0, rxInt, RISING); // Interupt 0, Pin 2


#define MPU6050_ADDRESS     MPU6050_ADDRESS_AD0_HIGH
#define MPU6050_GYRO_FS     MPU6050_GYRO_FS_250         // +-250,500,1000,2000 deg/s
#define MPU6050_DLPF_BW     MPU6050_DLPF_BW_256         //0x07 // 5,10,20,42,98,188,256 Hz

// resolution devider
#if MPU6050_GYRO_FS==0
#define MPU_RESOLUTION_DIVIDER     131.0
#elif MPU6050_GYRO_FS==1
#define MPU_RESOLUTION_DIVIDER     65.5
#elif MPU6050_GYRO_FS==2
#define MPU_RESOLUTION_DIVIDER     32.8
#elif MPU6050_GYRO_FS==3
#define MPU_RESOLUTION_DIVIDER     16.4
#endif

#define ACC_FRQ         20

#if defined(PROMINI)
	#define LEDPIN_PINMODE             pinMode (13, OUTPUT);
	#define LEDPIN_TOGGLE              PINB |= 1<<5;     //switch LEDPIN state (digital PIN 13)
	#define LEDPIN_OFF                 PORTB &= ~(1<<5);
	#define LEDPIN_ON                  PORTB |= (1<<5);
        #define LEDPIN_STATE               bitRead(PORTB, 5);
#endif

#endif	/* DEF_H */

