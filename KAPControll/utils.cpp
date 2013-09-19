/*
 */

#include <Arduino.h>

#include "def.h"

void signalLed(const char chrMorse[]) {
        for (unsigned int i = 0; i < strlen(chrMorse); i++)
        {
                LEDPIN_ON
                if (chrMorse[i] == '.') {
                        delay(200);
                } else {
                        delay(600);
                }
                LEDPIN_OFF
                delay(200);
        }
}