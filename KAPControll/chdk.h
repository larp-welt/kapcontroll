/* 
 * File:   chdk.h
 * Author: tiggr
 *
 * Created on 7. April 2013, 23:33
 */

#include <stdint.h>

#ifndef CHDK_H
#define	CHDK_H

#define CHDK_ZOOM_IN		2000
#define CHDK_ZOOM_OUT		2500
#define CHDK_SHOOT		 500
#define CHDK_EXP_MINUS		4000
#define CHDK_EXP_ZERO		4500
#define CHDK_EXP_PLUS	        5000
#define S_PAUSE                  300
#define DEADTIME	       30000  // min. time between two commands

#define MAX_COMMANDS            32
#define TIMER_CLOCK_FREQ 2000000.0 //2MHz for /8 prescale from 16MHz


class CHDK {
        public:
            void init(int8_t pin);
            void shoot();
            void zoom(int8_t direction);
            void exposure(int8_t direction);
            
        protected:
            void send(const int command);
            unsigned char SetupTimer2(float timeoutFrequency);
};


#endif	/* CHDK_H */

