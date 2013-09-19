/*
 * I want it easy, without many interupts, or many wires. 
 * So I read RC Signals from Sum Signal at Pin0
 */

#include <Arduino.h>
#include <pins_arduino.h>
#include <stdint.h>

#include "def.h"
#include "configuration.h"
#include "RX.h"


volatile uint16_t rcValue[8] = {1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502}; // interval [1000;2000]
static uint8_t rcChannel[8] = {SERIAL_SUM_PPM}; //Channel order for PPM SUM RX Configs

// global variable for the whole project
int16_t rcData[8]; // interval [1000;2000]

void configureReceiver() {
    PPM_PIN_INTERRUPT
}


// Read PPM SUM RX Data
void rxInt() {
    uint16_t now,diff;
    static uint16_t last = 0;
    static uint8_t chan = 0;

    now = micros();
    diff = now - last;
    last = now;
    if(diff>3000) chan = 0;
    else {
      if(900<diff && diff<2200 && chan<8 ) {   //Only if the signal is between these values it is valid, otherwise the failsafe counter should move up
        rcValue[chan] = diff;
      }
    chan++;
    }
}


/**************************************************************************************/
/***************          combine and sort the RX Datas            ********************/
/**************************************************************************************/
uint16_t readRawRC(uint8_t chan) {
  uint16_t data;
  uint8_t oldSREG;
  oldSREG = SREG; cli(); // Let's disable interrupts
  data = rcValue[rcChannel[chan]]; // Let's copy the data Atomically
  SREG = oldSREG; sei();// Let's enable the interrupts
  return data; // We return the value correctly copied when the IRQ's where disabled
}

/**************************************************************************************/
/***************          compute and Filter the RX data           ********************/
/**************************************************************************************/
void computeRC() {
    static int16_t rcData4Values[8][4], rcDataMean[8];
    static uint8_t rc4ValuesIndex = 0;
    uint8_t chan,a;
    rc4ValuesIndex++;
    for (chan = 0; chan < 8; chan++) {
      rcData4Values[chan][rc4ValuesIndex%4] = readRawRC(chan);
      rcDataMean[chan] = 0;
      for (a=0;a<4;a++) rcDataMean[chan] += rcData4Values[chan][a];
      rcDataMean[chan]= (rcDataMean[chan]+2)/4;
      if ( rcDataMean[chan] < rcData[chan] -3)  rcData[chan] = rcDataMean[chan]+2;
      if ( rcDataMean[chan] > rcData[chan] +3)  rcData[chan] = rcDataMean[chan]-2;
    }
}