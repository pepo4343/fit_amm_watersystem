#ifndef _ULTRASONIC_H
#define _ULTRASONIC_H


#include "Arduino.h"

extern void ulInit();

extern long getDistanceCM();

extern long getVolume(long distance);

#endif
