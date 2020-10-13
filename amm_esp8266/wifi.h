#ifndef _WIFI_H
#define _WIFI_H

#include "Arduino.h"
#include <ESP8266WiFi.h>

extern void internetInit();
extern char isInternetConnected();
extern void writeSerial(char *message);
#endif