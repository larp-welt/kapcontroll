/*
 
 */

#include <Arduino.h>
#include <stdint.h>

#include "def.h"
#include "chdk.h"

volatile static int cmdQueque[MAX_COMMANDS][2];
volatile static int quequeWrite = 0;
volatile static int quequeRead = 0;
volatile static int timerLoadValue;
volatile static int latency;
volatile static int campin;


void CHDK::init(int8_t pin) {
    campin = pin;
    quequeWrite = 0;
    quequeRead = 0;
    timerLoadValue = SetupTimer2(1000);
}


void CHDK::shoot() {
    send(CHDK_SHOOT);
}


void CHDK::zoom(int8_t direction) {
    if (direction > 0) {
        send(CHDK_ZOOM_IN);
    } else {
        send(CHDK_ZOOM_OUT);
    }
}


void CHDK::exposure(int8_t value) {
    if (value < 0) { send(CHDK_EXP_MINUS); }
    if (value == 0) { send(CHDK_EXP_ZERO); }
    if (value > 0) { send(CHDK_EXP_PLUS); }
}


void CHDK::send(const int command)
{
    static unsigned int last = 0;

	if (micros()-DEADTIME > last)
	{
		   cmdQueque[quequeWrite][0] = command;
		   cmdQueque[quequeWrite][1] = 1;
		   if (++quequeWrite >= MAX_COMMANDS) quequeWrite = 0;

		   cmdQueque[quequeWrite][0] = S_PAUSE;
		   cmdQueque[quequeWrite][1] = 0;
		   if (++quequeWrite >= MAX_COMMANDS) quequeWrite = 0;

		   last = micros();
	}
}


unsigned char CHDK::SetupTimer2(float timeoutFrequency) {
	//http://www.uchobby.com/index.php/2007/11/24/arduino-interrupts/
	unsigned char result; //The timer load value.

	//Calculate the timer load value
	result=(int)((257.0-(TIMER_CLOCK_FREQ/timeoutFrequency))+0.5);
	//The 257 really should be 256 but I get better results with 257.

	//Timer2 Settings: Timer Prescaler /8, mode 0
	//Timer clock = 16MHz/8 = 2Mhz or 0.5us
	//The /8 prescale gives us a good range to work with
	//so we just hard code this for now.
	TCCR2A = 0;
	TCCR2B = 0<<CS22 | 1<<CS21 | 0<<CS20;

	//Timer2 Overflow Interrupt Enable
	TIMSK2 = 1<<TOIE2;

	//load the timer for its first cycle
	TCNT2=result;

	return(result);
}


ISR(TIMER2_OVF_vect) {
	static int wait = 0;
	static int state = 1;

	if (!wait)
	{
		if (state) digitalWrite(campin, LOW);
		if (quequeRead != quequeWrite)
		{
			wait = cmdQueque[quequeRead][0];
			state = cmdQueque[quequeRead][1];
			if (state) digitalWrite(campin, HIGH);
			if (++quequeRead >= MAX_COMMANDS) quequeRead = 0;
		}
	} else {
		wait--;
	}
	latency=TCNT2;
	TCNT2=latency+timerLoadValue;
}
