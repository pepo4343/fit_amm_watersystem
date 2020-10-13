#ifndef _CONFIG_H
#define _CONFIG_H

#include "Arduino.h"
#include <ESP8266WiFi.h>


#define ALIAS "ADMIN"

#define READY_PIN D1

extern const char* ssid;
extern const char* password;
extern const char* mqttServer;

extern IPAddress ip;

extern IPAddress gateway;

extern IPAddress subnet;

extern IPAddress dns;


#endif