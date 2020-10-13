#include "mqtt.h"

#include "config.h"

#include "uart.h"

#include "function.h"

#define BUFFER_SIZE 200

extern PubSubClient mqttClient;

extern unsigned long lastMsg;

extern SoftwareSerial NodeSerial;

void onMessage(char *topic, byte *payload, unsigned int length)
{
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < length; i++)
    {
        buffer[i] = payload[i];
    }
    buffer[length] = '\0';
    String dataStr = String(buffer);
    String topicStr = String(topic);

    splitString(topicStr, '/');
    splitString(topicStr, '/');
    splitString(topicStr, '/');
    splitString(topicStr, '/');
    String tp = splitString(topicStr, '/');
    if (tp == "control_mode")
    {

        NodeSerial.print(F("FIT CNM;"));
        NodeSerial.println(dataStr);
    }

    if (tp == "control_row1")
    {

        NodeSerial.print(F("FIT CN1;"));
        NodeSerial.println(dataStr);
    }

    if (tp == "control_row2")
    {

        NodeSerial.print(F("FIT CN2;"));
        NodeSerial.println(dataStr);
    }
    if (tp == "control_fertilizer")
    {

        NodeSerial.print(F("FIT CNF;"));
        NodeSerial.println(dataStr);
    }


    if (tp == "schedule_row1")
    {

        NodeSerial.print(F("FIT SC1;"));
        NodeSerial.println(dataStr);
    }
    if (tp == "schedule_row2")
    {
        NodeSerial.print(F("FIT SC2;"));
        NodeSerial.println(dataStr);
    }

    lastMsg = millis();
}

void mqttInit()
{
    mqttClient.setCallback(onMessage);
    mqttClient.setServer(mqttServer, 1883);
}

void mqttConnect()
{

    if (!mqttClient.connected())
    {
        // Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(ALIAS))
        {
            // Serial.println("connected");

            // Once connected, publish an announcement...
            mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/status");
            mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/schedule_row1");
            mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/schedule_row2");
            mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/control_mode");
            mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/control_row1");
            mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/control_row2");
            mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/control_fertilizer");
            // mqttClient.subscribe("fit/amm/" ALIAS "/watersystem/fertilizer");
            // client.subscribe(Fog_Topic);
        }
        else
        {
            // Serial.print("failed, rc=");
            // Serial.print(mqttClient.state());
            // Serial.println(" try again in 1 seconds");
            // Wait 5 seconds before retrying
        }
    }
    else

    {
        // Serial.println("running");
        mqttClient.publish("fit/amm/" ALIAS "/watersystem/status", "");
    }
}

void mqttLoop()
{
    mqttClient.loop();
}
