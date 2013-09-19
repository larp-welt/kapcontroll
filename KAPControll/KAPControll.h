// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef KAPControll_H_
#define KAPControll_H_
#include "Arduino.h"
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


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project KAPControll here




//Do not add code below this line
#endif /* KAPControll_H_ */
