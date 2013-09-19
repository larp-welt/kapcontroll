/*

 */

// TODO: using compass for PAN at RC, PANO and AUTO

#include <Arduino.h>
#include <avr/io.h>
#include <Servo.h>
#include <Wire.h>
#include "stdint.h"

#include "config.h"
#include "SerialCommand.h"
#include "commands.h"
#include "RX.h"
#include "configuration.h"
#include "pid.h"
#include "utils.h"
#include "chdk.h"
#include "filter.h"


// sensors
#include "MPU6050.h"
#include "HMC5883L.h"


Servo TiltServo, PanServo, RollServo;
MPU6050 mpu(MPU6050_ADDRESS);
PID pidTilt, pidPan;
CHDK camera;
Complementary tiltFilter, panFilter;

float xGyroOffset, yGyroOffset, zGyroOffset;
unsigned long timer=0;   


float arctan2(float x, float y) {
     /** http://dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization
      * \return angle in °
      * \param x, y
      */
     float angle; 
     float coeff_1 = PI/4;
     float coeff_2 = 3*coeff_1;
     float abs_y = fabs(y)+1e-10 ;     // kludge to prevent 0/0 condition
     
     if (x>=0) {
        float r = (x - abs_y) / (x + abs_y);
        angle = coeff_1 - coeff_1 * r;
     }
     else {
        float r = (x + abs_y) / (abs_y - x);
        angle = coeff_2 - coeff_1 * r;
     }
     if (y < 0) return(-angle* (180.0f / PI));     // negate if in quad III or IV
     else return(angle* (180.0f / PI));
} // end of arctan2


void gyroCalibration() {
     xGyroOffset = 0;
     yGyroOffset = 0;
     zGyroOffset = 0; 
     
     for(int i=0;i<500;i++) {
         xGyroOffset += mpu.getRotationX();
         yGyroOffset += mpu.getRotationY();
         zGyroOffset += mpu.getRotationZ();
         delay(10);
    }

    xGyroOffset = xGyroOffset/500.0;
    yGyroOffset = yGyroOffset/500.0;
    zGyroOffset = zGyroOffset/500.0;
} // end of gyroCalibration


void readGyro(float* gyroX, float* gyroY, float* gyroZ) {
    int16_t x,y,z;
    
    mpu.getRotation(&x,&y,&z);

    *gyroX = (x-xGyroOffset)/MPU_RESOLUTION_DIVIDER;
    *gyroY = (y-yGyroOffset)/MPU_RESOLUTION_DIVIDER;
    *gyroZ = (z-zGyroOffset)/MPU_RESOLUTION_DIVIDER;
} // end of readGyro


int rc(float angle, int8_t  axis) {
    int rctime;
    
    switch (axis) {
        case TILT:
            rctime = (angle -  config.rcTiltY0) / config.rcTiltM;
            break;
        case ROLL:
            rctime = 1500;
            break;
        case PAN:
            rctime = 1500;
            break;
    }
    
    return rctime;
} // end of rc


void readAcc(float* tilt, float* roll) {
     int16_t x, y, z;
     static int8_t count = 0;
     
     count++;
     if (count == ACC_FRQ) {
         // calculate tilt
         mpu.getAcceleration(&x,&y,&z);
         
         *tilt = arctan2(y-config.accYZero, z-config.accZZero);
     }
     if (count == ACC_FRQ+1) {
         // XXX 
         *roll = 0;
         count = 0;
     }
} // end of readAcc

void rcCalibration() {
     static int tiltMin = 1500;
     static int tiltMax = 1500;
     static float tiltAngleMin = 45;
     static float tiltAngleMax = 45;
     float rawTilt, rawRoll;
     
     readAcc(&rawTilt, &rawRoll);
     if (rcData[TILT] > tiltMax) {
         tiltMax = rcData[TILT];
         tiltAngleMax = rawTilt;
     }
     if (rcData[TILT] < tiltMin) {
         tiltMin = rcData[TILT];
         tiltAngleMin = rawTilt;
     }
     config.rcTiltM = (tiltAngleMax-tiltAngleMin)/(tiltMax - tiltMin);
     config.rcTiltY0 = tiltAngleMax - config.rcTiltM * tiltMax;
} // end of rcCalibration


void accCalibration() {
    int16_t x, y, z;
    int xSum, ySum, zSum;
    
    for (int8_t i=0; i<500; i++) {
        mpu.getAcceleration(&x,&y,&z);
        xSum += x;
        ySum += y;
        zSum += z;
        delay(1);
    }
    
    config.accXZero = xSum/500;
    config.accYZero = ySum/500;
    config.accZZero = zSum/500;
}


void setup() {
     LEDPIN_PINMODE
     LEDPIN_OFF
     
     Serial.begin(9600);
     Wire.begin();
          
     sCmd.addCommand("LED", ToggleLED);
     sCmd.addCommand("SHW", Show);
     sCmd.addCommand("PRT", Echo);
     sCmd.addCommand("MDE", Mode);
     sCmd.addCommand("WRT", Write);
     sCmd.addCommand("PID", setPid);
     sCmd.addCommand("//", NoOp);
     sCmd.setDefaultHandler(UnknownCommand);
     Serial.println(F("// - Serial communication started"));
     
     eepromReadConfiguration();
     Serial.println(F("// - Configuration done"));
     
     configureReceiver();
     Serial.println(F("// - RC started"));
     
     TiltServo.attach(TILT_SERVO);
     PanServo.attach(PAN_SERVO);
     // XXX RollServo.attach(ROLL_SERVO);
     camera.init(CAMPIN);
     Serial.println(F("// - Servos and cam ready"));
     
     mpu.initialize();
     mpu.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);          // Set Clock to ZGyro
     mpu.setFullScaleGyroRange(MPU6050_GYRO_FS);           // Set Gyro Sensitivity to config.h
     mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);       // +- 2G
     mpu.setDLPFMode(MPU6050_DLPF_BW);                     // Set Gyro Low Pass Filter to config.h
     mpu.setRate(0);                                       // 0=1kHz, 1=500Hz, 2=333Hz, 3=250Hz, 4=200Hz
     mpu.setSleepEnabled(false);
     if(mpu.testConnection()) Serial.println(F("// - MPU6050 ok"));
     gyroCalibration();

     pidTilt.init(config.tiltP, config.tiltI, config.tiltD);
     pidPan.init(config.panP, config.panI, config.panD);
     tiltFilter.setTau(0.8);

     // Required to prevent NAN error from MPU
     mpu.resetDMP();
     mpu.resetFIFO();
     
     Serial.print(F("// KAPControl "));
     Serial.print(KAPCONTROLL);
     Serial.print(F(" on Arduino "));
     Serial.println(ARDUINO);
     Serial.println(F("// Ready!"));
     signalLed("---");
     LEDPIN_ON
             
     timer = micros();
} // end of setup


void loop() {
     float rawTilt=0;
     float rawRoll=0;
     float tiltAngle;
     float xGyro, yGyro, zGyro;
     unsigned long now;
     int tilt, pan;
     
     // get all necessary data
     sCmd.readSerial();
     computeRC();
     now = micros();
     readGyro(&xGyro, &yGyro, &zGyro);
     readAcc(&rawTilt, &rawRoll);
     tiltAngle = rc(tiltFilter.filter(xGyro, now-timer, rawTilt), TILT);
     
     switch (config.mode) {
         case MODE_RC:
             tilt = pidTilt.calculate(tiltAngle, rcData[TILT], now-timer);
             pan = rcData[PAN];
             
             TiltServo.writeMicroseconds(tilt);
             PanServo.writeMicroseconds(pan);
             // XXX maybe, sometimes, we will stabilize ROLL too!
             break;
         case MODE_AUTO:
             NoOp();
             break;
         case MODE_PANO:
             NoOp();
             break;
         case MODE_CALIB:
             switch (submode) {
                 case SUBMODE_ACC:
                     accCalibration();
                     submode = SUBMODE_NULL;
                     break;
                 case SUBMODE_RC:
                     TiltServo.writeMicroseconds(TILT);
                     rcCalibration();
                     break;
                 case SUBMODE_NULL:
                     TiltServo.writeMicroseconds(TILT);
                     break;
             }
             break;
     } // end of switch
     
     timer = micros();
} // end of loop
