/* 
 * File:   pid.h
 * Author: tiggr
 *
 * Created on 7. April 2013, 19:46
 */

#ifndef PID_H
#define	PID_H

class PID {
    protected:
        float oldError, errorSum;
        float Kp, Ki, Kd;

    public:
        void init(float P, float I, float D);
        float calculate(float isValue, float targetValue, float deltaT);
        void setPID(float P, float I, float D);
        void reset();
};

#endif	/* PID_H */

