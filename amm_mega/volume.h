#ifndef _VOLUME_H
#define _VOLUME_H

#include "Arduino.h"

#define HEIGHT_ADDRESS 3000
#define HEIGHT_PADDING 10

#define DIAMETER_CM 52


extern void initUS();
extern void getPercent( unsigned int &currentPercent);
extern void setHeight();
#endif 