#ifndef _MQTT_H
#define _MQTT_H

#include "Arduino.h"
#include <PubSubClient.h>



extern void mqttInit();
extern void mqttConnect();
extern void mqttLoop();

#endif