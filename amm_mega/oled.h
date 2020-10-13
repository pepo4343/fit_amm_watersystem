#ifndef _OLED_H
#define _OLED_H

#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4


extern void oledInit();
extern void oledWrite();
#endif
