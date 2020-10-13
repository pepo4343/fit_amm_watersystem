#ifndef _RTC_H
#define _RTC_H

#include "Arduino.h"

#include <Arduino_FreeRTOS.h>

#include <EthernetUdp.h>
#include <TimeLib.h>

// #include <ThreeWire.h>
// #include <RtcDS1302.h>

#include <Wire.h>
#include <RtcDS3231.h>


#define RTC_CLK A0
#define RTC_DAT A1
#define RTC_RST A2



extern void printDateTime(char* date );
extern void RTCInit();
extern void setRTC(char*Date , char* Time);
extern void getRTC(int* currentHour , int* currentMinute , int* currentSecond);

extern char RTCisValid();
extern void udpInit();

extern void sendNTPpacket();
extern void setRTCFromNTP();

extern long getSecOfDay();


#endif