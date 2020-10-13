#ifndef _UART__H
#define _UART__H

#include "Arduino.h"

#include <SoftwareSerial.h>

extern void uartInit(void (*callback)(char*  , char* ));
extern void uartLoop();
void writeSerial(char *message);


#endif
