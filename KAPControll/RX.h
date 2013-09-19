/* 
 * File:   RX.h
 * Author: tiggr
 *
 * Created on 11. Oktober 2012, 20:42
 */

#ifndef RX_H
#define	RX_H

#include <stdint.h>
#include "def.h"

// Raw RC Values will be stored here
//extern uint16_t rcValue[8];
extern int16_t rcData[8];


void configureReceiver();
void rxInt();
void computeRC();




#endif	/* RX_H */

