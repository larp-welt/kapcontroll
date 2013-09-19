/*
 */

#include <math.h>

#include "pid.h"

#define SMALL_ERROR     0.001

void PID::init(float P, float I, float D) {
     setPID(P, I, D);
     reset();
}


float PID::calculate(float isValue, float targetValue, float deltaT) { 
     float error;
     float out;

     error = targetValue - isValue;
     errorSum =+ error / deltaT;
     // XXX use a constrain on errorSum

     if (fabs(error) < SMALL_ERROR) {
         // ignore very low errors!
         out = 0.0;
     } else {
         out = Kp * error; // P
         out += Ki * errorSum; // I
         out += Kd * (oldError - error)/deltaT; // D
     }
     
     oldError = error;
     
     return out;
}


void PID::setPID(float P, float I, float D) {
     Kp = P;
     Ki = I;
     Kd = D;
 }


void PID::reset() {
    errorSum = 0;
    oldError = 0;
}