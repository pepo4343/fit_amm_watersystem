#ifndef _NTP_H
#define _NTP_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
extern void ntpInit();
extern void sendNTPpacket();
extern void ntpLoop();
#endif